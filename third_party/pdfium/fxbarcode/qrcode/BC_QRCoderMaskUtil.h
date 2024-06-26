// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_QRCODE_BC_QRCODERMASKUTIL_H_
#define FXBARCODE_QRCODE_BC_QRCODERMASKUTIL_H_

class CBC_CommonByteMatrix;

class CBC_QRCoderMaskUtil {
 public:
  CBC_QRCoderMaskUtil() = delete;
  ~CBC_QRCoderMaskUtil() = delete;

  static bool GetDataMaskBit(int32_t maskPattern, int32_t x, int32_t y);

  static int32_t ApplyMaskPenaltyRule1(CBC_CommonByteMatrix* matrix);
  static int32_t ApplyMaskPenaltyRule2(CBC_CommonByteMatrix* matrix);
  static int32_t ApplyMaskPenaltyRule3(CBC_CommonByteMatrix* matrix);
  static int32_t ApplyMaskPenaltyRule4(CBC_CommonByteMatrix* matrix);
};

#endif  // FXBARCODE_QRCODE_BC_QRCODERMASKUTIL_H_
