// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_ASCIIENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_ASCIIENCODER_H_

#include "fxbarcode/datamatrix/BC_Encoder.h"

class CBC_EncoderContext;

class CBC_ASCIIEncoder final : public CBC_Encoder {
 public:
  CBC_ASCIIEncoder();
  ~CBC_ASCIIEncoder() override;

  // CBC_Encoder
  CBC_HighLevelEncoder::Encoding GetEncodingMode() override;
  bool Encode(CBC_EncoderContext* context) override;
};

#endif  // FXBARCODE_DATAMATRIX_BC_ASCIIENCODER_H_
