// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_QRCODE_BC_QRCODERMATRIXUTIL_H_
#define FXBARCODE_QRCODE_BC_QRCODERMATRIXUTIL_H_

#include <stdint.h>

class CBC_CommonByteMatrix;
class CBC_QRCoderErrorCorrectionLevel;
class CBC_QRCoderBitVector;

class CBC_QRCoderMatrixUtil {
 public:
  CBC_QRCoderMatrixUtil() = delete;
  ~CBC_QRCoderMatrixUtil() = delete;

  static bool BuildMatrix(CBC_QRCoderBitVector* dataBits,
                          const CBC_QRCoderErrorCorrectionLevel* ecLevel,
                          int32_t version,
                          int32_t maskPattern,
                          CBC_CommonByteMatrix* matrix);
};

#endif  // FXBARCODE_QRCODE_BC_QRCODERMATRIXUTIL_H_
