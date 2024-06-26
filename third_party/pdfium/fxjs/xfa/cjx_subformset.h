// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_SUBFORMSET_H_
#define FXJS_XFA_CJX_SUBFORMSET_H_

#include "fxjs/xfa/cjx_container.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_SubformSet;

class CJX_SubformSet final : public CJX_Container {
 public:
  explicit CJX_SubformSet(CXFA_SubformSet* node);
  ~CJX_SubformSet() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(instanceIndex);

 private:
  using Type__ = CJX_SubformSet;
  using ParentType__ = CJX_Container;

  static const TypeTag static_type__ = TypeTag::SubformSet;
};

#endif  // FXJS_XFA_CJX_SUBFORMSET_H_
