// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/RectSVGInterpolation.h"

#include "wtf/StdLibExtras.h"

namespace blink {

PassOwnPtrWillBeRawPtr<InterpolableValue> RectSVGInterpolation::toInterpolableValue(SVGPropertyBase* value)
{
    RefPtrWillBeRawPtr<SVGRect> rect = toSVGRect(value);
    float element[] = { rect->x(), rect->y(), rect->width(), rect->height() };
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(WTF_ARRAY_LENGTH(element));
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(element); i++) {
        result->set(i, InterpolableNumber::create(element[i]));
    }
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGRect> RectSVGInterpolation::fromInterpolableValue(const InterpolableValue& value)
{
    const InterpolableList& list = toInterpolableList(value);
    RefPtrWillBeRawPtr<SVGRect> result = SVGRect::create();
    result->setX(toInterpolableNumber(list.get(0))->value());
    result->setY(toInterpolableNumber(list.get(1))->value());
    result->setWidth(toInterpolableNumber(list.get(2))->value());
    result->setHeight(toInterpolableNumber(list.get(3))->value());
    return result;
}

}
