// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/ImageStyleInterpolation.h"

#include "core/css/resolver/StyleBuilder.h"


namespace blink {

bool ImageStyleInterpolation::canCreateFrom(const CSSValue& value)
{
    return value.isImageValue() || value.isGradientValue();
}

void ImageStyleInterpolation::apply(StyleResolverState& state) const
{
    double cachedValue = toInterpolableNumber(m_cachedValue.get())->value();
    if (cachedValue <= 0.0) {
        StyleBuilder::applyProperty(m_id, state, m_initialImage.get());
        return;
    }
    if (cachedValue >= 1.0) {
        StyleBuilder::applyProperty(m_id, state, m_finalImage.get());
        return;
    }
    RefPtrWillBeRawPtr<CSSCrossfadeValue> crossfadeValue = CSSCrossfadeValue::create(m_initialImage, m_finalImage);
    crossfadeValue->setPercentage(CSSPrimitiveValue::create(cachedValue, CSSPrimitiveValue::CSS_NUMBER));

    StyleBuilder::applyProperty(m_id, state, crossfadeValue.get());
}

DEFINE_TRACE(ImageStyleInterpolation)
{
    visitor->trace(m_initialImage);
    visitor->trace(m_finalImage);
    StyleInterpolation::trace(visitor);
}

} // namespace blink
