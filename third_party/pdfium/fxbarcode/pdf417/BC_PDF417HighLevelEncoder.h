// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_PDF417_BC_PDF417HIGHLEVELENCODER_H_
#define FXBARCODE_PDF417_BC_PDF417HIGHLEVELENCODER_H_

#include <vector>

#include "core/fxcrt/fx_string.h"
#include "fxbarcode/pdf417/BC_PDF417.h"

#include "third_party/base/optional.h"

class CBC_PDF417HighLevelEncoder {
 public:
  CBC_PDF417HighLevelEncoder() = delete;
  ~CBC_PDF417HighLevelEncoder() = delete;

  static Optional<WideString> EncodeHighLevel(WideStringView msg);

 private:
  enum class EncodingMode { kUnknown = 0, kText, kByte, kNumeric };

  enum class SubMode { kAlpha = 0, kLower, kMixed, kPunctuation };

  static SubMode EncodeText(const WideString& msg,
                            size_t startpos,
                            size_t count,
                            SubMode initialSubmode,
                            WideString* sb);
  static void EncodeBinary(const std::vector<uint8_t>& bytes,
                           size_t startpos,
                           size_t count,
                           EncodingMode startmode,
                           WideString* sb);
  static void EncodeNumeric(const WideString& msg,
                            size_t startpos,
                            size_t count,
                            WideString* sb);
  static size_t DetermineConsecutiveDigitCount(WideString msg, size_t startpos);
  static size_t DetermineConsecutiveTextCount(WideString msg, size_t startpos);
  static Optional<size_t> DetermineConsecutiveBinaryCount(
      WideString msg,
      std::vector<uint8_t>* bytes,
      size_t startpos);

  friend class PDF417HighLevelEncoderTest_EncodeNumeric_Test;
  friend class PDF417HighLevelEncoderTest_EncodeBinary_Test;
  friend class PDF417HighLevelEncoderTest_EncodeText_Test;
  friend class PDF417HighLevelEncoderTest_ConsecutiveDigitCount_Test;
  friend class PDF417HighLevelEncoderTest_ConsecutiveTextCount_Test;
  friend class PDF417HighLevelEncoderTest_ConsecutiveBinaryCount_Test;
};

#endif  // FXBARCODE_PDF417_BC_PDF417HIGHLEVELENCODER_H_
