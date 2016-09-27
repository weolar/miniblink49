/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#ifndef VTTToken_h
#define VTTToken_h

namespace blink {

class VTTTokenTypes {
public:
    enum Type {
        Uninitialized,
        Character,
        StartTag,
        EndTag,
        TimestampTag,
    };
};

class VTTToken {
public:
    typedef VTTTokenTypes Type;

    VTTToken() : m_type(Type::Uninitialized) { }

    static VTTToken StringToken(const String& characterData)
    {
        return VTTToken(Type::Character, characterData);
    }
    static VTTToken StartTag(const String& tagName, const AtomicString& classes = emptyAtom, const AtomicString& annotation = emptyAtom)
    {
        VTTToken token(Type::StartTag, tagName);
        token.m_classes = classes;
        token.m_annotation = annotation;
        return token;
    }
    static VTTToken EndTag(const String& tagName)
    {
        return VTTToken(Type::EndTag, tagName);
    }
    static VTTToken TimestampTag(const String& timestampData)
    {
        return VTTToken(Type::TimestampTag, timestampData);
    }

    Type::Type type() const { return m_type; }
    const String& name() const { return m_data; }
    const String& characters() const { return m_data; }
    const AtomicString& classes() const { return m_classes; }
    const AtomicString& annotation() const { return m_annotation; }

private:
    VTTToken(Type::Type type, const String& data)
        : m_type(type)
        , m_data(data) { }

    Type::Type m_type;
    String m_data;
    AtomicString m_annotation;
    AtomicString m_classes;
};

}

#endif
