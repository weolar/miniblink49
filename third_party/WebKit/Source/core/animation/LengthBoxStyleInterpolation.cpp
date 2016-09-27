// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/LengthBoxStyleInterpolation.h"

#include "core/css/resolver/StyleBuilder.h"

namespace blink {

namespace {

bool onlyInterpolateBetweenLengthAndCSSValueAuto(Rect& startRect, Rect& endRect)
{
    return startRect.left()->isLength() != endRect.left()->isLength()
        && startRect.right()->isLength() != endRect.right()->isLength()
        && startRect.top()->isLength() != endRect.top()->isLength()
        && startRect.bottom()->isLength() != endRect.bottom()->isLength();
}

} // namespace

PassRefPtrWillBeRawPtr<LengthBoxStyleInterpolation> LengthBoxStyleInterpolation::maybeCreateFrom(CSSValue& start, CSSValue& end, CSSPropertyID id)
{
    bool startRect = start.isPrimitiveValue() && toCSSPrimitiveValue(start).isRect();
    bool endRect = end.isPrimitiveValue() && toCSSPrimitiveValue(end).isRect();

    if (startRect && endRect)
        return adoptRefWillBeNoop(new LengthBoxStyleInterpolation(lengthBoxtoInterpolableValue(start, end, false), lengthBoxtoInterpolableValue(end, start, true), id, &start, &end));
    return nullptr;
}

PassOwnPtrWillBeRawPtr<InterpolableValue> LengthBoxStyleInterpolation::lengthBoxtoInterpolableValue(const CSSValue& lengthBox, const CSSValue& matchingValue, bool isEndInterpolation)
{
    const int numberOfSides = 4;
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(numberOfSides);
    Rect* rect = toCSSPrimitiveValue(lengthBox).getRectValue();
    Rect* matchingRect = toCSSPrimitiveValue(matchingValue).getRectValue();
    CSSPrimitiveValue* side[numberOfSides] = { rect->left(), rect->right(), rect->top(), rect->bottom() };
    CSSPrimitiveValue* matchingSide[numberOfSides] = { matchingRect->left(), matchingRect->right(), matchingRect->top(), matchingRect->bottom() };

    for (size_t i = 0; i < numberOfSides; i++) {
        if (side[i]->isValueID() || matchingSide[i]->isValueID()) {
            result->set(i, InterpolableBool::create(isEndInterpolation));
        } else {
            ASSERT(LengthStyleInterpolation::canCreateFrom(*side[i]));
            result->set(i, LengthStyleInterpolation::toInterpolableValue(*side[i]));
        }
    }
    return result.release();
}

bool LengthBoxStyleInterpolation::usesDefaultInterpolation(const CSSValue& start, const CSSValue& end)
{
    if (!start.isPrimitiveValue() || !end.isPrimitiveValue())
        return false;
    const CSSPrimitiveValue& startValue = toCSSPrimitiveValue(start);
    const CSSPrimitiveValue& endValue = toCSSPrimitiveValue(end);
    if ((startValue.isValueID() && startValue.getValueID() == CSSValueAuto)
        || (endValue.isValueID() && endValue.getValueID() == CSSValueAuto))
        return true;
    return onlyInterpolateBetweenLengthAndCSSValueAuto(*startValue.getRectValue(), *endValue.getRectValue());
}

namespace {

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> indexedValueToLength(InterpolableList& lengthBox, size_t i, CSSPrimitiveValue* start[], CSSPrimitiveValue* end[])
{
    if (lengthBox.get(i)->isBool()) {
        if (toInterpolableBool(lengthBox.get(i))->value())
            return end[i];
        return start[i];
    }
    return LengthStyleInterpolation::fromInterpolableValue(*lengthBox.get(i), RangeAll);
}

}

PassRefPtrWillBeRawPtr<CSSValue> LengthBoxStyleInterpolation::interpolableValueToLengthBox(InterpolableValue* value, const CSSValue& originalStart, const CSSValue& originalEnd)
{
    InterpolableList* lengthBox = toInterpolableList(value);
    Rect* startRect = toCSSPrimitiveValue(originalStart).getRectValue();
    Rect* endRect = toCSSPrimitiveValue(originalEnd).getRectValue();
    CSSPrimitiveValue* startSides[4] = { startRect->left(), startRect->right(), startRect->top(), startRect->bottom() };
    CSSPrimitiveValue* endSides[4] = { endRect->left(), endRect->right(), endRect->top(), endRect->bottom() };
    RefPtrWillBeRawPtr<Rect> result = Rect::create();

    result->setLeft(indexedValueToLength(*lengthBox, 0, startSides, endSides));
    result->setRight(indexedValueToLength(*lengthBox, 1, startSides, endSides));
    result->setTop(indexedValueToLength(*lengthBox, 2, startSides, endSides));
    result->setBottom(indexedValueToLength(*lengthBox, 3, startSides, endSides));

    return CSSPrimitiveValue::create(result.release());
}

void LengthBoxStyleInterpolation::apply(StyleResolverState& state) const
{
    if (m_cachedValue.get()->isBool())
        StyleBuilder::applyProperty(m_id, state, toInterpolableBool(m_cachedValue.get())->value() ? m_endCSSValue.get() : m_startCSSValue.get());
    else
        StyleBuilder::applyProperty(m_id, state, interpolableValueToLengthBox(m_cachedValue.get(), *m_startCSSValue, *m_endCSSValue).get());
}

DEFINE_TRACE(LengthBoxStyleInterpolation)
{
    StyleInterpolation::trace(visitor);
    visitor->trace(m_startCSSValue);
    visitor->trace(m_endCSSValue);
}

}
