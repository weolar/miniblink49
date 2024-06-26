// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_LOGPSEUDOMODEL_H_
#define FXJS_XFA_CJX_LOGPSEUDOMODEL_H_

#include "fxjs/xfa/cjx_object.h"
#include "fxjs/xfa/jse_define.h"

class CScript_LogPseudoModel;

// TODO(dsinclair): This doesn't exist in the spec. Remove after
// xfa_basic_data_element_script is removed.
class CJX_LogPseudoModel final : public CJX_Object {
 public:
  explicit CJX_LogPseudoModel(CScript_LogPseudoModel* model);
  ~CJX_LogPseudoModel() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(message);
  JSE_METHOD(traceEnabled);
  JSE_METHOD(traceActivate);
  JSE_METHOD(traceDeactivate);
  JSE_METHOD(trace);

 private:
  using Type__ = CJX_LogPseudoModel;
  using ParentType__ = CJX_Object;

  static const TypeTag static_type__ = TypeTag::LogPseudoModel;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_LOGPSEUDOMODEL_H_
