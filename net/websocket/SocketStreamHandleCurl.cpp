/*
 * Copyright (C) 2009 Brent Fulgham.  All rights reserved.
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

#include "config.h"
#include "net/websocket/SocketStreamHandle.h"

#include "net/websocket/SocketStreamHandleClient.h"
#include "net/websocket/SocketStreamError.h"
#include "net/ActivatingObjCheck.h"
#include "net/CheckNetOnline.h"
#include "third_party/WebKit/Source/platform/Logging.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/wtf/MainThread.h"
#include "wke/wkeGlobalVar.h"
#include "base/thread.h"
#include <process.h>
#include <Netlistmgr.h>

using namespace blink;

namespace net {

SocketStreamHandle::SocketStreamHandle(const KURL& url, SocketStreamHandleClient* client)
    : SocketStreamHandleBase(url, client)
    , m_workerThread(0)
    , m_stopThread(0)
    , m_readDataTaskCount(0)
{
    m_id = ActivatingObjCheck::inst()->genId();
    ActivatingObjCheck::inst()->add((intptr_t)m_id);
    WTF_LOG(Network, "SocketStreamHandle %p new client %p", this, m_client);
    ASSERT(isMainThread());
    startThread();
}

SocketStreamHandle::~SocketStreamHandle()
{
    WTF_LOG(Network, "SocketStreamHandle %p delete", this);
    ASSERT(!m_workerThread);
    ActivatingObjCheck::inst()->remove((intptr_t)m_id);
}

int SocketStreamHandle::platformSend(const char* data, int length)
{
    WTF_LOG(Network, "SocketStreamHandle %p platformSend", this);

    ASSERT(isMainThread());

    startThread();

    auto copy = createCopy(data, length);

    WTF::Locker<WTF::Mutex> lock(m_mutexSend);
    m_sendData.append(adoptPtr(new SocketData(copy, length)));

    return length;
}

void SocketStreamHandle::platformClose()
{
    WTF_LOG(Network, "SocketStreamHandle %p platformClose", this);

    ASSERT(isMainThread());

    stopThread();

    if (m_client && ActivatingObjCheck::inst()->isActivating((intptr_t)m_clientId))
        m_client->didCloseSocketStream(this);
}

static void s_mainThreadReadData(void* param)
{
    SocketStreamHandle* hanlde = (SocketStreamHandle*)param;
    hanlde->mainThreadReadData();
}

static void s_mainThreadFail(void* param)
{
    SocketStreamHandle* hanlde = (SocketStreamHandle*)param;
    hanlde->mainThreadFail();
}

void SocketStreamHandle::mainThreadFail()
{
    stopThread();
    SocketStreamError err(-2);
    if (m_client && ActivatingObjCheck::inst()->isActivating((intptr_t)m_clientId))
        m_client->didFailSocketStream(this, err);
    deref();
}

void SocketStreamHandle::mainThreadReadData()
{
    atomicDecrement(&m_readDataTaskCount);
    didReceiveData();
    deref();
}

bool SocketStreamHandle::readData(CURL* curlHandle)
{
    ASSERT(!isMainThread());

    const int bufferSize = 1024 * 300;
    char* data = new char[bufferSize];
    size_t bytesRead = 0;

    CURLcode ret = curl_easy_recv(curlHandle, data, bufferSize, &bytesRead);
    ASSERT(bytesRead <= bufferSize);

    if (ret == CURLE_OK && bytesRead >= 0) {
        m_mutexReceive.lock();
        m_receiveData.append(adoptPtr(new SocketData(data, static_cast<int>(bytesRead))));
        m_mutexReceive.unlock();

        ref();

        if (0 == m_readDataTaskCount) {
            atomicIncrement(&m_readDataTaskCount);
            WTF::internal::callOnMainThread(s_mainThreadReadData, this);
        }
        return true;
    }

    delete[] data;

    if (ret == CURLE_AGAIN)
        return true;

    return false;
}

bool SocketStreamHandle::sendData(CURL* curlHandle)
{
    ASSERT(!isMainThread());

    while (true) {
        m_mutexSend.lock();
        if (!m_sendData.size()) {
            m_mutexSend.unlock();
            break;
        }
        PassOwnPtr<SocketData> sendData = m_sendData.takeFirst();
        m_mutexSend.unlock();

        int totalBytesSent = 0;
        while (totalBytesSent < sendData->size) {
            size_t bytesSent = 0;
            CURLcode ret = curl_easy_send(curlHandle, sendData->data + totalBytesSent, sendData->size - totalBytesSent, &bytesSent);
            if (ret == CURLE_OK)
                totalBytesSent += bytesSent;
            else
                break;
        }

        // Insert remaining data into send queue.

        if (totalBytesSent < sendData->size) {
            const int restLength = sendData->size - totalBytesSent;
            auto copy = createCopy(sendData->data + totalBytesSent, restLength);

            WTF::Locker<WTF::Mutex> lock(m_mutexSend);
            m_sendData.prepend(adoptPtr(new SocketData(copy, restLength)));

            return false;
        }
    }

    return true;
}

// in microseconds
int SocketStreamHandle::waitForAvailableData(CURL* curlHandle, long long selectTimeout)
{
    ASSERT(!isMainThread());

    long long usec = selectTimeout * 1000;

    struct timeval timeout;
    if (usec <= (0)) {
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
    } else {
        timeout.tv_sec = usec / 1000000;
        timeout.tv_usec = usec % 1000000;
    }

    long socket;
    CURLcode err = curl_easy_getinfo(curlHandle, CURLINFO_LASTSOCKET, &socket);
    if (err != CURLE_OK) {
        char* output = (char*)malloc(0x100);
        sprintf(output, "SocketStreamHandle::waitForAvailableData fail 1: %d\n", err);
        OutputDebugStringA(output);
        free(output);

        return -1;
    }

    ::Sleep(50);

    fd_set fdread;
    FD_ZERO(&fdread);
    FD_SET(socket, &fdread);
    int rc = ::select(0, &fdread, nullptr, nullptr, &timeout);

    if (rc != 1 && rc != 0) {
        char* output = (char*)malloc(0x100);
        sprintf(output, "SocketStreamHandle::waitForAvailableData fail 2: %d\n", err);
        OutputDebugStringA(output);
        free(output);
        return -1;
    }
    return rc == 1 ? 1 : 0;
}

static unsigned __stdcall webSocketThread(void* param)
{
    base::SetThreadName("WebSocketThread");
    SocketStreamHandle* invocation = (static_cast<SocketStreamHandle*>(param));
    invocation->threadFunction();
    return 0;
}

static void s_mainThreadRun(void* param)
{
    SocketStreamHandle* hanlde = (SocketStreamHandle*)param;
    hanlde->mainThreadRun();
}

void SocketStreamHandle::mainThreadRun()
{
    // Check reference count to fix a crash.
    // When the call is invoked on the main thread after all other references are released, the SocketStreamClient
    // is already deleted. Accessing the SocketStreamClient in didOpenSocket() will then cause a crash.
    if (refCount() > 1)
        didOpenSocket();
    deref();
}

void SocketStreamHandle::threadFunction()
{
    ASSERT(!isMainThread());

    CURL* curlHandle = curl_easy_init();

    if (!curlHandle)
        return;

    String url = m_url.host();
    unsigned short port = m_url.port();
    bool isSSL = !m_url.protocolIs("ws");
    if (0 == port)
        port = isSSL ? 443 : 80;
    
    if (isSSL)
        url = "https://" + url;
    else
        url = "http://" + url;

    //curl_easy_setopt(curlHandle, CURLOPT_URL, m_url.host().utf8().data());
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.utf8().data());

    curl_easy_setopt(curlHandle, CURLOPT_PORT, port);
    curl_easy_setopt(curlHandle, CURLOPT_CONNECT_ONLY);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT_MS, 500);

    static const int kAllowedProtocols = CURLPROTO_FILE | CURLPROTO_FTP | CURLPROTO_FTPS | CURLPROTO_HTTP | CURLPROTO_HTTPS;
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, false); // ignoreSSLErrors
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, FALSE);
    curl_easy_setopt(curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
    curl_easy_setopt(curlHandle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5); // 5 minutes
    curl_easy_setopt(curlHandle, CURLOPT_PROTOCOLS, kAllowedProtocols);
    curl_easy_setopt(curlHandle, CURLOPT_REDIR_PROTOCOLS, kAllowedProtocols);

    if (wke::g_DNS)
        curl_easy_setopt(curlHandle, CURLOPT_DNS_SERVERS, wke::g_DNS->c_str());

    if (wke::g_curlSetoptCallback)
        wke::g_curlSetoptCallback(curlHandle, wke::curlSetopt);
   
    // Connect to host
    const int kRetryCount = 5;
    CURLcode curlCode = CURLE_FAILED_INIT;
    for (int i = 0; i < kRetryCount; ++i) {
        if (0 != m_stopThread)
            break;
        curlCode = curl_easy_perform(curlHandle);
    }
    if (CURLE_OK != curlCode) {
        ref();
        WTF::internal::callOnMainThread(s_mainThreadFail, this);
        return;
    }

    IUnknown* pUnknown = nullptr;
    INetworkListManager* pNetworkListManager = getNetworkList(&pUnknown);

    ref();

    WTF::internal::callOnMainThread(s_mainThreadRun, this);

    int retryCount = 0;
    while (0 == m_stopThread) {
        // Send queued data
        sendData(curlHandle);

        // Wait until socket has available data
        int waitResult = waitForAvailableData(curlHandle, 50);

        if (1 == waitResult) {
            readData(curlHandle);
            continue;
        } else if (-1 == waitResult) {
            ++retryCount;
        }

        int checkNetwork = checkIsNetwork(pNetworkListManager);
        if (retryCount > 3 || FALSE == checkNetwork) {
            ref();
            WTF::internal::callOnMainThread(s_mainThreadFail, this);
            break;
        }
    }

    curl_easy_cleanup(curlHandle);

    if (pNetworkListManager)
        pNetworkListManager->Release();
    if (pUnknown)
        pUnknown->Release();
}

void SocketStreamHandle::startThread()
{
    ASSERT(isMainThread());

    if (m_workerThread)
        return;

    ref(); // stopThread() will call deref().

    m_workerThread = static_cast<ThreadIdentifier>(_beginthreadex(0, 0, webSocketThread, this, 0, nullptr));
}

int waitForThreadCompletion(ThreadIdentifier threadID)
{
    ASSERT(threadID);

    HANDLE threadHandle = (HANDLE)(threadID);
//     if (!threadHandle)
//         WTF_LOG("ThreadIdentifier %u did not correspond to an active thread when trying to quit", threadID);

    DWORD joinResult = ::WaitForSingleObject(threadHandle, INFINITE);
//     if (joinResult == WAIT_FAILED)
//         WTF_LOG("ThreadIdentifier %u was found to be deadlocked trying to quit", threadID);

    CloseHandle(threadHandle);
    //clearThreadHandleForIdentifier(threadID);

    return joinResult;
}

void SocketStreamHandle::stopThread()
{
    ASSERT(isMainThread());

    if (!m_workerThread)
        return;

    InterlockedExchange(reinterpret_cast<long volatile*>(&m_stopThread), 1);
    waitForThreadCompletion(m_workerThread);
    m_workerThread = 0;
    deref();
}

void SocketStreamHandle::didReceiveData()
{
    ASSERT(isMainThread());

    m_mutexReceive.lock();
    Deque<OwnPtr<SocketData>> receiveData;
    receiveData.swap(m_receiveData);
    m_mutexReceive.unlock();

    for (auto& socketData : receiveData) {
        if (socketData->size > 0) {
            if (m_client && state() == Open && ActivatingObjCheck::inst()->isActivating((intptr_t)m_clientId))
                m_client->didReceiveSocketStreamData(this, socketData->data, socketData->size);
        } else {
            //platformClose();
            //OutputDebugStringA("SocketStreamHandle::didReceiveData, receiveData is 0\n");
        }
    }
}

void SocketStreamHandle::didOpenSocket()
{
    ASSERT(isMainThread());

    m_state = Open;

    if (m_client && ActivatingObjCheck::inst()->isActivating((intptr_t)m_clientId))
        m_client->didOpenSocketStream(this);
}

char* SocketStreamHandle::createCopy(const char* data, int length)
{
    char* copy = (new char[length]);
    memcpy(copy, data, length);

    return copy;
}

void SocketStreamHandle::didReceiveAuthenticationChallenge(const AuthenticationChallenge&)
{
    notImplemented();
}

void SocketStreamHandle::receivedCredential(const AuthenticationChallenge&, const Credential&)
{
    notImplemented();
}

void SocketStreamHandle::receivedRequestToContinueWithoutCredential(const AuthenticationChallenge&)
{
    notImplemented();
}

void SocketStreamHandle::receivedCancellation(const AuthenticationChallenge&)
{
    notImplemented();
}

void SocketStreamHandle::receivedRequestToPerformDefaultHandling(const AuthenticationChallenge&)
{
    notImplemented();
}

void SocketStreamHandle::receivedChallengeRejection(const AuthenticationChallenge&)
{
    notImplemented();
}

} // namespace net