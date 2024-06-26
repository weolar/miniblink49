// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_SOURCE_H_
#define FXJS_XFA_CJX_SOURCE_H_

#include "fxjs/xfa/cjx_node.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Source;

class CJX_Source final : public CJX_Node {
 public:
  explicit CJX_Source(CXFA_Source* src);
  ~CJX_Source() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_METHOD(addNew);
  JSE_METHOD(cancel);
  JSE_METHOD(cancelBatch);
  JSE_METHOD(close);
  JSE_METHOD(deleteItem /*delete*/);
  JSE_METHOD(first);
  JSE_METHOD(hasDataChanged);
  JSE_METHOD(isBOF);
  JSE_METHOD(isEOF);
  JSE_METHOD(last);
  JSE_METHOD(next);
  JSE_METHOD(open);
  JSE_METHOD(previous);
  JSE_METHOD(requery);
  JSE_METHOD(resync);
  JSE_METHOD(update);
  JSE_METHOD(updateBatch);

  JSE_PROP(db);

 private:
  using Type__ = CJX_Source;
  using ParentType__ = CJX_Node;

  static const TypeTag static_type__ = TypeTag::Source;
  static const CJX_MethodSpec MethodSpecs[];
};

#endif  // FXJS_XFA_CJX_SOURCE_H_
