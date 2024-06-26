// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_imagerenderer.h"

#include <algorithm>
#include <memory>

#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_image.h"
#include "core/fpdfapi/page/cpdf_imageobject.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_shadingpattern.h"
#include "core/fpdfapi/page/cpdf_tilingpattern.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fpdfapi/render/cpdf_renderstatus.h"
#include "core/fpdfapi/render/cpdf_transferfunc.h"
#include "core/fpdfdoc/cpdf_occontext.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/dib/cfx_dibbase.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagestretcher.h"
#include "core/fxge/dib/cfx_imagetransformer.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

#ifdef _SKIA_SUPPORT_
#include "core/fxge/skia/fx_skia_device.h"
#endif

CPDF_ImageRenderer::CPDF_ImageRenderer() = default;

CPDF_ImageRenderer::~CPDF_ImageRenderer() = default;

bool CPDF_ImageRenderer::StartLoadDIBBase() {
  CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
  FX_RECT image_rect = image_rect_f.GetOuterRect();
  if (!image_rect.Valid())
    return false;

  if (m_Loader.Start(m_pImageObject.Get(),
                     m_pRenderStatus->GetContext()->GetPageCache(), m_bStdCS,
                     m_pRenderStatus->GetGroupFamily(),
                     m_pRenderStatus->GetLoadMask(), m_pRenderStatus.Get())) {
    m_Status = 4;
    return true;
  }
  return false;
}

bool CPDF_ImageRenderer::StartRenderDIBBase() {
  if (!m_Loader.GetBitmap())
    return false;

  CPDF_GeneralState& state = m_pImageObject->m_GeneralState;
  m_BitmapAlpha = FXSYS_round(255 * state.GetFillAlpha());
  m_pDIBBase = m_Loader.GetBitmap();
  if (GetRenderOptions().ColorModeIs(CPDF_RenderOptions::kAlpha) &&
      !m_Loader.GetMask()) {
    return StartBitmapAlpha();
  }
  if (state.GetTR()) {
    if (!state.GetTransferFunc())
      state.SetTransferFunc(m_pRenderStatus->GetTransferFunc(state.GetTR()));

    if (state.GetTransferFunc() && !state.GetTransferFunc()->GetIdentity())
      m_pDIBBase = m_Loader.TranslateImage(state.GetTransferFunc());
  }
  m_FillArgb = 0;
  m_bPatternColor = false;
  m_pPattern = nullptr;
  if (m_pDIBBase->IsAlphaMask()) {
    const CPDF_Color* pColor = m_pImageObject->m_ColorState.GetFillColor();
    if (pColor && pColor->IsPattern()) {
      m_pPattern = pColor->GetPattern();
      if (m_pPattern)
        m_bPatternColor = true;
    }
    m_FillArgb = m_pRenderStatus->GetFillArgb(m_pImageObject.Get());
  } else if (GetRenderOptions().ColorModeIs(CPDF_RenderOptions::kGray)) {
    RetainPtr<CFX_DIBitmap> pClone = m_pDIBBase->Clone(nullptr);
    if (!pClone)
      return false;

    pClone->ConvertColorScale(0xffffff, 0);
    m_pDIBBase = pClone;
  }
  m_ResampleOptions = FXDIB_ResampleOptions();
  if (GetRenderOptions().GetOptions().bForceDownsample)
    m_ResampleOptions.bInterpolateDownsample = true;
  else if (GetRenderOptions().GetOptions().bForceDownsample)
    m_ResampleOptions.bHalftone = true;

  if (m_pRenderStatus->GetRenderDevice()->GetDeviceClass() != FXDC_DISPLAY)
    HandleFilters();

  if (GetRenderOptions().GetOptions().bNoImageSmooth)
    m_ResampleOptions.bNoSmoothing = true;
  else if (m_pImageObject->GetImage()->IsInterpol())
    m_ResampleOptions.bInterpolateBilinear = true;

  if (m_Loader.GetMask())
    return DrawMaskedImage();

  if (m_bPatternColor)
    return DrawPatternImage();

  if (m_BitmapAlpha != 255 || !state.HasRef() || !state.GetFillOP() ||
      state.GetOPMode() != 0 || state.GetBlendType() != BlendMode::kNormal ||
      state.GetStrokeAlpha() != 1.0f || state.GetFillAlpha() != 1.0f) {
    return StartDIBBase();
  }
  CPDF_Document* pDocument = nullptr;
  CPDF_Page* pPage = nullptr;
  if (auto* pPageCache = m_pRenderStatus->GetContext()->GetPageCache()) {
    pPage = pPageCache->GetPage();
    pDocument = pPage->GetDocument();
  } else {
    pDocument = m_pImageObject->GetImage()->GetDocument();
  }
  CPDF_Dictionary* pPageResources =
      pPage ? pPage->m_pPageResources.Get() : nullptr;
  CPDF_Object* pCSObj =
      m_pImageObject->GetImage()->GetStream()->GetDict()->GetDirectObjectFor(
          "ColorSpace");
  CPDF_ColorSpace* pColorSpace =
      pDocument->LoadColorSpace(pCSObj, pPageResources);
  if (!pColorSpace)
    return StartDIBBase();
  int format = pColorSpace->GetFamily();
  if (format == PDFCS_DEVICECMYK || format == PDFCS_SEPARATION ||
      format == PDFCS_DEVICEN) {
    m_BlendType = BlendMode::kDarken;
  }
  pDocument->GetPageData()->ReleaseColorSpace(pCSObj);
  return StartDIBBase();
}

bool CPDF_ImageRenderer::Start(CPDF_RenderStatus* pStatus,
                               CPDF_ImageObject* pImageObject,
                               const CFX_Matrix& mtObj2Device,
                               bool bStdCS,
                               BlendMode blendType) {
  ASSERT(pImageObject);
  m_pRenderStatus = pStatus;
  m_bStdCS = bStdCS;
  m_pImageObject = pImageObject;
  m_BlendType = blendType;
  m_mtObj2Device = mtObj2Device;
  const CPDF_Dictionary* pOC = m_pImageObject->GetImage()->GetOC();
  if (pOC && GetRenderOptions().GetOCContext() &&
      !GetRenderOptions().GetOCContext()->CheckOCGVisible(pOC)) {
    return false;
  }
  m_ImageMatrix = m_pImageObject->matrix() * mtObj2Device;
  if (StartLoadDIBBase())
    return true;
  return StartRenderDIBBase();
}

bool CPDF_ImageRenderer::Start(CPDF_RenderStatus* pStatus,
                               const RetainPtr<CFX_DIBBase>& pDIBBase,
                               FX_ARGB bitmap_argb,
                               int bitmap_alpha,
                               const CFX_Matrix& mtImage2Device,
                               const FXDIB_ResampleOptions& options,
                               bool bStdCS,
                               BlendMode blendType) {
  m_pRenderStatus = pStatus;
  m_pDIBBase = pDIBBase;
  m_FillArgb = bitmap_argb;
  m_BitmapAlpha = bitmap_alpha;
  m_ImageMatrix = mtImage2Device;
  m_ResampleOptions = options;
  m_bStdCS = bStdCS;
  m_BlendType = blendType;
  return StartDIBBase();
}

bool CPDF_ImageRenderer::NotDrawing() const {
  return m_pRenderStatus->IsPrint() &&
         !(m_pRenderStatus->GetRenderDevice()->GetRenderCaps() &
           FXRC_BLEND_MODE);
}

FX_RECT CPDF_ImageRenderer::GetDrawRect() const {
  FX_RECT rect = m_ImageMatrix.GetUnitRect().GetOuterRect();
  rect.Intersect(m_pRenderStatus->GetRenderDevice()->GetClipBox());
  return rect;
}

CFX_Matrix CPDF_ImageRenderer::GetDrawMatrix(const FX_RECT& rect) const {
  CFX_Matrix new_matrix = m_ImageMatrix;
  new_matrix.Translate(-rect.left, -rect.top);
  return new_matrix;
}

void CPDF_ImageRenderer::CalculateDrawImage(
    CFX_DefaultRenderDevice* pBitmapDevice1,
    CFX_DefaultRenderDevice* pBitmapDevice2,
    const RetainPtr<CFX_DIBBase>& pDIBBase,
    const CFX_Matrix& mtNewMatrix,
    const FX_RECT& rect) const {
  CPDF_RenderStatus bitmap_render(m_pRenderStatus->GetContext(),
                                  pBitmapDevice2);
  bitmap_render.SetDropObjects(m_pRenderStatus->GetDropObjects());
  bitmap_render.SetStdCS(true);
  bitmap_render.Initialize(nullptr, nullptr);

  CPDF_ImageRenderer image_render;
  if (image_render.Start(&bitmap_render, pDIBBase, 0xffffffff, 255, mtNewMatrix,
                         m_ResampleOptions, true, BlendMode::kNormal)) {
    image_render.Continue(nullptr);
  }
  if (m_Loader.MatteColor() == 0xffffffff)
    return;
  int matte_r = FXARGB_R(m_Loader.MatteColor());
  int matte_g = FXARGB_G(m_Loader.MatteColor());
  int matte_b = FXARGB_B(m_Loader.MatteColor());
  for (int row = 0; row < rect.Height(); row++) {
    uint8_t* dest_scan = pBitmapDevice1->GetBitmap()->GetWritableScanline(row);
    const uint8_t* mask_scan = pBitmapDevice2->GetBitmap()->GetScanline(row);
    for (int col = 0; col < rect.Width(); col++) {
      int alpha = *mask_scan++;
      if (!alpha) {
        dest_scan += 4;
        continue;
      }
      int orig = (*dest_scan - matte_b) * 255 / alpha + matte_b;
      *dest_scan++ = pdfium::clamp(orig, 0, 255);
      orig = (*dest_scan - matte_g) * 255 / alpha + matte_g;
      *dest_scan++ = pdfium::clamp(orig, 0, 255);
      orig = (*dest_scan - matte_r) * 255 / alpha + matte_r;
      *dest_scan++ = pdfium::clamp(orig, 0, 255);
      dest_scan++;
    }
  }
}

const CPDF_RenderOptions& CPDF_ImageRenderer::GetRenderOptions() const {
  return m_pRenderStatus->GetRenderOptions();
}

bool CPDF_ImageRenderer::DrawPatternImage() {
  if (NotDrawing()) {
    m_Result = false;
    return false;
  }

  FX_RECT rect = GetDrawRect();
  if (rect.IsEmpty())
    return false;

  CFX_Matrix new_matrix = GetDrawMatrix(rect);
  CFX_DefaultRenderDevice bitmap_device1;
  if (!bitmap_device1.Create(rect.Width(), rect.Height(), FXDIB_Rgb32, nullptr))
    return true;

  bitmap_device1.GetBitmap()->Clear(0xffffff);

  CPDF_RenderStatus bitmap_render(m_pRenderStatus->GetContext(),
                                  &bitmap_device1);
  bitmap_render.SetOptions(GetRenderOptions());
  bitmap_render.SetDropObjects(m_pRenderStatus->GetDropObjects());
  bitmap_render.SetStdCS(true);
  bitmap_render.Initialize(nullptr, nullptr);

  CFX_Matrix patternDevice = m_mtObj2Device;
  patternDevice.Translate(static_cast<float>(-rect.left),
                          static_cast<float>(-rect.top));
  if (CPDF_TilingPattern* pTilingPattern = m_pPattern->AsTilingPattern()) {
    bitmap_render.DrawTilingPattern(pTilingPattern, m_pImageObject.Get(),
                                    patternDevice, false);
  } else if (CPDF_ShadingPattern* pShadingPattern =
                 m_pPattern->AsShadingPattern()) {
    bitmap_render.DrawShadingPattern(pShadingPattern, m_pImageObject.Get(),
                                     patternDevice, false);
  }

  CFX_DefaultRenderDevice bitmap_device2;
  if (!bitmap_device2.Create(rect.Width(), rect.Height(), FXDIB_8bppRgb,
                             nullptr)) {
    return true;
  }
  bitmap_device2.GetBitmap()->Clear(0);
  CalculateDrawImage(&bitmap_device1, &bitmap_device2, m_pDIBBase, new_matrix,
                     rect);
  bitmap_device2.GetBitmap()->ConvertFormat(FXDIB_8bppMask);
  bitmap_device1.GetBitmap()->MultiplyAlpha(bitmap_device2.GetBitmap());
  bitmap_device1.GetBitmap()->MultiplyAlpha(255);
  m_pRenderStatus->GetRenderDevice()->SetDIBitsWithBlend(
      bitmap_device1.GetBitmap(), rect.left, rect.top, m_BlendType);
  return false;
}

bool CPDF_ImageRenderer::DrawMaskedImage() {
  if (NotDrawing()) {
    m_Result = false;
    return false;
  }

  FX_RECT rect = GetDrawRect();
  if (rect.IsEmpty())
    return false;

  CFX_Matrix new_matrix = GetDrawMatrix(rect);
  CFX_DefaultRenderDevice bitmap_device1;
  if (!bitmap_device1.Create(rect.Width(), rect.Height(), FXDIB_Rgb32, nullptr))
    return true;

#if defined _SKIA_SUPPORT_
  bitmap_device1.Clear(0xffffff);
#else
  bitmap_device1.GetBitmap()->Clear(0xffffff);
#endif
  CPDF_RenderStatus bitmap_render(m_pRenderStatus->GetContext(),
                                  &bitmap_device1);
  bitmap_render.SetDropObjects(m_pRenderStatus->GetDropObjects());
  bitmap_render.SetStdCS(true);
  bitmap_render.Initialize(nullptr, nullptr);
  CPDF_ImageRenderer image_render;
  if (image_render.Start(&bitmap_render, m_pDIBBase, 0, 255, new_matrix,
                         m_ResampleOptions, true, BlendMode::kNormal)) {
    image_render.Continue(nullptr);
  }
  CFX_DefaultRenderDevice bitmap_device2;
  if (!bitmap_device2.Create(rect.Width(), rect.Height(), FXDIB_8bppRgb,
                             nullptr))
    return true;

#if defined _SKIA_SUPPORT_
  bitmap_device2.Clear(0);
#else
  bitmap_device2.GetBitmap()->Clear(0);
#endif
  CalculateDrawImage(&bitmap_device1, &bitmap_device2, m_Loader.GetMask(),
                     new_matrix, rect);
#ifdef _SKIA_SUPPORT_
  m_pRenderStatus->GetRenderDevice()->SetBitsWithMask(
      bitmap_device1.GetBitmap(), bitmap_device2.GetBitmap(), rect.left,
      rect.top, m_BitmapAlpha, m_BlendType);
#else
  bitmap_device2.GetBitmap()->ConvertFormat(FXDIB_8bppMask);
  bitmap_device1.GetBitmap()->MultiplyAlpha(bitmap_device2.GetBitmap());
  if (m_BitmapAlpha < 255)
    bitmap_device1.GetBitmap()->MultiplyAlpha(m_BitmapAlpha);
  m_pRenderStatus->GetRenderDevice()->SetDIBitsWithBlend(
      bitmap_device1.GetBitmap(), rect.left, rect.top, m_BlendType);
#endif  //  _SKIA_SUPPORT_
  return false;
}

bool CPDF_ImageRenderer::StartDIBBase() {
  if (!m_ResampleOptions.bInterpolateDownsample && m_pDIBBase->GetBPP() > 1) {
    FX_SAFE_SIZE_T image_size = m_pDIBBase->GetBPP();
    image_size /= 8;
    image_size *= m_pDIBBase->GetWidth();
    image_size *= m_pDIBBase->GetHeight();
    if (!image_size.IsValid())
      return false;

    if (image_size.ValueOrDie() > FPDF_HUGE_IMAGE_SIZE &&
        !m_ResampleOptions.bHalftone) {
      m_ResampleOptions.bInterpolateBilinear = true;
    }
  }
#ifdef _SKIA_SUPPORT_
  RetainPtr<CFX_DIBitmap> premultiplied = m_pDIBBase->Clone(nullptr);
  if (m_pDIBBase->HasAlpha())
    CFX_SkiaDeviceDriver::PreMultiply(premultiplied);
  if (m_pRenderStatus->GetRenderDevice()->StartDIBitsWithBlend(
          premultiplied, m_BitmapAlpha, m_FillArgb, m_ImageMatrix,
          m_ResampleOptions, &m_DeviceHandle, m_BlendType)) {
    if (m_DeviceHandle) {
      m_Status = 3;
      return true;
    }
    return false;
  }
#else
  if (m_pRenderStatus->GetRenderDevice()->StartDIBitsWithBlend(
          m_pDIBBase, m_BitmapAlpha, m_FillArgb, m_ImageMatrix,
          m_ResampleOptions, &m_DeviceHandle, m_BlendType)) {
    if (m_DeviceHandle) {
      m_Status = 3;
      return true;
    }
    return false;
  }
#endif
  CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
  FX_RECT image_rect = image_rect_f.GetOuterRect();
  int dest_width = image_rect.Width();
  int dest_height = image_rect.Height();
  if ((fabs(m_ImageMatrix.b) >= 0.5f || m_ImageMatrix.a == 0) ||
      (fabs(m_ImageMatrix.c) >= 0.5f || m_ImageMatrix.d == 0)) {
    if (NotDrawing()) {
      m_Result = false;
      return false;
    }

    FX_RECT clip_box = m_pRenderStatus->GetRenderDevice()->GetClipBox();
    clip_box.Intersect(image_rect);
    m_Status = 2;
    m_pTransformer = pdfium::MakeUnique<CFX_ImageTransformer>(
        m_pDIBBase, m_ImageMatrix, m_ResampleOptions, &clip_box);
    return true;
  }
  if (m_ImageMatrix.a < 0)
    dest_width = -dest_width;

  if (m_ImageMatrix.d > 0)
    dest_height = -dest_height;

  int dest_left = dest_width > 0 ? image_rect.left : image_rect.right;
  int dest_top = dest_height > 0 ? image_rect.top : image_rect.bottom;
  if (m_pDIBBase->IsOpaqueImage() && m_BitmapAlpha == 255) {
    if (m_pRenderStatus->GetRenderDevice()->StretchDIBitsWithFlagsAndBlend(
            m_pDIBBase, dest_left, dest_top, dest_width, dest_height,
            m_ResampleOptions, m_BlendType)) {
      return false;
    }
  }
  if (m_pDIBBase->IsAlphaMask()) {
    if (m_BitmapAlpha != 255)
      m_FillArgb = FXARGB_MUL_ALPHA(m_FillArgb, m_BitmapAlpha);
    if (m_pRenderStatus->GetRenderDevice()->StretchBitMaskWithFlags(
            m_pDIBBase, dest_left, dest_top, dest_width, dest_height,
            m_FillArgb, m_ResampleOptions)) {
      return false;
    }
  }
  if (NotDrawing()) {
    m_Result = false;
    return true;
  }

  FX_RECT clip_box = m_pRenderStatus->GetRenderDevice()->GetClipBox();
  FX_RECT dest_rect = clip_box;
  dest_rect.Intersect(image_rect);
  FX_RECT dest_clip(
      dest_rect.left - image_rect.left, dest_rect.top - image_rect.top,
      dest_rect.right - image_rect.left, dest_rect.bottom - image_rect.top);
  RetainPtr<CFX_DIBitmap> pStretched = m_pDIBBase->StretchTo(
      dest_width, dest_height, m_ResampleOptions, &dest_clip);
  if (pStretched) {
    m_pRenderStatus->CompositeDIBitmap(pStretched, dest_rect.left,
                                       dest_rect.top, m_FillArgb, m_BitmapAlpha,
                                       m_BlendType, CPDF_Transparency());
  }
  return false;
}

bool CPDF_ImageRenderer::StartBitmapAlpha() {
  if (m_pDIBBase->IsOpaqueImage()) {
    CFX_PathData path;
    path.AppendRect(0, 0, 1, 1);
    path.Transform(m_ImageMatrix);
    uint32_t fill_color =
        ArgbEncode(0xff, m_BitmapAlpha, m_BitmapAlpha, m_BitmapAlpha);
    m_pRenderStatus->GetRenderDevice()->DrawPath(&path, nullptr, nullptr,
                                                 fill_color, 0, FXFILL_WINDING);
    return false;
  }
  RetainPtr<CFX_DIBBase> pAlphaMask;
  if (m_pDIBBase->IsAlphaMask())
    pAlphaMask = m_pDIBBase;
  else
    pAlphaMask = m_pDIBBase->CloneAlphaMask();

  if (fabs(m_ImageMatrix.b) >= 0.5f || fabs(m_ImageMatrix.c) >= 0.5f) {
    int left;
    int top;
    RetainPtr<CFX_DIBitmap> pTransformed =
        pAlphaMask->TransformTo(m_ImageMatrix, &left, &top);
    if (!pTransformed)
      return true;

    m_pRenderStatus->GetRenderDevice()->SetBitMask(
        pTransformed, left, top,
        ArgbEncode(0xff, m_BitmapAlpha, m_BitmapAlpha, m_BitmapAlpha));
    return false;
  }
  CFX_FloatRect image_rect_f = m_ImageMatrix.GetUnitRect();
  FX_RECT image_rect = image_rect_f.GetOuterRect();
  int dest_width =
      m_ImageMatrix.a > 0 ? image_rect.Width() : -image_rect.Width();
  int dest_height =
      m_ImageMatrix.d > 0 ? -image_rect.Height() : image_rect.Height();
  int left = dest_width > 0 ? image_rect.left : image_rect.right;
  int top = dest_height > 0 ? image_rect.top : image_rect.bottom;
  m_pRenderStatus->GetRenderDevice()->StretchBitMask(
      pAlphaMask, left, top, dest_width, dest_height,
      ArgbEncode(0xff, m_BitmapAlpha, m_BitmapAlpha, m_BitmapAlpha));
  return false;
}

bool CPDF_ImageRenderer::Continue(PauseIndicatorIface* pPause) {
  if (m_Status == 2) {
    if (m_pTransformer->Continue(pPause))
      return true;

    RetainPtr<CFX_DIBitmap> pBitmap = m_pTransformer->DetachBitmap();
    if (!pBitmap)
      return false;

    if (pBitmap->IsAlphaMask()) {
      if (m_BitmapAlpha != 255)
        m_FillArgb = FXARGB_MUL_ALPHA(m_FillArgb, m_BitmapAlpha);
      m_Result = m_pRenderStatus->GetRenderDevice()->SetBitMask(
          pBitmap, m_pTransformer->result().left, m_pTransformer->result().top,
          m_FillArgb);
    } else {
      if (m_BitmapAlpha != 255)
        pBitmap->MultiplyAlpha(m_BitmapAlpha);
      m_Result = m_pRenderStatus->GetRenderDevice()->SetDIBitsWithBlend(
          pBitmap, m_pTransformer->result().left, m_pTransformer->result().top,
          m_BlendType);
    }
    return false;
  }
  if (m_Status == 3) {
    return m_pRenderStatus->GetRenderDevice()->ContinueDIBits(
        m_DeviceHandle.get(), pPause);
  }

  if (m_Status == 4) {
    if (m_Loader.Continue(pPause, m_pRenderStatus.Get()))
      return true;

    if (StartRenderDIBBase())
      return Continue(pPause);
  }
  return false;
}

void CPDF_ImageRenderer::HandleFilters() {
  CPDF_Object* pFilters =
      m_pImageObject->GetImage()->GetStream()->GetDict()->GetDirectObjectFor(
          "Filter");
  if (!pFilters)
    return;

  if (pFilters->IsName()) {
    ByteString bsDecodeType = pFilters->GetString();
    if (bsDecodeType == "DCTDecode" || bsDecodeType == "JPXDecode")
      m_ResampleOptions.bLossy = true;
    return;
  }

  CPDF_Array* pArray = pFilters->AsArray();
  if (!pArray)
    return;

  for (size_t i = 0; i < pArray->size(); i++) {
    ByteString bsDecodeType = pArray->GetStringAt(i);
    if (bsDecodeType == "DCTDecode" || bsDecodeType == "JPXDecode") {
      m_ResampleOptions.bLossy = true;
      break;
    }
  }
}
