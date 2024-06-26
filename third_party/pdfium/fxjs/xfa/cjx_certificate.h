// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_CERTIFICATE_H_
#define FXJS_XFA_CJX_CERTIFICATE_H_

#include "fxjs/xfa/cjx_textnode.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Certificate;

class CJX_Certificate final : public CJX_TextNode {
 public:
  explicit CJX_Certificate(CXFA_Certificate* node);
  ~CJX_Certificate() override;
};

#endif  // FXJS_XFA_CJX_CERTIFICATE_H_
