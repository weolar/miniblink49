/*
 * Copyright (C) 2005, 2006, 2008, 2011 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/loader/HistoryItem.h"

#include "core/dom/Document.h"
#include "core/html/forms/FormController.h"
#include "platform/network/ResourceRequest.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "wtf/Assertions.h"
#include "wtf/CurrentTime.h"
#include "wtf/text/CString.h"

namespace blink {

static long long generateSequenceNumber()
{
    // Initialize to the current time to reduce the likelihood of generating
    // identifiers that overlap with those from past/future browser sessions.
    static long long next = static_cast<long long>(currentTime() * 1000000.0);
    return ++next;
}

HistoryItem::HistoryItem()
    : m_pageScaleFactor(0)
    , m_itemSequenceNumber(generateSequenceNumber())
    , m_documentSequenceNumber(generateSequenceNumber())
    , m_scrollRestorationType(ScrollRestorationAuto)
{
}

HistoryItem::~HistoryItem()
{
}

const String& HistoryItem::urlString() const
{
    return m_urlString;
}

KURL HistoryItem::url() const
{
    return KURL(ParsedURLString, m_urlString);
}

const Referrer& HistoryItem::referrer() const
{
    return m_referrer;
}

const String& HistoryItem::target() const
{
    return m_target;
}

void HistoryItem::setURLString(const String& urlString)
{
    if (m_urlString != urlString)
        m_urlString = urlString;
}

void HistoryItem::setURL(const KURL& url)
{
    setURLString(url.string());
}

void HistoryItem::setReferrer(const Referrer& referrer)
{
    // This should be a RELEASE_ASSERT.
    m_referrer = SecurityPolicy::generateReferrer(referrer.referrerPolicy, url(), referrer.referrer);
}

void HistoryItem::setTarget(const String& target)
{
    m_target = target;
}

const FloatPoint& HistoryItem::pinchViewportScrollPoint() const
{
    return m_pinchViewportScrollPoint;
}

void HistoryItem::setPinchViewportScrollPoint(const FloatPoint& point)
{
    m_pinchViewportScrollPoint = point;
}

const IntPoint& HistoryItem::scrollPoint() const
{
    return m_scrollPoint;
}

void HistoryItem::setScrollPoint(const IntPoint& point)
{
    m_scrollPoint = point;
}

float HistoryItem::pageScaleFactor() const
{
    return m_pageScaleFactor;
}

void HistoryItem::setPageScaleFactor(float scaleFactor)
{
    m_pageScaleFactor = scaleFactor;
}

void HistoryItem::setDocumentState(const Vector<String>& state)
{
    ASSERT(!m_documentState);
    m_documentStateVector = state;
}

void HistoryItem::setDocumentState(DocumentState* state)
{
    m_documentState = state;
}

const Vector<String>& HistoryItem::documentState()
{
    if (m_documentState)
        m_documentStateVector = m_documentState->toStateVector();
    return m_documentStateVector;
}

Vector<String> HistoryItem::getReferencedFilePaths()
{
    return FormController::getReferencedFilePaths(documentState());
}

void HistoryItem::clearDocumentState()
{
    m_documentState.clear();
    m_documentStateVector.clear();
}

void HistoryItem::setStateObject(PassRefPtr<SerializedScriptValue> object)
{
    m_stateObject = object;
}

const AtomicString& HistoryItem::formContentType() const
{
    return m_formContentType;
}

void HistoryItem::setFormInfoFromRequest(const ResourceRequest& request)
{
    if (equalIgnoringCase(request.httpMethod(), "POST")) {
        // FIXME: Eventually we have to make this smart enough to handle the case where
        // we have a stream for the body to handle the "data interspersed with files" feature.
        m_formData = request.httpBody();
        m_formContentType = request.httpContentType();
    } else {
        m_formData = nullptr;
        m_formContentType = nullAtom;
    }
}

void HistoryItem::setFormData(PassRefPtr<FormData> formData)
{
    m_formData = formData;
}

void HistoryItem::setFormContentType(const AtomicString& formContentType)
{
    m_formContentType = formContentType;
}

FormData* HistoryItem::formData()
{
    return m_formData.get();
}

bool HistoryItem::isCurrentDocument(Document* doc) const
{
    // FIXME: We should find a better way to check if this is the current document.
    return equalIgnoringFragmentIdentifier(url(), doc->url());
}

DEFINE_TRACE(HistoryItem)
{
    visitor->trace(m_documentState);
}

} // namespace blink
