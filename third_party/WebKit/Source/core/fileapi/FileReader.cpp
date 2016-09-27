/*
 * Copyright (C) 2010 Google Inc.  All rights reserved.
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
#include "core/fileapi/FileReader.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/CrossThreadTask.h"
#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/events/ProgressEvent.h"
#include "core/fileapi/File.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "platform/Logging.h"
#include "platform/Supplementable.h"
#include "wtf/CurrentTime.h"
#include "wtf/Deque.h"
#include "wtf/HashSet.h"
#include "wtf/text/CString.h"

namespace blink {

namespace {

#if !LOG_DISABLED
const CString utf8BlobUUID(Blob* blob)
{
    return blob->uuid().utf8();
}

const CString utf8FilePath(Blob* blob)
{
    return blob->hasBackingFile() ? toFile(blob)->path().utf8() : "";
}
#endif

} // namespace

// Embedders like chromium limit the number of simultaneous requests to avoid
// excessive IPC congestion. We limit this to 100 per thread to throttle the
// requests (the value is arbitrarily chosen).
static const size_t kMaxOutstandingRequestsPerThread = 100;
static const double progressNotificationIntervalMS = 50;

class FileReader::ThrottlingController final : public NoBaseWillBeGarbageCollectedFinalized<FileReader::ThrottlingController>, public WillBeHeapSupplement<ExecutionContext> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(FileReader::ThrottlingController);
public:
    static ThrottlingController* from(ExecutionContext* context)
    {
        if (!context)
            return 0;

        ThrottlingController* controller = static_cast<ThrottlingController*>(WillBeHeapSupplement<ExecutionContext>::from(*context, supplementName()));
        if (!controller) {
            controller = new ThrottlingController();
            WillBeHeapSupplement<ExecutionContext>::provideTo(*context, supplementName(), adoptPtrWillBeNoop(controller));
        }
        return controller;
    }

    ~ThrottlingController() { }

    enum FinishReaderType { DoNotRunPendingReaders, RunPendingReaders };

    static void pushReader(ExecutionContext* context, FileReader* reader)
    {
        ThrottlingController* controller = from(context);
        if (!controller)
            return;

        reader->m_asyncOperationId = InspectorInstrumentation::traceAsyncOperationStarting(context, "FileReader");
        controller->pushReader(reader);
    }

    static FinishReaderType removeReader(ExecutionContext* context, FileReader* reader)
    {
        ThrottlingController* controller = from(context);
        if (!controller)
            return DoNotRunPendingReaders;

        return controller->removeReader(reader);
    }

    static void finishReader(ExecutionContext* context, FileReader* reader, FinishReaderType nextStep)
    {
        InspectorInstrumentation::traceAsyncOperationCompleted(context, reader->m_asyncOperationId);

        ThrottlingController* controller = from(context);
        if (!controller)
            return;

        controller->finishReader(reader, nextStep);
    }

    DEFINE_INLINE_TRACE()
    {
#if ENABLE(OILPAN)
        visitor->trace(m_pendingReaders);
        visitor->trace(m_runningReaders);
#endif
        WillBeHeapSupplement<ExecutionContext>::trace(visitor);
    }

private:
    ThrottlingController()
        : m_maxRunningReaders(kMaxOutstandingRequestsPerThread)
    {
    }

    void pushReader(FileReader* reader)
    {
        if (m_pendingReaders.isEmpty()
            && m_runningReaders.size() < m_maxRunningReaders) {
            reader->executePendingRead();
            ASSERT(!m_runningReaders.contains(reader));
            m_runningReaders.add(reader);
            return;
        }
        m_pendingReaders.append(reader);
        executeReaders();
    }

    FinishReaderType removeReader(FileReader* reader)
    {
        FileReaderHashSet::const_iterator hashIter = m_runningReaders.find(reader);
        if (hashIter != m_runningReaders.end()) {
            m_runningReaders.remove(hashIter);
            return RunPendingReaders;
        }
        FileReaderDeque::const_iterator dequeEnd = m_pendingReaders.end();
        for (FileReaderDeque::const_iterator it = m_pendingReaders.begin(); it != dequeEnd; ++it) {
            if (*it == reader) {
                m_pendingReaders.remove(it);
                break;
            }
        }
        return DoNotRunPendingReaders;
    }

    void finishReader(FileReader* reader, FinishReaderType nextStep)
    {
        if (nextStep == RunPendingReaders)
            executeReaders();
    }

    void executeReaders()
    {
        while (m_runningReaders.size() < m_maxRunningReaders) {
            if (m_pendingReaders.isEmpty())
                return;
            FileReader* reader = m_pendingReaders.takeFirst();
            reader->executePendingRead();
            m_runningReaders.add(reader);
        }
    }

    static const char* supplementName() { return "FileReaderThrottlingController"; }

    const size_t m_maxRunningReaders;

    using FileReaderDeque = PersistentHeapDequeWillBeHeapDeque<Member<FileReader>>;
    using FileReaderHashSet = PersistentHeapHashSetWillBeHeapHashSet<Member<FileReader>>;

    FileReaderDeque m_pendingReaders;
    FileReaderHashSet m_runningReaders;
};

FileReader* FileReader::create(ExecutionContext* context)
{
    FileReader* fileReader = new FileReader(context);
    fileReader->suspendIfNeeded();
    return fileReader;
}

FileReader::FileReader(ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_state(EMPTY)
    , m_loadingState(LoadingStateNone)
    , m_readType(FileReaderLoader::ReadAsBinaryString)
    , m_lastProgressNotificationTimeMS(0)
    , m_asyncOperationId(0)
{
}

FileReader::~FileReader()
{
    terminate();
}

const AtomicString& FileReader::interfaceName() const
{
    return EventTargetNames::FileReader;
}

void FileReader::stop()
{
    // The delayed abort task tidies up and advances to the DONE state.
    if (m_loadingState == LoadingStateAborted)
        return;

    if (hasPendingActivity())
        ThrottlingController::finishReader(executionContext(), this, ThrottlingController::removeReader(executionContext(), this));
    terminate();
}

bool FileReader::hasPendingActivity() const
{
    return m_state == LOADING;
}

void FileReader::readAsArrayBuffer(Blob* blob, ExceptionState& exceptionState)
{
    ASSERT(blob);
    WTF_LOG(FileAPI, "FileReader: reading as array buffer: %s %s\n", utf8BlobUUID(blob).data(), utf8FilePath(blob).data());

    readInternal(blob, FileReaderLoader::ReadAsArrayBuffer, exceptionState);
}

void FileReader::readAsBinaryString(Blob* blob, ExceptionState& exceptionState)
{
    ASSERT(blob);
    WTF_LOG(FileAPI, "FileReader: reading as binary: %s %s\n", utf8BlobUUID(blob).data(), utf8FilePath(blob).data());

    readInternal(blob, FileReaderLoader::ReadAsBinaryString, exceptionState);
}

void FileReader::readAsText(Blob* blob, const String& encoding, ExceptionState& exceptionState)
{
    ASSERT(blob);
    WTF_LOG(FileAPI, "FileReader: reading as text: %s %s\n", utf8BlobUUID(blob).data(), utf8FilePath(blob).data());

    m_encoding = encoding;
    readInternal(blob, FileReaderLoader::ReadAsText, exceptionState);
}

void FileReader::readAsText(Blob* blob, ExceptionState& exceptionState)
{
    readAsText(blob, String(), exceptionState);
}

void FileReader::readAsDataURL(Blob* blob, ExceptionState& exceptionState)
{
    ASSERT(blob);
    WTF_LOG(FileAPI, "FileReader: reading as data URL: %s %s\n", utf8BlobUUID(blob).data(), utf8FilePath(blob).data());

    readInternal(blob, FileReaderLoader::ReadAsDataURL, exceptionState);
}

void FileReader::readInternal(Blob* blob, FileReaderLoader::ReadType type, ExceptionState& exceptionState)
{
    // If multiple concurrent read methods are called on the same FileReader, InvalidStateError should be thrown when the state is LOADING.
    if (m_state == LOADING) {
        exceptionState.throwDOMException(InvalidStateError, "The object is already busy reading Blobs.");
        return;
    }

    if (blob->hasBeenClosed()) {
        exceptionState.throwDOMException(InvalidStateError, String(blob->isFile() ? "File" : "Blob") + " has been closed.");
        return;
    }

    ExecutionContext* context = executionContext();
    if (!context) {
        exceptionState.throwDOMException(AbortError, "Reading from a detached FileReader is not supported.");
        return;
    }

    // A document loader will not load new resources once the Document has detached from its frame.
    if (context->isDocument() && !toDocument(context)->frame()) {
        exceptionState.throwDOMException(AbortError, "Reading from a Document-detached FileReader is not supported.");
        return;
    }

    // "Snapshot" the Blob data rather than the Blob itself as ongoing
    // read operations should not be affected if close() is called on
    // the Blob being read.
    m_blobDataHandle = blob->blobDataHandle();
    m_blobType = blob->type();
    m_readType = type;
    m_state = LOADING;
    m_loadingState = LoadingStatePending;
    m_error = nullptr;
    ASSERT(ThrottlingController::from(context));
    ThrottlingController::pushReader(context, this);
}

void FileReader::executePendingRead()
{
    ASSERT(m_loadingState == LoadingStatePending);
    m_loadingState = LoadingStateLoading;

    m_loader = FileReaderLoader::create(m_readType, this);
    m_loader->setEncoding(m_encoding);
    m_loader->setDataType(m_blobType);
    m_loader->start(executionContext(), m_blobDataHandle);
    m_blobDataHandle = nullptr;
}

static void delayedAbort(FileReader* reader)
{
    reader->doAbort();
}

void FileReader::abort()
{
    WTF_LOG(FileAPI, "FileReader: aborting\n");

    if (m_loadingState != LoadingStateLoading
        && m_loadingState != LoadingStatePending) {
        return;
    }
    m_loadingState = LoadingStateAborted;

    // Schedule to have the abort done later since abort() might be called from the event handler and we do not want the resource loading code to be in the stack.
    executionContext()->postTask(
        FROM_HERE, createCrossThreadTask(&delayedAbort, AllowAccessLater(this)));
}

void FileReader::doAbort()
{
    ASSERT(m_state != DONE);

    terminate();

    m_error = FileError::create(FileError::ABORT_ERR);

    // Unregister the reader.
    ThrottlingController::FinishReaderType finalStep = ThrottlingController::removeReader(executionContext(), this);

    fireEvent(EventTypeNames::error);
    fireEvent(EventTypeNames::abort);
    fireEvent(EventTypeNames::loadend);

    // All possible events have fired and we're done, no more pending activity.
    ThrottlingController::finishReader(executionContext(), this, finalStep);
}

void FileReader::result(StringOrArrayBuffer& resultAttribute) const
{
    if (!m_loader || m_error)
        return;

    if (m_readType == FileReaderLoader::ReadAsArrayBuffer)
        resultAttribute.setArrayBuffer(m_loader->arrayBufferResult());
    else
        resultAttribute.setString(m_loader->stringResult());
}

void FileReader::terminate()
{
    if (m_loader) {
        m_loader->cancel();
        m_loader = nullptr;
    }
    m_state = DONE;
    m_loadingState = LoadingStateNone;
}

void FileReader::didStartLoading()
{
    fireEvent(EventTypeNames::loadstart);
}

void FileReader::didReceiveData()
{
    // Fire the progress event at least every 50ms.
    double now = currentTimeMS();
    if (!m_lastProgressNotificationTimeMS)
        m_lastProgressNotificationTimeMS = now;
    else if (now - m_lastProgressNotificationTimeMS > progressNotificationIntervalMS) {
        fireEvent(EventTypeNames::progress);
        m_lastProgressNotificationTimeMS = now;
    }
}

void FileReader::didFinishLoading()
{
    if (m_loadingState == LoadingStateAborted)
        return;
    ASSERT(m_loadingState == LoadingStateLoading);

    // It's important that we change m_loadingState before firing any events
    // since any of the events could call abort(), which internally checks
    // if we're still loading (therefore we need abort process) or not.
    m_loadingState = LoadingStateNone;

    fireEvent(EventTypeNames::progress);

    ASSERT(m_state != DONE);
    m_state = DONE;

    // Unregister the reader.
    ThrottlingController::FinishReaderType finalStep = ThrottlingController::removeReader(executionContext(), this);

    fireEvent(EventTypeNames::load);
    fireEvent(EventTypeNames::loadend);

    // All possible events have fired and we're done, no more pending activity.
    ThrottlingController::finishReader(executionContext(), this, finalStep);
}

void FileReader::didFail(FileError::ErrorCode errorCode)
{
    if (m_loadingState == LoadingStateAborted)
        return;
    ASSERT(m_loadingState == LoadingStateLoading);
    m_loadingState = LoadingStateNone;

    ASSERT(m_state != DONE);
    m_state = DONE;

    m_error = FileError::create(static_cast<FileError::ErrorCode>(errorCode));

    // Unregister the reader.
    ThrottlingController::FinishReaderType finalStep = ThrottlingController::removeReader(executionContext(), this);

    fireEvent(EventTypeNames::error);
    fireEvent(EventTypeNames::loadend);

    // All possible events have fired and we're done, no more pending activity.
    ThrottlingController::finishReader(executionContext(), this, finalStep);
}

void FileReader::fireEvent(const AtomicString& type)
{
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::traceAsyncCallbackStarting(executionContext(), m_asyncOperationId);
    if (!m_loader) {
        dispatchEvent(ProgressEvent::create(type, false, 0, 0));
        InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
        return;
    }

    if (m_loader->totalBytes() >= 0)
        dispatchEvent(ProgressEvent::create(type, true, m_loader->bytesLoaded(), m_loader->totalBytes()));
    else
        dispatchEvent(ProgressEvent::create(type, false, m_loader->bytesLoaded(), 0));

    InspectorInstrumentation::traceAsyncCallbackCompleted(cookie);
}

DEFINE_TRACE(FileReader)
{
    visitor->trace(m_error);
    RefCountedGarbageCollectedEventTargetWithInlineData<FileReader>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

} // namespace blink
