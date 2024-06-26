// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
/*
 * Copyright 2011 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fxbarcode/cbc_onecode.h"

#include <utility>

#include "fxbarcode/oned/BC_OneDimWriter.h"

CBC_OneCode::CBC_OneCode(std::unique_ptr<CBC_Writer> pWriter)
    : CBC_CodeBase(std::move(pWriter)) {}

CBC_OneCode::~CBC_OneCode() {}

bool CBC_OneCode::CheckContentValidity(WideStringView contents) {
  return GetOneDimWriter()->CheckContentValidity(contents);
}

WideString CBC_OneCode::FilterContents(WideStringView contents) {
  return GetOneDimWriter()->FilterContents(contents);
}

void CBC_OneCode::SetPrintChecksum(bool checksum) {
  GetOneDimWriter()->SetPrintChecksum(checksum);
}

void CBC_OneCode::SetDataLength(int32_t length) {
  GetOneDimWriter()->SetDataLength(length);
}

void CBC_OneCode::SetCalChecksum(bool calc) {
  GetOneDimWriter()->SetCalcChecksum(calc);
}

bool CBC_OneCode::SetFont(CFX_Font* cFont) {
  return GetOneDimWriter()->SetFont(cFont);
}

void CBC_OneCode::SetFontSize(float size) {
  GetOneDimWriter()->SetFontSize(size);
}

void CBC_OneCode::SetFontStyle(int32_t style) {
  GetOneDimWriter()->SetFontStyle(style);
}

void CBC_OneCode::SetFontColor(FX_ARGB color) {
  GetOneDimWriter()->SetFontColor(color);
}

CBC_OneDimWriter* CBC_OneCode::GetOneDimWriter() {
  return static_cast<CBC_OneDimWriter*>(m_pBCWriter.get());
}
