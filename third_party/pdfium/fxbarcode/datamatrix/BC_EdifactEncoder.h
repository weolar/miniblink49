// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_EDIFACTENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_EDIFACTENCODER_H_

#include "fxbarcode/datamatrix/BC_Encoder.h"

class CBC_EdifactEncoder final : public CBC_Encoder {
 public:
  CBC_EdifactEncoder();
  ~CBC_EdifactEncoder() override;

  // CBC_Encoder
  CBC_HighLevelEncoder::Encoding GetEncodingMode() override;
  bool Encode(CBC_EncoderContext* context) override;
};

#endif  // FXBARCODE_DATAMATRIX_BC_EDIFACTENCODER_H_
