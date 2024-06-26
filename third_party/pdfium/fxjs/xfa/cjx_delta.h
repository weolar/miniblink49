// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_DELTA_H_
#define FXJS_XFA_CJX_DELTA_H_

#include "fxjs/xfa/cjx_object.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Delta;

class CJX_Delta final : public CJX_Object {
 public:
  explicit CJX_Delta(CXFA_Delta* delta);
  ~CJX_Delta() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

//   JSE_METHOD(restore);
// 
//   JSE_PROP(currentValue);
//   JSE_PROP(savedValue);
//   JSE_PROP(target);

 private:
  using Type__ = CJX_Delta;
  using ParentType__ = CJX_Object;

  static const TypeTag static_type__ = TypeTag::Delta;
  // static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_DELTA_H_
