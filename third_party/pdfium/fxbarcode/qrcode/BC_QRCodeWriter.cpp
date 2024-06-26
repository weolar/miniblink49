// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2008 ZXing authors
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

#include "fxbarcode/qrcode/BC_QRCodeWriter.h"

#include "fxbarcode/common/BC_CommonByteMatrix.h"
#include "fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "fxbarcode/qrcode/BC_QRCoder.h"
#include "fxbarcode/qrcode/BC_QRCoderEncoder.h"
#include "fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "fxbarcode/qrcode/BC_QRCoderMode.h"
#include "fxbarcode/qrcode/BC_QRCoderVersion.h"
#include "third_party/base/stl_util.h"

CBC_QRCodeWriter::CBC_QRCodeWriter() : CBC_TwoDimWriter(true) {}

CBC_QRCodeWriter::~CBC_QRCodeWriter() = default;

bool CBC_QRCodeWriter::SetErrorCorrectionLevel(int32_t level) {
  if (level < 0 || level > 3) {
    return false;
  }
  set_error_correction_level(level);
  return true;
}

std::vector<uint8_t> CBC_QRCodeWriter::Encode(WideStringView contents,
                                              int32_t ecLevel,
                                              int32_t* pOutWidth,
                                              int32_t* pOutHeight) {
  std::vector<uint8_t> results;
  CBC_QRCoderErrorCorrectionLevel* ec = nullptr;
  switch (ecLevel) {
    case 0:
      ec = CBC_QRCoderErrorCorrectionLevel::L;
      break;
    case 1:
      ec = CBC_QRCoderErrorCorrectionLevel::M;
      break;
    case 2:
      ec = CBC_QRCoderErrorCorrectionLevel::Q;
      break;
    case 3:
      ec = CBC_QRCoderErrorCorrectionLevel::H;
      break;
    default:
      return results;
  }
  CBC_QRCoder qr;
  if (!CBC_QRCoderEncoder::Encode(contents, ec, &qr))
    return results;

  *pOutWidth = qr.GetMatrixWidth();
  *pOutHeight = qr.GetMatrixWidth();
  results = pdfium::Vector2D<uint8_t>(*pOutWidth, *pOutHeight);
  memcpy(results.data(), qr.GetMatrix()->GetArray().data(),
         *pOutWidth * *pOutHeight);
  return results;
}
