// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/theme/cfwl_barcodetp.h"

#include "xfa/fwl/cfwl_barcode.h"
#include "xfa/fwl/cfwl_themebackground.h"
#include "xfa/fwl/cfwl_widget.h"

CFWL_BarcodeTP::CFWL_BarcodeTP() {}

CFWL_BarcodeTP::~CFWL_BarcodeTP() {}

void CFWL_BarcodeTP::DrawBackground(const CFWL_ThemeBackground& pParams) {
  switch (pParams.m_iPart) {
    case CFWL_Part::Border:
      DrawBorder(pParams.m_pGraphics.Get(), pParams.m_rtPart, pParams.m_matrix);
      break;
    case CFWL_Part::Background:
      FillBackground(pParams.m_pGraphics.Get(), pParams.m_rtPart,
                     pParams.m_matrix);
      break;
    default:
      break;
  }
}
