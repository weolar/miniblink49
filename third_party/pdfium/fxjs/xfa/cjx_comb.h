// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_COMB_H_
#define FXJS_XFA_CJX_COMB_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Comb;

class CJX_Comb final : public CJX_Node {
 public:
  explicit CJX_Comb(CXFA_Comb* node);
  ~CJX_Comb() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(numberOfCells);

 private:
  using Type__ = CJX_Comb;
  using ParentType__ = CJX_Node;

  static const TypeTag static_type__ = TypeTag::Comb;
};

#endif  // FXJS_XFA_CJX_COMB_H_
