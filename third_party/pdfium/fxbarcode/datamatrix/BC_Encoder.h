// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_ENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_ENCODER_H_

#include "fxbarcode/datamatrix/BC_HighLevelEncoder.h"

class CBC_EncoderContext;

class CBC_Encoder {
 public:
  CBC_Encoder();
  virtual ~CBC_Encoder();

  virtual CBC_HighLevelEncoder::Encoding GetEncodingMode() = 0;
  virtual bool Encode(CBC_EncoderContext* context) = 0;
};

#endif  // FXBARCODE_DATAMATRIX_BC_ENCODER_H_
