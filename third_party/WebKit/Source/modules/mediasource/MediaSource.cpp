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
#include "modules/mediasource/MediaSource.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/dom/ExceptionCode.h"
#include "core/events/Event.h"
#include "core/events/GenericEventQueue.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/TimeRanges.h"
#include "modules/mediasource/MediaSourceRegistry.h"
#include "platform/ContentType.h"
#include "platform/Logging.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "public/platform/WebMediaSource.h"
#include "public/platform/WebSourceBuffer.h"
#include "wtf/text/CString.h"

using blink::WebMediaSource;
using blink::WebSourceBuffer;

namespace blink {

static bool throwExceptionIfClosedOrUpdating(bool isOpen, bool isUpdating, ExceptionState& exceptionState)
{
    if (!isOpen) {
        exceptionState.throwDOMException(InvalidStateError, "The MediaSource's readyState is not 'open'.");
        return true;
    }
    if (isUpdating) {
        exceptionState.throwDOMException(InvalidStateError, "The 'updating' attribute is true on one or more of this MediaSource's SourceBuffers.");
        return true;
    }

    return false;
}

const AtomicString& MediaSource::openKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, open, ("open", AtomicString::ConstructFromLiteral));
    return open;
}

const AtomicString& MediaSource::closedKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, closed, ("closed", AtomicString::ConstructFromLiteral));
    return closed;
}

const AtomicString& MediaSource::endedKeyword()
{
    DEFINE_STATIC_LOCAL(const AtomicString, ended, ("ended", AtomicString::ConstructFromLiteral));
    return ended;
}

MediaSource* MediaSource::create(ExecutionContext* context)
{
    MediaSource* mediaSource = new MediaSource(context);
    mediaSource->suspendIfNeeded();
    return mediaSource;
}

MediaSource::MediaSource(ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_readyState(closedKeyword())
    , m_asyncEventQueue(GenericEventQueue::create(this))
    , m_attachedElement(nullptr)
    , m_sourceBuffers(SourceBufferList::create(executionContext(), m_asyncEventQueue.get()))
    , m_activeSourceBuffers(SourceBufferList::create(executionContext(), m_asyncEventQueue.get()))
    , m_isAddedToRegistry(false)
{
    WTF_LOG(Media, "MediaSource::MediaSource %p", this);
}

MediaSource::~MediaSource()
{
    WTF_LOG(Media, "MediaSource::~MediaSource %p", this);
#if !ENABLE(OILPAN)
    ASSERT(isClosed());
#endif
}

SourceBuffer* MediaSource::addSourceBuffer(const String& type, ExceptionState& exceptionState)
{
    WTF_LOG(Media, "MediaSource::addSourceBuffer(%s) %p", type.ascii().data(), this);

    // 2.2 https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#widl-MediaSource-addSourceBuffer-SourceBuffer-DOMString-type
    // 1. If type is an empty string then throw an InvalidAccessError exception
    // and abort these steps.
    if (type.isEmpty()) {
        exceptionState.throwDOMException(InvalidAccessError, "The type provided is empty.");
        return 0;
    }

    // 2. If type contains a MIME type that is not supported ..., then throw a
    // NotSupportedError exception and abort these steps.
    if (!isTypeSupported(type)) {
        exceptionState.throwDOMException(NotSupportedError, "The type provided ('" + type + "') is unsupported.");
        return 0;
    }

    // 4. If the readyState attribute is not in the "open" state then throw an
    // InvalidStateError exception and abort these steps.
    if (!isOpen()) {
        exceptionState.throwDOMException(InvalidStateError, "The MediaSource's readyState is not 'open'.");
        return 0;
    }

    // 5. Create a new SourceBuffer object and associated resources.
    ContentType contentType(type);
    Vector<String> codecs = contentType.codecs();
    OwnPtr<WebSourceBuffer> webSourceBuffer = createWebSourceBuffer(contentType.type(), codecs, exceptionState);

    if (!webSourceBuffer) {
        ASSERT(exceptionState.code() == NotSupportedError || exceptionState.code() == QuotaExceededError);
        // 2. If type contains a MIME type that is not supported ..., then throw a NotSupportedError exception and abort these steps.
        // 3. If the user agent can't handle any more SourceBuffer objects then throw a QuotaExceededError exception and abort these steps
        return 0;
    }

    SourceBuffer* buffer = SourceBuffer::create(webSourceBuffer.release(), this, m_asyncEventQueue.get());
    // 6. Add the new object to sourceBuffers and fire a addsourcebuffer on that object.
    m_sourceBuffers->add(buffer);

    // 7. Return the new object to the caller.
    return buffer;
}

void MediaSource::removeSourceBuffer(SourceBuffer* buffer, ExceptionState& exceptionState)
{
    WTF_LOG(Media, "MediaSource::removeSourceBuffer() %p", this);

    // 2.2 https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#widl-MediaSource-removeSourceBuffer-void-SourceBuffer-sourceBuffer

    // 1. If sourceBuffer specifies an object that is not in sourceBuffers then
    // throw a NotFoundError exception and abort these steps.
    if (!m_sourceBuffers->length() || !m_sourceBuffers->contains(buffer)) {
        exceptionState.throwDOMException(NotFoundError, "The SourceBuffer provided is not contained in this MediaSource.");
        return;
    }

    // 2. If the sourceBuffer.updating attribute equals true, then run the following steps: ...
    buffer->abortIfUpdating();

    // Steps 3-8 are related to updating audioTracks, videoTracks, and textTracks which aren't implmented yet.
    // FIXME(91649): support track selection

    // 9. If sourceBuffer is in activeSourceBuffers, then remove sourceBuffer from activeSourceBuffers ...
    m_activeSourceBuffers->remove(buffer);

    // 10. Remove sourceBuffer from sourceBuffers and fire a removesourcebuffer event
    // on that object.
    m_sourceBuffers->remove(buffer);

    // 11. Destroy all resources for sourceBuffer.
    buffer->removedFromMediaSource();
}

void MediaSource::onReadyStateChange(const AtomicString& oldState, const AtomicString& newState)
{
    if (isOpen()) {
        scheduleEvent(EventTypeNames::sourceopen);
        return;
    }

    if (oldState == openKeyword() && newState == endedKeyword()) {
        scheduleEvent(EventTypeNames::sourceended);
        return;
    }

    ASSERT(isClosed());

    m_activeSourceBuffers->clear();

    // Clear SourceBuffer references to this object.
    for (unsigned long i = 0; i < m_sourceBuffers->length(); ++i)
        m_sourceBuffers->item(i)->removedFromMediaSource();
    m_sourceBuffers->clear();

    scheduleEvent(EventTypeNames::sourceclose);
}

bool MediaSource::isUpdating() const
{
    // Return true if any member of |m_sourceBuffers| is updating.
    for (unsigned long i = 0; i < m_sourceBuffers->length(); ++i) {
        if (m_sourceBuffers->item(i)->updating())
            return true;
    }

    return false;
}

bool MediaSource::isTypeSupported(const String& type)
{
    WTF_LOG(Media, "MediaSource::isTypeSupported(%s)", type.ascii().data());

    // Section 2.2 isTypeSupported() method steps.
    // https://dvcs.w3.org/hg/html-media/raw-file/tip/media-source/media-source.html#widl-MediaSource-isTypeSupported-boolean-DOMString-type
    // 1. If type is an empty string, then return false.
    if (type.isNull() || type.isEmpty())
        return false;

    ContentType contentType(type);
    String codecs = contentType.parameter("codecs");

    // 2. If type does not contain a valid MIME type string, then return false.
    if (contentType.type().isEmpty())
        return false;

    // Note: MediaSource.isTypeSupported() returning true implies that HTMLMediaElement.canPlayType() will return "maybe" or "probably"
    // since it does not make sense for a MediaSource to support a type the HTMLMediaElement knows it cannot play.
    if (HTMLMediaElement::supportsType(contentType, String()) == WebMimeRegistry::IsNotSupported)
        return false;

    // 3. If type contains a media type or media subtype that the MediaSource does not support, then return false.
    // 4. If type contains at a codec that the MediaSource does not support, then return false.
    // 5. If the MediaSource does not support the specified combination of media type, media subtype, and codecs then return false.
    // 6. Return true.
    return MIMETypeRegistry::isSupportedMediaSourceMIMEType(contentType.type(), codecs);
}

const AtomicString& MediaSource::interfaceName() const
{
    return EventTargetNames::MediaSource;
}

ExecutionContext* MediaSource::executionContext() const
{
    return ActiveDOMObject::executionContext();
}

DEFINE_TRACE(MediaSource)
{
#if ENABLE(OILPAN)
    visitor->trace(m_asyncEventQueue);
#endif
    visitor->trace(m_attachedElement);
    visitor->trace(m_sourceBuffers);
    visitor->trace(m_activeSourceBuffers);
    RefCountedGarbageCollectedEventTargetWithInlineData<MediaSource>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

void MediaSource::setWebMediaSourceAndOpen(PassOwnPtr<WebMediaSource> webMediaSource)
{
    TRACE_EVENT_ASYNC_END0("media", "MediaSource::attachToElement", this);
    ASSERT(webMediaSource);
    ASSERT(!m_webMediaSource);
    ASSERT(m_attachedElement);
    m_webMediaSource = webMediaSource;
    setReadyState(openKeyword());
}

void MediaSource::addedToRegistry()
{
    ASSERT(!m_isAddedToRegistry);
    m_isAddedToRegistry = true;
}

void MediaSource::removedFromRegistry()
{
    ASSERT(m_isAddedToRegistry);
    m_isAddedToRegistry = false;
}

double MediaSource::duration() const
{
    return isClosed() ? std::numeric_limits<float>::quiet_NaN() : m_webMediaSource->duration();
}

PassRefPtrWillBeRawPtr<TimeRanges> MediaSource::buffered() const
{
    // Implements MediaSource algorithm for HTMLMediaElement.buffered.
    // https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#htmlmediaelement-extensions
    WillBeHeapVector<RefPtrWillBeMember<TimeRanges>> ranges(m_activeSourceBuffers->length());
    for (size_t i = 0; i < m_activeSourceBuffers->length(); ++i)
        ranges[i] = m_activeSourceBuffers->item(i)->buffered(ASSERT_NO_EXCEPTION);

    // 1. If activeSourceBuffers.length equals 0 then return an empty TimeRanges object and abort these steps.
    if (ranges.isEmpty())
        return TimeRanges::create();

    // 2. Let active ranges be the ranges returned by buffered for each SourceBuffer object in activeSourceBuffers.
    // 3. Let highest end time be the largest range end time in the active ranges.
    double highestEndTime = -1;
    for (size_t i = 0; i < ranges.size(); ++i) {
        unsigned length = ranges[i]->length();
        if (length)
            highestEndTime = std::max(highestEndTime, ranges[i]->end(length - 1, ASSERT_NO_EXCEPTION));
    }

    // Return an empty range if all ranges are empty.
    if (highestEndTime < 0)
        return TimeRanges::create();

    // 4. Let intersection ranges equal a TimeRange object containing a single range from 0 to highest end time.
    RefPtrWillBeRawPtr<TimeRanges> intersectionRanges = TimeRanges::create(0, highestEndTime);

    // 5. For each SourceBuffer object in activeSourceBuffers run the following steps:
    bool ended = readyState() == endedKeyword();
    for (size_t i = 0; i < ranges.size(); ++i) {
        // 5.1 Let source ranges equal the ranges returned by the buffered attribute on the current SourceBuffer.
        TimeRanges* sourceRanges = ranges[i].get();

        // 5.2 If readyState is "ended", then set the end time on the last range in source ranges to highest end time.
        if (ended && sourceRanges->length())
            sourceRanges->add(sourceRanges->start(sourceRanges->length() - 1, ASSERT_NO_EXCEPTION), highestEndTime);

        // 5.3 Let new intersection ranges equal the the intersection between the intersection ranges and the source ranges.
        // 5.4 Replace the ranges in intersection ranges with the new intersection ranges.
        intersectionRanges->intersectWith(sourceRanges);
    }

    return intersectionRanges.release();
}

PassRefPtrWillBeRawPtr<TimeRanges> MediaSource::seekable() const
{
    // Implements MediaSource algorithm for HTMLMediaElement.seekable.
    // https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#htmlmediaelement-extensions

    double sourceDuration = duration();
    // If duration equals NaN: Return an empty TimeRanges object.
    if (std::isnan(sourceDuration))
        return TimeRanges::create();

    // If duration equals positive Infinity:
    if (sourceDuration == std::numeric_limits<double>::infinity()) {
        RefPtrWillBeRawPtr<TimeRanges> buffered = m_attachedElement->buffered();

        // 1. If the HTMLMediaElement.buffered attribute returns an empty TimeRanges object, then
        // return an empty TimeRanges object and abort these steps.
        if (buffered->length() == 0)
            return TimeRanges::create();

        // 2. Return a single range with a start time of 0 and an end time equal to the highest end
        // time reported by the HTMLMediaElement.buffered attribute.
        return TimeRanges::create(0, buffered->end(buffered->length() - 1, ASSERT_NO_EXCEPTION));
    }

    // 3. Otherwise: Return a single range with a start time of 0 and an end time equal to duration.
    return TimeRanges::create(0, sourceDuration);
}

void MediaSource::setDuration(double duration, ExceptionState& exceptionState)
{
    // 2.1 http://www.w3.org/TR/media-source/#widl-MediaSource-duration
    // 1. If the value being set is negative or NaN then throw an InvalidAccessError
    // exception and abort these steps.
    if (std::isnan(duration)) {
        exceptionState.throwDOMException(InvalidAccessError, ExceptionMessages::notAFiniteNumber(duration, "duration"));
        return;
    }
    if (duration < 0.0) {
        exceptionState.throwDOMException(InvalidAccessError, ExceptionMessages::indexExceedsMinimumBound("duration", duration, 0.0));
        return;
    }

    // 2. If the readyState attribute is not "open" then throw an InvalidStateError
    // exception and abort these steps.
    // 3. If the updating attribute equals true on any SourceBuffer in sourceBuffers,
    // then throw an InvalidStateError exception and abort these steps.
    if (throwExceptionIfClosedOrUpdating(isOpen(), isUpdating(), exceptionState))
        return;

    // 4. Run the duration change algorithm with new duration set to the value being
    // assigned to this attribute.
    durationChangeAlgorithm(duration);
}

void MediaSource::durationChangeAlgorithm(double newDuration)
{
    // Section 2.6.4 Duration change
    // https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#duration-change-algorithm
    // 1. If the current value of duration is equal to new duration, then return.
    if (newDuration == duration())
        return;

    // 2. Set old duration to the current value of duration.
    double oldDuration = duration();

    bool requestSeek = m_attachedElement->currentTime() > newDuration;

    // 3. Update duration to new duration.
    m_webMediaSource->setDuration(newDuration);

    // 4. If the new duration is less than old duration, then call remove(new duration, old duration) on all all objects in sourceBuffers.
    if (newDuration < oldDuration) {
        for (size_t i = 0; i < m_sourceBuffers->length(); ++i)
            m_sourceBuffers->item(i)->remove(newDuration, oldDuration, ASSERT_NO_EXCEPTION);
    }

    // 5. If a user agent is unable to partially render audio frames or text cues that start before and end after the duration, then run the following steps:
    // NOTE: Currently we assume that the media engine is able to render partial frames/cues. If a media
    // engine gets added that doesn't support this, then we'll need to add logic to handle the substeps.

    // 6. Update the media controller duration to new duration and run the HTMLMediaElement duration change algorithm.
    m_attachedElement->durationChanged(newDuration, requestSeek);
}

void MediaSource::setReadyState(const AtomicString& state)
{
    ASSERT(state == openKeyword() || state == closedKeyword() || state == endedKeyword());

    AtomicString oldState = readyState();
    WTF_LOG(Media, "MediaSource::setReadyState() %p : %s -> %s", this, oldState.ascii().data(), state.ascii().data());

    if (state == closedKeyword()) {
        m_webMediaSource.clear();
        m_attachedElement.clear();
    }

    if (oldState == state)
        return;

    m_readyState = state;

    onReadyStateChange(oldState, state);
}

void MediaSource::endOfStream(const AtomicString& error, ExceptionState& exceptionState)
{
    DEFINE_STATIC_LOCAL(const AtomicString, network, ("network", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, decode, ("decode", AtomicString::ConstructFromLiteral));

    if (error == network) {
        endOfStreamInternal(WebMediaSource::EndOfStreamStatusNetworkError, exceptionState);
    } else if (error == decode) {
        endOfStreamInternal(WebMediaSource::EndOfStreamStatusDecodeError, exceptionState);
    } else {
        ASSERT_NOT_REACHED(); // IDL enforcement should prevent this case.
    }
}

void MediaSource::endOfStream(ExceptionState& exceptionState)
{
    endOfStreamInternal(WebMediaSource::EndOfStreamStatusNoError, exceptionState);
}

void MediaSource::endOfStreamInternal(const WebMediaSource::EndOfStreamStatus eosStatus, ExceptionState& exceptionState)
{
    // 2.2 http://www.w3.org/TR/media-source/#widl-MediaSource-endOfStream-void-EndOfStreamError-error
    // 1. If the readyState attribute is not in the "open" state then throw an
    // InvalidStateError exception and abort these steps.
    // 2. If the updating attribute equals true on any SourceBuffer in sourceBuffers, then throw an
    // InvalidStateError exception and abort these steps.
    if (throwExceptionIfClosedOrUpdating(isOpen(), isUpdating(), exceptionState))
        return;

    // 3. Run the end of stream algorithm with the error parameter set to error.
    //   1. Change the readyState attribute value to "ended".
    //   2. Queue a task to fire a simple event named sourceended at the MediaSource.
    setReadyState(endedKeyword());

    //   3. Do various steps based on |eosStatus|.
    m_webMediaSource->markEndOfStream(eosStatus);
}

bool MediaSource::isOpen() const
{
    return readyState() == openKeyword();
}

void MediaSource::setSourceBufferActive(SourceBuffer* sourceBuffer)
{
    ASSERT(!m_activeSourceBuffers->contains(sourceBuffer));

    // https://dvcs.w3.org/hg/html-media/raw-file/tip/media-source/media-source.html#widl-MediaSource-activeSourceBuffers
    // SourceBuffer objects in SourceBuffer.activeSourceBuffers must appear in
    // the same order as they appear in SourceBuffer.sourceBuffers.
    // SourceBuffer transitions to active are not guaranteed to occur in the
    // same order as buffers in |m_sourceBuffers|, so this method needs to
    // insert |sourceBuffer| into |m_activeSourceBuffers|.
    size_t indexInSourceBuffers = m_sourceBuffers->find(sourceBuffer);
    ASSERT(indexInSourceBuffers != kNotFound);

    size_t insertPosition = 0;
    while (insertPosition < m_activeSourceBuffers->length()
        && m_sourceBuffers->find(m_activeSourceBuffers->item(insertPosition)) < indexInSourceBuffers) {
        ++insertPosition;
    }

    m_activeSourceBuffers->insert(insertPosition, sourceBuffer);
}

bool MediaSource::isClosed() const
{
    return readyState() == closedKeyword();
}

void MediaSource::close()
{
    setReadyState(closedKeyword());
}

bool MediaSource::attachToElement(HTMLMediaElement* element)
{
    if (m_attachedElement)
        return false;

    ASSERT(isClosed());

    TRACE_EVENT_ASYNC_BEGIN0("media", "MediaSource::attachToElement", this);
    m_attachedElement = element;
    return true;
}

void MediaSource::openIfInEndedState()
{
    if (m_readyState != endedKeyword())
        return;

    setReadyState(openKeyword());
    m_webMediaSource->unmarkEndOfStream();
}

bool MediaSource::hasPendingActivity() const
{
    return m_attachedElement || m_webMediaSource
        || m_asyncEventQueue->hasPendingEvents()
        || m_isAddedToRegistry;
}

void MediaSource::stop()
{
    m_asyncEventQueue->close();
    if (!isClosed())
        setReadyState(closedKeyword());
    m_webMediaSource.clear();
}

PassOwnPtr<WebSourceBuffer> MediaSource::createWebSourceBuffer(const String& type, const Vector<String>& codecs, ExceptionState& exceptionState)
{
    WebSourceBuffer* webSourceBuffer = 0;

    switch (m_webMediaSource->addSourceBuffer(type, codecs, &webSourceBuffer)) {
    case WebMediaSource::AddStatusOk:
        return adoptPtr(webSourceBuffer);
    case WebMediaSource::AddStatusNotSupported:
        ASSERT(!webSourceBuffer);
        // 2.2 https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#widl-MediaSource-addSourceBuffer-SourceBuffer-DOMString-type
        // Step 2: If type contains a MIME type ... that is not supported with the types
        // specified for the other SourceBuffer objects in sourceBuffers, then throw
        // a NotSupportedError exception and abort these steps.
        exceptionState.throwDOMException(NotSupportedError, "The type provided ('" + type + "') is not supported.");
        return nullptr;
    case WebMediaSource::AddStatusReachedIdLimit:
        ASSERT(!webSourceBuffer);
        // 2.2 https://dvcs.w3.org/hg/html-media/raw-file/default/media-source/media-source.html#widl-MediaSource-addSourceBuffer-SourceBuffer-DOMString-type
        // Step 3: If the user agent can't handle any more SourceBuffer objects then throw
        // a QuotaExceededError exception and abort these steps.
        exceptionState.throwDOMException(QuotaExceededError, "This MediaSource has reached the limit of SourceBuffer objects it can handle. No additional SourceBuffer objects may be added.");
        return nullptr;
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

void MediaSource::scheduleEvent(const AtomicString& eventName)
{
    ASSERT(m_asyncEventQueue);

    RefPtrWillBeRawPtr<Event> event = Event::create(eventName);
    event->setTarget(this);

    m_asyncEventQueue->enqueueEvent(event.release());
}

URLRegistry& MediaSource::registry() const
{
    return MediaSourceRegistry::registry();
}

} // namespace blink
