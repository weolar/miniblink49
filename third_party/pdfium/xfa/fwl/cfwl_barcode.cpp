// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_barcode.h"

#include <utility>

#include "third_party/base/ptr_util.h"
#include "xfa/fgas/font/cfgas_gefont.h"
#include "xfa/fwl/cfwl_notedriver.h"
#include "xfa/fwl/cfwl_themepart.h"
#include "xfa/fwl/cfx_barcode.h"
#include "xfa/fwl/ifwl_themeprovider.h"
#include "xfa/fwl/theme/cfwl_utils.h"

CFWL_Barcode::CFWL_Barcode(const CFWL_App* app)
    : CFWL_Edit(app, pdfium::MakeUnique<CFWL_WidgetProperties>(), nullptr),
      m_dwStatus(0),
      m_type(BC_UNKNOWN),
      m_dwAttributeMask(FWL_BCDATTRIBUTE_NONE) {}

CFWL_Barcode::~CFWL_Barcode() {}

FWL_Type CFWL_Barcode::GetClassID() const {
  return FWL_Type::Barcode;
}

void CFWL_Barcode::Update() {
  if (IsLocked())
    return;

  CFWL_Edit::Update();
  GenerateBarcodeImageCache();
}

void CFWL_Barcode::DrawWidget(CXFA_Graphics* pGraphics,
                              const CFX_Matrix& matrix) {
  if (!pGraphics)
    return;
  if (!m_pProperties->m_pThemeProvider)
    return;
  if ((m_pProperties->m_dwStates & FWL_WGTSTATE_Focused) == 0) {
    GenerateBarcodeImageCache();
    if (!m_pBarcodeEngine || (m_dwStatus & XFA_BCS_EncodeSuccess) == 0)
      return;

    CFX_Matrix mt;
    mt.e = GetRTClient().left;
    mt.f = GetRTClient().top;
    mt.Concat(matrix);

    m_pBarcodeEngine->RenderDevice(pGraphics->GetRenderDevice(), &matrix);
    return;
  }
  CFWL_Edit::DrawWidget(pGraphics, matrix);
}

void CFWL_Barcode::SetType(BC_TYPE type) {
  if (m_type == type)
    return;

  m_pBarcodeEngine.reset();
  m_type = type;
  m_dwStatus = XFA_BCS_NeedUpdate;
}

void CFWL_Barcode::SetText(const WideString& wsText,
                           CFDE_TextEditEngine::RecordOperation op) {
  m_pBarcodeEngine.reset();
  m_dwStatus = XFA_BCS_NeedUpdate;
  CFWL_Edit::SetText(wsText, op);
}

bool CFWL_Barcode::IsProtectedType() const {
  if (!m_pBarcodeEngine)
    return true;

  BC_TYPE tEngineType = m_pBarcodeEngine->GetType();
  if (tEngineType == BC_QR_CODE || tEngineType == BC_PDF417 ||
      tEngineType == BC_DATAMATRIX) {
    return true;
  }
  return false;
}

void CFWL_Barcode::OnProcessEvent(CFWL_Event* pEvent) {
  if (pEvent->GetType() == CFWL_Event::Type::TextWillChange) {
    m_pBarcodeEngine.reset();
    m_dwStatus = XFA_BCS_NeedUpdate;
  }
  CFWL_Edit::OnProcessEvent(pEvent);
}

void CFWL_Barcode::SetCharEncoding(BC_CHAR_ENCODING encoding) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_CHARENCODING;
  m_eCharEncoding = encoding;
}

void CFWL_Barcode::SetModuleHeight(int32_t height) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_MODULEHEIGHT;
  m_nModuleHeight = height;
}

void CFWL_Barcode::SetModuleWidth(int32_t width) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_MODULEWIDTH;
  m_nModuleWidth = width;
}

void CFWL_Barcode::SetDataLength(int32_t dataLength) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_DATALENGTH;
  m_nDataLength = dataLength;
  SetLimit(dataLength);
}

void CFWL_Barcode::SetCalChecksum(bool calChecksum) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_CALCHECKSUM;
  m_bCalChecksum = calChecksum;
}

void CFWL_Barcode::SetPrintChecksum(bool printChecksum) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_PRINTCHECKSUM;
  m_bPrintChecksum = printChecksum;
}

void CFWL_Barcode::SetTextLocation(BC_TEXT_LOC location) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_TEXTLOCATION;
  m_eTextLocation = location;
}

void CFWL_Barcode::SetWideNarrowRatio(int8_t ratio) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_WIDENARROWRATIO;
  m_nWideNarrowRatio = ratio;
}

void CFWL_Barcode::SetStartChar(char startChar) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_STARTCHAR;
  m_cStartChar = startChar;
}

void CFWL_Barcode::SetEndChar(char endChar) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_ENDCHAR;
  m_cEndChar = endChar;
}

void CFWL_Barcode::SetErrorCorrectionLevel(int32_t ecLevel) {
  m_dwAttributeMask |= FWL_BCDATTRIBUTE_ECLEVEL;
  m_nECLevel = ecLevel;
}

void CFWL_Barcode::GenerateBarcodeImageCache() {
  if ((m_dwStatus & XFA_BCS_NeedUpdate) == 0)
    return;

  m_dwStatus = 0;
  CreateBarcodeEngine();
  if (!m_pBarcodeEngine)
    return;

  IFWL_ThemeProvider* pTheme = GetAvailableTheme();
  if (pTheme) {
    CFWL_ThemePart part;
    part.m_pWidget = this;
    if (RetainPtr<CFGAS_GEFont> pFont = pTheme->GetFont(part)) {
      if (CFX_Font* pCXFont = pFont->GetDevFont())
        m_pBarcodeEngine->SetFont(pCXFont);
    }
    m_pBarcodeEngine->SetFontSize(pTheme->GetFontSize(part));
    m_pBarcodeEngine->SetFontColor(pTheme->GetTextColor(part));
  } else {
    m_pBarcodeEngine->SetFontSize(FWLTHEME_CAPACITY_FontSize);
  }

  m_pBarcodeEngine->SetHeight(int32_t(GetRTClient().height));
  m_pBarcodeEngine->SetWidth(int32_t(GetRTClient().width));
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_CHARENCODING)
    m_pBarcodeEngine->SetCharEncoding(m_eCharEncoding);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_MODULEHEIGHT)
    m_pBarcodeEngine->SetModuleHeight(m_nModuleHeight);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_MODULEWIDTH)
    m_pBarcodeEngine->SetModuleWidth(m_nModuleWidth);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_DATALENGTH)
    m_pBarcodeEngine->SetDataLength(m_nDataLength);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_CALCHECKSUM)
    m_pBarcodeEngine->SetCalChecksum(m_bCalChecksum);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_PRINTCHECKSUM)
    m_pBarcodeEngine->SetPrintChecksum(m_bPrintChecksum);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_TEXTLOCATION)
    m_pBarcodeEngine->SetTextLocation(m_eTextLocation);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_WIDENARROWRATIO)
    m_pBarcodeEngine->SetWideNarrowRatio(m_nWideNarrowRatio);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_STARTCHAR)
    m_pBarcodeEngine->SetStartChar(m_cStartChar);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_ENDCHAR)
    m_pBarcodeEngine->SetEndChar(m_cEndChar);
  if (m_dwAttributeMask & FWL_BCDATTRIBUTE_ECLEVEL)
    m_pBarcodeEngine->SetErrorCorrectionLevel(m_nECLevel);

  m_dwStatus = m_pBarcodeEngine->Encode(GetText().AsStringView())
                   ? XFA_BCS_EncodeSuccess
                   : 0;
}

void CFWL_Barcode::CreateBarcodeEngine() {
  if (m_pBarcodeEngine || m_type == BC_UNKNOWN)
    return;

  m_pBarcodeEngine = CFX_Barcode::Create(m_type);
}
