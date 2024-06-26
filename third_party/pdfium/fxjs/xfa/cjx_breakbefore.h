// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_BREAKBEFORE_H_
#define FXJS_XFA_CJX_BREAKBEFORE_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_BreakBefore;

class CJX_BreakBefore final : public CJX_Node {
 public:
  explicit CJX_BreakBefore(CXFA_BreakBefore* node);
  ~CJX_BreakBefore() override;
};

#endif  // FXJS_XFA_CJX_BREAKBEFORE_H_
