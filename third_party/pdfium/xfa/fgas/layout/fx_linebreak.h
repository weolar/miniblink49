// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_LAYOUT_FX_LINEBREAK_H_
#define XFA_FGAS_LAYOUT_FX_LINEBREAK_H_

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/fx_unicode.h"

enum class FX_LINEBREAKTYPE : uint8_t {
  kUNKNOWN = 0x00,
  kDIRECT_BRK = 0x1A,
  kINDIRECT_BRK = 0x2B,
  kCOM_INDIRECT_BRK = 0x3C,
  kCOM_PROHIBITED_BRK = 0x4D,
  kPROHIBITED_BRK = 0x5E,
  kHANGUL_SPACE_BRK = 0x6F,
};

FX_LINEBREAKTYPE GetLineBreakTypeFromPair(FX_BREAKPROPERTY curr_char,
                                          FX_BREAKPROPERTY next_char);

#endif  // XFA_FGAS_LAYOUT_FX_LINEBREAK_H_
