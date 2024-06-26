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

#include "fxbarcode/cbc_ean13.h"

#include <memory>

#include "fxbarcode/oned/BC_OnedEAN13Writer.h"
#include "third_party/base/ptr_util.h"

CBC_EAN13::CBC_EAN13()
    : CBC_EANCode(pdfium::MakeUnique<CBC_OnedEAN13Writer>()) {}

CBC_EAN13::~CBC_EAN13() = default;

BC_TYPE CBC_EAN13::GetType() {
  return BC_EAN13;
}

BCFORMAT CBC_EAN13::GetFormat() const {
  return BCFORMAT_EAN_13;
}

size_t CBC_EAN13::GetMaxLength() const {
  return 12;
}
