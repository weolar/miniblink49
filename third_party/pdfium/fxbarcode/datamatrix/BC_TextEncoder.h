// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_TEXTENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_TEXTENCODER_H_

#include "fxbarcode/datamatrix/BC_C40Encoder.h"

class CBC_TextEncoder final : public CBC_C40Encoder {
 public:
  CBC_TextEncoder();
  ~CBC_TextEncoder() override;

  // CBC_C40Encoder
  CBC_HighLevelEncoder::Encoding GetEncodingMode() override;
  int32_t EncodeChar(wchar_t c, WideString* sb) override;
};

#endif  // FXBARCODE_DATAMATRIX_BC_TEXTENCODER_H_
