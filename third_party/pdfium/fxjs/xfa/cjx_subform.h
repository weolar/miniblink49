// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_SUBFORM_H_
#define FXJS_XFA_CJX_SUBFORM_H_

#include "fxjs/xfa/cjx_container.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Delta;

class CJX_Subform final : public CJX_Container {
 public:
  explicit CJX_Subform(CXFA_Node* container);
  ~CJX_Subform() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(execCalculate);
  JSE_METHOD(execEvent);
  JSE_METHOD(execInitialize);
  JSE_METHOD(execValidate);

  JSE_PROP(instanceIndex);
  JSE_PROP(layout);
  JSE_PROP(locale);
  JSE_PROP(validationMessage);

 private:
  using Type__ = CJX_Subform;
  using ParentType__ = CJX_Container;

  static const TypeTag static_type__ = TypeTag::Subform;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_SUBFORM_H_
