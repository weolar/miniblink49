// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_dibbase.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_imageobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "core/fpdfapi/render/cpdf_renderstatus.h"
#include "core/fxcodec/codec/ccodec_basicmodule.h"
#include "core/fxcodec/codec/ccodec_jbig2module.h"
#include "core/fxcodec/codec/ccodec_jpegmodule.h"
#include "core/fxcodec/codec/ccodec_jpxmodule.h"
#include "core/fxcodec/codec/ccodec_scanlinedecoder.h"
#include "core/fxcodec/codec/cjpx_decoder.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/cfx_fixedbufgrow.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

constexpr int kMaxImageDimension = 0x01FFFF;

unsigned int GetBits8(const uint8_t* pData, uint64_t bitpos, size_t nbits) {
  ASSERT(nbits == 1 || nbits == 2 || nbits == 4 || nbits == 8 || nbits == 16);
  ASSERT((bitpos & (nbits - 1)) == 0);
  unsigned int byte = pData[bitpos / 8];
  if (nbits == 8)
    return byte;

  if (nbits == 16)
    return byte * 256 + pData[bitpos / 8 + 1];

  return (byte >> (8 - nbits - (bitpos % 8))) & ((1 << nbits) - 1);
}

bool GetBitValue(const uint8_t* pSrc, uint32_t pos) {
  return pSrc[pos / 8] & (1 << (7 - pos % 8));
}

// Just to sanity check and filter out obvious bad values.
bool IsMaybeValidBitsPerComponent(int bpc) {
  return bpc >= 0 && bpc <= 16;
}

bool IsAllowedBitsPerComponent(int bpc) {
  return bpc == 1 || bpc == 2 || bpc == 4 || bpc == 8 || bpc == 16;
}

bool IsAllowedICCComponents(int nComp) {
  return nComp == 1 || nComp == 3 || nComp == 4;
}

bool IsColorIndexOutOfBounds(uint8_t index, const DIB_COMP_DATA& comp_datum) {
  return index < comp_datum.m_ColorKeyMin || index > comp_datum.m_ColorKeyMax;
}

bool AreColorIndicesOutOfBounds(const uint8_t* indices,
                                const DIB_COMP_DATA* comp_data,
                                size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (IsColorIndexOutOfBounds(indices[i], comp_data[i]))
      return true;
  }
  return false;
}

// Wrapper class to use with std::unique_ptr for CJPX_Decoder.
class JpxBitMapContext {
 public:
  explicit JpxBitMapContext(CCodec_JpxModule* jpx_module)
      : jpx_module_(jpx_module), decoder_(nullptr) {}

  ~JpxBitMapContext() {}

  void set_decoder(std::unique_ptr<CJPX_Decoder> decoder) {
    decoder_ = std::move(decoder);
  }

  CJPX_Decoder* decoder() { return decoder_.get(); }

 private:
  CCodec_JpxModule* const jpx_module_;  // Weak pointer.
  std::unique_ptr<CJPX_Decoder> decoder_;

  // Disallow evil constructors
  JpxBitMapContext(const JpxBitMapContext&);
  void operator=(const JpxBitMapContext&);
};

}  // namespace

CPDF_DIBBase::CPDF_DIBBase() {}

CPDF_DIBBase::~CPDF_DIBBase() {
  if (m_pColorSpace && m_pDocument) {
    auto* pPageData = m_pDocument->GetPageData();
    if (pPageData) {
      auto* pSpace = m_pColorSpace.Release();
      pPageData->ReleaseColorSpace(pSpace->GetArray());
    }
  }
}

bool CPDF_DIBBase::Load(CPDF_Document* pDoc, const CPDF_Stream* pStream) {
  if (!pStream)
    return false;

  m_pDocument = pDoc;
  m_pDict = pStream->GetDict();
  if (!m_pDict)
    return false;

  m_pStream = pStream;
  m_Width = m_pDict->GetIntegerFor("Width");
  m_Height = m_pDict->GetIntegerFor("Height");
  if (m_Width <= 0 || m_Height <= 0 || m_Width > kMaxImageDimension ||
      m_Height > kMaxImageDimension) {
    return false;
  }
  m_GroupFamily = 0;
  m_bLoadMask = false;
  if (!LoadColorInfo(nullptr, nullptr))
    return false;

  if (m_bDoBpcCheck && (m_bpc == 0 || m_nComponents == 0))
    return false;

  FX_SAFE_UINT32 src_size =
      CalculatePitch8(m_bpc, m_nComponents, m_Width) * m_Height;
  if (!src_size.IsValid())
    return false;

  m_pStreamAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
  m_pStreamAcc->LoadAllDataImageAcc(src_size.ValueOrDie());
  if (m_pStreamAcc->GetSize() == 0 || !m_pStreamAcc->GetData())
    return false;

  if (CreateDecoder() == LoadState::kFail)
    return false;

  if (m_bImageMask) {
    m_bpp = 1;
    m_bpc = 1;
    m_nComponents = 1;
    m_AlphaFlag = 1;
  } else if (m_bpc * m_nComponents == 1) {
    m_bpp = 1;
  } else if (m_bpc * m_nComponents <= 8) {
    m_bpp = 8;
  } else {
    m_bpp = 24;
  }
  FX_SAFE_UINT32 pitch = CalculatePitch32(m_bpp, m_Width);
  if (!pitch.IsValid())
    return false;

  m_pLineBuf.reset(FX_Alloc(uint8_t, pitch.ValueOrDie()));
  LoadPalette();
  if (m_bColorKey) {
    m_bpp = 32;
    m_AlphaFlag = 2;
    pitch = CalculatePitch32(m_bpp, m_Width);
    if (!pitch.IsValid())
      return false;

    m_pMaskedLine.reset(FX_Alloc(uint8_t, pitch.ValueOrDie()));
  }
  m_Pitch = pitch.ValueOrDie();
  return true;
}

bool CPDF_DIBBase::ContinueToLoadMask() {
  if (m_bImageMask) {
    m_bpp = 1;
    m_bpc = 1;
    m_nComponents = 1;
    m_AlphaFlag = 1;
  } else if (m_bpc * m_nComponents == 1) {
    m_bpp = 1;
  } else if (m_bpc * m_nComponents <= 8) {
    m_bpp = 8;
  } else {
    m_bpp = 24;
  }
  if (!m_bpc || !m_nComponents) {
    return false;
  }
  FX_SAFE_UINT32 pitch = CalculatePitch32(m_bpp, m_Width);
  if (!pitch.IsValid())
    return false;

  m_pLineBuf.reset(FX_Alloc(uint8_t, pitch.ValueOrDie()));
  if (m_pColorSpace && m_bStdCS) {
    m_pColorSpace->EnableStdConversion(true);
  }
  LoadPalette();
  if (m_bColorKey) {
    m_bpp = 32;
    m_AlphaFlag = 2;
    pitch = CalculatePitch32(m_bpp, m_Width);
    if (!pitch.IsValid())
      return false;
    m_pMaskedLine.reset(FX_Alloc(uint8_t, pitch.ValueOrDie()));
  }
  m_Pitch = pitch.ValueOrDie();
  return true;
}

CPDF_DIBBase::LoadState CPDF_DIBBase::StartLoadDIBBase(
    CPDF_Document* pDoc,
    const CPDF_Stream* pStream,
    bool bHasMask,
    const CPDF_Dictionary* pFormResources,
    CPDF_Dictionary* pPageResources,
    bool bStdCS,
    uint32_t GroupFamily,
    bool bLoadMask) {
  if (!pStream)
    return LoadState::kFail;

  m_pDocument = pDoc;
  m_pDict = pStream->GetDict();
  m_pStream = pStream;
  m_bStdCS = bStdCS;
  m_bHasMask = bHasMask;
  m_Width = m_pDict->GetIntegerFor("Width");
  m_Height = m_pDict->GetIntegerFor("Height");
  if (m_Width <= 0 || m_Height <= 0 || m_Width > kMaxImageDimension ||
      m_Height > kMaxImageDimension) {
    return LoadState::kFail;
  }
  m_GroupFamily = GroupFamily;
  m_bLoadMask = bLoadMask;
  if (!LoadColorInfo(m_pStream->IsInline() ? pFormResources : nullptr,
                     pPageResources)) {
    return LoadState::kFail;
  }
  if (m_bDoBpcCheck && (m_bpc == 0 || m_nComponents == 0))
    return LoadState::kFail;

  FX_SAFE_UINT32 src_size =
      CalculatePitch8(m_bpc, m_nComponents, m_Width) * m_Height;
  if (!src_size.IsValid())
    return LoadState::kFail;

  m_pStreamAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
  m_pStreamAcc->LoadAllDataImageAcc(src_size.ValueOrDie());
  if (m_pStreamAcc->GetSize() == 0 || !m_pStreamAcc->GetData())
    return LoadState::kFail;

  LoadState iCreatedDecoder = CreateDecoder();
  if (iCreatedDecoder == LoadState::kFail)
    return LoadState::kFail;

  if (!ContinueToLoadMask())
    return LoadState::kFail;

  LoadState iLoadedMask = m_bHasMask ? StartLoadMask() : LoadState::kSuccess;
  if (iCreatedDecoder == LoadState::kContinue ||
      iLoadedMask == LoadState::kContinue) {
    return LoadState::kContinue;
  }

  ASSERT(iCreatedDecoder == LoadState::kSuccess);
  ASSERT(iLoadedMask == LoadState::kSuccess);
  if (m_pColorSpace && m_bStdCS)
    m_pColorSpace->EnableStdConversion(false);
  return LoadState::kSuccess;
}

CPDF_DIBBase::LoadState CPDF_DIBBase::ContinueLoadDIBBase(
    PauseIndicatorIface* pPause) {
  if (m_Status == LoadState::kContinue)
    return ContinueLoadMaskDIB(pPause);

  if (m_Status == LoadState::kFail)
    return LoadState::kFail;

  if (m_pStreamAcc->GetImageDecoder() == "JPXDecode")
    return LoadState::kFail;

  FXCODEC_STATUS iDecodeStatus;
  CCodec_Jbig2Module* pJbig2Module = CPDF_ModuleMgr::Get()->GetJbig2Module();
  if (!m_pJbig2Context) {
    m_pJbig2Context = pdfium::MakeUnique<CCodec_Jbig2Context>();
    if (m_pStreamAcc->GetImageParam()) {
      const CPDF_Stream* pGlobals =
          m_pStreamAcc->GetImageParam()->GetStreamFor("JBIG2Globals");
      if (pGlobals) {
        m_pGlobalStream = pdfium::MakeRetain<CPDF_StreamAcc>(pGlobals);
        m_pGlobalStream->LoadAllDataFiltered();
      }
    }
    iDecodeStatus = pJbig2Module->StartDecode(
        m_pJbig2Context.get(), m_pDocument->CodecContext(), m_Width, m_Height,
        m_pStreamAcc, m_pGlobalStream, m_pCachedBitmap->GetBuffer(),
        m_pCachedBitmap->GetPitch(), pPause);
  } else {
    iDecodeStatus = pJbig2Module->ContinueDecode(m_pJbig2Context.get(), pPause);
  }

  if (iDecodeStatus < 0) {
    m_pJbig2Context.reset();
    m_pCachedBitmap.Reset();
    m_pGlobalStream.Reset();
    return LoadState::kFail;
  }
  if (iDecodeStatus == FXCODEC_STATUS_DECODE_TOBECONTINUE)
    return LoadState::kContinue;

  LoadState iContinueStatus = LoadState::kSuccess;
  if (m_bHasMask) {
    if (ContinueLoadMaskDIB(pPause) == LoadState::kContinue) {
      iContinueStatus = LoadState::kContinue;
      m_Status = LoadState::kContinue;
    }
  }
  if (iContinueStatus == LoadState::kContinue)
    return LoadState::kContinue;

  if (m_pColorSpace && m_bStdCS)
    m_pColorSpace->EnableStdConversion(false);
  return iContinueStatus;
}

bool CPDF_DIBBase::LoadColorInfo(const CPDF_Dictionary* pFormResources,
                                 const CPDF_Dictionary* pPageResources) {
  m_bpc_orig = m_pDict->GetIntegerFor("BitsPerComponent");
  if (!IsMaybeValidBitsPerComponent(m_bpc_orig))
    return false;

  if (m_pDict->GetIntegerFor("ImageMask"))
    m_bImageMask = true;

  if (m_bImageMask || !m_pDict->KeyExist("ColorSpace")) {
    if (!m_bImageMask) {
      const CPDF_Object* pFilter = m_pDict->GetDirectObjectFor("Filter");
      if (pFilter) {
        ByteString filter;
        if (pFilter->IsName()) {
          filter = pFilter->GetString();
        } else if (const CPDF_Array* pArray = pFilter->AsArray()) {
          filter = pArray->GetStringAt(pArray->size() - 1);
        }

        if (filter == "JPXDecode") {
          m_bDoBpcCheck = false;
          return true;
        }
      }
    }
    m_bImageMask = true;
    m_bpc = m_nComponents = 1;
    const CPDF_Array* pDecode = m_pDict->GetArrayFor("Decode");
    m_bDefaultDecode = !pDecode || !pDecode->GetIntegerAt(0);
    return true;
  }

  const CPDF_Object* pCSObj = m_pDict->GetDirectObjectFor("ColorSpace");
  if (!pCSObj)
    return false;

  CPDF_DocPageData* pDocPageData = m_pDocument->GetPageData();
  if (pFormResources)
    m_pColorSpace = pDocPageData->GetColorSpace(pCSObj, pFormResources);
  if (!m_pColorSpace)
    m_pColorSpace = pDocPageData->GetColorSpace(pCSObj, pPageResources);
  if (!m_pColorSpace)
    return false;

  m_Family = m_pColorSpace->GetFamily();
  m_nComponents = m_pColorSpace->CountComponents();
  if (m_Family == PDFCS_ICCBASED && pCSObj->IsName()) {
    ByteString cs = pCSObj->GetString();
    if (cs == "DeviceGray")
      m_nComponents = 1;
    else if (cs == "DeviceRGB")
      m_nComponents = 3;
    else if (cs == "DeviceCMYK")
      m_nComponents = 4;
  }
  ValidateDictParam();
  return GetDecodeAndMaskArray(&m_bDefaultDecode, &m_bColorKey);
}

bool CPDF_DIBBase::GetDecodeAndMaskArray(bool* bDefaultDecode,
                                         bool* bColorKey) {
  if (!m_pColorSpace)
    return false;

  m_CompData.resize(m_nComponents);
  int max_data = (1 << m_bpc) - 1;
  const CPDF_Array* pDecode = m_pDict->GetArrayFor("Decode");
  if (pDecode) {
    for (uint32_t i = 0; i < m_nComponents; i++) {
      m_CompData[i].m_DecodeMin = pDecode->GetNumberAt(i * 2);
      float max = pDecode->GetNumberAt(i * 2 + 1);
      m_CompData[i].m_DecodeStep = (max - m_CompData[i].m_DecodeMin) / max_data;
      float def_value;
      float def_min;
      float def_max;
      m_pColorSpace->GetDefaultValue(i, &def_value, &def_min, &def_max);
      if (m_Family == PDFCS_INDEXED)
        def_max = max_data;
      if (def_min != m_CompData[i].m_DecodeMin || def_max != max)
        *bDefaultDecode = false;
    }
  } else {
    for (uint32_t i = 0; i < m_nComponents; i++) {
      float def_value;
      m_pColorSpace->GetDefaultValue(i, &def_value, &m_CompData[i].m_DecodeMin,
                                     &m_CompData[i].m_DecodeStep);
      if (m_Family == PDFCS_INDEXED)
        m_CompData[i].m_DecodeStep = max_data;
      m_CompData[i].m_DecodeStep =
          (m_CompData[i].m_DecodeStep - m_CompData[i].m_DecodeMin) / max_data;
    }
  }
  if (m_pDict->KeyExist("SMask"))
    return true;

  const CPDF_Object* pMask = m_pDict->GetDirectObjectFor("Mask");
  if (!pMask)
    return true;

  if (const CPDF_Array* pArray = pMask->AsArray()) {
    if (pArray->size() >= m_nComponents * 2) {
      for (uint32_t i = 0; i < m_nComponents; i++) {
        int min_num = pArray->GetIntegerAt(i * 2);
        int max_num = pArray->GetIntegerAt(i * 2 + 1);
        m_CompData[i].m_ColorKeyMin = std::max(min_num, 0);
        m_CompData[i].m_ColorKeyMax = std::min(max_num, max_data);
      }
    }
    *bColorKey = true;
  }
  return true;
}

CPDF_DIBBase::LoadState CPDF_DIBBase::CreateDecoder() {
  ByteString decoder = m_pStreamAcc->GetImageDecoder();
  if (decoder.IsEmpty())
    return LoadState::kSuccess;

  if (m_bDoBpcCheck && m_bpc == 0)
    return LoadState::kFail;

  if (decoder == "JPXDecode") {
    m_pCachedBitmap = LoadJpxBitmap();
    return m_pCachedBitmap ? LoadState::kSuccess : LoadState::kFail;
  }

  if (decoder == "JBIG2Decode") {
    m_pCachedBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
    if (!m_pCachedBitmap->Create(
            m_Width, m_Height, m_bImageMask ? FXDIB_1bppMask : FXDIB_1bppRgb)) {
      m_pCachedBitmap.Reset();
      return LoadState::kFail;
    }
    m_Status = LoadState::kSuccess;
    return LoadState::kContinue;
  }

  pdfium::span<const uint8_t> src_span = m_pStreamAcc->GetSpan();
  const CPDF_Dictionary* pParams = m_pStreamAcc->GetImageParam();
  if (decoder == "CCITTFaxDecode") {
    m_pDecoder = CreateFaxDecoder(src_span, m_Width, m_Height, pParams);
  } else if (decoder == "FlateDecode") {
    m_pDecoder = CreateFlateDecoder(src_span, m_Width, m_Height, m_nComponents,
                                    m_bpc, pParams);
  } else if (decoder == "RunLengthDecode") {
    CCodec_ModuleMgr* pEncoders = CPDF_ModuleMgr::Get()->GetCodecModule();
    m_pDecoder = pEncoders->GetBasicModule()->CreateRunLengthDecoder(
        src_span, m_Width, m_Height, m_nComponents, m_bpc);
  } else if (decoder == "DCTDecode") {
    if (!CreateDCTDecoder(src_span, pParams))
      return LoadState::kFail;
  }
  if (!m_pDecoder)
    return LoadState::kFail;

  FX_SAFE_UINT32 requested_pitch =
      CalculatePitch8(m_bpc, m_nComponents, m_Width);
  if (!requested_pitch.IsValid())
    return LoadState::kFail;
  FX_SAFE_UINT32 provided_pitch = CalculatePitch8(
      m_pDecoder->GetBPC(), m_pDecoder->CountComps(), m_pDecoder->GetWidth());
  if (!provided_pitch.IsValid())
    return LoadState::kFail;
  if (provided_pitch.ValueOrDie() < requested_pitch.ValueOrDie())
    return LoadState::kFail;
  return LoadState::kSuccess;
}

bool CPDF_DIBBase::CreateDCTDecoder(pdfium::span<const uint8_t> src_span,
                                    const CPDF_Dictionary* pParams) {
  CCodec_JpegModule* pJpegModule = CPDF_ModuleMgr::Get()->GetJpegModule();
  m_pDecoder = pJpegModule->CreateDecoder(
      src_span, m_Width, m_Height, m_nComponents,
      !pParams || pParams->GetIntegerFor("ColorTransform", 1));
  if (m_pDecoder)
    return true;

  bool bTransform = false;
  int comps;
  int bpc;
  if (!pJpegModule->LoadInfo(src_span, &m_Width, &m_Height, &comps, &bpc,
                             &bTransform)) {
    return false;
  }
  if (!CPDF_Image::IsValidJpegComponent(comps) ||
      !CPDF_Image::IsValidJpegBitsPerComponent(bpc)) {
    return false;
  }

  if (m_nComponents == static_cast<uint32_t>(comps)) {
    m_bpc = bpc;
    m_pDecoder = pJpegModule->CreateDecoder(src_span, m_Width, m_Height,
                                            m_nComponents, bTransform);
    return true;
  }

  m_nComponents = static_cast<uint32_t>(comps);
  m_CompData.clear();
  if (m_pColorSpace) {
    switch (m_Family) {
      case PDFCS_DEVICEGRAY:
      case PDFCS_DEVICERGB:
      case PDFCS_DEVICECMYK: {
        uint32_t dwMinComps = ComponentsForFamily(m_Family);
        if (m_pColorSpace->CountComponents() < dwMinComps ||
            m_nComponents < dwMinComps) {
          return false;
        }
        break;
      }
      case PDFCS_LAB: {
        if (m_nComponents != 3 || m_pColorSpace->CountComponents() < 3)
          return false;
        break;
      }
      case PDFCS_ICCBASED: {
        if (!IsAllowedICCComponents(m_nComponents) ||
            !IsAllowedICCComponents(m_pColorSpace->CountComponents()) ||
            m_pColorSpace->CountComponents() < m_nComponents) {
          return false;
        }
        break;
      }
      default: {
        if (m_pColorSpace->CountComponents() != m_nComponents)
          return false;
        break;
      }
    }
  } else {
    if (m_Family == PDFCS_LAB && m_nComponents != 3)
      return false;
  }
  if (!GetDecodeAndMaskArray(&m_bDefaultDecode, &m_bColorKey))
    return false;

  m_bpc = bpc;
  m_pDecoder = pJpegModule->CreateDecoder(src_span, m_Width, m_Height,
                                          m_nComponents, bTransform);
  return true;
}

RetainPtr<CFX_DIBitmap> CPDF_DIBBase::LoadJpxBitmap() {
  CCodec_JpxModule* pJpxModule = CPDF_ModuleMgr::Get()->GetJpxModule();
  auto context = pdfium::MakeUnique<JpxBitMapContext>(pJpxModule);
  context->set_decoder(
      pJpxModule->CreateDecoder(m_pStreamAcc->GetSpan(), m_pColorSpace.Get()));
  if (!context->decoder())
    return nullptr;

  if (!context->decoder()->StartDecode())
    return nullptr;

  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t components = 0;
  pJpxModule->GetImageInfo(context->decoder(), &width, &height, &components);
  if (static_cast<int>(width) < m_Width || static_cast<int>(height) < m_Height)
    return nullptr;

  bool bSwapRGB = false;
  if (m_pColorSpace) {
    if (components != m_pColorSpace->CountComponents())
      return nullptr;

    if (m_pColorSpace == CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB)) {
      bSwapRGB = true;
      m_pColorSpace = nullptr;
    }
  } else {
    if (components == 3) {
      bSwapRGB = true;
    } else if (components == 4) {
      m_pColorSpace = CPDF_ColorSpace::GetStockCS(PDFCS_DEVICECMYK);
    }
    m_nComponents = components;
  }

  FXDIB_Format format;
  if (components == 1) {
    format = FXDIB_8bppRgb;
  } else if (components <= 3) {
    format = FXDIB_Rgb;
  } else if (components == 4) {
    format = FXDIB_Rgb32;
  } else {
    width = (width * components + 2) / 3;
    format = FXDIB_Rgb;
  }

  auto pCachedBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  if (!pCachedBitmap->Create(width, height, format))
    return nullptr;

  pCachedBitmap->Clear(0xFFFFFFFF);
  std::vector<uint8_t> output_offsets(components);
  for (uint32_t i = 0; i < components; ++i)
    output_offsets[i] = i;
  if (bSwapRGB) {
    output_offsets[0] = 2;
    output_offsets[2] = 0;
  }
  if (!pJpxModule->Decode(context->decoder(), pCachedBitmap->GetBuffer(),
                          pCachedBitmap->GetPitch(), output_offsets)) {
    return nullptr;
  }

  if (m_pColorSpace && m_pColorSpace->GetFamily() == PDFCS_INDEXED &&
      m_bpc < 8) {
    int scale = 8 - m_bpc;
    for (uint32_t row = 0; row < height; ++row) {
      uint8_t* scanline = pCachedBitmap->GetWritableScanline(row);
      for (uint32_t col = 0; col < width; ++col) {
        *scanline = (*scanline) >> scale;
        ++scanline;
      }
    }
  }
  m_bpc = 8;
  return pCachedBitmap;
}

CPDF_DIBBase::LoadState CPDF_DIBBase::StartLoadMask() {
  m_MatteColor = 0XFFFFFFFF;
  m_pMaskStream = m_pDict->GetStreamFor("SMask");
  if (!m_pMaskStream) {
    m_pMaskStream = ToStream(m_pDict->GetDirectObjectFor("Mask"));
    return m_pMaskStream ? StartLoadMaskDIB() : LoadState::kSuccess;
  }

  const CPDF_Array* pMatte = m_pMaskStream->GetDict()->GetArrayFor("Matte");
  if (pMatte && m_pColorSpace && m_Family != PDFCS_PATTERN &&
      m_pColorSpace->CountComponents() <= m_nComponents) {
    std::vector<float> colors(m_nComponents);
    for (uint32_t i = 0; i < m_nComponents; i++)
      colors[i] = pMatte->GetFloatAt(i);

    float R;
    float G;
    float B;
    m_pColorSpace->GetRGB(colors.data(), &R, &G, &B);
    m_MatteColor = ArgbEncode(0, FXSYS_round(R * 255), FXSYS_round(G * 255),
                              FXSYS_round(B * 255));
  }
  return StartLoadMaskDIB();
}

CPDF_DIBBase::LoadState CPDF_DIBBase::ContinueLoadMaskDIB(
    PauseIndicatorIface* pPause) {
  if (!m_pMask)
    return LoadState::kSuccess;

  LoadState ret = m_pMask->ContinueLoadDIBBase(pPause);
  if (ret == LoadState::kContinue)
    return LoadState::kContinue;

  if (m_pColorSpace && m_bStdCS)
    m_pColorSpace->EnableStdConversion(false);

  if (ret == LoadState::kFail) {
    m_pMask.Reset();
    return LoadState::kFail;
  }
  return LoadState::kSuccess;
}

RetainPtr<CPDF_DIBBase> CPDF_DIBBase::DetachMask() {
  return std::move(m_pMask);
}

bool CPDF_DIBBase::IsJBigImage() const {
  return m_pStreamAcc->GetImageDecoder() == "JBIG2Decode";
}

CPDF_DIBBase::LoadState CPDF_DIBBase::StartLoadMaskDIB() {
  m_pMask = pdfium::MakeRetain<CPDF_DIBBase>();
  LoadState ret =
      m_pMask->StartLoadDIBBase(m_pDocument.Get(), m_pMaskStream.Get(), false,
                                nullptr, nullptr, true, 0, false);
  if (ret == LoadState::kContinue) {
    if (m_Status == LoadState::kFail)
      m_Status = LoadState::kContinue;
    return LoadState::kContinue;
  }
  if (ret == LoadState::kFail)
    m_pMask.Reset();
  return LoadState::kSuccess;
}

void CPDF_DIBBase::LoadPalette() {
  if (!m_pColorSpace || m_Family == PDFCS_PATTERN)
    return;

  if (m_bpc == 0)
    return;

  // Use FX_SAFE_UINT32 just to be on the safe side, in case |m_bpc| or
  // |m_nComponents| somehow gets a bad value.
  FX_SAFE_UINT32 safe_bits = m_bpc;
  safe_bits *= m_nComponents;
  uint32_t bits = safe_bits.ValueOrDefault(255);
  if (bits > 8)
    return;

  if (bits == 1) {
    if (m_bDefaultDecode &&
        (m_Family == PDFCS_DEVICEGRAY || m_Family == PDFCS_DEVICERGB)) {
      return;
    }
    if (m_pColorSpace->CountComponents() > 3) {
      return;
    }
    float color_values[3];
    color_values[0] = m_CompData[0].m_DecodeMin;
    color_values[1] = color_values[0];
    color_values[2] = color_values[0];

    float R = 0.0f;
    float G = 0.0f;
    float B = 0.0f;
    m_pColorSpace->GetRGB(color_values, &R, &G, &B);

    FX_ARGB argb0 = ArgbEncode(255, FXSYS_round(R * 255), FXSYS_round(G * 255),
                               FXSYS_round(B * 255));
    color_values[0] += m_CompData[0].m_DecodeStep;
    color_values[1] += m_CompData[0].m_DecodeStep;
    color_values[2] += m_CompData[0].m_DecodeStep;
    m_pColorSpace->GetRGB(color_values, &R, &G, &B);
    FX_ARGB argb1 = ArgbEncode(255, FXSYS_round(R * 255), FXSYS_round(G * 255),
                               FXSYS_round(B * 255));
    if (argb0 != 0xFF000000 || argb1 != 0xFFFFFFFF) {
      SetPaletteArgb(0, argb0);
      SetPaletteArgb(1, argb1);
    }
    return;
  }
  if (m_bpc == 8 && m_bDefaultDecode &&
      m_pColorSpace == CPDF_ColorSpace::GetStockCS(PDFCS_DEVICEGRAY)) {
    return;
  }

  int palette_count = 1 << bits;
  // Using at least 16 elements due to the call m_pColorSpace->GetRGB().
  std::vector<float> color_values(std::max(m_nComponents, 16u));
  for (int i = 0; i < palette_count; i++) {
    int color_data = i;
    for (uint32_t j = 0; j < m_nComponents; j++) {
      int encoded_component = color_data % (1 << m_bpc);
      color_data /= 1 << m_bpc;
      color_values[j] = m_CompData[j].m_DecodeMin +
                        m_CompData[j].m_DecodeStep * encoded_component;
    }
    float R = 0;
    float G = 0;
    float B = 0;
    if (m_nComponents == 1 && m_Family == PDFCS_ICCBASED &&
        m_pColorSpace->CountComponents() > 1) {
      int nComponents = m_pColorSpace->CountComponents();
      std::vector<float> temp_buf(nComponents);
      for (int k = 0; k < nComponents; ++k)
        temp_buf[k] = color_values[0];
      m_pColorSpace->GetRGB(temp_buf.data(), &R, &G, &B);
    } else {
      m_pColorSpace->GetRGB(color_values.data(), &R, &G, &B);
    }
    SetPaletteArgb(i, ArgbEncode(255, FXSYS_round(R * 255),
                                 FXSYS_round(G * 255), FXSYS_round(B * 255)));
  }
}

void CPDF_DIBBase::ValidateDictParam() {
  m_bpc = m_bpc_orig;
  const CPDF_Object* pFilter = m_pDict->GetDirectObjectFor("Filter");
  if (pFilter) {
    if (pFilter->IsName()) {
      ByteString filter = pFilter->GetString();
      if (filter == "CCITTFaxDecode" || filter == "JBIG2Decode") {
        m_bpc = 1;
        m_nComponents = 1;
      } else if (filter == "RunLengthDecode") {
        if (m_bpc != 1) {
          m_bpc = 8;
        }
      } else if (filter == "DCTDecode") {
        m_bpc = 8;
      }
    } else if (const CPDF_Array* pArray = pFilter->AsArray()) {
      ByteString filter = pArray->GetStringAt(pArray->size() - 1);
      if (filter == "CCITTFaxDecode" || filter == "JBIG2Decode") {
        m_bpc = 1;
        m_nComponents = 1;
      } else if (filter == "DCTDecode") {
        // Previously, filter == "RunLengthDecode" was checked in the "if"
        // statement as well, but too many documents don't conform to it.
        m_bpc = 8;
      }
    }
  }

  if (!IsAllowedBitsPerComponent(m_bpc))
    m_bpc = 0;
}

void CPDF_DIBBase::TranslateScanline24bpp(uint8_t* dest_scan,
                                          const uint8_t* src_scan) const {
  if (m_bpc == 0)
    return;

  if (TranslateScanline24bppDefaultDecode(dest_scan, src_scan))
    return;

  // Using at least 16 elements due to the call m_pColorSpace->GetRGB().
  std::vector<float> color_values(std::max(m_nComponents, 16u));
  float R = 0.0f;
  float G = 0.0f;
  float B = 0.0f;
  uint64_t src_bit_pos = 0;
  uint64_t src_byte_pos = 0;
  size_t dest_byte_pos = 0;
  const bool bpp8 = m_bpc == 8;
  for (int column = 0; column < m_Width; column++) {
    for (uint32_t color = 0; color < m_nComponents; color++) {
      if (bpp8) {
        uint8_t data = src_scan[src_byte_pos++];
        color_values[color] = m_CompData[color].m_DecodeMin +
                              m_CompData[color].m_DecodeStep * data;
      } else {
        unsigned int data = GetBits8(src_scan, src_bit_pos, m_bpc);
        color_values[color] = m_CompData[color].m_DecodeMin +
                              m_CompData[color].m_DecodeStep * data;
        src_bit_pos += m_bpc;
      }
    }

    if (TransMask()) {
      float k = 1.0f - color_values[3];
      R = (1.0f - color_values[0]) * k;
      G = (1.0f - color_values[1]) * k;
      B = (1.0f - color_values[2]) * k;
    } else if (m_Family != PDFCS_PATTERN) {
      m_pColorSpace->GetRGB(color_values.data(), &R, &G, &B);
    }
    R = pdfium::clamp(R, 0.0f, 1.0f);
    G = pdfium::clamp(G, 0.0f, 1.0f);
    B = pdfium::clamp(B, 0.0f, 1.0f);
    dest_scan[dest_byte_pos] = static_cast<uint8_t>(B * 255);
    dest_scan[dest_byte_pos + 1] = static_cast<uint8_t>(G * 255);
    dest_scan[dest_byte_pos + 2] = static_cast<uint8_t>(R * 255);
    dest_byte_pos += 3;
  }
}

bool CPDF_DIBBase::TranslateScanline24bppDefaultDecode(
    uint8_t* dest_scan,
    const uint8_t* src_scan) const {
  if (!m_bDefaultDecode)
    return false;

  if (m_Family != PDFCS_DEVICERGB && m_Family != PDFCS_CALRGB) {
    if (m_bpc != 8)
      return false;

    if (m_nComponents == m_pColorSpace->CountComponents()) {
      m_pColorSpace->TranslateImageLine(dest_scan, src_scan, m_Width, m_Width,
                                        m_Height, TransMask());
    }
    return true;
  }

  if (m_nComponents != 3)
    return true;

  const uint8_t* src_pos = src_scan;
  switch (m_bpc) {
    case 8:
      for (int column = 0; column < m_Width; column++) {
        *dest_scan++ = src_pos[2];
        *dest_scan++ = src_pos[1];
        *dest_scan++ = *src_pos;
        src_pos += 3;
      }
      break;
    case 16:
      for (int col = 0; col < m_Width; col++) {
        *dest_scan++ = src_pos[4];
        *dest_scan++ = src_pos[2];
        *dest_scan++ = *src_pos;
        src_pos += 6;
      }
      break;
    default:
      const unsigned int max_data = (1 << m_bpc) - 1;
      uint64_t src_bit_pos = 0;
      size_t dest_byte_pos = 0;
      for (int column = 0; column < m_Width; column++) {
        unsigned int R = GetBits8(src_scan, src_bit_pos, m_bpc);
        src_bit_pos += m_bpc;
        unsigned int G = GetBits8(src_scan, src_bit_pos, m_bpc);
        src_bit_pos += m_bpc;
        unsigned int B = GetBits8(src_scan, src_bit_pos, m_bpc);
        src_bit_pos += m_bpc;
        R = std::min(R, max_data);
        G = std::min(G, max_data);
        B = std::min(B, max_data);
        dest_scan[dest_byte_pos] = B * 255 / max_data;
        dest_scan[dest_byte_pos + 1] = G * 255 / max_data;
        dest_scan[dest_byte_pos + 2] = R * 255 / max_data;
        dest_byte_pos += 3;
      }
      break;
  }
  return true;
}

uint8_t* CPDF_DIBBase::GetBuffer() const {
  return m_pCachedBitmap ? m_pCachedBitmap->GetBuffer() : nullptr;
}

const uint8_t* CPDF_DIBBase::GetScanline(int line) const {
  if (m_bpc == 0)
    return nullptr;

  FX_SAFE_UINT32 src_pitch = CalculatePitch8(m_bpc, m_nComponents, m_Width);
  if (!src_pitch.IsValid())
    return nullptr;
  uint32_t src_pitch_value = src_pitch.ValueOrDie();

  const uint8_t* pSrcLine = nullptr;
  if (m_pCachedBitmap && src_pitch_value <= m_pCachedBitmap->GetPitch()) {
    if (line >= m_pCachedBitmap->GetHeight()) {
      line = m_pCachedBitmap->GetHeight() - 1;
    }
    pSrcLine = m_pCachedBitmap->GetScanline(line);
  } else if (m_pDecoder) {
    pSrcLine = m_pDecoder->GetScanline(line);
  } else if (m_pStreamAcc->GetSize() >= (line + 1) * src_pitch_value) {
    pSrcLine = m_pStreamAcc->GetData() + line * src_pitch_value;
  }
  if (!pSrcLine) {
    uint8_t* pLineBuf = m_pMaskedLine ? m_pMaskedLine.get() : m_pLineBuf.get();
    memset(pLineBuf, 0xFF, m_Pitch);
    return pLineBuf;
  }

  if (m_bpc * m_nComponents == 1) {
    if (m_bImageMask && m_bDefaultDecode) {
      for (uint32_t i = 0; i < src_pitch_value; i++)
        m_pLineBuf.get()[i] = ~pSrcLine[i];
      return m_pLineBuf.get();
    }

    if (!m_bColorKey) {
      memcpy(m_pLineBuf.get(), pSrcLine, src_pitch_value);
      return m_pLineBuf.get();
    }

    uint32_t reset_argb = m_pPalette ? m_pPalette.get()[0] : 0xFF000000;
    uint32_t set_argb = m_pPalette ? m_pPalette.get()[1] : 0xFFFFFFFF;
    if (m_CompData[0].m_ColorKeyMin == 0)
      reset_argb = 0;
    if (m_CompData[0].m_ColorKeyMax == 1)
      set_argb = 0;
    set_argb = FXARGB_TODIB(set_argb);
    reset_argb = FXARGB_TODIB(reset_argb);
    uint32_t* dest_scan = reinterpret_cast<uint32_t*>(m_pMaskedLine.get());
    for (int col = 0; col < m_Width; col++) {
      *dest_scan = GetBitValue(pSrcLine, col) ? set_argb : reset_argb;
      dest_scan++;
    }
    return m_pMaskedLine.get();
  }
  if (m_bpc * m_nComponents <= 8) {
    if (m_bpc == 8) {
      memcpy(m_pLineBuf.get(), pSrcLine, src_pitch_value);
    } else {
      uint64_t src_bit_pos = 0;
      for (int col = 0; col < m_Width; col++) {
        unsigned int color_index = 0;
        for (uint32_t color = 0; color < m_nComponents; color++) {
          unsigned int data = GetBits8(pSrcLine, src_bit_pos, m_bpc);
          color_index |= data << (color * m_bpc);
          src_bit_pos += m_bpc;
        }
        m_pLineBuf.get()[col] = color_index;
      }
    }
    if (!m_bColorKey)
      return m_pLineBuf.get();

    uint8_t* pDestPixel = m_pMaskedLine.get();
    const uint8_t* pSrcPixel = m_pLineBuf.get();
    for (int col = 0; col < m_Width; col++) {
      uint8_t index = *pSrcPixel++;
      if (m_pPalette) {
        *pDestPixel++ = FXARGB_B(m_pPalette.get()[index]);
        *pDestPixel++ = FXARGB_G(m_pPalette.get()[index]);
        *pDestPixel++ = FXARGB_R(m_pPalette.get()[index]);
      } else {
        *pDestPixel++ = index;
        *pDestPixel++ = index;
        *pDestPixel++ = index;
      }
      *pDestPixel = IsColorIndexOutOfBounds(index, m_CompData[0]) ? 0xFF : 0;
      pDestPixel++;
    }
    return m_pMaskedLine.get();
  }
  if (m_bColorKey) {
    if (m_nComponents == 3 && m_bpc == 8) {
      uint8_t* alpha_channel = m_pMaskedLine.get() + 3;
      for (int col = 0; col < m_Width; col++) {
        const uint8_t* pPixel = pSrcLine + col * 3;
        alpha_channel[col * 4] =
            AreColorIndicesOutOfBounds(pPixel, m_CompData.data(), 3) ? 0xFF : 0;
      }
    } else {
      memset(m_pMaskedLine.get(), 0xFF, m_Pitch);
    }
  }
  if (m_pColorSpace) {
    TranslateScanline24bpp(m_pLineBuf.get(), pSrcLine);
    pSrcLine = m_pLineBuf.get();
  }
  if (!m_bColorKey)
    return pSrcLine;

  const uint8_t* pSrcPixel = pSrcLine;
  uint8_t* pDestPixel = m_pMaskedLine.get();
  for (int col = 0; col < m_Width; col++) {
    *pDestPixel++ = *pSrcPixel++;
    *pDestPixel++ = *pSrcPixel++;
    *pDestPixel++ = *pSrcPixel++;
    pDestPixel++;
  }
  return m_pMaskedLine.get();
}

bool CPDF_DIBBase::SkipToScanline(int line, PauseIndicatorIface* pPause) const {
  return m_pDecoder && m_pDecoder->SkipToScanline(line, pPause);
}

void CPDF_DIBBase::DownSampleScanline(int line,
                                      uint8_t* dest_scan,
                                      int dest_bpp,
                                      int dest_width,
                                      bool bFlipX,
                                      int clip_left,
                                      int clip_width) const {
  if (line < 0 || !dest_scan || dest_bpp <= 0 || dest_width <= 0 ||
      clip_left < 0 || clip_width <= 0) {
    return;
  }

  uint32_t src_width = m_Width;
  FX_SAFE_UINT32 pitch = CalculatePitch8(m_bpc, m_nComponents, m_Width);
  if (!pitch.IsValid())
    return;

  const uint8_t* pSrcLine = nullptr;
  if (m_pCachedBitmap) {
    pSrcLine = m_pCachedBitmap->GetScanline(line);
  } else if (m_pDecoder) {
    pSrcLine = m_pDecoder->GetScanline(line);
  } else {
    uint32_t src_pitch = pitch.ValueOrDie();
    pitch *= (line + 1);
    if (!pitch.IsValid()) {
      return;
    }

    if (m_pStreamAcc->GetSize() >= pitch.ValueOrDie()) {
      pSrcLine = m_pStreamAcc->GetData() + line * src_pitch;
    }
  }
  int orig_Bpp = m_bpc * m_nComponents / 8;
  int dest_Bpp = dest_bpp / 8;
  if (!pSrcLine) {
    memset(dest_scan, 0xFF, dest_Bpp * clip_width);
    return;
  }

  FX_SAFE_INT32 max_src_x = clip_left;
  max_src_x += clip_width - 1;
  max_src_x *= src_width;
  max_src_x /= dest_width;
  if (!max_src_x.IsValid())
    return;

  if (m_bpc * m_nComponents == 1) {
    DownSampleScanline1Bit(orig_Bpp, dest_Bpp, src_width, pSrcLine, dest_scan,
                           dest_width, bFlipX, clip_left, clip_width);
  } else if (m_bpc * m_nComponents <= 8) {
    DownSampleScanline8Bit(orig_Bpp, dest_Bpp, src_width, pSrcLine, dest_scan,
                           dest_width, bFlipX, clip_left, clip_width);
  } else {
    DownSampleScanline32Bit(orig_Bpp, dest_Bpp, src_width, pSrcLine, dest_scan,
                            dest_width, bFlipX, clip_left, clip_width);
  }
}

void CPDF_DIBBase::DownSampleScanline1Bit(int orig_Bpp,
                                          int dest_Bpp,
                                          uint32_t src_width,
                                          const uint8_t* pSrcLine,
                                          uint8_t* dest_scan,
                                          int dest_width,
                                          bool bFlipX,
                                          int clip_left,
                                          int clip_width) const {
  if (m_bColorKey && !m_bImageMask) {
    uint32_t reset_argb = m_pPalette ? m_pPalette.get()[0] : 0xFF000000;
    uint32_t set_argb = m_pPalette ? m_pPalette.get()[1] : 0xFFFFFFFF;
    if (m_CompData[0].m_ColorKeyMin == 0)
      reset_argb = 0;
    if (m_CompData[0].m_ColorKeyMax == 1)
      set_argb = 0;
    set_argb = FXARGB_TODIB(set_argb);
    reset_argb = FXARGB_TODIB(reset_argb);
    uint32_t* dest_scan_dword = reinterpret_cast<uint32_t*>(dest_scan);
    for (int i = 0; i < clip_width; i++) {
      uint32_t src_x = (clip_left + i) * src_width / dest_width;
      if (bFlipX)
        src_x = src_width - src_x - 1;
      src_x %= src_width;
      dest_scan_dword[i] = GetBitValue(pSrcLine, src_x) ? set_argb : reset_argb;
    }
    return;
  }

  uint32_t set_argb = 0xFFFFFFFF;
  uint32_t reset_argb = 0;
  if (m_bImageMask) {
    if (m_bDefaultDecode) {
      set_argb = 0;
      reset_argb = 0xFFFFFFFF;
    }
  } else if (m_pPalette && dest_Bpp != 1) {
    reset_argb = m_pPalette.get()[0];
    set_argb = m_pPalette.get()[1];
  }
  for (int i = 0; i < clip_width; i++) {
    uint32_t src_x = (clip_left + i) * src_width / dest_width;
    if (bFlipX)
      src_x = src_width - src_x - 1;
    src_x %= src_width;
    int dest_pos = i * dest_Bpp;
    uint32_t value_argb = GetBitValue(pSrcLine, src_x) ? set_argb : reset_argb;
    if (dest_Bpp == 1) {
      dest_scan[dest_pos] = static_cast<uint8_t>(value_argb);
    } else if (dest_Bpp == 3) {
      dest_scan[dest_pos] = FXARGB_B(value_argb);
      dest_scan[dest_pos + 1] = FXARGB_G(value_argb);
      dest_scan[dest_pos + 2] = FXARGB_R(value_argb);
    } else {
      *reinterpret_cast<uint32_t*>(dest_scan + dest_pos) = value_argb;
    }
  }
}

void CPDF_DIBBase::DownSampleScanline8Bit(int orig_Bpp,
                                          int dest_Bpp,
                                          uint32_t src_width,
                                          const uint8_t* pSrcLine,
                                          uint8_t* dest_scan,
                                          int dest_width,
                                          bool bFlipX,
                                          int clip_left,
                                          int clip_width) const {
  if (m_bpc < 8) {
    uint64_t src_bit_pos = 0;
    for (uint32_t col = 0; col < src_width; col++) {
      unsigned int color_index = 0;
      for (uint32_t color = 0; color < m_nComponents; color++) {
        unsigned int data = GetBits8(pSrcLine, src_bit_pos, m_bpc);
        color_index |= data << (color * m_bpc);
        src_bit_pos += m_bpc;
      }
      m_pLineBuf.get()[col] = color_index;
    }
    pSrcLine = m_pLineBuf.get();
  }
  if (m_bColorKey) {
    for (int i = 0; i < clip_width; i++) {
      uint32_t src_x = (clip_left + i) * src_width / dest_width;
      if (bFlipX) {
        src_x = src_width - src_x - 1;
      }
      src_x %= src_width;
      uint8_t* pDestPixel = dest_scan + i * 4;
      uint8_t index = pSrcLine[src_x];
      if (m_pPalette) {
        *pDestPixel++ = FXARGB_B(m_pPalette.get()[index]);
        *pDestPixel++ = FXARGB_G(m_pPalette.get()[index]);
        *pDestPixel++ = FXARGB_R(m_pPalette.get()[index]);
      } else {
        *pDestPixel++ = index;
        *pDestPixel++ = index;
        *pDestPixel++ = index;
      }
      *pDestPixel = (index < m_CompData[0].m_ColorKeyMin ||
                     index > m_CompData[0].m_ColorKeyMax)
                        ? 0xFF
                        : 0;
    }
    return;
  }
  for (int i = 0; i < clip_width; i++) {
    uint32_t src_x = (clip_left + i) * src_width / dest_width;
    if (bFlipX)
      src_x = src_width - src_x - 1;
    src_x %= src_width;
    uint8_t index = pSrcLine[src_x];
    if (dest_Bpp == 1) {
      dest_scan[i] = index;
    } else {
      int dest_pos = i * dest_Bpp;
      FX_ARGB argb = m_pPalette.get()[index];
      dest_scan[dest_pos] = FXARGB_B(argb);
      dest_scan[dest_pos + 1] = FXARGB_G(argb);
      dest_scan[dest_pos + 2] = FXARGB_R(argb);
    }
  }
}

void CPDF_DIBBase::DownSampleScanline32Bit(int orig_Bpp,
                                           int dest_Bpp,
                                           uint32_t src_width,
                                           const uint8_t* pSrcLine,
                                           uint8_t* dest_scan,
                                           int dest_width,
                                           bool bFlipX,
                                           int clip_left,
                                           int clip_width) const {
  // last_src_x used to store the last seen src_x position which should be
  // in [0, src_width). Set the initial value to be an invalid src_x value.
  uint32_t last_src_x = src_width;
  FX_ARGB last_argb = ArgbEncode(0xFF, 0xFF, 0xFF, 0xFF);
  float unit_To8Bpc = 255.0f / ((1 << m_bpc) - 1);
  for (int i = 0; i < clip_width; i++) {
    int dest_x = clip_left + i;
    uint32_t src_x = (bFlipX ? (dest_width - dest_x - 1) : dest_x) *
                     (int64_t)src_width / dest_width;
    src_x %= src_width;

    uint8_t* pDestPixel = dest_scan + i * dest_Bpp;
    FX_ARGB argb;
    if (src_x == last_src_x) {
      argb = last_argb;
    } else {
      CFX_FixedBufGrow<uint8_t, 16> extracted_components(m_nComponents);
      const uint8_t* pSrcPixel = nullptr;
      if (m_bpc % 8 != 0) {
        // No need to check for 32-bit overflow, as |src_x| is bounded by
        // |src_width| and DownSampleScanline() already checked for overflow
        // with the pitch calculation.
        size_t num_bits = src_x * m_bpc * m_nComponents;
        uint64_t src_bit_pos = num_bits % 8;
        pSrcPixel = pSrcLine + num_bits / 8;
        for (uint32_t j = 0; j < m_nComponents; ++j) {
          extracted_components[j] = static_cast<uint8_t>(
              GetBits8(pSrcPixel, src_bit_pos, m_bpc) * unit_To8Bpc);
          src_bit_pos += m_bpc;
        }
        pSrcPixel = extracted_components;
      } else {
        pSrcPixel = pSrcLine + src_x * orig_Bpp;
        if (m_bpc == 16) {
          for (uint32_t j = 0; j < m_nComponents; ++j)
            extracted_components[j] = pSrcPixel[j * 2];
          pSrcPixel = extracted_components;
        }
      }

      if (m_pColorSpace) {
        uint8_t color[4];
        const bool bTransMask = TransMask();
        if (!m_bDefaultDecode) {
          for (uint32_t j = 0; j < m_nComponents; ++j) {
            float component_value = static_cast<float>(pSrcPixel[j]);
            int color_value = static_cast<int>(
                (m_CompData[j].m_DecodeMin +
                 m_CompData[j].m_DecodeStep * component_value) *
                    255.0f +
                0.5f);
            extracted_components[j] = pdfium::clamp(color_value, 0, 255);
          }
        }
        const uint8_t* pSrc =
            m_bDefaultDecode ? pSrcPixel : extracted_components;
        m_pColorSpace->TranslateImageLine(color, pSrc, 1, 0, 0, bTransMask);
        argb = ArgbEncode(0xFF, color[2], color[1], color[0]);
      } else {
        argb = ArgbEncode(0xFF, pSrcPixel[2], pSrcPixel[1], pSrcPixel[0]);
      }
      if (m_bColorKey) {
        int alpha = 0xFF;
        if (m_nComponents == 3 && m_bpc == 8) {
          alpha = (pSrcPixel[0] < m_CompData[0].m_ColorKeyMin ||
                   pSrcPixel[0] > m_CompData[0].m_ColorKeyMax ||
                   pSrcPixel[1] < m_CompData[1].m_ColorKeyMin ||
                   pSrcPixel[1] > m_CompData[1].m_ColorKeyMax ||
                   pSrcPixel[2] < m_CompData[2].m_ColorKeyMin ||
                   pSrcPixel[2] > m_CompData[2].m_ColorKeyMax)
                      ? 0xFF
                      : 0;
        }
        argb &= 0xFFFFFF;
        argb |= alpha << 24;
      }
      last_src_x = src_x;
      last_argb = argb;
    }
    if (dest_Bpp == 4) {
      *reinterpret_cast<uint32_t*>(pDestPixel) = FXARGB_TODIB(argb);
    } else {
      *pDestPixel++ = FXARGB_B(argb);
      *pDestPixel++ = FXARGB_G(argb);
      *pDestPixel = FXARGB_R(argb);
    }
  }
}

bool CPDF_DIBBase::TransMask() const {
  return m_bLoadMask && m_GroupFamily == PDFCS_DEVICECMYK &&
         m_Family == PDFCS_DEVICECMYK;
}
