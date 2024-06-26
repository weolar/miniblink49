// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_MEDIUM_H_
#define FXJS_XFA_CJX_MEDIUM_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Medium;

class CJX_Medium final : public CJX_Node {
 public:
  explicit CJX_Medium(CXFA_Medium* node);
  ~CJX_Medium() override;
};

#endif  // FXJS_XFA_CJX_MEDIUM_H_
