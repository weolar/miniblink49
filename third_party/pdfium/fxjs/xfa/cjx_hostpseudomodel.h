// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_HOSTPSEUDOMODEL_H_
#define FXJS_XFA_CJX_HOSTPSEUDOMODEL_H_

#include "fxjs/xfa/cjx_object.h"
#include "fxjs/xfa/jse_define.h"
#include "xfa/fxfa/fxfa_basic.h"

class CFXJSE_Value;
class CScript_HostPseudoModel;

class CJX_HostPseudoModel final : public CJX_Object {
 public:
  explicit CJX_HostPseudoModel(CScript_HostPseudoModel* model);
  ~CJX_HostPseudoModel() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(beep);
  JSE_METHOD(documentCountInBatch);
  JSE_METHOD(documentInBatch);
  JSE_METHOD(exportData);
  JSE_METHOD(getFocus);
  JSE_METHOD(gotoURL);
  JSE_METHOD(importData);
  JSE_METHOD(messageBox);
  JSE_METHOD(openList);
  JSE_METHOD(pageDown);
  JSE_METHOD(pageUp);
  JSE_METHOD(print);
  JSE_METHOD(resetData);
  JSE_METHOD(response);
  JSE_METHOD(setFocus);

  JSE_PROP(appType);
  JSE_PROP(calculationsEnabled);
  JSE_PROP(currentPage);
  JSE_PROP(language);
  JSE_PROP(numPages);
  JSE_PROP(platform);
  JSE_PROP(title);
  JSE_PROP(validationsEnabled);
  JSE_PROP(variation);
  JSE_PROP(version);

  // TODO(dsinclair): Remove when xfa_basic_data_element_script is removed.
  // Doesn't exist in spec
  JSE_PROP(name);

 private:
  using Type__ = CJX_HostPseudoModel;
  using ParentType__ = CJX_Object;

  static const TypeTag static_type__ = TypeTag::HostPseudoModel;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_HOSTPSEUDOMODEL_H_
