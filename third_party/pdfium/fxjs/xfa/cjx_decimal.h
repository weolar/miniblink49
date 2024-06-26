// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_DECIMAL_H_
#define FXJS_XFA_CJX_DECIMAL_H_

#include "fxjs/xfa/cjx_content.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Decimal;

class CJX_Decimal final : public CJX_Content {
 public:
  explicit CJX_Decimal(CXFA_Decimal* node);
  ~CJX_Decimal() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(defaultValue); /* {default} */
  JSE_PROP(value);

 private:
  using Type__ = CJX_Decimal;
  using ParentType__ = CJX_Content;

  static const TypeTag static_type__ = TypeTag::Decimal;
};

#endif  // FXJS_XFA_CJX_DECIMAL_H_
