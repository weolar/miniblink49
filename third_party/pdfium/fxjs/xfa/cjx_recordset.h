// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_RECORDSET_H_
#define FXJS_XFA_CJX_RECORDSET_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_RecordSet;

class CJX_RecordSet final : public CJX_Node {
 public:
  explicit CJX_RecordSet(CXFA_RecordSet* node);
  ~CJX_RecordSet() override;
};

#endif  // FXJS_XFA_CJX_RECORDSET_H_
