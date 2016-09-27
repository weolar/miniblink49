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

#include "config.h"

#include "modules/encoding/TextEncoder.h"

#include "bindings/core/v8/ExceptionState.h"
#include "modules/encoding/Encoding.h"
#include "wtf/text/CString.h"
#include "wtf/text/TextEncodingRegistry.h"

namespace blink {

TextEncoder* TextEncoder::create(const String& utfLabel, ExceptionState& exceptionState)
{
    WTF::TextEncoding encoding(utfLabel.stripWhiteSpace(&Encoding::isASCIIWhiteSpace));
    if (!encoding.isValid()) {
        exceptionState.throwRangeError("The encoding label provided ('" + utfLabel + "') is invalid.");
        return 0;
    }

    String name(encoding.name());
    if (name != "UTF-8" && name != "UTF-16LE" && name != "UTF-16BE") {
        exceptionState.throwRangeError("The encoding provided ('" + utfLabel + "') is not one of 'utf-8', 'utf-16', or 'utf-16be'.");
        return 0;
    }

    return new TextEncoder(encoding);
}

TextEncoder::TextEncoder(const WTF::TextEncoding& encoding)
    : m_encoding(encoding)
    , m_codec(newTextCodec(encoding))
{
}

TextEncoder::~TextEncoder()
{
}

String TextEncoder::encoding() const
{
    String name = String(m_encoding.name()).lower();
    ASSERT(name == "utf-8" || name == "utf-16le" || name == "utf-16be");
    return name;
}

PassRefPtr<DOMUint8Array> TextEncoder::encode(const String& input)
{
    CString result;
    if (input.is8Bit())
        result = m_codec->encode(input.characters8(), input.length(), WTF::QuestionMarksForUnencodables);
    else
        result = m_codec->encode(input.characters16(), input.length(), WTF::QuestionMarksForUnencodables);

    const char* buffer = result.data();
    const unsigned char* unsignedBuffer = reinterpret_cast<const unsigned char*>(buffer);

    return DOMUint8Array::create(unsignedBuffer, result.length());
}

} // namespace blink
