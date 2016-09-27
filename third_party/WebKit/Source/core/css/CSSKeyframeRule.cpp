/*
 * Copyright (C) 2007, 2008, 2012 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/css/CSSKeyframeRule.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/KeyframeStyleRuleCSSStyleDeclaration.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

CSSKeyframeRule::CSSKeyframeRule(StyleRuleKeyframe* keyframe, CSSKeyframesRule* parent)
    : CSSRule(0)
    , m_keyframe(keyframe)
{
    setParentRule(parent);
}

CSSKeyframeRule::~CSSKeyframeRule()
{
#if !ENABLE(OILPAN)
    if (m_propertiesCSSOMWrapper)
        m_propertiesCSSOMWrapper->clearParentRule();
#endif
}

void CSSKeyframeRule::setKeyText(const String& keyText, ExceptionState& exceptionState)
{
    if (!m_keyframe->setKeyText(keyText))
        exceptionState.throwDOMException(SyntaxError, "The key '" + keyText + "' is invalid and cannot be parsed");

    toCSSKeyframesRule(parentRule())->styleChanged();
}

CSSStyleDeclaration* CSSKeyframeRule::style() const
{
    if (!m_propertiesCSSOMWrapper)
        m_propertiesCSSOMWrapper = KeyframeStyleRuleCSSStyleDeclaration::create(m_keyframe->mutableProperties(), const_cast<CSSKeyframeRule*>(this));
    return m_propertiesCSSOMWrapper.get();
}

void CSSKeyframeRule::reattach(StyleRuleBase*)
{
    // No need to reattach, the underlying data is shareable on mutation.
    ASSERT_NOT_REACHED();
}

DEFINE_TRACE(CSSKeyframeRule)
{
    visitor->trace(m_keyframe);
    visitor->trace(m_propertiesCSSOMWrapper);
    CSSRule::trace(visitor);
}

} // namespace blink
