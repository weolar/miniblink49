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

#include "fxbarcode/cbc_code39.h"

#include <memory>

#include "fxbarcode/oned/BC_OnedCode39Writer.h"
#include "third_party/base/ptr_util.h"

CBC_Code39::CBC_Code39()
    : CBC_OneCode(pdfium::MakeUnique<CBC_OnedCode39Writer>()) {}

CBC_Code39::~CBC_Code39() {}

bool CBC_Code39::Encode(WideStringView contents) {
  if (contents.IsEmpty())
    return false;

  BCFORMAT format = BCFORMAT_CODE_39;
  int32_t outWidth = 0;
  int32_t outHeight = 0;
  auto* pWriter = GetOnedCode39Writer();
  WideString filtercontents = pWriter->FilterContents(contents);
  m_renderContents = pWriter->RenderTextContents(contents);
  ByteString byteString = filtercontents.ToUTF8();
  std::unique_ptr<uint8_t, FxFreeDeleter> data(
      pWriter->Encode(byteString, format, outWidth, outHeight));
  return data && pWriter->RenderResult(m_renderContents.AsStringView(),
                                       data.get(), outWidth);
}

bool CBC_Code39::RenderDevice(CFX_RenderDevice* device,
                              const CFX_Matrix* matrix) {
  auto* pWriter = GetOnedCode39Writer();
  WideString renderCon;
  if (!pWriter->encodedContents(m_renderContents.AsStringView(), &renderCon))
    return false;
  return pWriter->RenderDeviceResult(device, matrix, renderCon.AsStringView());
}

BC_TYPE CBC_Code39::GetType() {
  return BC_CODE39;
}

CBC_OnedCode39Writer* CBC_Code39::GetOnedCode39Writer() {
  return static_cast<CBC_OnedCode39Writer*>(m_pBCWriter.get());
}
