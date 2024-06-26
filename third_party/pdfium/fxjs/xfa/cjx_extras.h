// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_EXTRAS_H_
#define FXJS_XFA_CJX_EXTRAS_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Extras;

class CJX_Extras final : public CJX_Node {
 public:
  explicit CJX_Extras(CXFA_Extras* node);
  ~CJX_Extras() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(type);

 private:
  using Type__ = CJX_Extras;
  using ParentType__ = CJX_Node;

  static const TypeTag static_type__ = TypeTag::Extras;
};

#endif  // FXJS_XFA_CJX_EXTRAS_H_
