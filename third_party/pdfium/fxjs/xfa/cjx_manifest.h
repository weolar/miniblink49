// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_MANIFEST_H_
#define FXJS_XFA_CJX_MANIFEST_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Manifest;

class CJX_Manifest final : public CJX_Node {
 public:
  explicit CJX_Manifest(CXFA_Manifest* manifest);
  ~CJX_Manifest() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(evaluate);

  JSE_PROP(defaultValue); /* {default} */

 private:
  using Type__ = CJX_Manifest;
  using ParentType__ = CJX_Node;

  static const TypeTag static_type__ = TypeTag::Manifest;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_MANIFEST_H_
