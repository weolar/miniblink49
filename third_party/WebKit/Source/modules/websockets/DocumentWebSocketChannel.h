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

#ifndef DocumentWebSocketChannel_h
#define DocumentWebSocketChannel_h

#include "core/dom/ContextLifecycleObserver.h"
#include "core/fileapi/Blob.h"
#include "core/fileapi/FileError.h"
#include "core/frame/ConsoleTypes.h"
#include "modules/ModulesExport.h"
#include "modules/websockets/WebSocketChannel.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "public/platform/WebSocketHandle.h"
#include "public/platform/WebSocketHandleClient.h"
#include "wtf/Deque.h"
#include "wtf/FastAllocBase.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/CString.h"
#include "wtf/text/WTFString.h"
#include <stdint.h>

namespace blink {

class Document;
class WebSocketHandshakeRequest;
class WebSocketHandshakeRequestInfo;
class WebSocketHandshakeResponseInfo;

// This class is a WebSocketChannel subclass that works with a Document in a
// DOMWindow (i.e. works in the main thread).
class MODULES_EXPORT DocumentWebSocketChannel final : public WebSocketChannel, public WebSocketHandleClient, public ContextLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DocumentWebSocketChannel);
public:
    // You can specify the source file and the line number information
    // explicitly by passing the last parameter.
    // In the usual case, they are set automatically and you don't have to
    // pass it.
    // Specify handle explicitly only in tests.
    static DocumentWebSocketChannel* create(ExecutionContext* context, WebSocketChannelClient* client, const String& sourceURL = String(), unsigned lineNumber = 0, WebSocketHandle *handle = 0)
    {
        return new DocumentWebSocketChannel(context, client, sourceURL, lineNumber, handle);
    }
    ~DocumentWebSocketChannel() override;

    // WebSocketChannel functions.
    bool connect(const KURL&, const String& protocol) override;
    void send(const CString& message) override;
    void send(const DOMArrayBuffer&, unsigned byteOffset, unsigned byteLength) override;
    void send(PassRefPtr<BlobDataHandle>) override;
    void sendTextAsCharVector(PassOwnPtr<Vector<char>> data) override;
    void sendBinaryAsCharVector(PassOwnPtr<Vector<char>> data) override;
    // Start closing handshake. Use the CloseEventCodeNotSpecified for the code
    // argument to omit payload.
    void close(int code, const String& reason) override;
    void fail(const String& reason, MessageLevel, const String&, unsigned lineNumber) override;
    void disconnect() override;

    DECLARE_VIRTUAL_TRACE();

private:
    enum MessageType {
        MessageTypeText,
        MessageTypeBlob,
        MessageTypeArrayBuffer,
        MessageTypeTextAsCharVector,
        MessageTypeBinaryAsCharVector,
        MessageTypeClose,
    };

    struct Message {
        explicit Message(const CString&);
        explicit Message(PassRefPtr<BlobDataHandle>);
        explicit Message(PassRefPtr<DOMArrayBuffer>);
        // For WorkerWebSocketChannel
        explicit Message(PassOwnPtr<Vector<char>>, MessageType);
        // Close message
        Message(unsigned short code, const String& reason);

        MessageType type;

        CString text;
        RefPtr<BlobDataHandle> blobDataHandle;
        RefPtr<DOMArrayBuffer> arrayBuffer;
        OwnPtr<Vector<char>> vectorData;
        unsigned short code;
        String reason;
    };

    struct ReceivedMessage {
        bool isMessageText;
        Vector<char> data;
    };

    class BlobLoader;

    DocumentWebSocketChannel(ExecutionContext*, WebSocketChannelClient*, const String&, unsigned, WebSocketHandle*);
    void sendInternal(WebSocketHandle::MessageType, const char* data, size_t totalSize, uint64_t* consumedBufferedAmount);
    void processSendQueue();
    void flowControlIfNecessary();
    void failAsError(const String& reason) { fail(reason, ErrorMessageLevel, m_sourceURLAtConstruction, m_lineNumberAtConstruction); }
    void abortAsyncOperations();
    void handleDidClose(bool wasClean, unsigned short code, const String& reason);
    Document* document(); // can be called only when m_identifier > 0.

    // WebSocketHandleClient functions.
    void didConnect(WebSocketHandle*, const WebString& selectedProtocol, const WebString& extensions) override;
    void didStartOpeningHandshake(WebSocketHandle*, const WebSocketHandshakeRequestInfo&) override;
    void didFinishOpeningHandshake(WebSocketHandle*, const WebSocketHandshakeResponseInfo&) override;
    void didFail(WebSocketHandle*, const WebString& message) override;
    void didReceiveData(WebSocketHandle*, bool fin, WebSocketHandle::MessageType, const char* data, size_t /* size */) override;
    void didClose(WebSocketHandle*, bool wasClean, unsigned short code, const WebString& reason) override;
    void didReceiveFlowControl(WebSocketHandle*, int64_t quota) override;
    void didStartClosingHandshake(WebSocketHandle*) override;

    // Methods for BlobLoader.
    void didFinishLoadingBlob(PassRefPtr<DOMArrayBuffer>);
    void didFailLoadingBlob(FileError::ErrorCode);

    // m_handle is a handle of the connection.
    // m_handle == 0 means this channel is closed.
    OwnPtr<WebSocketHandle> m_handle;

    // m_client can be deleted while this channel is alive, but this class
    // expects that disconnect() is called before the deletion.
    Member<WebSocketChannelClient> m_client;
    KURL m_url;
    // m_identifier > 0 means calling scriptContextExecution() returns a Document.
    unsigned long m_identifier;
    Member<BlobLoader> m_blobLoader;
    Deque<OwnPtr<Message>> m_messages;
    Vector<char> m_receivingMessageData;

    bool m_receivingMessageTypeIsText;
    uint64_t m_sendingQuota;
    uint64_t m_receivedDataSizeForFlowControl;
    size_t m_sentSizeOfTopMessage;

    String m_sourceURLAtConstruction;
    unsigned m_lineNumberAtConstruction;
    RefPtr<WebSocketHandshakeRequest> m_handshakeRequest;

    static const uint64_t receivedDataSizeForFlowControlHighWaterMark = 1 << 15;
};

} // namespace blink

#endif // DocumentWebSocketChannel_h
