// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_CONTAINER_H_
#define FXJS_XFA_CJX_CONTAINER_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Node;

class CJX_Container : public CJX_Node {
 public:
  explicit CJX_Container(CXFA_Node* node);
  ~CJX_Container() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

//   JSE_METHOD(getDelta);
//   JSE_METHOD(getDeltas);

 private:
  using Type__ = CJX_Container;
  using ParentType__ = CJX_Node;

  static const TypeTag static_type__ = TypeTag::Container;
  //static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_CONTAINER_H_
