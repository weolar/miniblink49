// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/KeyframeStyleRuleCSSStyleDeclaration.h"

#include "core/css/CSSKeyframeRule.h"
#include "core/css/CSSKeyframesRule.h"

namespace blink {

KeyframeStyleRuleCSSStyleDeclaration::KeyframeStyleRuleCSSStyleDeclaration(MutableStylePropertySet& propertySetArg, CSSKeyframeRule* parentRule)
    : StyleRuleCSSStyleDeclaration(propertySetArg, parentRule)
{
}

void KeyframeStyleRuleCSSStyleDeclaration::didMutate(MutationType type)
{
    StyleRuleCSSStyleDeclaration::didMutate(type);
    toCSSKeyframesRule(m_parentRule->parentRule())->styleChanged();
}

} // namespace blink
