// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSFunctionValue_h
#define CSSFunctionValue_h

#include "core/CSSValueKeywords.h"
#include "core/css/CSSValueList.h"

namespace blink {

class CSSFunctionValue : public CSSValueList {
public:
    static PassRefPtrWillBeRawPtr<CSSFunctionValue> create(CSSValueID id)
    {
        return adoptRefWillBeNoop(new CSSFunctionValue(id));
    }

    String customCSSText() const;

    bool equals(const CSSFunctionValue& other) const { return m_valueID == other.m_valueID && CSSValueList::equals(other); }
    CSSValueID functionType() const { return m_valueID; }

    DEFINE_INLINE_TRACE_AFTER_DISPATCH() { CSSValueList::traceAfterDispatch(visitor); }

private:
    CSSFunctionValue(CSSValueID id)
        : CSSValueList(FunctionClass, CommaSeparator)
        , m_valueID(id)
    {
    }

    const CSSValueID m_valueID;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSFunctionValue, isFunctionValue());

} // namespace blink

#endif

