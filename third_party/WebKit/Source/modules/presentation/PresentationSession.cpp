// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/presentation/PresentationSession.h"

#include "core/dom/DOMArrayBuffer.h"
#include "core/dom/DOMArrayBufferView.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/events/Event.h"
#include "core/events/MessageEvent.h"
#include "core/fileapi/FileReaderLoader.h"
#include "core/fileapi/FileReaderLoaderClient.h"
#include "core/frame/LocalFrame.h"
#include "modules/EventTargetModules.h"
#include "modules/presentation/Presentation.h"
#include "modules/presentation/PresentationController.h"
#include "public/platform/modules/presentation/WebPresentationSessionClient.h"
#include "wtf/Assertions.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/AtomicString.h"

namespace blink {

namespace {

// TODO(mlamouri): refactor in one common place.
WebPresentationClient* presentationClient(ExecutionContext* executionContext)
{
    ASSERT(executionContext && executionContext->isDocument());

    Document* document = toDocument(executionContext);
    if (!document->frame())
        return nullptr;
    PresentationController* controller = PresentationController::from(*document->frame());
    return controller ? controller->client() : nullptr;
}

const AtomicString& SessionStateToString(WebPresentationSessionState state)
{
    DEFINE_STATIC_LOCAL(const AtomicString, connectedValue, ("connected", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, disconnectedValue, ("disconnected", AtomicString::ConstructFromLiteral));

    switch (state) {
    case WebPresentationSessionState::Connected:
        return connectedValue;
    case WebPresentationSessionState::Disconnected:
        return disconnectedValue;
    }

    ASSERT_NOT_REACHED();
    return disconnectedValue;
}

void throwPresentationDisconnectedError(ExceptionState& exceptionState)
{
    exceptionState.throwDOMException(InvalidStateError, "Presentation session is disconnected.");
}

} // namespace

class PresentationSession::BlobLoader final : public GarbageCollectedFinalized<PresentationSession::BlobLoader>, public FileReaderLoaderClient {
public:
    BlobLoader(PassRefPtr<BlobDataHandle> blobDataHandle, PresentationSession* presentationSession)
        : m_presentationSession(presentationSession)
        , m_loader(FileReaderLoader::ReadAsArrayBuffer, this)
    {
        m_loader.start(m_presentationSession->executionContext(), blobDataHandle);
    }
    ~BlobLoader() override { }

    // FileReaderLoaderClient functions.
    void didStartLoading() override { }
    void didReceiveData() override { }
    void didFinishLoading() override
    {
        m_presentationSession->didFinishLoadingBlob(m_loader.arrayBufferResult());
    }
    void didFail(FileError::ErrorCode errorCode) override
    {
        m_presentationSession->didFailLoadingBlob(errorCode);
    }

    void cancel()
    {
        m_loader.cancel();
    }

    DEFINE_INLINE_TRACE()
    {
        visitor->trace(m_presentationSession);
    }

private:
    Member<PresentationSession> m_presentationSession;
    FileReaderLoader m_loader;
};

PresentationSession::PresentationSession(LocalFrame* frame, const String& id, const String& url)
    : DOMWindowProperty(frame)
    , m_id(id)
    , m_url(url)
    , m_state(WebPresentationSessionState::Connected)
    , m_binaryType(BinaryTypeBlob)
{
}

PresentationSession::~PresentationSession()
{
    ASSERT(!m_blobLoader);
}

// static
PresentationSession* PresentationSession::take(WebPresentationSessionClient* client, Presentation* presentation)
{
    ASSERT(client);
    ASSERT(presentation);

    PresentationSession* session = new PresentationSession(presentation->frame(), client->getId(), client->getUrl());
    presentation->registerSession(session);
    return session;
}

const AtomicString& PresentationSession::interfaceName() const
{
    return EventTargetNames::PresentationSession;
}

ExecutionContext* PresentationSession::executionContext() const
{
    if (!frame())
        return nullptr;
    return frame()->document();}

DEFINE_TRACE(PresentationSession)
{
    visitor->trace(m_blobLoader);
    RefCountedGarbageCollectedEventTargetWithInlineData<PresentationSession>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

const AtomicString& PresentationSession::state() const
{
    return SessionStateToString(m_state);
}

void PresentationSession::send(const String& message, ExceptionState& exceptionState)
{
    if (!canSendMessage(exceptionState))
        return;

    m_messages.append(adoptPtr(new Message(message)));
    handleMessageQueue();
}

void PresentationSession::send(PassRefPtr<DOMArrayBuffer> arrayBuffer, ExceptionState& exceptionState)
{
    ASSERT(arrayBuffer && arrayBuffer->buffer());
    if (!canSendMessage(exceptionState))
        return;

    m_messages.append(adoptPtr(new Message(arrayBuffer)));
    handleMessageQueue();
}

void PresentationSession::send(PassRefPtr<DOMArrayBufferView> arrayBufferView, ExceptionState& exceptionState)
{
    ASSERT(arrayBufferView);
    if (!canSendMessage(exceptionState))
        return;

    m_messages.append(adoptPtr(new Message(arrayBufferView->buffer())));
    handleMessageQueue();
}

void PresentationSession::send(Blob* data, ExceptionState& exceptionState)
{
    ASSERT(data);
    if (!canSendMessage(exceptionState))
        return;

    m_messages.append(adoptPtr(new Message(data->blobDataHandle())));
    handleMessageQueue();
}

bool PresentationSession::canSendMessage(ExceptionState& exceptionState)
{
    if (m_state == WebPresentationSessionState::Disconnected) {
        throwPresentationDisconnectedError(exceptionState);
        return false;
    }

    // The session can send a message if there is a client available.
    return !!presentationClient(executionContext());
}

void PresentationSession::handleMessageQueue()
{
    WebPresentationClient* client = presentationClient(executionContext());
    if (!client)
        return;

    while (!m_messages.isEmpty() && !m_blobLoader) {
        Message* message = m_messages.first().get();
        switch (message->type) {
        case MessageTypeText:
            client->sendString(m_url, m_id, message->text);
            m_messages.removeFirst();
            break;
        case MessageTypeArrayBuffer:
            client->sendArrayBuffer(m_url, m_id, static_cast<const uint8_t*>(message->arrayBuffer->data()), message->arrayBuffer->byteLength());
            m_messages.removeFirst();
            break;
        case MessageTypeBlob:
            ASSERT(!m_blobLoader);
            m_blobLoader = new BlobLoader(message->blobDataHandle, this);
            break;
        }
    }
}

String PresentationSession::binaryType() const
{
    switch (m_binaryType) {
    case BinaryTypeBlob:
        return "blob";
    case BinaryTypeArrayBuffer:
        return "arraybuffer";
    }
    ASSERT_NOT_REACHED();
    return String();
}

void PresentationSession::setBinaryType(const String& binaryType)
{
    if (binaryType == "blob") {
        m_binaryType = BinaryTypeBlob;
        return;
    }
    if (binaryType == "arraybuffer") {
        m_binaryType = BinaryTypeArrayBuffer;
        return;
    }
    ASSERT_NOT_REACHED();
}

void PresentationSession::didReceiveTextMessage(const String& message)
{
    if (m_state == WebPresentationSessionState::Disconnected)
        return;

    dispatchEvent(MessageEvent::create(message));
}

void PresentationSession::didReceiveBinaryMessage(const uint8_t* data, size_t length)
{
    if (m_state == WebPresentationSessionState::Disconnected)
        return;

    switch (m_binaryType) {
    case BinaryTypeBlob: {
        OwnPtr<BlobData> blobData = BlobData::create();
        blobData->appendBytes(data, length);
        Blob* blob = Blob::create(BlobDataHandle::create(blobData.release(), length));
        dispatchEvent(MessageEvent::create(blob));
        return;
    }
    case BinaryTypeArrayBuffer:
        RefPtr<DOMArrayBuffer> buffer = DOMArrayBuffer::create(data, length);
        dispatchEvent(MessageEvent::create(buffer.release()));
        return;
    }
    ASSERT_NOT_REACHED();
}

void PresentationSession::close()
{
    if (m_state != WebPresentationSessionState::Connected)
        return;
    WebPresentationClient* client = presentationClient(executionContext());
    if (client)
        client->closeSession(m_url, m_id);

    // Cancel current Blob loading if any.
    if (m_blobLoader) {
        m_blobLoader->cancel();
        m_blobLoader.clear();
    }

    // Clear message queue.
    Deque<OwnPtr<Message>> empty;
    m_messages.swap(empty);
}

bool PresentationSession::matches(WebPresentationSessionClient* client) const
{
    return client && m_url == static_cast<String>(client->getUrl()) && m_id == static_cast<String>(client->getId());
}

void PresentationSession::didChangeState(WebPresentationSessionState state)
{
    if (m_state == state)
        return;

    m_state = state;
    dispatchEvent(Event::create(EventTypeNames::statechange));
}

void PresentationSession::didFinishLoadingBlob(PassRefPtr<DOMArrayBuffer> buffer)
{
    ASSERT(!m_messages.isEmpty() && m_messages.first()->type == MessageTypeBlob);
    ASSERT(buffer && buffer->buffer());
    // Send the loaded blob immediately here and continue processing the queue.
    WebPresentationClient* client = presentationClient(executionContext());
    if (client)
        client->sendBlobData(m_url, m_id, static_cast<const uint8_t*>(buffer->data()), buffer->byteLength());

    m_messages.removeFirst();
    m_blobLoader.clear();
    handleMessageQueue();
}

void PresentationSession::didFailLoadingBlob(FileError::ErrorCode errorCode)
{
    ASSERT(!m_messages.isEmpty() && m_messages.first()->type == MessageTypeBlob);
    // FIXME: generate error message?
    // Ignore the current failed blob item and continue with next items.
    m_messages.removeFirst();
    m_blobLoader.clear();
    handleMessageQueue();
}

} // namespace blink
