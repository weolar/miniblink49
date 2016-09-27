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
#include "modules/encoding/TextDecoder.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMArrayBufferView.h"
#include "core/dom/ExceptionCode.h"
#include "modules/encoding/Encoding.h"
#include "wtf/StringExtras.h"
#include "wtf/text/TextEncodingRegistry.h"

namespace blink {

TextDecoder* TextDecoder::create(const String& label, const TextDecoderOptions& options, ExceptionState& exceptionState)
{
    WTF::TextEncoding encoding(label.stripWhiteSpace(&Encoding::isASCIIWhiteSpace));
    // The replacement encoding is not valid, but the Encoding API also
    // rejects aliases of the replacement encoding.
    if (!encoding.isValid() || !strcasecmp(encoding.name(), "replacement")) {
        exceptionState.throwRangeError("The encoding label provided ('" + label + "') is invalid.");
        return 0;
    }

    return new TextDecoder(encoding, options.fatal(), options.ignoreBOM());
}


TextDecoder::TextDecoder(const WTF::TextEncoding& encoding, bool fatal, bool ignoreBOM)
    : m_encoding(encoding)
    , m_codec(newTextCodec(encoding))
    , m_fatal(fatal)
    , m_ignoreBOM(ignoreBOM)
    , m_bomSeen(false)
{
}

TextDecoder::~TextDecoder()
{
}

String TextDecoder::encoding() const
{
    String name = String(m_encoding.name()).lower();
    // Where possible, encoding aliases should be handled by changes to Chromium's ICU or Blink's WTF.
    // The same codec is used, but WTF maintains a different name/identity for these.
    if (name == "iso-8859-1" || name == "us-ascii")
        return "windows-1252";
    return name;
}

String TextDecoder::decode(const BufferSource& input, const TextDecodeOptions& options, ExceptionState& exceptionState)
{
    ASSERT(!input.isNull());
    if (input.isArrayBufferView()) {
        const char* start = static_cast<const char*>(input.getAsArrayBufferView()->baseAddress());
        size_t length = input.getAsArrayBufferView()->byteLength();
        return decode(start, length, options, exceptionState);
    }
    ASSERT(input.isArrayBuffer());
    const char* start = static_cast<const char*>(input.getAsArrayBuffer()->data());
    size_t length = input.getAsArrayBuffer()->byteLength();
    return decode(start, length, options, exceptionState);
}

String TextDecoder::decode(const char* start, size_t length, const TextDecodeOptions& options, ExceptionState& exceptionState)
{
    WTF::FlushBehavior flush = options.stream() ? WTF::DoNotFlush : WTF::DataEOF;

    bool sawError = false;
    String s = m_codec->decode(start, length, flush, m_fatal, sawError);

    if (m_fatal && sawError) {
        exceptionState.throwTypeError("The encoded data was not valid.");
        return String();
    }

    if (!m_ignoreBOM && !m_bomSeen && !s.isEmpty()) {
        m_bomSeen = true;
        String name(m_encoding.name());
        if ((name == "UTF-8" || name == "UTF-16LE" || name == "UTF-16BE") && s[0] == 0xFEFF)
            s.remove(0);
    }

    if (flush)
        m_bomSeen = false;

    return s;
}

String TextDecoder::decode(ExceptionState& exceptionState)
{
    TextDecodeOptions options;
    return decode(nullptr, 0, options, exceptionState);
}

} // namespace blink
