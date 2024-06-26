// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_TIME_H_
#define FXJS_XFA_CJX_TIME_H_

#include "fxjs/xfa/cjx_content.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Time;

class CJX_Time final : public CJX_Content {
 public:
  explicit CJX_Time(CXFA_Time* node);
  ~CJX_Time() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(defaultValue); /* {default} */
  JSE_PROP(value);

 private:
  using Type__ = CJX_Time;
  using ParentType__ = CJX_Content;

  static const TypeTag static_type__ = TypeTag::Time;
};

#endif  // FXJS_XFA_CJX_TIME_H_
