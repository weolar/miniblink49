// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_PROGRESSIVERENDERER_H_
#define CORE_FPDFAPI_RENDER_CPDF_PROGRESSIVERENDERER_H_

#include <memory>

#include "core/fpdfapi/page/cpdf_pageobjectlist.h"
#include "core/fpdfapi/render/cpdf_rendercontext.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"

class CPDF_RenderOptions;
class CPDF_RenderStatus;
class CFX_RenderDevice;
class PauseIndicatorIface;

class CPDF_ProgressiveRenderer {
 public:
  // Must match FDF_RENDER_* definitions in public/fpdf_progressive.h, but
  // cannot #include that header. fpdfsdk/fpdf_progressive.cpp has
  // static_asserts to make sure the two sets of values match.
  enum Status {
    Ready,          // FPDF_RENDER_READY
    ToBeContinued,  // FPDF_RENDER_TOBECONTINUED
    Done,           // FPDF_RENDER_DONE
    Failed          // FPDF_RENDER_FAILED
  };

  static int ToFPDFStatus(Status status) { return static_cast<int>(status); }

  CPDF_ProgressiveRenderer(CPDF_RenderContext* pContext,
                           CFX_RenderDevice* pDevice,
                           const CPDF_RenderOptions* pOptions);
  ~CPDF_ProgressiveRenderer();

  Status GetStatus() const { return m_Status; }
  void Start(PauseIndicatorIface* pPause);
  void Continue(PauseIndicatorIface* pPause);

 private:
  // Maximum page objects to render before checking for pause.
  static const int kStepLimit = 100;

  Status m_Status;
  UnownedPtr<CPDF_RenderContext> const m_pContext;
  UnownedPtr<CFX_RenderDevice> const m_pDevice;
  const CPDF_RenderOptions* const m_pOptions;
  std::unique_ptr<CPDF_RenderStatus> m_pRenderStatus;
  CFX_FloatRect m_ClipRect;
  uint32_t m_LayerIndex;
  CPDF_RenderContext::Layer* m_pCurrentLayer;
  CPDF_PageObjectList::const_iterator m_LastObjectRendered;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_PROGRESSIVERENDERER_H_
