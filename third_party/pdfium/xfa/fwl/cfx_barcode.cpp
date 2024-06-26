// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfx_barcode.h"

#include <memory>

#include "fxbarcode/cbc_codabar.h"
#include "fxbarcode/cbc_code128.h"
#include "fxbarcode/cbc_code39.h"
#include "fxbarcode/cbc_codebase.h"
#include "fxbarcode/cbc_datamatrix.h"
#include "fxbarcode/cbc_ean13.h"
#include "fxbarcode/cbc_ean8.h"
#include "fxbarcode/cbc_pdf417i.h"
#include "fxbarcode/cbc_qrcode.h"
#include "fxbarcode/cbc_upca.h"
#include "fxbarcode/utils.h"
#include "third_party/base/ptr_util.h"

namespace {

std::unique_ptr<CBC_CodeBase> CreateBarCodeEngineObject(BC_TYPE type) {
  switch (type) {
    case BC_CODE39:
      return pdfium::MakeUnique<CBC_Code39>();
    case BC_CODABAR:
      return pdfium::MakeUnique<CBC_Codabar>();
    case BC_CODE128:
      return pdfium::MakeUnique<CBC_Code128>(BC_CODE128_B);
    case BC_CODE128_B:
      return pdfium::MakeUnique<CBC_Code128>(BC_CODE128_B);
    case BC_CODE128_C:
      return pdfium::MakeUnique<CBC_Code128>(BC_CODE128_C);
    case BC_EAN8:
      return pdfium::MakeUnique<CBC_EAN8>();
    case BC_UPCA:
      return pdfium::MakeUnique<CBC_UPCA>();
    case BC_EAN13:
      return pdfium::MakeUnique<CBC_EAN13>();
    case BC_QR_CODE:
      return pdfium::MakeUnique<CBC_QRCode>();
    case BC_PDF417:
      return pdfium::MakeUnique<CBC_PDF417I>();
    case BC_DATAMATRIX:
      return pdfium::MakeUnique<CBC_DataMatrix>();
    case BC_UNKNOWN:
    default:
      return nullptr;
  }
}

}  // namespace

CFX_Barcode::CFX_Barcode() {}

CFX_Barcode::~CFX_Barcode() {}

std::unique_ptr<CFX_Barcode> CFX_Barcode::Create(BC_TYPE type) {
  auto barcode = pdfium::WrapUnique(new CFX_Barcode());  // Private ctor.
  barcode->m_pBCEngine = CreateBarCodeEngineObject(type);
  return barcode;
}

BC_TYPE CFX_Barcode::GetType() {
  return m_pBCEngine ? m_pBCEngine->GetType() : BC_UNKNOWN;
}

bool CFX_Barcode::SetCharEncoding(BC_CHAR_ENCODING encoding) {
  return m_pBCEngine ? m_pBCEngine->SetCharEncoding(encoding) : false;
}

bool CFX_Barcode::SetModuleHeight(int32_t moduleHeight) {
  return m_pBCEngine ? m_pBCEngine->SetModuleHeight(moduleHeight) : false;
}

bool CFX_Barcode::SetModuleWidth(int32_t moduleWidth) {
  return m_pBCEngine ? m_pBCEngine->SetModuleWidth(moduleWidth) : false;
}

bool CFX_Barcode::SetHeight(int32_t height) {
  return m_pBCEngine ? m_pBCEngine->SetHeight(height) : false;
}

bool CFX_Barcode::SetWidth(int32_t width) {
  return m_pBCEngine ? m_pBCEngine->SetWidth(width) : false;
}

bool CFX_Barcode::SetPrintChecksum(bool checksum) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine.get())
                                ->SetPrintChecksum(checksum),
                            true)
                         : false;
    default:
      return false;
  }
}

bool CFX_Barcode::SetDataLength(int32_t length) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine.get())
                                ->SetDataLength(length),
                            true)
                         : false;
    default:
      return false;
  }
}

bool CFX_Barcode::SetCalChecksum(bool state) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine.get())
                                ->SetCalChecksum(state),
                            true)
                         : false;
    default:
      return false;
  }
}

bool CFX_Barcode::SetFont(CFX_Font* pFont) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine
                 ? static_cast<CBC_OneCode*>(m_pBCEngine.get())->SetFont(pFont)
                 : false;
    default:
      return false;
  }
}

bool CFX_Barcode::SetFontSize(float size) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine.get())
                                ->SetFontSize(size),
                            true)
                         : false;
    default:
      return false;
  }
}

bool CFX_Barcode::SetFontColor(FX_ARGB color) {
  switch (GetType()) {
    case BC_CODE39:
    case BC_CODABAR:
    case BC_CODE128:
    case BC_CODE128_B:
    case BC_CODE128_C:
    case BC_EAN8:
    case BC_EAN13:
    case BC_UPCA:
      return m_pBCEngine ? (static_cast<CBC_OneCode*>(m_pBCEngine.get())
                                ->SetFontColor(color),
                            true)
                         : false;
    default:
      return false;
  }
}

bool CFX_Barcode::SetTextLocation(BC_TEXT_LOC location) {
  return m_pBCEngine && m_pBCEngine->SetTextLocation(location);
}

bool CFX_Barcode::SetWideNarrowRatio(int8_t ratio) {
  return m_pBCEngine && m_pBCEngine->SetWideNarrowRatio(ratio);
}

bool CFX_Barcode::SetStartChar(char start) {
  return m_pBCEngine && m_pBCEngine->SetStartChar(start);
}

bool CFX_Barcode::SetEndChar(char end) {
  return m_pBCEngine && m_pBCEngine->SetEndChar(end);
}

bool CFX_Barcode::SetErrorCorrectionLevel(int32_t level) {
  return m_pBCEngine && m_pBCEngine->SetErrorCorrectionLevel(level);
}

bool CFX_Barcode::Encode(WideStringView contents) {
  return m_pBCEngine && m_pBCEngine->Encode(contents);
}

bool CFX_Barcode::RenderDevice(CFX_RenderDevice* device,
                               const CFX_Matrix* matrix) {
  return m_pBCEngine && m_pBCEngine->RenderDevice(device, matrix);
}
