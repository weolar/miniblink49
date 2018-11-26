/*
 * Copyright (C) 2012 Motorola Mobility Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Motorola Mobility Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/css/DOMWindowCSS.h"

#include "core/css/CSSMarkup.h"
#include "core/css/CSSPropertyMetadata.h"
#include "core/css/StylePropertySet.h"
#include "core/css/parser/CSSParser.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

bool DOMWindowCSS::supports(const String& property, const String& value)
{
    CSSPropertyID unresolvedProperty = unresolvedCSSPropertyID(property);
    if (unresolvedProperty == CSSPropertyInvalid)
        return false;
    ASSERT(CSSPropertyMetadata::isEnabledProperty(unresolvedProperty));

    // This will return false when !important is present
    RefPtrWillBeRawPtr<MutableStylePropertySet> dummyStyle = MutableStylePropertySet::create();
    return CSSParser::parseValue(dummyStyle.get(), unresolvedProperty, value, false, HTMLStandardMode, 0);
}

bool DOMWindowCSS::supports(const String& conditionText)
{
    return CSSParser::parseSupportsCondition(conditionText);
}

String DOMWindowCSS::escape(const String& ident)
{
    StringBuilder builder;
    serializeIdentifier(ident, builder);
    return builder.toString();
}

}
