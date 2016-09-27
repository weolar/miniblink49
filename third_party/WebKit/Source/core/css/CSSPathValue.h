// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSPathValue_h
#define CSSPathValue_h

#include "core/css/CSSValue.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class CSSPathValue : public CSSValue {
public:
    static PassRefPtrWillBeRawPtr<CSSPathValue> create(const String& pathString)
    {
        return adoptRefWillBeNoop(new CSSPathValue(pathString));
    }

    String customCSSText() const;

    bool equals(const CSSPathValue&) const;

    DECLARE_TRACE_AFTER_DISPATCH();

    const String& pathString() const { return m_pathString; }

private:
    CSSPathValue(const String& pathString);

    String m_pathString;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSPathValue, isPathValue());

} // namespace blink

#endif // CSSPathValue_h
