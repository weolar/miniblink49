// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_DIBBASE_H_
#define CORE_FPDFAPI_RENDER_CPDF_DIBBASE_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_clippath.h"
#include "core/fpdfapi/page/cpdf_countedobject.h"
#include "core/fpdfapi/page/cpdf_graphicstates.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/render/cpdf_imageloader.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/dib/cfx_dibbase.h"
#include "third_party/base/span.h"

class CCodec_Jbig2Context;
class CCodec_ScanlineDecoder;
class CPDF_Color;
class CPDF_Dictionary;
class CPDF_Document;
class CPDF_Stream;

struct DIB_COMP_DATA {
  float m_DecodeMin;
  float m_DecodeStep;
  int m_ColorKeyMin;
  int m_ColorKeyMax;
};

#define FPDF_HUGE_IMAGE_SIZE 60000000

class CPDF_DIBBase final : public CFX_DIBBase {
 public:
  enum class LoadState : uint8_t { kFail, kSuccess, kContinue };

  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  bool Load(CPDF_Document* pDoc, const CPDF_Stream* pStream);

  // CFX_DIBBase
  bool SkipToScanline(int line, PauseIndicatorIface* pPause) const override;
  uint8_t* GetBuffer() const override;
  const uint8_t* GetScanline(int line) const override;
  void DownSampleScanline(int line,
                          uint8_t* dest_scan,
                          int dest_bpp,
                          int dest_width,
                          bool bFlipX,
                          int clip_left,
                          int clip_width) const override;

  const CPDF_ColorSpace* GetColorSpace() const { return m_pColorSpace.Get(); }
  uint32_t GetMatteColor() const { return m_MatteColor; }

  LoadState StartLoadDIBBase(CPDF_Document* pDoc,
                             const CPDF_Stream* pStream,
                             bool bHasMask,
                             const CPDF_Dictionary* pFormResources,
                             CPDF_Dictionary* pPageResources,
                             bool bStdCS,
                             uint32_t GroupFamily,
                             bool bLoadMask);
  LoadState ContinueLoadDIBBase(PauseIndicatorIface* pPause);
  RetainPtr<CPDF_DIBBase> DetachMask();

  bool IsJBigImage() const;

 private:
  CPDF_DIBBase();
  ~CPDF_DIBBase() override;

  LoadState StartLoadMask();
  LoadState StartLoadMaskDIB();
  bool ContinueToLoadMask();
  LoadState ContinueLoadMaskDIB(PauseIndicatorIface* pPause);
  bool LoadColorInfo(const CPDF_Dictionary* pFormResources,
                     const CPDF_Dictionary* pPageResources);
  bool GetDecodeAndMaskArray(bool* bDefaultDecode, bool* bColorKey);
  RetainPtr<CFX_DIBitmap> LoadJpxBitmap();
  void LoadPalette();
  LoadState CreateDecoder();
  bool CreateDCTDecoder(pdfium::span<const uint8_t> src_data,
                        const CPDF_Dictionary* pParams);
  void TranslateScanline24bpp(uint8_t* dest_scan,
                              const uint8_t* src_scan) const;
  bool TranslateScanline24bppDefaultDecode(uint8_t* dest_scan,
                                           const uint8_t* src_scan) const;
  void ValidateDictParam();
  void DownSampleScanline1Bit(int orig_Bpp,
                              int dest_Bpp,
                              uint32_t src_width,
                              const uint8_t* pSrcLine,
                              uint8_t* dest_scan,
                              int dest_width,
                              bool bFlipX,
                              int clip_left,
                              int clip_width) const;
  void DownSampleScanline8Bit(int orig_Bpp,
                              int dest_Bpp,
                              uint32_t src_width,
                              const uint8_t* pSrcLine,
                              uint8_t* dest_scan,
                              int dest_width,
                              bool bFlipX,
                              int clip_left,
                              int clip_width) const;
  void DownSampleScanline32Bit(int orig_Bpp,
                               int dest_Bpp,
                               uint32_t src_width,
                               const uint8_t* pSrcLine,
                               uint8_t* dest_scan,
                               int dest_width,
                               bool bFlipX,
                               int clip_left,
                               int clip_width) const;
  bool TransMask() const;

  UnownedPtr<CPDF_Document> m_pDocument;
  UnownedPtr<const CPDF_Stream> m_pStream;
  UnownedPtr<const CPDF_Dictionary> m_pDict;
  RetainPtr<CPDF_StreamAcc> m_pStreamAcc;
  UnownedPtr<CPDF_ColorSpace> m_pColorSpace;
  uint32_t m_Family = 0;
  uint32_t m_bpc = 0;
  uint32_t m_bpc_orig = 0;
  uint32_t m_nComponents = 0;
  uint32_t m_GroupFamily = 0;
  uint32_t m_MatteColor = 0;
  bool m_bLoadMask = false;
  bool m_bDefaultDecode = true;
  bool m_bImageMask = false;
  bool m_bDoBpcCheck = true;
  bool m_bColorKey = false;
  bool m_bHasMask = false;
  bool m_bStdCS = false;
  std::vector<DIB_COMP_DATA> m_CompData;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pLineBuf;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pMaskedLine;
  RetainPtr<CFX_DIBitmap> m_pCachedBitmap;
  RetainPtr<CPDF_DIBBase> m_pMask;
  RetainPtr<CPDF_StreamAcc> m_pGlobalStream;
  std::unique_ptr<CCodec_ScanlineDecoder> m_pDecoder;

  // Must come after |m_pCachedBitmap|.
  std::unique_ptr<CCodec_Jbig2Context> m_pJbig2Context;

  UnownedPtr<const CPDF_Stream> m_pMaskStream;
  LoadState m_Status = LoadState::kFail;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_DIBBASE_H_
