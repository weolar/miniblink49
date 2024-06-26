// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_X12ENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_X12ENCODER_H_

#include "fxbarcode/datamatrix/BC_C40Encoder.h"

class CBC_X12Encoder final : public CBC_C40Encoder {
 public:
  CBC_X12Encoder();
  ~CBC_X12Encoder() override;

  // CBC_C40Encoder
  CBC_HighLevelEncoder::Encoding GetEncodingMode() override;
  bool Encode(CBC_EncoderContext* context) override;
  bool HandleEOD(CBC_EncoderContext* context, WideString* buffer) override;
  int32_t EncodeChar(wchar_t c, WideString* sb) override;
};

#endif  // FXBARCODE_DATAMATRIX_BC_X12ENCODER_H_
