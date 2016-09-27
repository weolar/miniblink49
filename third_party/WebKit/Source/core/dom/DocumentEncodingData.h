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

#ifndef DocumentEncodingData_h
#define DocumentEncodingData_h

#include "platform/CrossThreadCopier.h"
#include "wtf/text/TextEncoding.h"

namespace blink {
class TextResourceDecoder;

class DocumentEncodingData {
public:
    DocumentEncodingData();
    explicit DocumentEncodingData(const TextResourceDecoder&);

    const WTF::TextEncoding& encoding() const { return m_encoding; }
    void setEncoding(const WTF::TextEncoding&);
    bool wasDetectedHeuristically() const { return m_wasDetectedHeuristically; }
    bool sawDecodingError() const { return m_sawDecodingError; }

private:
    WTF::TextEncoding m_encoding;
    bool m_wasDetectedHeuristically;
    bool m_sawDecodingError;
};

template<> struct CrossThreadCopierBase<false, false, false, DocumentEncodingData> : public CrossThreadCopierPassThrough<DocumentEncodingData> {
};

inline bool operator!=(const DocumentEncodingData& a, const DocumentEncodingData& b)
{
    return a.encoding() != b.encoding()
        || a.wasDetectedHeuristically() != b.wasDetectedHeuristically()
        || a.sawDecodingError() != b.sawDecodingError();
}

} // namespace blink

#endif // DocumentEncodingData_h
