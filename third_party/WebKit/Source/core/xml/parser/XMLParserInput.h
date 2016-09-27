/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef XMLParserInput_h
#define XMLParserInput_h

#include "wtf/text/WTFString.h"

namespace blink {

class XMLParserInput {
public:
    explicit XMLParserInput(const String& source)
        : m_source(source)
        , m_encoding(0)
        , m_data(0)
        , m_size(0)
    {
        if (m_source.isEmpty())
            return;

        const UChar BOM = 0xFEFF;
        const unsigned char BOMHighByte = *reinterpret_cast<const unsigned char*>(&BOM);

        if (m_source.is8Bit()) {
            m_encoding = "iso-8859-1";
            m_data = reinterpret_cast<const char*>(m_source.characters8());
            m_size = m_source.length() * sizeof(LChar);
        } else {
            m_encoding = BOMHighByte == 0xFF ? "UTF-16LE" : "UTF-16BE";
            m_data = reinterpret_cast<const char*>(m_source.characters16());
            m_size = m_source.length() * sizeof(UChar);
        }
    }

    const char* encoding() const { return m_encoding; }
    const char* data() const { return m_data; }
    int size() const { return m_size; }

private:
    String m_source;
    const char* m_encoding;
    const char* m_data;
    int m_size;
};

}

#endif
