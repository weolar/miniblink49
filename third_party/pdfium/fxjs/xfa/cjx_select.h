// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_SELECT_H_
#define FXJS_XFA_CJX_SELECT_H_

#include "fxjs/xfa/cjx_textnode.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Select;

class CJX_Select final : public CJX_TextNode {
 public:
  explicit CJX_Select(CXFA_Select* node);
  ~CJX_Select() override;
};

#endif  // FXJS_XFA_CJX_SELECT_H_
