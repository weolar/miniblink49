// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PropertyHandle_h
#define PropertyHandle_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/dom/QualifiedName.h"

namespace blink {

class CORE_EXPORT PropertyHandle {
public:
    explicit PropertyHandle(CSSPropertyID property)
        : handleType(HandleCSSProperty)
        , property(property)
    {
        ASSERT(property != CSSPropertyInvalid);
    }

    explicit PropertyHandle(const QualifiedName& attributeName)
        : handleType(HandleSVGAttribute)
        , attribute(&attributeName)
    {
    }

    bool operator==(const PropertyHandle&) const;
    bool operator!=(const PropertyHandle& other) const { return !(*this == other); }

    unsigned hash() const;

    bool isCSSProperty() const { return handleType == HandleCSSProperty; }
    CSSPropertyID cssProperty() const { ASSERT(isCSSProperty()); return property; }

    bool isSVGAttribute() const { return handleType == HandleSVGAttribute; }
    const QualifiedName* svgAttribute() const { ASSERT(isSVGAttribute()); return attribute; }

private:
    enum HandleType {
        HandleEmptyValueForHashTraits,
        HandleDeletedValueForHashTraits,
        HandleCSSProperty,
        HandleSVGAttribute,
    };

    explicit PropertyHandle(HandleType handleType)
        : handleType(handleType)
        , attribute(nullptr)
    {
    }

    static PropertyHandle emptyValueForHashTraits() { return PropertyHandle(HandleEmptyValueForHashTraits); }

    static PropertyHandle deletedValueForHashTraits() { return PropertyHandle(HandleDeletedValueForHashTraits); }

    bool isDeletedValueForHashTraits() { return handleType == HandleDeletedValueForHashTraits; }

    HandleType handleType;
    union {
        CSSPropertyID property;
        const QualifiedName* attribute;
    };

    friend struct ::WTF::HashTraits<blink::PropertyHandle>;
};

} // namespace blink

namespace WTF {

template<> struct DefaultHash<blink::PropertyHandle> {
    struct Hash {
        static unsigned hash(const blink::PropertyHandle& handle) { return handle.hash(); }

        static bool equal(const blink::PropertyHandle& a, const blink::PropertyHandle& b) { return a == b; }

        static const bool safeToCompareToEmptyOrDeleted = true;
    };
};

template<> struct HashTraits<blink::PropertyHandle> : SimpleClassHashTraits<blink::PropertyHandle> {
    static const bool needsDestruction = false;
    static void constructDeletedValue(blink::PropertyHandle& slot, bool)
    {
        new (NotNull, &slot) blink::PropertyHandle(blink::PropertyHandle::deletedValueForHashTraits());
    }
    static bool isDeletedValue(blink::PropertyHandle value)
    {
        return value.isDeletedValueForHashTraits();
    }

    static blink::PropertyHandle emptyValue()
    {
        return blink::PropertyHandle::emptyValueForHashTraits();
    }
};

} // namespace WTF

#endif // PropertyHandle_h
