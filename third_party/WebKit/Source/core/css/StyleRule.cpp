/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * (C) 2002-2003 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2002, 2005, 2006, 2008, 2012 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/css/StyleRule.h"

#include "core/css/CSSFontFaceRule.h"
#include "core/css/CSSImportRule.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/CSSMediaRule.h"
#include "core/css/CSSPageRule.h"
#include "core/css/CSSStyleRule.h"
#include "core/css/CSSSupportsRule.h"
#include "core/css/CSSViewportRule.h"
#include "core/css/StyleRuleImport.h"
#include "core/css/StyleRuleKeyframe.h"
#include "core/css/StyleRuleNamespace.h"

namespace blink {

struct SameSizeAsStyleRuleBase : public RefCountedWillBeGarbageCollectedFinalized<SameSizeAsStyleRuleBase> {
    unsigned bitfields;
};

static_assert(sizeof(StyleRuleBase) <= sizeof(SameSizeAsStyleRuleBase), "StyleRuleBase should stay small");

PassRefPtrWillBeRawPtr<CSSRule> StyleRuleBase::createCSSOMWrapper(CSSStyleSheet* parentSheet) const
{
    return createCSSOMWrapper(parentSheet, 0);
}

PassRefPtrWillBeRawPtr<CSSRule> StyleRuleBase::createCSSOMWrapper(CSSRule* parentRule) const
{
    return createCSSOMWrapper(0, parentRule);
}

DEFINE_TRACE(StyleRuleBase)
{
    switch (type()) {
    case Charset:
        toStyleRuleCharset(this)->traceAfterDispatch(visitor);
        return;
    case Style:
        toStyleRule(this)->traceAfterDispatch(visitor);
        return;
    case Page:
        toStyleRulePage(this)->traceAfterDispatch(visitor);
        return;
    case FontFace:
        toStyleRuleFontFace(this)->traceAfterDispatch(visitor);
        return;
    case Media:
        toStyleRuleMedia(this)->traceAfterDispatch(visitor);
        return;
    case Supports:
        toStyleRuleSupports(this)->traceAfterDispatch(visitor);
        return;
    case Import:
        toStyleRuleImport(this)->traceAfterDispatch(visitor);
        return;
    case Keyframes:
        toStyleRuleKeyframes(this)->traceAfterDispatch(visitor);
        return;
    case Keyframe:
        toStyleRuleKeyframe(this)->traceAfterDispatch(visitor);
        return;
    case Namespace:
        toStyleRuleNamespace(this)->traceAfterDispatch(visitor);
        return;
    case Viewport:
        toStyleRuleViewport(this)->traceAfterDispatch(visitor);
        return;
    }
    ASSERT_NOT_REACHED();
}

void StyleRuleBase::finalizeGarbageCollectedObject()
{
    switch (type()) {
    case Charset:
        toStyleRuleCharset(this)->~StyleRuleCharset();
        return;
    case Style:
        toStyleRule(this)->~StyleRule();
        return;
    case Page:
        toStyleRulePage(this)->~StyleRulePage();
        return;
    case FontFace:
        toStyleRuleFontFace(this)->~StyleRuleFontFace();
        return;
    case Media:
        toStyleRuleMedia(this)->~StyleRuleMedia();
        return;
    case Supports:
        toStyleRuleSupports(this)->~StyleRuleSupports();
        return;
    case Import:
        toStyleRuleImport(this)->~StyleRuleImport();
        return;
    case Keyframes:
        toStyleRuleKeyframes(this)->~StyleRuleKeyframes();
        return;
    case Keyframe:
        toStyleRuleKeyframe(this)->~StyleRuleKeyframe();
        return;
    case Namespace:
        toStyleRuleNamespace(this)->~StyleRuleNamespace();
        return;
    case Viewport:
        toStyleRuleViewport(this)->~StyleRuleViewport();
        return;
    }
    ASSERT_NOT_REACHED();
}

void StyleRuleBase::destroy()
{
    switch (type()) {
    case Charset:
        delete toStyleRuleCharset(this);
        return;
    case Style:
        delete toStyleRule(this);
        return;
    case Page:
        delete toStyleRulePage(this);
        return;
    case FontFace:
        delete toStyleRuleFontFace(this);
        return;
    case Media:
        delete toStyleRuleMedia(this);
        return;
    case Supports:
        delete toStyleRuleSupports(this);
        return;
    case Import:
        delete toStyleRuleImport(this);
        return;
    case Keyframes:
        delete toStyleRuleKeyframes(this);
        return;
    case Keyframe:
        delete toStyleRuleKeyframe(this);
        return;
    case Namespace:
        delete toStyleRuleNamespace(this);
        return;
    case Viewport:
        delete toStyleRuleViewport(this);
        return;
    }
    ASSERT_NOT_REACHED();
}

PassRefPtrWillBeRawPtr<StyleRuleBase> StyleRuleBase::copy() const
{
    switch (type()) {
    case Style:
        return toStyleRule(this)->copy();
    case Page:
        return toStyleRulePage(this)->copy();
    case FontFace:
        return toStyleRuleFontFace(this)->copy();
    case Media:
        return toStyleRuleMedia(this)->copy();
    case Supports:
        return toStyleRuleSupports(this)->copy();
    case Import:
        // FIXME: Copy import rules.
        ASSERT_NOT_REACHED();
        return nullptr;
    case Keyframes:
        return toStyleRuleKeyframes(this)->copy();
    case Viewport:
        return toStyleRuleViewport(this)->copy();
    case Charset:
    case Keyframe:
    case Namespace:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSRule> StyleRuleBase::createCSSOMWrapper(CSSStyleSheet* parentSheet, CSSRule* parentRule) const
{
    RefPtrWillBeRawPtr<CSSRule> rule = nullptr;
    StyleRuleBase* self = const_cast<StyleRuleBase*>(this);
    switch (type()) {
    case Style:
        rule = CSSStyleRule::create(toStyleRule(self), parentSheet);
        break;
    case Page:
        rule = CSSPageRule::create(toStyleRulePage(self), parentSheet);
        break;
    case FontFace:
        rule = CSSFontFaceRule::create(toStyleRuleFontFace(self), parentSheet);
        break;
    case Media:
        rule = CSSMediaRule::create(toStyleRuleMedia(self), parentSheet);
        break;
    case Supports:
        rule = CSSSupportsRule::create(toStyleRuleSupports(self), parentSheet);
        break;
    case Import:
        rule = CSSImportRule::create(toStyleRuleImport(self), parentSheet);
        break;
    case Keyframes:
        rule = CSSKeyframesRule::create(toStyleRuleKeyframes(self), parentSheet);
        break;
    case Viewport:
        rule = CSSViewportRule::create(toStyleRuleViewport(self), parentSheet);
        break;
    case Keyframe:
    case Namespace:
    case Charset:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
    if (parentRule)
        rule->setParentRule(parentRule);
    return rule.release();
}

unsigned StyleRule::averageSizeInBytes()
{
    return sizeof(StyleRule) + sizeof(CSSSelector) + StylePropertySet::averageSizeInBytes();
}

StyleRule::StyleRule(CSSSelectorList& selectorList, PassRefPtrWillBeRawPtr<StylePropertySet> properties)
    : StyleRuleBase(Style)
    , m_properties(properties)
{
    m_selectorList.adopt(selectorList);
}

StyleRule::StyleRule(const StyleRule& o)
    : StyleRuleBase(o)
    , m_properties(o.m_properties->mutableCopy())
    , m_selectorList(o.m_selectorList)
{
}

StyleRule::~StyleRule()
{
}

MutableStylePropertySet& StyleRule::mutableProperties()
{
    if (!m_properties->isMutable())
        m_properties = m_properties->mutableCopy();
    return *toMutableStylePropertySet(m_properties.get());
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRule)
{
    visitor->trace(m_properties);
    StyleRuleBase::traceAfterDispatch(visitor);
}

StyleRulePage::StyleRulePage(CSSSelectorList& selectorList, PassRefPtrWillBeRawPtr<StylePropertySet> properties)
    : StyleRuleBase(Page)
    , m_properties(properties)
{
    m_selectorList.adopt(selectorList);
}

StyleRulePage::StyleRulePage(const StyleRulePage& o)
    : StyleRuleBase(o)
    , m_properties(o.m_properties->mutableCopy())
    , m_selectorList(o.m_selectorList)
{
}

StyleRulePage::~StyleRulePage()
{
}

MutableStylePropertySet& StyleRulePage::mutableProperties()
{
    if (!m_properties->isMutable())
        m_properties = m_properties->mutableCopy();
    return *toMutableStylePropertySet(m_properties.get());
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRulePage)
{
    visitor->trace(m_properties);
    StyleRuleBase::traceAfterDispatch(visitor);
}

StyleRuleFontFace::StyleRuleFontFace(PassRefPtrWillBeRawPtr<StylePropertySet> properties)
    : StyleRuleBase(FontFace)
    , m_properties(properties)
{
}

StyleRuleFontFace::StyleRuleFontFace(const StyleRuleFontFace& o)
    : StyleRuleBase(o)
    , m_properties(o.m_properties->mutableCopy())
{
}

StyleRuleFontFace::~StyleRuleFontFace()
{
}

MutableStylePropertySet& StyleRuleFontFace::mutableProperties()
{
    if (!m_properties->isMutable())
        m_properties = m_properties->mutableCopy();
    return *toMutableStylePropertySet(m_properties);
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRuleFontFace)
{
    visitor->trace(m_properties);
    StyleRuleBase::traceAfterDispatch(visitor);
}

StyleRuleGroup::StyleRuleGroup(Type type, WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>>& adoptRule)
    : StyleRuleBase(type)
{
    m_childRules.swap(adoptRule);
}

StyleRuleGroup::StyleRuleGroup(const StyleRuleGroup& o)
    : StyleRuleBase(o)
    , m_childRules(o.m_childRules.size())
{
    for (unsigned i = 0; i < m_childRules.size(); ++i)
        m_childRules[i] = o.m_childRules[i]->copy();
}

void StyleRuleGroup::wrapperInsertRule(unsigned index, PassRefPtrWillBeRawPtr<StyleRuleBase> rule)
{
    m_childRules.insert(index, rule);
}

void StyleRuleGroup::wrapperRemoveRule(unsigned index)
{
    m_childRules.remove(index);
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRuleGroup)
{
    visitor->trace(m_childRules);
    StyleRuleBase::traceAfterDispatch(visitor);
}

StyleRuleMedia::StyleRuleMedia(PassRefPtrWillBeRawPtr<MediaQuerySet> media, WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>>& adoptRules)
    : StyleRuleGroup(Media, adoptRules)
    , m_mediaQueries(media)
{
}

StyleRuleMedia::StyleRuleMedia(const StyleRuleMedia& o)
    : StyleRuleGroup(o)
{
    if (o.m_mediaQueries)
        m_mediaQueries = o.m_mediaQueries->copy();
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRuleMedia)
{
    visitor->trace(m_mediaQueries);
    StyleRuleGroup::traceAfterDispatch(visitor);
}

StyleRuleSupports::StyleRuleSupports(const String& conditionText, bool conditionIsSupported, WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>>& adoptRules)
    : StyleRuleGroup(Supports, adoptRules)
    , m_conditionText(conditionText)
    , m_conditionIsSupported(conditionIsSupported)
{
}

StyleRuleSupports::StyleRuleSupports(const StyleRuleSupports& o)
    : StyleRuleGroup(o)
    , m_conditionText(o.m_conditionText)
    , m_conditionIsSupported(o.m_conditionIsSupported)
{
}

StyleRuleViewport::StyleRuleViewport(PassRefPtrWillBeRawPtr<StylePropertySet> properties)
    : StyleRuleBase(Viewport)
    , m_properties(properties)
{
}

StyleRuleViewport::StyleRuleViewport(const StyleRuleViewport& o)
    : StyleRuleBase(o)
    , m_properties(o.m_properties->mutableCopy())
{
}

StyleRuleViewport::~StyleRuleViewport()
{
}

MutableStylePropertySet& StyleRuleViewport::mutableProperties()
{
    if (!m_properties->isMutable())
        m_properties = m_properties->mutableCopy();
    return *toMutableStylePropertySet(m_properties);
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRuleViewport)
{
    visitor->trace(m_properties);
    StyleRuleBase::traceAfterDispatch(visitor);
}

} // namespace blink
