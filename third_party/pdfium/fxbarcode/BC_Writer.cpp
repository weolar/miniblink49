// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxbarcode/BC_Writer.h"

CBC_Writer::CBC_Writer() = default;

CBC_Writer::~CBC_Writer() = default;

bool CBC_Writer::SetCharEncoding(int32_t encoding) {
  m_CharEncoding = encoding;
  return true;
}

bool CBC_Writer::SetModuleHeight(int32_t moduleHeight) {
  if (moduleHeight > 10 || moduleHeight < 1)
    return false;

  m_ModuleHeight = moduleHeight;
  return true;
}

bool CBC_Writer::SetModuleWidth(int32_t moduleWidth) {
  if (moduleWidth > 10 || moduleWidth < 1)
    return false;

  m_ModuleWidth = moduleWidth;
  return true;
}

bool CBC_Writer::SetHeight(int32_t height) {
  m_Height = height;
  return true;
}

bool CBC_Writer::SetWidth(int32_t width) {
  m_Width = width;
  return true;
}

void CBC_Writer::SetBackgroundColor(FX_ARGB backgroundColor) {
  m_backgroundColor = backgroundColor;
}

void CBC_Writer::SetBarcodeColor(FX_ARGB foregroundColor) {
  m_barColor = foregroundColor;
}

bool CBC_Writer::SetTextLocation(BC_TEXT_LOC location) {
  return false;
}

bool CBC_Writer::SetWideNarrowRatio(int8_t ratio) {
  return false;
}

bool CBC_Writer::SetStartChar(char start) {
  return false;
}

bool CBC_Writer::SetEndChar(char end) {
  return false;
}

bool CBC_Writer::SetErrorCorrectionLevel(int32_t level) {
  return false;
}
