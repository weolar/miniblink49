// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_rendercontext.h"

#include "core/fpdfapi/page/cpdf_page.h"
#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_pageobjectholder.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/render/cpdf_pagerendercache.h"
#include "core/fpdfapi/render/cpdf_progressiverenderer.h"
#include "core/fpdfapi/render/cpdf_renderoptions.h"
#include "core/fpdfapi/render/cpdf_renderstatus.h"
#include "core/fpdfapi/render/cpdf_textrenderer.h"
#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_dib.h"

CPDF_RenderContext::CPDF_RenderContext(CPDF_Page* pPage)
    : m_pDocument(pPage->GetDocument()),
      m_pPageResources(pPage->m_pPageResources.Get()),
      m_pPageCache(pPage->GetRenderCache()) {}

CPDF_RenderContext::CPDF_RenderContext(CPDF_Document* pDoc,
                                       CPDF_PageRenderCache* pPageCache)
    : m_pDocument(pDoc), m_pPageResources(nullptr), m_pPageCache(pPageCache) {}

CPDF_RenderContext::~CPDF_RenderContext() {}

void CPDF_RenderContext::GetBackground(const RetainPtr<CFX_DIBitmap>& pBuffer,
                                       const CPDF_PageObject* pObj,
                                       const CPDF_RenderOptions* pOptions,
                                       CFX_Matrix* pFinalMatrix) {
  CFX_DefaultRenderDevice device;
  device.Attach(pBuffer, false, nullptr, false);

  device.FillRect(FX_RECT(0, 0, device.GetWidth(), device.GetHeight()),
                  0xffffffff);
  Render(&device, pObj, pOptions, pFinalMatrix);
}

void CPDF_RenderContext::AppendLayer(CPDF_PageObjectHolder* pObjectHolder,
                                     const CFX_Matrix* pObject2Device) {
  m_Layers.emplace_back();
  m_Layers.back().m_pObjectHolder = pObjectHolder;
  if (pObject2Device)
    m_Layers.back().m_Matrix = *pObject2Device;
}

void CPDF_RenderContext::Render(CFX_RenderDevice* pDevice,
                                const CPDF_RenderOptions* pOptions,
                                const CFX_Matrix* pLastMatrix) {
  Render(pDevice, nullptr, pOptions, pLastMatrix);
}

void CPDF_RenderContext::Render(CFX_RenderDevice* pDevice,
                                const CPDF_PageObject* pStopObj,
                                const CPDF_RenderOptions* pOptions,
                                const CFX_Matrix* pLastMatrix) {
  for (auto& layer : m_Layers) {
    CFX_RenderDevice::StateRestorer restorer(pDevice);
    CPDF_RenderStatus status(this, pDevice);
    if (pOptions)
      status.SetOptions(*pOptions);
    status.SetStopObject(pStopObj);
    status.SetTransparency(layer.m_pObjectHolder->GetTransparency());
    if (pLastMatrix) {
      const CFX_Matrix& last_matrix = *pLastMatrix;
      CFX_Matrix final_matrix = layer.m_Matrix * last_matrix;
      status.SetDeviceMatrix(last_matrix);
      status.Initialize(nullptr, nullptr);
      status.RenderObjectList(layer.m_pObjectHolder.Get(), final_matrix);
    } else {
      status.Initialize(nullptr, nullptr);
      status.RenderObjectList(layer.m_pObjectHolder.Get(), layer.m_Matrix);
    }
    if (status.GetRenderOptions().GetOptions().bLimitedImageCache) {
      m_pPageCache->CacheOptimization(
          status.GetRenderOptions().GetCacheSizeLimit());
    }
    if (status.IsStopped())
      break;
  }
}

CPDF_RenderContext::Layer::Layer() {}

CPDF_RenderContext::Layer::Layer(const Layer& that) = default;

CPDF_RenderContext::Layer::~Layer() {}
