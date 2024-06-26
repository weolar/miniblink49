// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_SCALEDRENDERBUFFER_H_
#define CORE_FPDFAPI_RENDER_CPDF_SCALEDRENDERBUFFER_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/unowned_ptr.h"

class CFX_DefaultRenderDevice;
class CFX_RenderDevice;
class CPDF_PageObject;
class CPDF_RenderContext;
class CPDF_RenderOptions;

class CPDF_ScaledRenderBuffer {
 public:
  CPDF_ScaledRenderBuffer();
  ~CPDF_ScaledRenderBuffer();

  bool Initialize(CPDF_RenderContext* pContext,
                  CFX_RenderDevice* pDevice,
                  const FX_RECT& pRect,
                  const CPDF_PageObject* pObj,
                  const CPDF_RenderOptions* pOptions,
                  int max_dpi);

  CFX_RenderDevice* GetDevice() const;
  const CFX_Matrix& GetMatrix() const { return m_Matrix; }
  void OutputToDevice();

 private:
  UnownedPtr<CFX_RenderDevice> m_pDevice;
  UnownedPtr<CPDF_RenderContext> m_pContext;
  FX_RECT m_Rect;
  UnownedPtr<const CPDF_PageObject> m_pObject;
  std::unique_ptr<CFX_DefaultRenderDevice> m_pBitmapDevice;
  CFX_Matrix m_Matrix;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_SCALEDRENDERBUFFER_H_
