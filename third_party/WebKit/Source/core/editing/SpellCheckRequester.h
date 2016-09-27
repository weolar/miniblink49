/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef SpellCheckRequester_h
#define SpellCheckRequester_h

#include "core/dom/Element.h"
#include "core/dom/Range.h"
#include "platform/Timer.h"
#include "platform/text/TextChecking.h"
#include "wtf/Deque.h"
#include "wtf/Noncopyable.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class LocalFrame;
class SpellCheckRequester;
class TextCheckerClient;

class SpellCheckRequest final : public TextCheckingRequest {
public:
    static PassRefPtrWillBeRawPtr<SpellCheckRequest> create(TextCheckingTypeMask, TextCheckingProcessType, PassRefPtrWillBeRawPtr<Range> checkingRange, PassRefPtrWillBeRawPtr<Range> paragraphRange, int requestNumber = 0);
    ~SpellCheckRequest() override;

    PassRefPtrWillBeRawPtr<Range> checkingRange() const { return m_checkingRange; }
    PassRefPtrWillBeRawPtr<Range> paragraphRange() const { return m_paragraphRange; }
    PassRefPtrWillBeRawPtr<Element> rootEditableElement() const { return m_rootEditableElement; }

    void setCheckerAndSequence(SpellCheckRequester*, int sequence);
#if !ENABLE(OILPAN)
    void requesterDestroyed();
#endif

    const TextCheckingRequestData& data() const override;
    void didSucceed(const Vector<TextCheckingResult>&) override;
    void didCancel() override;

    int requestNumber() const { return m_requestNumber; }

    DECLARE_VIRTUAL_TRACE();

private:
    SpellCheckRequest(PassRefPtrWillBeRawPtr<Range> checkingRange, PassRefPtrWillBeRawPtr<Range> paragraphRange, const String&, TextCheckingTypeMask, TextCheckingProcessType, const Vector<uint32_t>& documentMarkersInRange, const Vector<unsigned>& documentMarkerOffsets, int requestNumber);

    RawPtrWillBeMember<SpellCheckRequester> m_requester;
    RefPtrWillBeMember<Range> m_checkingRange;
    RefPtrWillBeMember<Range> m_paragraphRange;
    RefPtrWillBeMember<Element> m_rootEditableElement;
    TextCheckingRequestData m_requestData;
    int m_requestNumber;
};

class SpellCheckRequester final : public NoBaseWillBeGarbageCollectedFinalized<SpellCheckRequester> {
    WTF_MAKE_NONCOPYABLE(SpellCheckRequester); WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(SpellCheckRequester);
public:
    static PassOwnPtrWillBeRawPtr<SpellCheckRequester> create(LocalFrame& frame)
    {
        return adoptPtrWillBeNoop(new SpellCheckRequester(frame));
    }

    ~SpellCheckRequester();
    DECLARE_TRACE();

    bool isAsynchronousEnabled() const;
    bool isCheckable(Range*) const;

    void requestCheckingFor(PassRefPtrWillBeRawPtr<SpellCheckRequest>);
    void cancelCheck();

    int lastRequestSequence() const
    {
        return m_lastRequestSequence;
    }

    int lastProcessedSequence() const
    {
        return m_lastProcessedSequence;
    }

private:
    friend class SpellCheckRequest;

    explicit SpellCheckRequester(LocalFrame&);

    bool canCheckAsynchronously(Range*) const;
    TextCheckerClient& client() const;
    void timerFiredToProcessQueuedRequest(Timer<SpellCheckRequester>*);
    void invokeRequest(PassRefPtrWillBeRawPtr<SpellCheckRequest>);
    void enqueueRequest(PassRefPtrWillBeRawPtr<SpellCheckRequest>);
    void didCheckSucceed(int sequence, const Vector<TextCheckingResult>&);
    void didCheckCancel(int sequence);
    void didCheck(int sequence, const Vector<TextCheckingResult>&);

    RawPtrWillBeMember<LocalFrame> m_frame;
    LocalFrame& frame() const
    {
        ASSERT(m_frame);
        return *m_frame;
    }

    int m_lastRequestSequence;
    int m_lastProcessedSequence;

    Timer<SpellCheckRequester> m_timerToProcessQueuedRequest;

    RefPtrWillBeMember<SpellCheckRequest> m_processingRequest;

    typedef WillBeHeapDeque<RefPtrWillBeMember<SpellCheckRequest>> RequestQueue;
    RequestQueue m_requestQueue;
};

} // namespace blink

#endif // SpellCheckRequester_h
