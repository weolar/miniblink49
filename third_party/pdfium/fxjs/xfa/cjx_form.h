// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_FORM_H_
#define FXJS_XFA_CJX_FORM_H_

#include "fxjs/xfa/cjx_model.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Form;

class CJX_Form final : public CJX_Model {
 public:
  explicit CJX_Form(CXFA_Form* form);
  ~CJX_Form() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(execCalculate);
  JSE_METHOD(execInitialize);
  JSE_METHOD(execValidate);
  JSE_METHOD(formNodes);
  JSE_METHOD(recalculate);
  JSE_METHOD(remerge);

 private:
  using Type__ = CJX_Form;
  using ParentType__ = CJX_Model;

  static const TypeTag static_type__ = TypeTag::Form;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_FORM_H_
