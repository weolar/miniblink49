// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/CSSPathValue.h"

namespace blink {

CSSPathValue::CSSPathValue(const String& pathString)
    : CSSValue(PathClass)
    , m_pathString(pathString)
{
}

String CSSPathValue::customCSSText() const
{
    return "path('" + m_pathString + "')";
}

bool CSSPathValue::equals(const CSSPathValue& other) const
{
    return m_pathString == other.m_pathString;
}

DEFINE_TRACE_AFTER_DISPATCH(CSSPathValue)
{
    CSSValue::traceAfterDispatch(visitor);
}

} // namespace blink
