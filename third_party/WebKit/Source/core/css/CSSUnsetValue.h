// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSUnsetValue_h
#define CSSUnsetValue_h

#include "core/css/CSSValue.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class CSSUnsetValue : public CSSValue {
public:
    static PassRefPtrWillBeRawPtr<CSSUnsetValue> create()
    {
        return adoptRefWillBeNoop(new CSSUnsetValue);
    }

    String customCSSText() const;

    bool equals(const CSSUnsetValue&) const { return true; }

    DEFINE_INLINE_TRACE_AFTER_DISPATCH() { CSSValue::traceAfterDispatch(visitor); }

private:
    CSSUnsetValue()
        : CSSValue(UnsetClass)
    {
    }
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSUnsetValue, isUnsetValue());

} // namespace blink

#endif // CSSUnsetValue_h
