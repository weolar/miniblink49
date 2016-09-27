/*
 * Copyright (C) 2006, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Research In Motion Limited. All rights reserved.
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

#ifndef HistoryItem_h
#define HistoryItem_h

#include "bindings/core/v8/SerializedScriptValue.h"
#include "core/CoreExport.h"
#include "core/loader/FrameLoaderTypes.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/IntPoint.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/Referrer.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class DocumentState;
class FormData;
class KURL;
class ResourceRequest;

class CORE_EXPORT HistoryItem final : public RefCountedWillBeGarbageCollectedFinalized<HistoryItem> {
public:
    static PassRefPtrWillBeRawPtr<HistoryItem> create()
    {
        return adoptRefWillBeNoop(new HistoryItem);
    }
    ~HistoryItem();

    const String& urlString() const;
    KURL url() const;

    const Referrer& referrer() const;
    const String& target() const;

    FormData* formData();
    const AtomicString& formContentType() const;

    const FloatPoint& pinchViewportScrollPoint() const;
    void setPinchViewportScrollPoint(const FloatPoint&);
    const IntPoint& scrollPoint() const;
    void setScrollPoint(const IntPoint&);

    float pageScaleFactor() const;
    void setPageScaleFactor(float);

    Vector<String> getReferencedFilePaths();
    const Vector<String>& documentState();
    void setDocumentState(const Vector<String>&);
    void setDocumentState(DocumentState*);
    void clearDocumentState();

    void setURL(const KURL&);
    void setURLString(const String&);
    void setReferrer(const Referrer&);
    void setTarget(const String&);

    void setStateObject(PassRefPtr<SerializedScriptValue>);
    SerializedScriptValue* stateObject() const { return m_stateObject.get(); }

    void setItemSequenceNumber(long long number) { m_itemSequenceNumber = number; }
    long long itemSequenceNumber() const { return m_itemSequenceNumber; }

    void setDocumentSequenceNumber(long long number) { m_documentSequenceNumber = number; }
    long long documentSequenceNumber() const { return m_documentSequenceNumber; }

    void setScrollRestorationType(HistoryScrollRestorationType  type) { m_scrollRestorationType = type; }
    HistoryScrollRestorationType scrollRestorationType() { return m_scrollRestorationType; }

    void setFormInfoFromRequest(const ResourceRequest&);
    void setFormData(PassRefPtr<FormData>);
    void setFormContentType(const AtomicString&);

    bool isCurrentDocument(Document*) const;

    DECLARE_TRACE();

private:
    HistoryItem();

    String m_urlString;
    Referrer m_referrer;
    String m_target;

    FloatPoint m_pinchViewportScrollPoint;
    IntPoint m_scrollPoint;
    float m_pageScaleFactor;
    Vector<String> m_documentStateVector;
    RefPtrWillBeMember<DocumentState> m_documentState;

    // If two HistoryItems have the same item sequence number, then they are
    // clones of one another. Traversing history from one such HistoryItem to
    // another is a no-op. HistoryItem clones are created for parent and
    // sibling frames when only a subframe navigates.
    int64_t m_itemSequenceNumber;

    // If two HistoryItems have the same document sequence number, then they
    // refer to the same instance of a document. Traversing history from one
    // such HistoryItem to another preserves the document.
    int64_t m_documentSequenceNumber;

    // Type of the scroll restoration for the history item determines if scroll
    // position should be restored when it is loaded during history traversal.
    HistoryScrollRestorationType m_scrollRestorationType;

    // Support for HTML5 History
    RefPtr<SerializedScriptValue> m_stateObject;

    // info used to repost form data
    RefPtr<FormData> m_formData;
    AtomicString m_formContentType;

}; // class HistoryItem

} // namespace blink

#endif // HISTORYITEM_H
