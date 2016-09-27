// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/StyleRuleKeyframe.h"

#include "core/css/StylePropertySet.h"
#include "core/css/parser/CSSParser.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

StyleRuleKeyframe::StyleRuleKeyframe(PassOwnPtr<Vector<double>> keys, PassRefPtrWillBeRawPtr<StylePropertySet> properties)
: StyleRuleBase(Keyframe)
, m_properties(properties)
, m_keys(*keys)
{
}

String StyleRuleKeyframe::keyText() const
{
    ASSERT(!m_keys.isEmpty());

    StringBuilder keyText;
    for (unsigned i = 0; i < m_keys.size(); ++i) {
        if (i)
            keyText.append(',');
        keyText.appendNumber(m_keys.at(i) * 100);
        keyText.append('%');
    }

    return keyText.toString();
}

bool StyleRuleKeyframe::setKeyText(const String& keyText)
{
    ASSERT(!keyText.isNull());

    OwnPtr<Vector<double>> keys = CSSParser::parseKeyframeKeyList(keyText);
    if (!keys || keys->isEmpty())
        return false;

    m_keys = *keys;
    return true;
}

const Vector<double>& StyleRuleKeyframe::keys() const
{
    return m_keys;
}

MutableStylePropertySet& StyleRuleKeyframe::mutableProperties()
{
    if (!m_properties->isMutable())
        m_properties = m_properties->mutableCopy();
    return *toMutableStylePropertySet(m_properties.get());
}

String StyleRuleKeyframe::cssText() const
{
    StringBuilder result;
    result.append(keyText());
    result.appendLiteral(" { ");
    String decls = m_properties->asText();
    result.append(decls);
    if (!decls.isEmpty())
        result.append(' ');
    result.append('}');
    return result.toString();
}

DEFINE_TRACE_AFTER_DISPATCH(StyleRuleKeyframe)
{
    visitor->trace(m_properties);
    StyleRuleBase::traceAfterDispatch(visitor);
}

} // namespace blink
