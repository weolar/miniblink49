// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_LIST_H_
#define FXJS_XFA_CJX_LIST_H_

#include "fxjs/xfa/cjx_object.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_List;

class CJX_List : public CJX_Object {
 public:
  explicit CJX_List(CXFA_List* list);
  ~CJX_List() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(append);
  JSE_METHOD(insert);
  JSE_METHOD(item);
  JSE_METHOD(remove);

  JSE_PROP(length);

 private:
  using Type__ = CJX_List;
  using ParentType__ = CJX_Object;

  static const TypeTag static_type__ = TypeTag::List;
  static const CJX_MethodSpec MethodSpecs[];

  CXFA_List* GetXFAList();
};

#endif  // FXJS_XFA_CJX_LIST_H_
