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

#ifndef SourceBuffer_h
#define SourceBuffer_h

#include "core/dom/ActiveDOMObject.h"
#include "core/fileapi/FileReaderLoaderClient.h"
#include "modules/EventTargetModules.h"
#include "modules/mediasource/TrackDefaultList.h"
#include "platform/AsyncMethodRunner.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebSourceBufferClient.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DOMArrayBuffer;
class DOMArrayBufferView;
class ExceptionState;
class FileReaderLoader;
class GenericEventQueue;
class MediaSource;
class Stream;
class TimeRanges;
class WebSourceBuffer;

class SourceBuffer final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<SourceBuffer>
    , public ActiveDOMObject
    , public FileReaderLoaderClient
    , public WebSourceBufferClient {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(SourceBuffer);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SourceBuffer);
    DEFINE_WRAPPERTYPEINFO();
public:
    static SourceBuffer* create(PassOwnPtr<WebSourceBuffer>, MediaSource*, GenericEventQueue*);
    static const AtomicString& segmentsKeyword();
    static const AtomicString& sequenceKeyword();

    ~SourceBuffer() override;

    // SourceBuffer.idl methods
    const AtomicString& mode() const { return m_mode; }
    void setMode(const AtomicString&, ExceptionState&);
    bool updating() const { return m_updating; }
    PassRefPtrWillBeRawPtr<TimeRanges> buffered(ExceptionState&) const;
    double timestampOffset() const;
    void setTimestampOffset(double, ExceptionState&);
    void appendBuffer(PassRefPtr<DOMArrayBuffer> data, ExceptionState&);
    void appendBuffer(PassRefPtr<DOMArrayBufferView> data, ExceptionState&);
    void appendStream(Stream*, ExceptionState&);
    void appendStream(Stream*, unsigned long long maxSize, ExceptionState&);
    void abort(ExceptionState&);
    void remove(double start, double end, ExceptionState&);
    double appendWindowStart() const;
    void setAppendWindowStart(double, ExceptionState&);
    double appendWindowEnd() const;
    void setAppendWindowEnd(double, ExceptionState&);
    TrackDefaultList* trackDefaults() const { return m_trackDefaults.get(); }
    void setTrackDefaults(TrackDefaultList*, ExceptionState&);

    void abortIfUpdating();
    void removedFromMediaSource();

    // ActiveDOMObject interface
    bool hasPendingActivity() const override;
    void suspend() override;
    void resume() override;
    void stop() override;

    // EventTarget interface
    ExecutionContext* executionContext() const override;
    const AtomicString& interfaceName() const override;

    // WebSourceBufferClient interface
    void initializationSegmentReceived() override;

    // Oilpan: eagerly release owned m_webSourceBuffer
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

private:
    SourceBuffer(PassOwnPtr<WebSourceBuffer>, MediaSource*, GenericEventQueue*);

    bool isRemoved() const;
    void scheduleEvent(const AtomicString& eventName);

    void appendBufferInternal(const unsigned char*, unsigned, ExceptionState&);
    void appendBufferAsyncPart();

    void removeAsyncPart();

    void appendStreamInternal(Stream*, ExceptionState&);
    void appendStreamAsyncPart();
    void appendStreamDone(bool success);
    void clearAppendStreamState();

    // FileReaderLoaderClient interface
    void didStartLoading() override;
    void didReceiveDataForClient(const char* data, unsigned dataLength) override;
    void didFinishLoading() override;
    void didFail(FileError::ErrorCode) override;

    OwnPtr<WebSourceBuffer> m_webSourceBuffer;
    Member<MediaSource> m_source;
    Member<TrackDefaultList> m_trackDefaults;
    RawPtrWillBeMember<GenericEventQueue> m_asyncEventQueue;

    AtomicString m_mode;
    bool m_updating;
    double m_timestampOffset;
    double m_appendWindowStart;
    double m_appendWindowEnd;
    bool m_firstInitializationSegmentReceived;

    Vector<unsigned char> m_pendingAppendData;
    size_t m_pendingAppendDataOffset;
    AsyncMethodRunner<SourceBuffer> m_appendBufferAsyncPartRunner;

    double m_pendingRemoveStart;
    double m_pendingRemoveEnd;
    AsyncMethodRunner<SourceBuffer> m_removeAsyncPartRunner;

    bool m_streamMaxSizeValid;
    unsigned long long m_streamMaxSize;
    AsyncMethodRunner<SourceBuffer> m_appendStreamAsyncPartRunner;
    Member<Stream> m_stream;
    OwnPtr<FileReaderLoader> m_loader;
};

} // namespace blink

#endif // SourceBuffer_h
