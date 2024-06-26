// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_XFA_CJX_EXDATA_H_
#define FXJS_XFA_CJX_EXDATA_H_

#include "fxjs/xfa/cjx_content.h"
#include "fxjs/xfa/jse_define.h"

class CXFA_ExData;

class CJX_ExData final : public CJX_Content {
 public:
  explicit CJX_ExData(CXFA_ExData* node);
  ~CJX_ExData() override;

  // CJX_Object:
  bool DynamicTypeIs(TypeTag eType) const override;

  JSE_PROP(defaultValue); /* {default} */

 private:
  using Type__ = CJX_ExData;
  using ParentType__ = CJX_Content;

  static const TypeTag static_type__ = TypeTag::ExData;
};

#endif  // FXJS_XFA_CJX_EXDATA_H_
