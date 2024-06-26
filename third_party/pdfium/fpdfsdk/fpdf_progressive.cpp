// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "public/fpdf_progressive.h"

#include <utility>

#include "core/fpdfapi/cpdf_pagerendercontext.h"
#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/render/cpdf_progressiverenderer.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_renderdevice.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/ipdfsdk_pauseadapter.h"
#include "public/fpdfview.h"
#include "third_party/base/ptr_util.h"

// These checks are here because core/ and public/ cannot depend on each other.
static_assert(CPDF_ProgressiveRenderer::Ready == FPDF_RENDER_READY,
              "CPDF_ProgressiveRenderer::Ready value mismatch");
static_assert(CPDF_ProgressiveRenderer::ToBeContinued ==
                  FPDF_RENDER_TOBECONTINUED,
              "CPDF_ProgressiveRenderer::ToBeContinued value mismatch");
static_assert(CPDF_ProgressiveRenderer::Done == FPDF_RENDER_DONE,
              "CPDF_ProgressiveRenderer::Done value mismatch");
static_assert(CPDF_ProgressiveRenderer::Failed == FPDF_RENDER_FAILED,
              "CPDF_ProgressiveRenderer::Failed value mismatch");

FPDF_EXPORT int FPDF_CALLCONV FPDF_RenderPageBitmap_Start(FPDF_BITMAP bitmap,
                                                          FPDF_PAGE page,
                                                          int start_x,
                                                          int start_y,
                                                          int size_x,
                                                          int size_y,
                                                          int rotate,
                                                          int flags,
                                                          IFSDK_PAUSE* pause) {
  if (!bitmap || !pause || pause->version != 1)
    return FPDF_RENDER_FAILED;

  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return FPDF_RENDER_FAILED;

  auto pOwnedContext = pdfium::MakeUnique<CPDF_PageRenderContext>();
  CPDF_PageRenderContext* pContext = pOwnedContext.get();
  pPage->SetRenderContext(std::move(pOwnedContext));

  RetainPtr<CFX_DIBitmap> pBitmap(CFXDIBitmapFromFPDFBitmap(bitmap));
  auto pOwnedDevice = pdfium::MakeUnique<CFX_DefaultRenderDevice>();
  CFX_DefaultRenderDevice* pDevice = pOwnedDevice.get();
  pContext->m_pDevice = std::move(pOwnedDevice);
  pDevice->Attach(pBitmap, !!(flags & FPDF_REVERSE_BYTE_ORDER), nullptr, false);

  IPDFSDK_PauseAdapter IPauseAdapter(pause);
  RenderPageWithContext(pContext, page, start_x, start_y, size_x, size_y,
                        rotate, flags, false, &IPauseAdapter);

#ifdef _SKIA_SUPPORT_PATHS_
  pDevice->Flush(false);
  pBitmap->UnPreMultiply();
#endif
  if (pContext->m_pRenderer) {
    return CPDF_ProgressiveRenderer::ToFPDFStatus(
        pContext->m_pRenderer->GetStatus());
  }
  return FPDF_RENDER_FAILED;
}

FPDF_EXPORT int FPDF_CALLCONV FPDF_RenderPage_Continue(FPDF_PAGE page,
                                                       IFSDK_PAUSE* pause) {
  if (!pause || pause->version != 1)
    return FPDF_RENDER_FAILED;

  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (!pPage)
    return FPDF_RENDER_FAILED;

  CPDF_PageRenderContext* pContext = pPage->GetRenderContext();
  if (pContext && pContext->m_pRenderer) {
    IPDFSDK_PauseAdapter IPauseAdapter(pause);
    pContext->m_pRenderer->Continue(&IPauseAdapter);
#ifdef _SKIA_SUPPORT_PATHS_
    CFX_RenderDevice* pDevice = pContext->m_pDevice.get();
    pDevice->Flush(false);
    pDevice->GetBitmap()->UnPreMultiply();
#endif
    return CPDF_ProgressiveRenderer::ToFPDFStatus(
        pContext->m_pRenderer->GetStatus());
  }
  return FPDF_RENDER_FAILED;
}

FPDF_EXPORT void FPDF_CALLCONV FPDF_RenderPage_Close(FPDF_PAGE page) {
  CPDF_Page* pPage = CPDFPageFromFPDFPage(page);
  if (pPage) {
#ifdef _SKIA_SUPPORT_PATHS_
    CPDF_PageRenderContext* pContext = pPage->GetRenderContext();
    if (pContext && pContext->m_pRenderer) {
      CFX_RenderDevice* pDevice = pContext->m_pDevice.get();
      pDevice->Flush(true);
      pDevice->GetBitmap()->UnPreMultiply();
    }
#endif
    pPage->SetRenderContext(nullptr);
  }
}
