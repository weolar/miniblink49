/*
 * Copyright 2014 The Chromium Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "config.h"
#include "StringConcatenate.h"

// This macro is helpful for testing how many intermediate Strings are created while evaluating an
// expression containing operator+.
#ifndef WTF_STRINGTYPEADAPTER_COPIED_WTF_STRING
#define WTF_STRINGTYPEADAPTER_COPIED_WTF_STRING() ((void)0)
#endif

void WTF::StringTypeAdapter<char*>::writeTo(LChar* destination)
{
    for (unsigned i = 0; i < m_length; ++i)
        destination[i] = static_cast<LChar>(m_buffer[i]);
}

void WTF::StringTypeAdapter<char*>::writeTo(UChar* destination)
{
    for (unsigned i = 0; i < m_length; ++i) {
        unsigned char c = m_buffer[i];
        destination[i] = c;
    }
}

WTF::StringTypeAdapter<LChar*>::StringTypeAdapter(LChar* buffer)
    : m_buffer(buffer)
    , m_length(strlen(reinterpret_cast<char*>(buffer)))
{
}

void WTF::StringTypeAdapter<LChar*>::writeTo(LChar* destination)
{
    memcpy(destination, m_buffer, m_length * sizeof(LChar));
}

void WTF::StringTypeAdapter<LChar*>::writeTo(UChar* destination)
{
    StringImpl::copyChars(destination, m_buffer, m_length);
}

WTF::StringTypeAdapter<const UChar*>::StringTypeAdapter(const UChar* buffer)
    : m_buffer(buffer)
{
    size_t len = 0;
    while (m_buffer[len] != UChar(0))
        ++len;

    RELEASE_ASSERT(len <= std::numeric_limits<unsigned>::max());

    m_length = len;
}

void WTF::StringTypeAdapter<const UChar*>::writeTo(UChar* destination)
{
    memcpy(destination, m_buffer, m_length * sizeof(UChar));
}

WTF::StringTypeAdapter<const char*>::StringTypeAdapter(const char* buffer)
: m_buffer(buffer)
, m_length(strlen(buffer))
{
}

void WTF::StringTypeAdapter<const char*>::writeTo(LChar* destination)
{
    memcpy(destination, m_buffer, static_cast<size_t>(m_length) * sizeof(LChar));
}

void WTF::StringTypeAdapter<const char*>::writeTo(UChar* destination)
{
    for (unsigned i = 0; i < m_length; ++i) {
        unsigned char c = m_buffer[i];
        destination[i] = c;
    }
}

WTF::StringTypeAdapter<const LChar*>::StringTypeAdapter(const LChar* buffer)
: m_buffer(buffer)
, m_length(strlen(reinterpret_cast<const char*>(buffer)))
{
}

void WTF::StringTypeAdapter<const LChar*>::writeTo(LChar* destination)
{
    memcpy(destination, m_buffer, static_cast<size_t>(m_length) * sizeof(LChar));
}

void WTF::StringTypeAdapter<const LChar*>::writeTo(UChar* destination)
{
    StringImpl::copyChars(destination, m_buffer, m_length);
}

void WTF::StringTypeAdapter<Vector<char>>::writeTo(LChar* destination)
{
    for (size_t i = 0; i < m_buffer.size(); ++i)
        destination[i] = static_cast<unsigned char>(m_buffer[i]);
}

void WTF::StringTypeAdapter<Vector<char>>::writeTo(UChar* destination)
{
    for (size_t i = 0; i < m_buffer.size(); ++i)
        destination[i] = static_cast<unsigned char>(m_buffer[i]);
}

void WTF::StringTypeAdapter<Vector<LChar>>::writeTo(LChar* destination)
{
    for (size_t i = 0; i < m_buffer.size(); ++i)
        destination[i] = m_buffer[i];
}

void WTF::StringTypeAdapter<Vector<LChar>>::writeTo(UChar* destination)
{
    for (size_t i = 0; i < m_buffer.size(); ++i)
        destination[i] = m_buffer[i];
}

void WTF::StringTypeAdapter<String>::writeTo(LChar* destination)
{
    unsigned length = m_buffer.length();

    ASSERT(is8Bit());
    const LChar* data = m_buffer.characters8();
    for (unsigned i = 0; i < length; ++i)
        destination[i] = data[i];

    WTF_STRINGTYPEADAPTER_COPIED_WTF_STRING();
}

void WTF::StringTypeAdapter<String>::writeTo(UChar* destination)
{
    unsigned length = m_buffer.length();

    if (is8Bit()) {
        const LChar* data = m_buffer.characters8();
        for (unsigned i = 0; i < length; ++i)
            destination[i] = data[i];
    } else {
        const UChar* data = m_buffer.characters16();
        for (unsigned i = 0; i < length; ++i)
            destination[i] = data[i];
    }

    WTF_STRINGTYPEADAPTER_COPIED_WTF_STRING();
}

