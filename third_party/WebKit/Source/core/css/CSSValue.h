/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef CSSValue_h
#define CSSValue_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/HashMap.h"
#include "wtf/ListHashSet.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"

namespace blink {

class CORE_EXPORT CSSValue : public RefCountedWillBeGarbageCollectedFinalized<CSSValue> {
public:
#if ENABLE(OILPAN)
    // Override operator new to allocate CSSValue subtype objects onto
    // a dedicated heap.
    GC_PLUGIN_IGNORE("crbug.com/443854")
    void* operator new(size_t size)
    {
        return allocateObject(size, false);
    }
    static void* allocateObject(size_t size, bool isEager)
    {
        ThreadState* state = ThreadStateFor<ThreadingTrait<CSSValue>::Affinity>::state();
        return Heap::allocateOnHeapIndex(state, size, isEager ? ThreadState::EagerSweepHeapIndex : ThreadState::CSSValueHeapIndex, GCInfoTrait<CSSValue>::index());
    }
#else
    // Override RefCounted's deref() to ensure operator delete is called on
    // the appropriate subclass type.
    void deref()
    {
        if (derefBase())
            destroy();
    }
#endif // !ENABLE(OILPAN)

    String cssText() const;

    bool isPrimitiveValue() const { return m_classType == PrimitiveClass; }
    bool isValueList() const { return m_classType >= ValueListClass; }

    bool isBaseValueList() const { return m_classType == ValueListClass; }

    bool isBorderImageSliceValue() const { return m_classType == BorderImageSliceClass; }
    bool isCanvasValue() const { return m_classType == CanvasClass; }
    bool isCursorImageValue() const { return m_classType == CursorImageClass; }
    bool isCrossfadeValue() const { return m_classType == CrossfadeClass; }
    bool isFontFeatureValue() const { return m_classType == FontFeatureClass; }
    bool isFontFaceSrcValue() const { return m_classType == FontFaceSrcClass; }
    bool isFunctionValue() const { return m_classType == FunctionClass; }
    bool isImageGeneratorValue() const { return m_classType >= CanvasClass && m_classType <= RadialGradientClass; }
    bool isGradientValue() const { return m_classType >= LinearGradientClass && m_classType <= RadialGradientClass; }
    bool isImageSetValue() const { return m_classType == ImageSetClass; }
    bool isImageValue() const { return m_classType == ImageClass; }
    bool isImplicitInitialValue() const;
    bool isInheritedValue() const { return m_classType == InheritedClass; }
    bool isInitialValue() const { return m_classType == InitialClass; }
    bool isUnsetValue() const { return m_classType == UnsetClass; }
    bool isCSSWideKeyword() const { return m_classType >= InheritedClass && m_classType <= UnsetClass; }
    bool isLinearGradientValue() const { return m_classType == LinearGradientClass; }
    bool isPathValue() const { return m_classType == PathClass; }
    bool isRadialGradientValue() const { return m_classType == RadialGradientClass; }
    bool isReflectValue() const { return m_classType == ReflectClass; }
    bool isShadowValue() const { return m_classType == ShadowClass; }
    bool isCubicBezierTimingFunctionValue() const { return m_classType == CubicBezierTimingFunctionClass; }
    bool isStepsTimingFunctionValue() const { return m_classType == StepsTimingFunctionClass; }
    bool isLineBoxContainValue() const { return m_classType == LineBoxContainClass; }
    bool isCalcValue() const {return m_classType == CalculationClass; }
    bool isGridTemplateAreasValue() const { return m_classType == GridTemplateAreasClass; }
    bool isSVGDocumentValue() const { return m_classType == CSSSVGDocumentClass; }
    bool isContentDistributionValue() const { return m_classType == CSSContentDistributionClass; }
    bool isUnicodeRangeValue() const { return m_classType == UnicodeRangeClass; }
    bool isGridLineNamesValue() const { return m_classType == GridLineNamesClass; }
	bool isCustomPropertyDeclaration() const { return m_classType == CustomPropertyDeclarationClass; }
	bool isVariableReferenceValue() const { return m_classType == VariableReferenceClass; }

    bool hasFailedOrCanceledSubresources() const;

    bool equals(const CSSValue&) const;

    void finalizeGarbageCollectedObject();
    DEFINE_INLINE_TRACE_AFTER_DISPATCH() { }
    DECLARE_TRACE();

    // ~CSSValue should be public, because non-public ~CSSValue causes C2248
    // error: 'blink::CSSValue::~CSSValue' : cannot access protected member
    // declared in class 'blink::CSSValue' when compiling
    // 'source\wtf\refcounted.h' by using msvc.
    ~CSSValue() { }

protected:

    static const size_t ClassTypeBits = 6;
    enum ClassType {
        PrimitiveClass,

        // Image classes.
        ImageClass,
        CursorImageClass,

        // Image generator classes.
        CanvasClass,
        CrossfadeClass,
        LinearGradientClass,
        RadialGradientClass,

        // Timing function classes.
        CubicBezierTimingFunctionClass,
        StepsTimingFunctionClass,

        // Other class types.
        BorderImageSliceClass,
        FontFeatureClass,
        FontFaceSrcClass,

        InheritedClass,
        InitialClass,
        UnsetClass,

        ReflectClass,
        ShadowClass,
        UnicodeRangeClass,
        LineBoxContainClass,
        CalculationClass,
        GridTemplateAreasClass,
        PathClass,

		VariableReferenceClass,
		CustomPropertyDeclarationClass,

        // SVG classes.
        CSSSVGDocumentClass,

        CSSContentDistributionClass,

        // List class types must appear after ValueListClass.
        ValueListClass,
        FunctionClass,
        ImageSetClass,
        GridLineNamesClass,
        // Do not append non-list class types here.
    };

    static const size_t ValueListSeparatorBits = 2;
    enum ValueListSeparator {
        SpaceSeparator,
        CommaSeparator,
        SlashSeparator
    };

    ClassType classType() const { return static_cast<ClassType>(m_classType); }

    explicit CSSValue(ClassType classType)
        : m_primitiveUnitType(0)
        , m_hasCachedCSSText(false)
        , m_isQuirkValue(false)
        , m_valueListSeparator(SpaceSeparator)
        , m_classType(classType)
    {
    }

    // NOTE: This class is non-virtual for memory and performance reasons.
    // Don't go making it virtual again unless you know exactly what you're doing!

private:
    void destroy();

protected:
    // The bits in this section are only used by specific subclasses but kept here
    // to maximize struct packing.

    // CSSPrimitiveValue bits:
    unsigned m_primitiveUnitType : 7; // CSSPrimitiveValue::UnitType
    mutable unsigned m_hasCachedCSSText : 1;
    unsigned m_isQuirkValue : 1;

    unsigned m_valueListSeparator : ValueListSeparatorBits;

private:
    unsigned m_classType : ClassTypeBits; // ClassType
};

template<typename CSSValueType, size_t inlineCapacity>
inline bool compareCSSValueVector(const WillBeHeapVector<RefPtrWillBeMember<CSSValueType>, inlineCapacity>& firstVector, const WillBeHeapVector<RefPtrWillBeMember<CSSValueType>, inlineCapacity>& secondVector)
{
    size_t size = firstVector.size();
    if (size != secondVector.size())
        return false;

    for (size_t i = 0; i < size; i++) {
        const RefPtrWillBeMember<CSSValueType>& firstPtr = firstVector[i];
        const RefPtrWillBeMember<CSSValueType>& secondPtr = secondVector[i];
        if (firstPtr == secondPtr || (firstPtr && secondPtr && firstPtr->equals(*secondPtr)))
            continue;
        return false;
    }
    return true;
}

template<typename CSSValueType>
inline bool compareCSSValuePtr(const RefPtr<CSSValueType>& first, const RefPtr<CSSValueType>& second)
{
    if (first == second)
        return true;
    if (!first || !second)
        return false;
    return first->equals(*second);
}

template<typename CSSValueType>
inline bool compareCSSValuePtr(const RawPtr<CSSValueType>& first, const RawPtr<CSSValueType>& second)
{
    if (first == second)
        return true;
    if (!first || !second)
        return false;
    return first->equals(*second);
}

template<typename CSSValueType>
inline bool compareCSSValuePtr(const Member<CSSValueType>& first, const Member<CSSValueType>& second)
{
    if (first == second)
        return true;
    if (!first || !second)
        return false;
    return first->equals(*second);
}

#define DEFINE_CSS_VALUE_TYPE_CASTS(thisType, predicate) \
    DEFINE_TYPE_CASTS(thisType, CSSValue, value, value->predicate, value.predicate)

} // namespace blink

#endif // CSSValue_h
