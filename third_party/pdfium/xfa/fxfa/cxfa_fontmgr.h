// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_CXFA_FONTMGR_H_
#define XFA_FXFA_CXFA_FONTMGR_H_

#include <map>

#include "core/fxcrt/fx_string.h"

class CFGAS_GEFont;
class CXFA_FFDoc;

class CXFA_FontMgr {
 public:
  CXFA_FontMgr();
  ~CXFA_FontMgr();

  RetainPtr<CFGAS_GEFont> GetFont(CXFA_FFDoc* hDoc,
                                  WideStringView wsFontFamily,
                                  uint32_t dwFontStyles);

 private:
  std::map<ByteString, RetainPtr<CFGAS_GEFont>> m_FontMap;
};

#endif  //  XFA_FXFA_CXFA_FONTMGR_H_
