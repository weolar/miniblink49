// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSDATA_H_
#define CORE_FXCRT_CSS_CFX_CSSDATA_H_

#include "core/fxcrt/css/cfx_css.h"
#include "core/fxcrt/css/cfx_cssnumbervalue.h"
#include "core/fxcrt/css/cfx_cssvalue.h"
#include "core/fxcrt/string_view_template.h"
#include "core/fxge/fx_dib.h"

class CFX_CSSData {
 public:
  struct Property {
    CFX_CSSProperty eName;
    uint32_t dwHash;  // Hashed as wide string.
    uint32_t dwType;
  };

  struct PropertyValue {
    CFX_CSSPropertyValue eName;
    uint32_t dwHash;  // Hashed as wide string.
  };

  struct LengthUnit {
    const wchar_t* value;
    CFX_CSSNumberType type;
  };

  struct Color {
    const wchar_t* name;
    FX_ARGB value;
  };

  static const Property* GetPropertyByName(WideStringView name);
  static const Property* GetPropertyByEnum(CFX_CSSProperty property);
  static const PropertyValue* GetPropertyValueByName(WideStringView wsName);
  static const LengthUnit* GetLengthUnitByName(WideStringView wsName);
  static const Color* GetColorByName(WideStringView wsName);
};

#endif  // CORE_FXCRT_CSS_CFX_CSSDATA_H_
