// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_picturebox.h"

#include <memory>

#include "third_party/base/ptr_util.h"

CFWL_PictureBox::CFWL_PictureBox(const CFWL_App* app)
    : CFWL_Widget(app, pdfium::MakeUnique<CFWL_WidgetProperties>(), nullptr) {
}

CFWL_PictureBox::~CFWL_PictureBox() {}

FWL_Type CFWL_PictureBox::GetClassID() const {
  return FWL_Type::PictureBox;
}

void CFWL_PictureBox::Update() {
  if (IsLocked())
    return;
  if (!m_pProperties->m_pThemeProvider)
    m_pProperties->m_pThemeProvider = GetAvailableTheme();

  m_rtClient = GetClientRect();
}

void CFWL_PictureBox::DrawWidget(CXFA_Graphics* pGraphics,
                                 const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;
  if (!m_pProperties->m_pThemeProvider)
    return;

  IFWL_ThemeProvider* pTheme = GetAvailableTheme();
  if (HasBorder())
    DrawBorder(pGraphics, CFWL_Part::Border, pTheme, matrix);
}

void CFWL_PictureBox::OnDrawWidget(CXFA_Graphics* pGraphics,
                                   const CFX_Matrix& matrix) {
  DrawWidget(pGraphics, matrix);
}
