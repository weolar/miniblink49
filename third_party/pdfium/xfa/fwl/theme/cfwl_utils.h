// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_THEME_CFWL_UTILS_H_
#define XFA_FWL_THEME_CFWL_UTILS_H_

#include "core/fxcrt/fx_system.h"

enum FWLTHEME_EDGE {
  FWLTHEME_EDGE_Flat = 0,
  FWLTHEME_EDGE_Raised,
  FWLTHEME_EDGE_Sunken
};

enum FWLTHEME_STATE {
  FWLTHEME_STATE_Normal = 1,
  FWLTHEME_STATE_Hover,
  FWLTHEME_STATE_Pressed,
  FWLTHEME_STATE_Disable
};

enum FWLTHEME_DIRECTION {
  FWLTHEME_DIRECTION_Up = 0,
  FWLTHEME_DIRECTION_Down,
  FWLTHEME_DIRECTION_Left,
  FWLTHEME_DIRECTION_Right
};

#define FWLTHEME_COLOR_EDGERB1 (ArgbEncode(255, 241, 239, 226))
#define FWLTHEME_COLOR_Background (ArgbEncode(255, 236, 233, 216))
#define FWLTHEME_COLOR_BKSelected (ArgbEncode(255, 153, 193, 218))

#define FWLTHEME_CAPACITY_FontSize 12.0f
#define FWLTHEME_CAPACITY_TextColor (ArgbEncode(255, 0, 0, 0))
#define FWLTHEME_CAPACITY_TextDisColor (ArgbEncode(255, 172, 168, 153))

#endif  // XFA_FWL_THEME_CFWL_UTILS_H_
