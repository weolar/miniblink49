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

#include "fxbarcode/cbc_pdf417i.h"

#include <vector>

#include "fxbarcode/pdf417/BC_PDF417Writer.h"
#include "third_party/base/ptr_util.h"

CBC_PDF417I::CBC_PDF417I()
    : CBC_CodeBase(pdfium::MakeUnique<CBC_PDF417Writer>()) {}

CBC_PDF417I::~CBC_PDF417I() {}

bool CBC_PDF417I::Encode(WideStringView contents) {
  int32_t width;
  int32_t height;
  auto* pWriter = GetPDF417Writer();
  std::vector<uint8_t> data = pWriter->Encode(contents, &width, &height);
  return pWriter->RenderResult(data, width, height);
}

bool CBC_PDF417I::RenderDevice(CFX_RenderDevice* device,
                               const CFX_Matrix* matrix) {
  GetPDF417Writer()->RenderDeviceResult(device, matrix);
  return true;
}

BC_TYPE CBC_PDF417I::GetType() {
  return BC_PDF417;
}

CBC_PDF417Writer* CBC_PDF417I::GetPDF417Writer() {
  return static_cast<CBC_PDF417Writer*>(m_pBCWriter.get());
}
