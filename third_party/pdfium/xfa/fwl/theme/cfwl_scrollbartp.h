// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_THEME_CFWL_SCROLLBARTP_H_
#define XFA_FWL_THEME_CFWL_SCROLLBARTP_H_

#include <memory>

#include "xfa/fwl/theme/cfwl_widgettp.h"

class CFWL_ScrollBarTP final : public CFWL_WidgetTP {
 public:
  CFWL_ScrollBarTP();
  ~CFWL_ScrollBarTP() override;

  // CFWL_WidgetTP
  void DrawBackground(const CFWL_ThemeBackground& pParams) override;

 private:
  struct SBThemeData {
    FX_ARGB clrTrackBKStart;
    FX_ARGB clrTrackBKEnd;
    FX_ARGB clrPawColorLight[4];
    FX_ARGB clrPawColorDark[4];
    FX_ARGB clrBtnBK[4][2];
    FX_ARGB clrBtnBorder[4];
  };

  void DrawThumbBtn(CXFA_Graphics* pGraphics,
                    const CFX_RectF& rect,
                    bool bVert,
                    FWLTHEME_STATE eState,
                    bool bPawButton,
                    const CFX_Matrix& matrix);
  void DrawTrack(CXFA_Graphics* pGraphics,
                 const CFX_RectF& rect,
                 bool bVert,
                 FWLTHEME_STATE eState,
                 bool bLowerTrack,
                 const CFX_Matrix& matrix);
  void DrawMaxMinBtn(CXFA_Graphics* pGraphics,
                     const CFX_RectF& rect,
                     FWLTHEME_DIRECTION eDict,
                     FWLTHEME_STATE eState,
                     const CFX_Matrix& matrix);
  void DrawPaw(CXFA_Graphics* pGraphics,
               const CFX_RectF& rect,
               bool bVert,
               FWLTHEME_STATE eState,
               const CFX_Matrix& matrix);
  void SetThemeData();

  std::unique_ptr<SBThemeData> m_pThemeData;
};

#endif  // XFA_FWL_THEME_CFWL_SCROLLBARTP_H_
