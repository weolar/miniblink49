// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSCustomPropertyDeclaration_h
#define CSSCustomPropertyDeclaration_h

#include "core/css/CSSValue.h"
#include "core/css/CSSVariableData.h"
#include "wtf/RefPtr.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class CSSCustomPropertyDeclaration : public CSSValue {
public:
    static PassRefPtrWillBeRawPtr<CSSCustomPropertyDeclaration> create(const AtomicString& name, PassRefPtr<CSSVariableData> value)
    {
        return adoptRefWillBeNoop(new CSSCustomPropertyDeclaration(name, value));
    }

    static PassRefPtrWillBeRawPtr<CSSCustomPropertyDeclaration> create(const AtomicString& name, CSSValueID id)
    {
        return adoptRefWillBeNoop(new CSSCustomPropertyDeclaration(name, id));
    }

    const AtomicString& name() const { return m_name; }
    CSSVariableData* value() const { return m_value.get(); }
    CSSValueID id() const { return m_valueId; }

    bool equals(const CSSCustomPropertyDeclaration& other) const { return this == &other; }

    DECLARE_TRACE_AFTER_DISPATCH();
private:
    CSSCustomPropertyDeclaration(const AtomicString& name, CSSValueID id)
        : CSSValue(CustomPropertyDeclarationClass)
        , m_name(name)
        , m_value(nullptr)
        , m_valueId(id)
    {
        ASSERT(id == CSSValueInherit || id == CSSValueInitial || id == CSSValueUnset);
    }

    CSSCustomPropertyDeclaration(const AtomicString& name, PassRefPtr<CSSVariableData> value)
        : CSSValue(CustomPropertyDeclarationClass)
        , m_name(name)
        , m_value(value)
        , m_valueId(CSSValueInternalVariableValue)
    {
    }

    const AtomicString m_name;
    RefPtr<CSSVariableData> m_value;
    CSSValueID m_valueId;
};

DEFINE_CSS_VALUE_TYPE_CASTS(CSSCustomPropertyDeclaration, isCustomPropertyDeclaration());

} // namespace blink

#endif // CSSCustomPropertyDeclaration_h