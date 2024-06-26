// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_C40ENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_C40ENCODER_H_

#include "core/fxcrt/widestring.h"
#include "fxbarcode/datamatrix/BC_Encoder.h"

class CBC_C40Encoder : public CBC_Encoder {
 public:
  CBC_C40Encoder();
  ~CBC_C40Encoder() override;

  // CBC_Encoder
  CBC_HighLevelEncoder::Encoding GetEncodingMode() override;
  bool Encode(CBC_EncoderContext* context) override;

  static void WriteNextTriplet(CBC_EncoderContext* context, WideString* buffer);

  virtual bool HandleEOD(CBC_EncoderContext* context, WideString* buffer);

  // Returns the number of characters appended to |sb|, or 0 on failure.
  virtual int32_t EncodeChar(wchar_t c, WideString* sb);

 private:
  // Moves back by 1 position in |context| and adjusts |buffer| accordingly
  // using |lastCharSize|. Returns the length of the current character in
  // |context| after adjusting the position. If the character cannot be encoded,
  // return -1.
  int32_t BacktrackOneCharacter(CBC_EncoderContext* context,
                                WideString* buffer,
                                int32_t lastCharSize);
};

#endif  // FXBARCODE_DATAMATRIX_BC_C40ENCODER_H_
