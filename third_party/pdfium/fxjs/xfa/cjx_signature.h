// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_SIGNATURE_H_
#define FXJS_XFA_CJX_SIGNATURE_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Signature;

class CJX_Signature final : public CJX_Node {
 public:
  explicit CJX_Signature(CXFA_Signature* node);
  ~CJX_Signature() override;
};

#endif  // FXJS_XFA_CJX_SIGNATURE_H_
