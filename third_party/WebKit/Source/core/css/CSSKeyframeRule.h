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

#ifndef CSSKeyframeRule_h
#define CSSKeyframeRule_h

#include "core/css/CSSRule.h"
#include "core/css/StyleRuleKeyframe.h"

namespace blink {

class CSSKeyframesRule;
class CSSStyleDeclaration;
class ExceptionState;
class KeyframeStyleRuleCSSStyleDeclaration;

class CSSKeyframeRule final : public CSSRule {
    DEFINE_WRAPPERTYPEINFO();
public:
    virtual ~CSSKeyframeRule();

    virtual String cssText() const override { return m_keyframe->cssText(); }
    virtual void reattach(StyleRuleBase*) override;

    String keyText() const { return m_keyframe->keyText(); }
    void setKeyText(const String&, ExceptionState&);

    CSSStyleDeclaration* style() const;

    DECLARE_VIRTUAL_TRACE();

private:
    CSSKeyframeRule(StyleRuleKeyframe*, CSSKeyframesRule* parent);

    virtual CSSRule::Type type() const override { return KEYFRAME_RULE; }

    RefPtrWillBeMember<StyleRuleKeyframe> m_keyframe;
    mutable RefPtrWillBeMember<KeyframeStyleRuleCSSStyleDeclaration> m_propertiesCSSOMWrapper;

    friend class CSSKeyframesRule;
};

DEFINE_CSS_RULE_TYPE_CASTS(CSSKeyframeRule, KEYFRAME_RULE);

} // namespace blink

#endif // CSSKeyframeRule_h
