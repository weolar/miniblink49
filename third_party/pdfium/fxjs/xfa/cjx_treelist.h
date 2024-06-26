// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_TREELIST_H_
#define FXJS_XFA_CJX_TREELIST_H_

#include "fxjs/xfa/cjx_list.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_TreeList;

class CJX_TreeList final : public CJX_List {
 public:
  explicit CJX_TreeList(CXFA_TreeList* list);
  ~CJX_TreeList() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(namedItem);

 private:
  using Type__ = CJX_TreeList;
  using ParentType__ = CJX_List;

  static const TypeTag static_type__ = TypeTag::TreeList;
  static const CJX_MethodSpec MethodSpecs[];

  CXFA_TreeList* GetXFATreeList();
};

#endif  // FXJS_XFA_CJX_TREELIST_H_
