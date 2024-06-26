// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSVALUELISTPARSER_H_
#define CORE_FXCRT_CSS_CFX_CSSVALUELISTPARSER_H_

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/fx_system.h"

class CFX_CSSValueListParser {
 public:
  CFX_CSSValueListParser(const wchar_t* psz, int32_t iLen, wchar_t separator);

  bool NextValue(CFX_CSSPrimitiveType* eType,
                 const wchar_t** pStart,
                 int32_t* iLength);
  void UseCommaSeparator() { m_Separator = ','; }

 private:
  int32_t SkipTo(wchar_t wch, bool breakOnSpace, bool matchBrackets);

  wchar_t m_Separator;
  const wchar_t* m_pCur;
  const wchar_t* m_pEnd;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSVALUELISTPARSER_H_
