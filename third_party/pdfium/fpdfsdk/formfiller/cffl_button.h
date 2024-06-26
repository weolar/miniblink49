// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_FORMFILLER_CFFL_BUTTON_H_
#define FPDFSDK_FORMFILLER_CFFL_BUTTON_H_

#include "core/fxcrt/fx_coordinates.h"
#include "fpdfsdk/formfiller/cffl_formfiller.h"

class CFX_RenderDevice;
class CFX_Matrix;
class CPDFSDK_Annot;
class CPDFSDK_FormFillEnvironment;
class CPDFSDK_PageView;
class CPDFSDK_Widget;

class CFFL_Button : public CFFL_FormFiller {
 public:
  CFFL_Button(CPDFSDK_FormFillEnvironment* pFormFillEnv,
              CPDFSDK_Widget* pWidget);
  ~CFFL_Button() override;

  // CFFL_FormFiller
  void OnMouseEnter(CPDFSDK_PageView* pPageView,
                    CPDFSDK_Annot* pAnnot) override;
  void OnMouseExit(CPDFSDK_PageView* pPageView, CPDFSDK_Annot* pAnnot) override;
  bool OnLButtonDown(CPDFSDK_PageView* pPageView,
                     CPDFSDK_Annot* pAnnot,
                     uint32_t nFlags,
                     const CFX_PointF& point) override;
  bool OnLButtonUp(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot* pAnnot,
                   uint32_t nFlags,
                   const CFX_PointF& point) override;
  bool OnMouseMove(CPDFSDK_PageView* pPageView,
                   CPDFSDK_Annot* pAnnot,
                   uint32_t nFlags,
                   const CFX_PointF& point) override;
  void OnDraw(CPDFSDK_PageView* pPageView,
              CPDFSDK_Annot* pAnnot,
              CFX_RenderDevice* pDevice,
              const CFX_Matrix& mtUser2Device) override;
  void OnDrawDeactive(CPDFSDK_PageView* pPageView,
                      CPDFSDK_Annot* pAnnot,
                      CFX_RenderDevice* pDevice,
                      const CFX_Matrix& mtUser2Device) override;

 private:
  bool m_bMouseIn;
  bool m_bMouseDown;
};

#endif  // FPDFSDK_FORMFILLER_CFFL_BUTTON_H_
