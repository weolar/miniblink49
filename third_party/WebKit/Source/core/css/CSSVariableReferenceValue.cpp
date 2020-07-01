// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/CSSVariableReferenceValue.h"

namespace blink {

DEFINE_TRACE_AFTER_DISPATCH(CSSVariableReferenceValue)
{
    //visitor->trace(m_data);
    CSSValue::traceAfterDispatch(visitor);
}

String CSSVariableReferenceValue::customCSSText() const
{
    // We may want to consider caching this value.
    return m_data->tokenRange().serialize();
}

} // namespace blink