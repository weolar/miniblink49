// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_URI_H_
#define FXJS_XFA_CJX_URI_H_

#include "fxjs/xfa/cjx_textnode.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Uri;

class CJX_Uri final : public CJX_TextNode {
 public:
  explicit CJX_Uri(CXFA_Uri* node);
  ~CJX_Uri() override;
};

#endif  // FXJS_XFA_CJX_URI_H_
