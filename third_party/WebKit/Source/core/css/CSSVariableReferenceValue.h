// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSVariableReferenceValue_h
#define CSSVariableReferenceValue_h

#include "core/css/CSSValue.h"
#include "core/css/CSSVariableData.h"
#include "wtf/RefPtr.h"

namespace blink {

class CSSVariableReferenceValue : public CSSValue {
public:
    static PassRefPtrWillBeRawPtr<CSSVariableReferenceValue> create(PassRefPtr<CSSVariableData> data)
    {
        return adoptRefWillBeNoop(new CSSVariableReferenceValue(data));
    }

    CSSVariableData* variableDataValue() const
    {
        return m_data.get();
    }

    bool equals(const CSSVariableReferenceValue& other) const { return m_data == other.m_data; }
    String customCSSText() const;

    DECLARE_TRACE_AFTER_DISPATCH();
private:
    CSSVariableReferenceValue(PassRefPtr<CSSVariableData> data)
        : CSSValue(VariableReferenceClass)
        , m_data(data)
    {
    }

    RefPtr<CSSVariableData> m_data;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSVariableReferenceValue, isVariableReferenceValue());

} // namespace blink

#endif // CSSVariableReferenceValue_h