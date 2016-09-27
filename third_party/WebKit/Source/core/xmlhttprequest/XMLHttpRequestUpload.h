/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef XMLHttpRequestUpload_h
#define XMLHttpRequestUpload_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/xmlhttprequest/XMLHttpRequest.h"
#include "core/xmlhttprequest/XMLHttpRequestEventTarget.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class ExecutionContext;

class XMLHttpRequestUpload final : public XMLHttpRequestEventTarget {
    DEFINE_WRAPPERTYPEINFO();
public:
    static XMLHttpRequestUpload* create(XMLHttpRequest* xmlHttpRequest)
    {
        return new XMLHttpRequestUpload(xmlHttpRequest);
    }

    XMLHttpRequest* xmlHttpRequest() const { return m_xmlHttpRequest; }

    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    void dispatchEventAndLoadEnd(const AtomicString&, bool, unsigned long long, unsigned long long);
    void dispatchProgressEvent(unsigned long long, unsigned long long);

    void handleRequestError(const AtomicString&);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit XMLHttpRequestUpload(XMLHttpRequest*);

    Member<XMLHttpRequest> m_xmlHttpRequest;

    // Last progress event values; used when issuing the
    // required 'progress' event on a request error or abort.
    unsigned long long m_lastBytesSent;
    unsigned long long m_lastTotalBytesToBeSent;
};

} // namespace blink

#endif // XMLHttpRequestUpload_h
