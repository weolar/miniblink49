// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSVariableData_h
#define CSSVariableData_h

#include "core/css/parser/CSSParserToken.h"
#include "core/css/parser/CSSParserTokenRange.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class CSSParserTokenRange;

class CSSVariableData : public RefCounted<CSSVariableData> {
    WTF_MAKE_NONCOPYABLE(CSSVariableData);
    //USING_FAST_MALLOC(CSSVariableData);
public:
    static PassRefPtr<CSSVariableData> create(const CSSParserTokenRange& range, bool needsVariableResolution = true)
    {
        return adoptRef(new CSSVariableData(range, needsVariableResolution));
    }

    static PassRefPtr<CSSVariableData> createResolved(const Vector<CSSParserToken>& resolvedTokens)
    {
        return adoptRef(new CSSVariableData(resolvedTokens));
    }

    CSSParserTokenRange tokenRange() { return m_tokens; }

    const Vector<CSSParserToken>& tokens() { return m_tokens; }

    bool needsVariableResolution() const { return m_needsVariableResolution; }
private:
    CSSVariableData(const CSSParserTokenRange&, bool needsVariableResolution);

    // We can safely copy the tokens (which have raw pointers to substrings) because
    // StylePropertySets contain references to CSSCustomPropertyDeclarations, which
    // point to the unresolved CSSVariableData values that own the backing strings
    // this will potentially reference.
    CSSVariableData(const Vector<CSSParserToken>& resolvedTokens)
        : m_tokens(resolvedTokens)
        , m_needsVariableResolution(false)
    { }

    void consumeAndUpdateTokens(const CSSParserTokenRange&);
    template<typename CharacterType> void updateTokens(const CSSParserTokenRange&);

    String m_backingString;
    Vector<CSSParserToken> m_tokens;
    const bool m_needsVariableResolution;
};

} // namespace blink

#endif // CSSVariableData_h