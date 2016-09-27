/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#ifndef OpenTypeSanitizer_h
#define OpenTypeSanitizer_h

#ifndef MINIBLINK_NOT_OTS
#include "third_party/ots/include/opentype-sanitiser.h"
#endif // #ifndef MINIBLINK_NOT_OTS
#pragma message("OpenTypeSanitizer.h")
#include "wtf/Forward.h"
#include "wtf/text/WTFString.h"

namespace blink {

class SharedBuffer;

class OpenTypeSanitizer {
public:
    explicit OpenTypeSanitizer(SharedBuffer* buffer)
        : m_buffer(buffer)
        , m_otsErrorString("")
    {
    }

    PassRefPtr<SharedBuffer> sanitize();

    static bool supportsFormat(const String&);
    String getErrorString() const { return static_cast<String>(m_otsErrorString); }

    void setErrorString(const String& errorString) { m_otsErrorString = errorString; }

private:
    SharedBuffer* const m_buffer;
    String m_otsErrorString;
};

#ifndef MINIBLINK_NOT_OTS
class BlinkOTSContext: public ots::OTSContext {
public:
    BlinkOTSContext()
        : m_errorString("")
    {
    }

    virtual void Message(int level, const char *format, ...);
    virtual ots::TableAction GetTableAction(uint32_t tag);
    String getErrorString() const { return static_cast<String>(m_errorString); }
private:
    String m_errorString;
};
#endif // #ifndef MINIBLINK_NOT_OTS

} // namespace blink

#endif // OpenTypeSanitizer_h
