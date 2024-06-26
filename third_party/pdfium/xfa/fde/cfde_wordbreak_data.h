// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FDE_CFDE_WORDBREAK_DATA_H_
#define XFA_FDE_CFDE_WORDBREAK_DATA_H_

#include <stdint.h>

enum class WordBreakProperty : uint8_t {
  kNone = 0,
  kCR,
  kLF,
  kNewLine,
  kExtend,
  kFormat,
  kKataKana,
  kALetter,
  kMidLetter,
  kMidNum,
  kMidNumLet,
  kNumeric,
  kExtendNumLet,
};

extern const uint16_t gs_FX_WordBreak_Table[];
extern const uint8_t gs_FX_WordBreak_CodePointProperties[];

#endif  // XFA_FDE_CFDE_WORDBREAK_DATA_H_
