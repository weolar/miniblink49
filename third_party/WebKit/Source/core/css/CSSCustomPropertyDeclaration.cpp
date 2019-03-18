// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/CSSCustomPropertyDeclaration.h"

namespace blink {

DEFINE_TRACE_AFTER_DISPATCH(CSSCustomPropertyDeclaration)
{
    //visitor->trace(m_value); // weolar
    CSSValue::traceAfterDispatch(visitor);
}

} // namespace blink