// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CSS_CFX_CSSCUSTOMPROPERTY_H_
#define CORE_FXCRT_CSS_CFX_CSSCUSTOMPROPERTY_H_

#include "core/fxcrt/fx_string.h"

class CFX_CSSCustomProperty {
 public:
  CFX_CSSCustomProperty(const WideString& name, const WideString& value);
  CFX_CSSCustomProperty(const CFX_CSSCustomProperty& prop);
  ~CFX_CSSCustomProperty();

  WideString name() const { return name_; }
  WideString value() const { return value_; }

 private:
  WideString name_;
  WideString value_;
};

#endif  // CORE_FXCRT_CSS_CFX_CSSCUSTOMPROPERTY_H_
