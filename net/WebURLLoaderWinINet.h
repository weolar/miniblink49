// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebURLLoaderWinINet_h
#define WebURLLoaderWinINet_h

#include "third_party/WebKit/public/platform/WebURLLoader.h"

typedef void* HINTERNET;

namespace content {
class WebURLLoaderImpl;
}

namespace net {

class LoadFileAsyncTask;
 
class WebURLLoaderWinINet {
public:
    WebURLLoaderWinINet(content::WebURLLoaderImpl* loader);
    ~WebURLLoaderWinINet();

    void init(bool asynchronously);

    // WebURLLoader methods:
    void loadSynchronously(
        const blink::WebURLRequest& request,
        blink::WebURLResponse& response,
        blink::WebURLError& error,
        blink::WebData& data);
    void loadAsynchronously(
        const blink::WebURLRequest& request,
        blink::WebURLLoaderClient* client);
    void cancel();
    void setDefersLoading(bool value);
    void didChangePriority(blink::WebURLRequest::Priority new_priority,
        int intra_priority_value);
    bool attachThreadedDataReceiver(blink::WebThreadedDataReceiver* threaded_data_receiver);

    void fileLoadImpl(const blink::KURL& url);

    blink::WebURLLoaderClient* client() { return m_client; }
    blink::WebURLRequest* firstRequest() { return m_request; }

    void onRedirect();
    void onTimeout();
    bool onRequestComplete();
    static void CALLBACK internetStatusCallback(HINTERNET, DWORD_PTR, DWORD, LPVOID, DWORD);

    bool requestCanceled() { return m_requestCanceled; }

    void onLoaderWillBeDelete();

    double startTime() const { return m_startTime; }

    blink::KURL* m_debugRedirectPath;

private:
    bool start(const blink::WebURLRequest& request, blink::WebURLLoaderClient* client, bool synchronously);

    blink::WebURLRequest* m_request;
    blink::WebURLLoaderClient* m_client;
    content::WebURLLoaderImpl* m_loader;

    HINTERNET m_internetHandle;
    HINTERNET m_connectHandle;
    HINTERNET m_requestHandle;
    bool m_sentEndRequest;
    Vector<char> m_formData;
    size_t m_bytesRemainingToWrite;
    bool m_loadSynchronously;
    bool m_hasReceivedResponse;

    String m_redirectUrl;
    bool m_bHaveRefAtOtherThread; // 在别的线程（如internetStatusCallback）调用过ref，在onRequestComplete里需要多一次deref

    bool m_requestCanceled;
    bool m_canDestroy;

    Vector<unsigned char> m_gzipDecompressData;
    int64_t m_totalEncodedDataLength;

    bool* m_hadDestroied;
    double m_startTime;

    LoadFileAsyncTask* m_fileAsyncTask;
};

}  // namespace net

#endif  // WebURLLoaderWinINet_h
