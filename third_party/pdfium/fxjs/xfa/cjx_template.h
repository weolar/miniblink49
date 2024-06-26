// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_TEMPLATE_H_
#define FXJS_XFA_CJX_TEMPLATE_H_

#include "fxjs/xfa/cjx_model.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Template;

class CJX_Template final : public CJX_Model {
 public:
  explicit CJX_Template(CXFA_Template* tmpl);
  ~CJX_Template() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  /* The docs list a |createNode| method on Template but that method already
   * exists on Model, also the |createNode| docs say it exists on Model not
   * on Template so I'm leaving |createNode| out of the template methods. */
  JSE_METHOD(execCalculate);
  JSE_METHOD(execInitialize);
  JSE_METHOD(execValidate);
  JSE_METHOD(formNodes);
  JSE_METHOD(recalculate);
  JSE_METHOD(remerge);

 private:
  using Type__ = CJX_Template;
  using ParentType__ = CJX_Model;

  static const TypeTag static_type__ = TypeTag::Template;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_TEMPLATE_H_
