// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_INTEGER_H_
#define FXJS_XFA_CJX_INTEGER_H_

#include "fxjs/xfa/cjx_content.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Integer;

class CJX_Integer final : public CJX_Content {
 public:
  explicit CJX_Integer(CXFA_Integer* node);
  ~CJX_Integer() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(defaultValue); /* {default} */
  JSE_PROP(value);

 private:
  using Type__ = CJX_Integer;
  using ParentType__ = CJX_Content;

  static const TypeTag static_type__ = TypeTag::Integer;
};

#endif  // FXJS_XFA_CJX_INTEGER_H_
