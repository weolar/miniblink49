// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_HIGHLEVELENCODER_H_
#define FXBARCODE_DATAMATRIX_BC_HIGHLEVELENCODER_H_

#include <vector>

#include "core/fxcrt/widestring.h"

class CBC_HighLevelEncoder {
 public:
  enum class Encoding : int8_t {
    UNKNOWN = -1,
    ASCII = 0,
    C40,
    TEXT,
    X12,
    EDIFACT,
    BASE256,
    LAST = BASE256,
  };

  CBC_HighLevelEncoder() = delete;
  ~CBC_HighLevelEncoder() = delete;

  // Returns an empty string on failure.
  static WideString EncodeHighLevel(const WideString& msg);

  static Encoding LookAheadTest(const WideString& msg,
                                size_t startpos,
                                Encoding currentMode);
  static bool IsExtendedASCII(wchar_t ch);

  static const wchar_t LATCH_TO_C40 = 230;
  static const wchar_t LATCH_TO_BASE256 = 231;
  static const wchar_t UPPER_SHIFT = 235;
  static const wchar_t LATCH_TO_ANSIX12 = 238;
  static const wchar_t LATCH_TO_TEXT = 239;
  static const wchar_t LATCH_TO_EDIFACT = 240;
  static const wchar_t C40_UNLATCH = 254;
  static const wchar_t X12_UNLATCH = 254;
};

#endif  // FXBARCODE_DATAMATRIX_BC_HIGHLEVELENCODER_H_
