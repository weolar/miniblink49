// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_DRAW_H_
#define FXJS_XFA_CJX_DRAW_H_

#include "fxjs/xfa/cjx_container.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_Draw;

class CJX_Draw final : public CJX_Container {
 public:
  explicit CJX_Draw(CXFA_Draw* node);
  ~CJX_Draw() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(defaultValue); /* {default} */
  JSE_PROP(rawValue);

 private:
  using Type__ = CJX_Draw;
  using ParentType__ = CJX_Container;

  static const TypeTag static_type__ = TypeTag::Draw;
};

#endif  // FXJS_XFA_CJX_DRAW_H_
