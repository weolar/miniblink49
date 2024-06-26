// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_IMAGE_H_
#define CORE_FPDFAPI_PAGE_CPDF_IMAGE_H_

#include <memory>

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "third_party/base/span.h"

class CFX_DIBBase;
class CFX_DIBitmap;
class CPDF_Document;
class CPDF_Page;
class PauseIndicatorIface;
class IFX_SeekableReadStream;

class CPDF_Image final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  static bool IsValidJpegComponent(int32_t comps);
  static bool IsValidJpegBitsPerComponent(int32_t bpc);

  void ConvertStreamToIndirectObject();

  CPDF_Dictionary* GetDict() const;
  CPDF_Stream* GetStream() const { return m_pStream.Get(); }
  const CPDF_Dictionary* GetOC() const { return m_pOC.Get(); }
  CPDF_Document* GetDocument() const { return m_pDocument.Get(); }

  int32_t GetPixelHeight() const { return m_Height; }
  int32_t GetPixelWidth() const { return m_Width; }

  bool IsInline() const { return m_bIsInline; }
  bool IsMask() const { return m_bIsMask; }
  bool IsInterpol() const { return m_bInterpolate; }

  RetainPtr<CFX_DIBBase> LoadDIBBase() const;

  void SetImage(const RetainPtr<CFX_DIBitmap>& pDIBitmap);
  void SetJpegImage(const RetainPtr<IFX_SeekableReadStream>& pFile);
  void SetJpegImageInline(const RetainPtr<IFX_SeekableReadStream>& pFile);

  void ResetCache(CPDF_Page* pPage);

  // Returns whether to Continue() or not.
  bool StartLoadDIBBase(const CPDF_Dictionary* pFormResource,
                        CPDF_Dictionary* pPageResource,
                        bool bStdCS,
                        uint32_t GroupFamily,
                        bool bLoadMask);

  // Returns whether to Continue() or not.
  bool Continue(PauseIndicatorIface* pPause);

  RetainPtr<CFX_DIBBase> DetachBitmap();
  RetainPtr<CFX_DIBBase> DetachMask();

  RetainPtr<CFX_DIBBase> m_pDIBBase;
  RetainPtr<CFX_DIBBase> m_pMask;
  uint32_t m_MatteColor = 0;

 private:
  explicit CPDF_Image(CPDF_Document* pDoc);
  CPDF_Image(CPDF_Document* pDoc, std::unique_ptr<CPDF_Stream> pStream);
  CPDF_Image(CPDF_Document* pDoc, uint32_t dwStreamObjNum);
  ~CPDF_Image() override;

  void FinishInitialization(CPDF_Dictionary* pStreamDict);
  std::unique_ptr<CPDF_Dictionary> InitJPEG(pdfium::span<uint8_t> src_span);

  int32_t m_Height = 0;
  int32_t m_Width = 0;
  bool m_bIsInline = false;
  bool m_bIsMask = false;
  bool m_bInterpolate = false;
  UnownedPtr<CPDF_Document> const m_pDocument;
  MaybeOwned<CPDF_Stream> m_pStream;
  UnownedPtr<const CPDF_Dictionary> m_pOC;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_IMAGE_H_
