// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_REF_H_
#define FXJS_XFA_CJX_REF_H_

#include "fxjs/xfa/cjx_textnode.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Ref;

class CJX_Ref final : public CJX_TextNode {
 public:
  explicit CJX_Ref(CXFA_Ref* node);
  ~CJX_Ref() override;
};

#endif  // FXJS_XFA_CJX_REF_H_
