// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_image.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "constants/stream_dict_common.h"
#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "core/fxcodec/codec/ccodec_jpegmodule.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_dib.h"
#include "third_party/base/numerics/safe_conversions.h"
#include "third_party/base/ptr_util.h"

// static
bool CPDF_Image::IsValidJpegComponent(int32_t comps) {
  return comps == 1 || comps == 3 || comps == 4;
}

// static
bool CPDF_Image::IsValidJpegBitsPerComponent(int32_t bpc) {
  return bpc == 1 || bpc == 2 || bpc == 4 || bpc == 8 || bpc == 16;
}

CPDF_Image::CPDF_Image(CPDF_Document* pDoc) : m_pDocument(pDoc) {
  ASSERT(m_pDocument);
}

CPDF_Image::CPDF_Image(CPDF_Document* pDoc,
                       std::unique_ptr<CPDF_Stream> pStream)
    : m_bIsInline(true), m_pDocument(pDoc), m_pStream(std::move(pStream)) {
  ASSERT(m_pDocument);
  ASSERT(m_pStream.IsOwned());
  FinishInitialization(m_pStream->GetDict());
}

CPDF_Image::CPDF_Image(CPDF_Document* pDoc, uint32_t dwStreamObjNum)
    : m_pDocument(pDoc),
      m_pStream(ToStream(pDoc->GetIndirectObject(dwStreamObjNum))) {
  ASSERT(m_pDocument);
  ASSERT(!m_pStream.IsOwned());
  FinishInitialization(m_pStream->GetDict());
}

CPDF_Image::~CPDF_Image() = default;

void CPDF_Image::FinishInitialization(CPDF_Dictionary* pStreamDict) {
  m_pOC = pStreamDict->GetDictFor("OC");
  m_bIsMask = !pStreamDict->KeyExist("ColorSpace") ||
              pStreamDict->GetIntegerFor("ImageMask");
  m_bInterpolate = !!pStreamDict->GetIntegerFor("Interpolate");
  m_Height = pStreamDict->GetIntegerFor("Height");
  m_Width = pStreamDict->GetIntegerFor("Width");
}

void CPDF_Image::ConvertStreamToIndirectObject() {
  if (!m_pStream->IsInline())
    return;

  ASSERT(m_pStream.IsOwned());
  m_pDocument->AddIndirectObject(m_pStream.Release());
}

CPDF_Dictionary* CPDF_Image::GetDict() const {
  return m_pStream ? m_pStream->GetDict() : nullptr;
}

std::unique_ptr<CPDF_Dictionary> CPDF_Image::InitJPEG(
    pdfium::span<uint8_t> src_span) {
  int32_t width;
  int32_t height;
  int32_t num_comps;
  int32_t bits;
  bool color_trans;
  if (!CPDF_ModuleMgr::Get()->GetJpegModule()->LoadInfo(
          src_span, &width, &height, &num_comps, &bits, &color_trans)) {
    return nullptr;
  }
  if (!IsValidJpegComponent(num_comps) || !IsValidJpegBitsPerComponent(bits))
    return nullptr;

  auto pDict = m_pDocument->New<CPDF_Dictionary>();
  pDict->SetNewFor<CPDF_Name>("Type", "XObject");
  pDict->SetNewFor<CPDF_Name>("Subtype", "Image");
  pDict->SetNewFor<CPDF_Number>("Width", width);
  pDict->SetNewFor<CPDF_Number>("Height", height);
  const char* csname = nullptr;
  if (num_comps == 1) {
    csname = "DeviceGray";
  } else if (num_comps == 3) {
    csname = "DeviceRGB";
  } else if (num_comps == 4) {
    csname = "DeviceCMYK";
    CPDF_Array* pDecode = pDict->SetNewFor<CPDF_Array>("Decode");
    for (int n = 0; n < 4; n++) {
      pDecode->AddNew<CPDF_Number>(1);
      pDecode->AddNew<CPDF_Number>(0);
    }
  }
  pDict->SetNewFor<CPDF_Name>("ColorSpace", csname);
  pDict->SetNewFor<CPDF_Number>("BitsPerComponent", bits);
  pDict->SetNewFor<CPDF_Name>("Filter", "DCTDecode");
  if (!color_trans) {
    CPDF_Dictionary* pParms =
        pDict->SetNewFor<CPDF_Dictionary>(pdfium::stream::kDecodeParms);
    pParms->SetNewFor<CPDF_Number>("ColorTransform", 0);
  }
  m_bIsMask = false;
  m_Width = width;
  m_Height = height;
  if (!m_pStream)
    m_pStream = pdfium::MakeUnique<CPDF_Stream>();
  return pDict;
}

void CPDF_Image::SetJpegImage(const RetainPtr<IFX_SeekableReadStream>& pFile) {
  uint32_t size = pdfium::base::checked_cast<uint32_t>(pFile->GetSize());
  if (!size)
    return;

  uint32_t dwEstimateSize = std::min(size, 8192U);
  std::vector<uint8_t> data(dwEstimateSize);
  if (!pFile->ReadBlockAtOffset(data.data(), 0, dwEstimateSize))
    return;

  std::unique_ptr<CPDF_Dictionary> pDict = InitJPEG(data);
  if (!pDict && size > dwEstimateSize) {
    data.resize(size);
    pFile->ReadBlockAtOffset(data.data(), 0, size);
    pDict = InitJPEG(data);
  }
  if (!pDict)
    return;

  m_pStream->InitStreamFromFile(pFile, std::move(pDict));
}

void CPDF_Image::SetJpegImageInline(
    const RetainPtr<IFX_SeekableReadStream>& pFile) {
  uint32_t size = pdfium::base::checked_cast<uint32_t>(pFile->GetSize());
  if (!size)
    return;

  std::vector<uint8_t> data(size);
  if (!pFile->ReadBlockAtOffset(data.data(), 0, size))
    return;

  std::unique_ptr<CPDF_Dictionary> pDict = InitJPEG(data);
  if (!pDict)
    return;

  m_pStream->InitStream(data, std::move(pDict));
}

void CPDF_Image::SetImage(const RetainPtr<CFX_DIBitmap>& pBitmap) {
  int32_t BitmapWidth = pBitmap->GetWidth();
  int32_t BitmapHeight = pBitmap->GetHeight();
  if (BitmapWidth < 1 || BitmapHeight < 1)
    return;

  auto pDict = m_pDocument->New<CPDF_Dictionary>();
  pDict->SetNewFor<CPDF_Name>("Type", "XObject");
  pDict->SetNewFor<CPDF_Name>("Subtype", "Image");
  pDict->SetNewFor<CPDF_Number>("Width", BitmapWidth);
  pDict->SetNewFor<CPDF_Number>("Height", BitmapHeight);

  const int32_t bpp = pBitmap->GetBPP();
  size_t dest_pitch = 0;
  bool bCopyWithoutAlpha = true;
  if (bpp == 1) {
    int32_t reset_a = 0;
    int32_t reset_r = 0;
    int32_t reset_g = 0;
    int32_t reset_b = 0;
    int32_t set_a = 0;
    int32_t set_r = 0;
    int32_t set_g = 0;
    int32_t set_b = 0;
    if (!pBitmap->IsAlphaMask()) {
      std::tie(reset_a, reset_r, reset_g, reset_b) =
          ArgbDecode(pBitmap->GetPaletteArgb(0));
      std::tie(set_a, set_r, set_g, set_b) =
          ArgbDecode(pBitmap->GetPaletteArgb(1));
    }
    if (set_a == 0 || reset_a == 0) {
      pDict->SetNewFor<CPDF_Boolean>("ImageMask", true);
      if (reset_a == 0) {
        CPDF_Array* pArray = pDict->SetNewFor<CPDF_Array>("Decode");
        pArray->AddNew<CPDF_Number>(1);
        pArray->AddNew<CPDF_Number>(0);
      }
    } else {
      CPDF_Array* pCS = pDict->SetNewFor<CPDF_Array>("ColorSpace");
      pCS->AddNew<CPDF_Name>("Indexed");
      pCS->AddNew<CPDF_Name>("DeviceRGB");
      pCS->AddNew<CPDF_Number>(1);
      ByteString ct;
      {
        // Span's lifetime must end before ReleaseBuffer() below.
        pdfium::span<char> pBuf = ct.GetBuffer(6);
        pBuf[0] = static_cast<char>(reset_r);
        pBuf[1] = static_cast<char>(reset_g);
        pBuf[2] = static_cast<char>(reset_b);
        pBuf[3] = static_cast<char>(set_r);
        pBuf[4] = static_cast<char>(set_g);
        pBuf[5] = static_cast<char>(set_b);
      }
      ct.ReleaseBuffer(6);
      pCS->AddNew<CPDF_String>(ct, true);
    }
    pDict->SetNewFor<CPDF_Number>("BitsPerComponent", 1);
    dest_pitch = (BitmapWidth + 7) / 8;
  } else if (bpp == 8) {
    int32_t iPalette = pBitmap->GetPaletteSize();
    if (iPalette > 0) {
      CPDF_Array* pCS = m_pDocument->NewIndirect<CPDF_Array>();
      pCS->AddNew<CPDF_Name>("Indexed");
      pCS->AddNew<CPDF_Name>("DeviceRGB");
      pCS->AddNew<CPDF_Number>(iPalette - 1);
      std::unique_ptr<uint8_t, FxFreeDeleter> pColorTable(
          FX_Alloc2D(uint8_t, iPalette, 3));
      uint8_t* ptr = pColorTable.get();
      for (int32_t i = 0; i < iPalette; i++) {
        uint32_t argb = pBitmap->GetPaletteArgb(i);
        ptr[0] = (uint8_t)(argb >> 16);
        ptr[1] = (uint8_t)(argb >> 8);
        ptr[2] = (uint8_t)argb;
        ptr += 3;
      }
      auto pNewDict = m_pDocument->New<CPDF_Dictionary>();
      CPDF_Stream* pCTS = m_pDocument->NewIndirect<CPDF_Stream>(
          std::move(pColorTable), iPalette * 3, std::move(pNewDict));
      pCS->Add(pCTS->MakeReference(m_pDocument.Get()));
      pDict->SetFor("ColorSpace", pCS->MakeReference(m_pDocument.Get()));
    } else {
      pDict->SetNewFor<CPDF_Name>("ColorSpace", "DeviceGray");
    }
    pDict->SetNewFor<CPDF_Number>("BitsPerComponent", 8);
    dest_pitch = BitmapWidth;
  } else {
    pDict->SetNewFor<CPDF_Name>("ColorSpace", "DeviceRGB");
    pDict->SetNewFor<CPDF_Number>("BitsPerComponent", 8);
    dest_pitch = BitmapWidth * 3;
    bCopyWithoutAlpha = false;
  }

  RetainPtr<CFX_DIBitmap> pMaskBitmap;
  if (pBitmap->HasAlpha())
    pMaskBitmap = pBitmap->CloneAlphaMask();

  if (pMaskBitmap) {
    int32_t maskWidth = pMaskBitmap->GetWidth();
    int32_t maskHeight = pMaskBitmap->GetHeight();
    std::unique_ptr<uint8_t, FxFreeDeleter> mask_buf;
    int32_t mask_size = 0;
    auto pMaskDict = m_pDocument->New<CPDF_Dictionary>();
    pMaskDict->SetNewFor<CPDF_Name>("Type", "XObject");
    pMaskDict->SetNewFor<CPDF_Name>("Subtype", "Image");
    pMaskDict->SetNewFor<CPDF_Number>("Width", maskWidth);
    pMaskDict->SetNewFor<CPDF_Number>("Height", maskHeight);
    pMaskDict->SetNewFor<CPDF_Name>("ColorSpace", "DeviceGray");
    pMaskDict->SetNewFor<CPDF_Number>("BitsPerComponent", 8);
    if (pMaskBitmap->GetFormat() != FXDIB_1bppMask) {
      mask_buf.reset(FX_Alloc2D(uint8_t, maskHeight, maskWidth));
      mask_size = maskHeight * maskWidth;  // Safe since checked alloc returned.
      for (int32_t a = 0; a < maskHeight; a++) {
        memcpy(mask_buf.get() + a * maskWidth, pMaskBitmap->GetScanline(a),
               maskWidth);
      }
    }
    pMaskDict->SetNewFor<CPDF_Number>("Length", mask_size);
    CPDF_Stream* pNewStream = m_pDocument->NewIndirect<CPDF_Stream>(
        std::move(mask_buf), mask_size, std::move(pMaskDict));
    pDict->SetFor("SMask", pNewStream->MakeReference(m_pDocument.Get()));
  }

  uint8_t* src_buf = pBitmap->GetBuffer();
  int32_t src_pitch = pBitmap->GetPitch();
  std::unique_ptr<uint8_t, FxFreeDeleter> dest_buf(
      FX_Alloc2D(uint8_t, dest_pitch, BitmapHeight));
  // Safe as checked alloc returned.
  size_t dest_size = dest_pitch * BitmapHeight;
  auto dest_span = pdfium::make_span(dest_buf.get(), dest_size);
  size_t dest_span_offset = 0;
  if (bCopyWithoutAlpha) {
    for (int32_t i = 0; i < BitmapHeight; i++) {
      memcpy(&dest_span[dest_span_offset], src_buf, dest_pitch);
      dest_span_offset += dest_pitch;
      src_buf += src_pitch;
    }
  } else {
    int32_t src_offset = 0;
    for (int32_t row = 0; row < BitmapHeight; row++) {
      size_t dest_span_row_offset = dest_span_offset;
      src_offset = row * src_pitch;
      for (int32_t column = 0; column < BitmapWidth; column++) {
        float alpha = 1;
        dest_span[dest_span_row_offset] =
            static_cast<uint8_t>(src_buf[src_offset + 2] * alpha);
        dest_span[dest_span_row_offset + 1] =
            static_cast<uint8_t>(src_buf[src_offset + 1] * alpha);
        dest_span[dest_span_row_offset + 2] =
            static_cast<uint8_t>(src_buf[src_offset] * alpha);
        dest_span_row_offset += 3;
        src_offset += bpp == 24 ? 3 : 4;
      }

      dest_span_offset += dest_pitch;
    }
  }
  if (!m_pStream)
    m_pStream = pdfium::MakeUnique<CPDF_Stream>();

  m_pStream->InitStream(dest_span, std::move(pDict));
  m_bIsMask = pBitmap->IsAlphaMask();
  m_Width = BitmapWidth;
  m_Height = BitmapHeight;
}

void CPDF_Image::ResetCache(CPDF_Page* pPage) {
  RetainPtr<CPDF_Image> pHolder(this);
  pPage->GetRenderCache()->ResetBitmap(pHolder);
}

RetainPtr<CFX_DIBBase> CPDF_Image::LoadDIBBase() const {
  auto source = pdfium::MakeRetain<CPDF_DIBBase>();
  if (!source->Load(m_pDocument.Get(), m_pStream.Get()))
    return nullptr;

  if (!source->IsJBigImage())
    return source;

  CPDF_DIBBase::LoadState ret = CPDF_DIBBase::LoadState::kContinue;
  while (ret == CPDF_DIBBase::LoadState::kContinue)
    ret = source->ContinueLoadDIBBase(nullptr);
  return ret == CPDF_DIBBase::LoadState::kSuccess ? source : nullptr;
}

RetainPtr<CFX_DIBBase> CPDF_Image::DetachBitmap() {
  return std::move(m_pDIBBase);
}

RetainPtr<CFX_DIBBase> CPDF_Image::DetachMask() {
  return std::move(m_pMask);
}

bool CPDF_Image::StartLoadDIBBase(const CPDF_Dictionary* pFormResource,
                                  CPDF_Dictionary* pPageResource,
                                  bool bStdCS,
                                  uint32_t GroupFamily,
                                  bool bLoadMask) {
  auto source = pdfium::MakeRetain<CPDF_DIBBase>();
  CPDF_DIBBase::LoadState ret = source->StartLoadDIBBase(
      m_pDocument.Get(), m_pStream.Get(), true, pFormResource, pPageResource,
      bStdCS, GroupFamily, bLoadMask);
  if (ret == CPDF_DIBBase::LoadState::kFail) {
    m_pDIBBase.Reset();
    return false;
  }
  m_pDIBBase = source;
  if (ret == CPDF_DIBBase::LoadState::kContinue)
    return true;

  m_pMask = source->DetachMask();
  m_MatteColor = source->GetMatteColor();
  return false;
}

bool CPDF_Image::Continue(PauseIndicatorIface* pPause) {
  RetainPtr<CPDF_DIBBase> pSource = m_pDIBBase.As<CPDF_DIBBase>();
  CPDF_DIBBase::LoadState ret = pSource->ContinueLoadDIBBase(pPause);
  if (ret == CPDF_DIBBase::LoadState::kContinue)
    return true;

  if (ret == CPDF_DIBBase::LoadState::kSuccess) {
    m_pMask = pSource->DetachMask();
    m_MatteColor = pSource->GetMatteColor();
  } else {
    m_pDIBBase.Reset();
  }
  return false;
}
