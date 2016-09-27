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

#ifndef TextDecoder_h
#define TextDecoder_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "modules/encoding/TextDecodeOptions.h"
#include "modules/encoding/TextDecoderOptions.h"
#include "platform/heap/Handle.h"
#include "wtf/text/TextCodec.h"
#include "wtf/text/TextEncoding.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ExceptionState;

typedef ArrayBufferOrArrayBufferView BufferSource;

class TextDecoder final : public GarbageCollectedFinalized<TextDecoder>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static TextDecoder* create(const String& label, const TextDecoderOptions&, ExceptionState&);
    ~TextDecoder();

    // Implement the IDL
    String encoding() const;
    bool fatal() const { return m_fatal; }
    bool ignoreBOM() const { return m_ignoreBOM; }
    String decode(const BufferSource&, const TextDecodeOptions&, ExceptionState&);
    String decode(ExceptionState&);

    DEFINE_INLINE_TRACE() { }

private:
    TextDecoder(const WTF::TextEncoding&, bool fatal, bool ignoreBOM);

    String decode(const char* start, size_t length, const TextDecodeOptions&, ExceptionState&);

    WTF::TextEncoding m_encoding;
    OwnPtr<WTF::TextCodec> m_codec;
    bool m_fatal;
    bool m_ignoreBOM;
    bool m_bomSeen;
};

} // namespace blink

#endif // TextDecoder_h
