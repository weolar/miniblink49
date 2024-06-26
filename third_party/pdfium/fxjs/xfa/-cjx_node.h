// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_NODE_H_
#define FXJS_XFA_CJX_NODE_H_

#include "fxjs/xfa/cjx_tree.h"
#include "fxjs/xfa/jse_define.h"
#include "xfa/fxfa/fxfa_basic.h"

class CXFA_Node;

class CJX_Node : public CJX_Tree {
 public:
  explicit CJX_Node(CXFA_Node* node);
  ~CJX_Node() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

//   JSE_METHOD(applyXSL);
//   JSE_METHOD(assignNode);
//   JSE_METHOD(clone);
//   JSE_METHOD(getAttribute);
//   JSE_METHOD(getElement);
//   JSE_METHOD(isPropertySpecified);
//   JSE_METHOD(loadXML);
//   JSE_METHOD(saveFilteredXML);
//   JSE_METHOD(saveXML);
//   JSE_METHOD(setAttribute);
//   JSE_METHOD(setElement);
// 
//   JSE_PROP(isContainer);
//   JSE_PROP(isNull);
//   JSE_PROP(model);
//   JSE_PROP(ns);
//   JSE_PROP(oneOfChild);

  CXFA_Node* GetXFANode() const;

 protected:
  int32_t execSingleEventByName(WideStringView wsEventName, XFA_Element eType);

 private:
  using Type__ = CJX_Node;
  using ParentType__ = CJX_Tree;

  static const TypeTag static_type__ = TypeTag::Node;
  //static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_NODE_H_
