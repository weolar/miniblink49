// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_FX_UNICODE_H_
#define CORE_FXCRT_FX_UNICODE_H_

#include "core/fxcrt/fx_system.h"

// NOTE: Order matters, less-than/greater-than comparisons are used.
enum class FX_BIDICLASS : uint8_t {
  kON = 0,    // Other Neutral
  kL = 1,     // Left Letter
  kR = 2,     // Right Letter
  kAN = 3,    // Arabic Number
  kEN = 4,    // European Number
  kAL = 5,    // Arabic Letter
  kNSM = 6,   // Non-spacing Mark
  kCS = 7,    // Common Number Separator
  kES = 8,    // European Separator
  kET = 9,    // European Number Terminator
  kBN = 10,   // Boundary Neutral
  kS = 11,    // Segment Separator
  kWS = 12,   // Whitespace
  kB = 13,    // Paragraph Separator
  kRLO = 14,  // Right-to-Left Override
  kRLE = 15,  // Right-to-Left Embedding
  kLRO = 16,  // Left-to-Right Override
  kLRE = 17,  // Left-to-Right Embedding
  kPDF = 18,  // Pop Directional Format
  kN = kON,
};

wchar_t FX_GetMirrorChar(wchar_t wch);
FX_BIDICLASS FX_GetBidiClass(wchar_t wch);

#if 1 // def PDF_ENABLE_XFA // weolar
// As defined in http://www.unicode.org/reports/tr14
enum class FX_BREAKPROPERTY : uint8_t {
  kOP = 0,
  kCL = 1,
  kQU = 2,
  kGL = 3,
  kNS = 4,
  kEX = 5,
  kSY = 6,
  kIS = 7,
  kPR = 8,
  kPO = 9,
  kNU = 10,
  kAL = 11,
  kID = 12,
  kIN = 13,
  kHY = 14,
  kBA = 15,
  kBB = 16,
  kB2 = 17,
  kZW = 18,
  kCM = 19,
  kWJ = 20,
  kH2 = 21,
  kH3 = 22,
  kJL = 23,
  kJV = 24,
  kJT = 25,
  kBK = 26,
  kCR = 27,
  kLF = 28,
  kNL = 29,
  kSA = 30,
  kSG = 31,
  kCB = 32,
  kXX = 33,
  kAI = 34,
  kSP = 35,
  kNONE = 36,
  kTB = 37,
};

enum class FX_CHARTYPE : uint8_t {
  kUnknown = 0,
  kTab,
  kSpace,
  kControl,
  kCombination,
  kNumeric,
  kNormal,
  kArabicAlef,
  kArabicSpecial,
  kArabicDistortion,
  kArabicNormal,
  kArabicForm,
  kArabic,
};

FX_CHARTYPE FX_GetCharType(wchar_t wch);

// Analagous to ULineBreak in icu's uchar.h, but permuted order, and a
// subset lacking some more recent additions.
FX_BREAKPROPERTY FX_GetBreakProperty(wchar_t wch);
#endif  // PDF_ENABLE_XFA

#endif  // CORE_FXCRT_FX_UNICODE_H_
