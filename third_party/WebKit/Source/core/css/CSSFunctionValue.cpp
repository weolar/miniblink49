// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/CSSFunctionValue.h"

#include "wtf/text/StringBuilder.h"

namespace blink {

String CSSFunctionValue::customCSSText() const
{
    StringBuilder result;
    result.append(getValueName(m_valueID));
    result.append('(');
    result.append(CSSValueList::customCSSText());
    result.append(')');
    return result.toString();
}

}
