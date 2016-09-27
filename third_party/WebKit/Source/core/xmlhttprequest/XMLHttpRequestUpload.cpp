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

#include "config.h"
#include "core/xmlhttprequest/XMLHttpRequestUpload.h"

#include "core/EventTypeNames.h"
#include "core/xmlhttprequest/XMLHttpRequestProgressEvent.h"
#include "wtf/Assertions.h"
#include "wtf/text/AtomicString.h"

namespace blink {

XMLHttpRequestUpload::XMLHttpRequestUpload(XMLHttpRequest* xmlHttpRequest)
    : m_xmlHttpRequest(xmlHttpRequest)
    , m_lastBytesSent(0)
    , m_lastTotalBytesToBeSent(0)
{
}

const AtomicString& XMLHttpRequestUpload::interfaceName() const
{
    return EventTargetNames::XMLHttpRequestUpload;
}

ExecutionContext* XMLHttpRequestUpload::executionContext() const
{
    return m_xmlHttpRequest->executionContext();
}

void XMLHttpRequestUpload::dispatchProgressEvent(unsigned long long bytesSent, unsigned long long totalBytesToBeSent)
{
    m_lastBytesSent = bytesSent;
    m_lastTotalBytesToBeSent = totalBytesToBeSent;
    dispatchEvent(XMLHttpRequestProgressEvent::create(EventTypeNames::progress, true, bytesSent, totalBytesToBeSent));
}

void XMLHttpRequestUpload::dispatchEventAndLoadEnd(const AtomicString& type, bool lengthComputable, unsigned long long bytesSent, unsigned long long total)
{
    ASSERT(type == EventTypeNames::load || type == EventTypeNames::abort || type == EventTypeNames::error || type == EventTypeNames::timeout);
    dispatchEvent(XMLHttpRequestProgressEvent::create(type, lengthComputable, bytesSent, total));
    dispatchEvent(XMLHttpRequestProgressEvent::create(EventTypeNames::loadend, lengthComputable, bytesSent, total));
}

void XMLHttpRequestUpload::handleRequestError(const AtomicString& type)
{
    bool lengthComputable = m_lastTotalBytesToBeSent > 0 && m_lastBytesSent <= m_lastTotalBytesToBeSent;
    dispatchEvent(XMLHttpRequestProgressEvent::create(EventTypeNames::progress, lengthComputable, m_lastBytesSent, m_lastTotalBytesToBeSent));
    dispatchEventAndLoadEnd(type, lengthComputable, m_lastBytesSent, m_lastTotalBytesToBeSent);
}

DEFINE_TRACE(XMLHttpRequestUpload)
{
    visitor->trace(m_xmlHttpRequest);
    XMLHttpRequestEventTarget::trace(visitor);
}

} // namespace blink
