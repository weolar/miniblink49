/*
 * Copyright (C) 2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2008, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef CSSParserString_h
#define CSSParserString_h

#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {

struct CSSParserString {
    void init(const LChar* characters, unsigned length)
    {
        m_data.characters8 = characters;
        m_length = length;
        m_is8Bit = true;
    }

    void init(const UChar* characters, unsigned length)
    {
        m_data.characters16 = characters;
        m_length = length;
        m_is8Bit = false;
    }

    void initRaw(const void* charactersRaw, unsigned length, bool is8Bit)
    {
        m_data.charactersRaw = charactersRaw;
        m_length = length;
        m_is8Bit = is8Bit;
    }

    void init(const String& string)
    {
        m_length = string.length();
        if (string.isNull()) {
            m_data.characters8 = 0;
            m_is8Bit = true;
            return;
        }
        if (string.is8Bit()) {
            m_data.characters8 = const_cast<LChar*>(string.characters8());
            m_is8Bit = true;
        } else {
            m_data.characters16 = const_cast<UChar*>(string.characters16());
            m_is8Bit = false;
        }
    }

    bool is8Bit() const { return m_is8Bit; }
    const LChar* characters8() const { ASSERT(is8Bit()); return m_data.characters8; }
    const UChar* characters16() const { ASSERT(!is8Bit()); return m_data.characters16; }

    unsigned length() const { return m_length; }

    UChar operator[](unsigned i) const
    {
        ASSERT_WITH_SECURITY_IMPLICATION(i < m_length);
        if (is8Bit())
            return m_data.characters8[i];
        return m_data.characters16[i];
    }

    bool equalIgnoringCase(const char* str) const
    {
        bool match = is8Bit() ? WTF::equalIgnoringCase(str, characters8(), length()) : WTF::equalIgnoringCase(str, characters16(), length());
        if (!match)
            return false;
        ASSERT(strlen(str) >= length());
        return str[length()] == '\0';
    }

    operator String() const { return is8Bit() ? String(m_data.characters8, m_length) : StringImpl::create8BitIfPossible(m_data.characters16, m_length); }
    operator AtomicString() const { return is8Bit() ? AtomicString(m_data.characters8, m_length) : AtomicString(m_data.characters16, m_length); }

    union {
        const LChar* characters8;
        const UChar* characters16;
        const void* charactersRaw;
    } m_data;
    unsigned m_length;
    bool m_is8Bit;
};

}

#endif
