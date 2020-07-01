/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc.  All rights reserved.
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

#ifndef SocketStreamHandle_h
#define SocketStreamHandle_h

#include "net/websocket/SocketStreamHandleBase.h"
#include "third_party/WebKit/Source/wtf/Deque.h"
#include "third_party/WebKit/Source/wtf/Locker.h"
#include "third_party/WebKit/Source/wtf/RefCounted.h"
#include "third_party/WebKit/Source/wtf/Threading.h"
#include "third_party/WebKit/Source/wtf/PassRefPtr.h"
#include "third_party/WebKit/Source/wtf/ThreadSafeRefCounted.h"
#include "third_party/WebKit/Source/wtf/ThreadingPrimitives.h"
#include "third_party/libcurl/include/curl/curl.h"

namespace blink {
class AuthenticationChallenge;
class Credential;
}

namespace net {

class SocketStreamHandleClient;

class SocketStreamHandle : public ThreadSafeRefCounted<SocketStreamHandle>, public SocketStreamHandleBase {
public:
    static RefPtr<SocketStreamHandle> create(const blink::KURL& url, SocketStreamHandleClient* client) { return adoptRef(new SocketStreamHandle(url, client)); }

    virtual ~SocketStreamHandle();

    void threadFunction();
    void mainThreadRun();
    void mainThreadReadData();

    int getId() const { return m_id;}

private:
    SocketStreamHandle(const blink::KURL&, SocketStreamHandleClient*);

    int platformSend(const char* data, int length) override;
    void platformClose() override;
    
    bool readData(CURL*);
    bool sendData(CURL*);
    bool waitForAvailableData(CURL*, long long selectTimeout);

    void startThread();
    void stopThread();

    void didReceiveData();
    void didOpenSocket();

    static char* createCopy(const char* data, int length);

    // No authentication for streams per se, but proxy may ask for credentials.
    void didReceiveAuthenticationChallenge(const blink::AuthenticationChallenge&);
    void receivedCredential(const blink::AuthenticationChallenge&, const blink::Credential&);
    void receivedRequestToContinueWithoutCredential(const blink::AuthenticationChallenge&);
    void receivedCancellation(const blink::AuthenticationChallenge&);
    void receivedRequestToPerformDefaultHandling(const blink::AuthenticationChallenge&);
    void receivedChallengeRejection(const blink::AuthenticationChallenge&);

    struct SocketData {
        SocketData(char* source, int length)
        {
            data = (source);
            size = length;
        }

        SocketData(SocketData&& other)
        {
            data = (other.data);
            size = other.size;
            other.size = 0;
        }

        ~SocketData()
        {
            ASSERT(data);
            delete[] data;
        }

        char* data;
        int size;
    };

    ThreadIdentifier m_workerThread;
    long m_stopThread;
    WTF::Mutex m_mutexSend;
    WTF::Mutex m_mutexReceive;
    Deque<OwnPtr<SocketData>> m_sendData;
    Deque<OwnPtr<SocketData>> m_receiveData;
    int m_readDataTaskCount;
    int m_id;
};

} // namespace net

#endif // SocketStreamHandle_h
