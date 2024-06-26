// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FGAS_FONT_CFGAS_DEFAULTFONTMANAGER_H_
#define XFA_FGAS_FONT_CFGAS_DEFAULTFONTMANAGER_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/retain_ptr.h"

class CFGAS_GEFont;
class CFGAS_FontMgr;

class CFGAS_DefaultFontManager {
 public:
  static RetainPtr<CFGAS_GEFont> GetFont(CFGAS_FontMgr* pFontMgr,
                                         WideStringView wsFontFamily,
                                         uint32_t dwFontStyles);
  static RetainPtr<CFGAS_GEFont> GetDefaultFont(CFGAS_FontMgr* pFontMgr,
                                                WideStringView wsFontFamily,
                                                uint32_t dwFontStyles);

  CFGAS_DefaultFontManager() = delete;
  CFGAS_DefaultFontManager(const CFGAS_DefaultFontManager&) = delete;
  CFGAS_DefaultFontManager& operator=(const CFGAS_DefaultFontManager&) = delete;
};

#endif  // XFA_FGAS_FONT_CFGAS_DEFAULTFONTMANAGER_H_
