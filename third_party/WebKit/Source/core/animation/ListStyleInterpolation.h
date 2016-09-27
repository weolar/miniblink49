// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ListStyleInterpolation_h
#define ListStyleInterpolation_h

#include "core/animation/StyleInterpolation.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/CSSValueList.h"
#include "core/css/resolver/StyleBuilder.h"

namespace blink {

template<typename InterpolationType, typename NonInterpolableData>
class ListStyleInterpolationImpl : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<ListStyleInterpolationImpl<InterpolationType, NonInterpolableData>> maybeCreateFromList(const CSSValue& start, const CSSValue& end, CSSPropertyID id, InterpolationRange range = RangeAll)
    {
        if (start.isValueList() && end.isValueList() && toCSSValueList(start).length() == toCSSValueList(end).length()) {
            const CSSValueList& startList = toCSSValueList(start);
            const CSSValueList& endList = toCSSValueList(end);

            for (size_t i = 0; i < toCSSValueList(start).length(); i++) {
                if (!InterpolationType::canCreateFrom(*startList.item(i), *endList.item(i))) {
                    return nullptr;
                }
            }

            Vector<typename InterpolationType::NonInterpolableType> startNonInterpolableData;

            OwnPtrWillBeRawPtr<InterpolableValue> startValue = listToInterpolableValue(start, &startNonInterpolableData);
            OwnPtrWillBeRawPtr<InterpolableValue> endValue = listToInterpolableValue(end);

            return adoptRefWillBeNoop(new ListStyleInterpolationImpl<InterpolationType, NonInterpolableData>(startValue.release(), endValue.release(), id, startNonInterpolableData, range));
        }
        return nullptr;
    }

    virtual void apply(StyleResolverState& state) const override
    {
        StyleBuilder::applyProperty(m_id, state, interpolableValueToList(m_cachedValue.get(), m_nonInterpolableData, m_range).get());
    }

private:
    ListStyleInterpolationImpl(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id,
        Vector<typename InterpolationType::NonInterpolableType> nonInterpolableData, InterpolationRange range = RangeAll)
        : StyleInterpolation(start, end, id)
        , m_range(range)
    {
        m_nonInterpolableData.swap(nonInterpolableData);
    }

    InterpolationRange m_range;

    Vector<typename InterpolationType::NonInterpolableType> m_nonInterpolableData;

    static PassOwnPtrWillBeRawPtr<InterpolableValue> listToInterpolableValue(const CSSValue& value, Vector<typename InterpolationType::NonInterpolableType>* nonInterpolableData = nullptr)
    {
        const CSSValueList& listValue = toCSSValueList(value);
        if (nonInterpolableData)
            nonInterpolableData->reserveCapacity(listValue.length());
        OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(listValue.length());
        typename InterpolationType::NonInterpolableType elementData = typename InterpolationType::NonInterpolableType();
        for (size_t i = 0; i < listValue.length(); i++) {
            result->set(i, InterpolationType::toInterpolableValue(*listValue.item(i), elementData));
            if (nonInterpolableData)
                nonInterpolableData->append(elementData);
        }
        return result.release();
    }

    static PassRefPtrWillBeRawPtr<CSSValue> interpolableValueToList(InterpolableValue* value, const Vector<typename InterpolationType::NonInterpolableType>& nonInterpolableData, InterpolationRange range = RangeAll)
    {
        InterpolableList* listValue = toInterpolableList(value);
        RefPtrWillBeRawPtr<CSSValueList> result = CSSValueList::createCommaSeparated();

        ASSERT(nonInterpolableData.size() == listValue->length());

        for (size_t i = 0; i < listValue->length(); i++)
            result->append(InterpolationType::fromInterpolableValue(*(listValue->get(i)), nonInterpolableData[i], range));
        return result.release();
    }

    friend class ListStyleInterpolationTest;
};

template<typename InterpolationType>
class ListStyleInterpolationImpl<InterpolationType, void> : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<ListStyleInterpolationImpl<InterpolationType, void>> maybeCreateFromList(const CSSValue& start, const CSSValue& end, CSSPropertyID id, InterpolationRange range = RangeAll)
    {
        if (!start.isValueList() || !end.isValueList())
            return nullptr;
        const CSSValueList& startList = toCSSValueList(start);
        const CSSValueList& endList = toCSSValueList(end);
        if (startList.length() != endList.length())
            return nullptr;
        for (const auto& value : startList) {
            if (!InterpolationType::canCreateFrom(*value))
                return nullptr;
        }
        for (const auto& value : endList) {
            if (!InterpolationType::canCreateFrom(*value))
                return nullptr;
        }
        return adoptRefWillBeNoop(new ListStyleInterpolationImpl<InterpolationType, void>(listToInterpolableValue(start), listToInterpolableValue(end), id, range));
    }

private:
    ListStyleInterpolationImpl(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id, InterpolationRange range = RangeAll)
        : StyleInterpolation(start, end, id), m_range(range)
    {
    }

    InterpolationRange m_range;

    static PassOwnPtrWillBeRawPtr<InterpolableValue> listToInterpolableValue(const CSSValue& value)
    {
        const CSSValueList& listValue = toCSSValueList(value);
        OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(listValue.length());
        for (size_t i = 0; i < listValue.length(); i++)
            result->set(i, InterpolationType::toInterpolableValue(*listValue.item(i)));
        return result.release();
    }

    static PassRefPtrWillBeRawPtr<CSSValue> interpolableValueToList(InterpolableValue* value, InterpolationRange range = RangeAll)
    {
        InterpolableList* listValue = toInterpolableList(value);
        RefPtrWillBeRawPtr<CSSValueList> result = CSSValueList::createCommaSeparated();

        for (size_t i = 0; i < listValue->length(); i++)
            result->append(InterpolationType::fromInterpolableValue(*(listValue->get(i)), range));
        return result.release();
    }

    virtual void apply(StyleResolverState& state) const override
    {
        StyleBuilder::applyProperty(m_id, state, interpolableValueToList(m_cachedValue.get(), m_range).get());
    }

    friend class ListStyleInterpolationTest;

};

template<typename InterpolationType>
class ListStyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<ListStyleInterpolationImpl<InterpolationType, typename InterpolationType::NonInterpolableType>>  maybeCreateFromList(const CSSValue& start, const CSSValue& end, CSSPropertyID id, InterpolationRange range = RangeAll)
    {
        return ListStyleInterpolationImpl<InterpolationType, typename InterpolationType::NonInterpolableType>::maybeCreateFromList(start, end, id, range);
    }
};

} // namespace blink

#endif // ListStyleInterpolation_h
