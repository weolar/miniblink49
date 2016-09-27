/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
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

#ifndef WorkerWebSocketChannel_h
#define WorkerWebSocketChannel_h

#include "core/frame/ConsoleTypes.h"
#include "modules/websockets/WebSocketChannel.h"
#include "modules/websockets/WebSocketChannelClient.h"
#include "platform/heap/Handle.h"
#include "wtf/Assertions.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"
#include <stdint.h>

namespace blink {

class BlobDataHandle;
class KURL;
class ExecutionContext;
class ExecutionContextTask;
class WebSocketChannelSyncHelper;
class WorkerGlobalScope;
class WorkerLoaderProxy;

class WorkerWebSocketChannel final : public WebSocketChannel {
    WTF_MAKE_NONCOPYABLE(WorkerWebSocketChannel);
public:
    static WebSocketChannel* create(WorkerGlobalScope& workerGlobalScope, WebSocketChannelClient* client, const String& sourceURL, unsigned lineNumber)
    {
        return new WorkerWebSocketChannel(workerGlobalScope, client, sourceURL, lineNumber);
    }
    ~WorkerWebSocketChannel() override;

    // WebSocketChannel functions.
    bool connect(const KURL&, const String& protocol) override;
    void send(const CString&) override;
    void send(const DOMArrayBuffer&, unsigned byteOffset, unsigned byteLength) override;
    void send(PassRefPtr<BlobDataHandle>) override;
    void sendTextAsCharVector(PassOwnPtr<Vector<char>>) override
    {
        ASSERT_NOT_REACHED();
    }
    void sendBinaryAsCharVector(PassOwnPtr<Vector<char>>) override
    {
        ASSERT_NOT_REACHED();
    }
    void close(int code, const String& reason) override;
    void fail(const String& reason, MessageLevel, const String&, unsigned) override;
    void disconnect() override; // Will suppress didClose().

    DECLARE_VIRTUAL_TRACE();

    class Bridge;
    // Allocated in the worker thread, but used in the main thread.
    class Peer final : public GarbageCollectedFinalized<Peer>, public WebSocketChannelClient {
        USING_GARBAGE_COLLECTED_MIXIN(Peer);
        WTF_MAKE_NONCOPYABLE(Peer);
    public:
        Peer(Bridge*, PassRefPtr<WorkerLoaderProxy>, WebSocketChannelSyncHelper*);
        ~Peer() override;

        // sourceURLAtConnection and lineNumberAtConnection parameters may
        // be shown when the connection fails.
        void initialize(const String& sourceURLAtConnection, unsigned lineNumberAtConnection, ExecutionContext*);

        void connect(const KURL&, const String& protocol);
        void sendTextAsCharVector(PassOwnPtr<Vector<char>>);
        void sendBinaryAsCharVector(PassOwnPtr<Vector<char>>);
        void sendBlob(PassRefPtr<BlobDataHandle>);
        void close(int code, const String& reason);
        void fail(const String& reason, MessageLevel, const String& sourceURL, unsigned lineNumber);
        void disconnect();

        DECLARE_VIRTUAL_TRACE();

        // WebSocketChannelClient functions.
        void didConnect(const String& subprotocol, const String& extensions) override;
        void didReceiveTextMessage(const String& payload) override;
        void didReceiveBinaryMessage(PassOwnPtr<Vector<char>>) override;
        void didConsumeBufferedAmount(uint64_t) override;
        void didStartClosingHandshake() override;
        void didClose(ClosingHandshakeCompletionStatus, unsigned short code, const String& reason) override;
        void didError() override;

    private:
        Member<Bridge> m_bridge;
        RefPtr<WorkerLoaderProxy> m_loaderProxy;
        Member<WebSocketChannel> m_mainWebSocketChannel;
        Member<WebSocketChannelSyncHelper> m_syncHelper;
    };

    // Bridge for Peer. Running on the worker thread.
    class Bridge final : public GarbageCollectedFinalized<Bridge> {
        WTF_MAKE_NONCOPYABLE(Bridge);
    public:
        Bridge(WebSocketChannelClient*, WorkerGlobalScope&);
        ~Bridge();
        // sourceURLAtConnection and lineNumberAtConnection parameters may
        // be shown when the connection fails.
        void initialize(const String& sourceURLAtConnection, unsigned lineNumberAtConnection);
        bool connect(const KURL&, const String& protocol);
        void send(const CString& message);
        void send(const DOMArrayBuffer&, unsigned byteOffset, unsigned byteLength);
        void send(PassRefPtr<BlobDataHandle>);
        void close(int code, const String& reason);
        void fail(const String& reason, MessageLevel, const String& sourceURL, unsigned lineNumber);
        void disconnect();

        // Returns null when |disconnect| has already been called.
        WebSocketChannelClient* client() { return m_client; }

        DECLARE_TRACE();

    private:
        // Returns false if shutdown event is received before method completion.
        bool waitForMethodCompletion(PassOwnPtr<ExecutionContextTask>);

        Member<WebSocketChannelClient> m_client;
        RefPtrWillBeMember<WorkerGlobalScope> m_workerGlobalScope;
        RefPtr<WorkerLoaderProxy> m_loaderProxy;
        Member<WebSocketChannelSyncHelper> m_syncHelper;
        Member<Peer> m_peer;
    };

private:
    WorkerWebSocketChannel(WorkerGlobalScope&, WebSocketChannelClient*, const String& sourceURL, unsigned lineNumber);

    Member<Bridge> m_bridge;
    String m_sourceURLAtConnection;
    unsigned m_lineNumberAtConnection;
};

} // namespace blink

#endif // WorkerWebSocketChannel_h
