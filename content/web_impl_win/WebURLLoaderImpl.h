// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebURLLoaderImpl_h
#define WebURLLoaderImpl_h

#include "third_party/WebKit/public/platform/WebURLLoader.h"

typedef void* HINTERNET;

namespace net {
class WebURLLoaderWinINet;
}

class CefSchemeLoaderImpl;

namespace content {

class WebURLLoaderImpl : public blink::WebURLLoader {
public:
    WebURLLoaderImpl();
    virtual ~WebURLLoaderImpl();

    // WebURLLoader methods:
    virtual void loadSynchronously(
        const blink::WebURLRequest& request,
        blink::WebURLResponse& response,
        blink::WebURLError& error,
        blink::WebData& data) override;
    virtual void loadAsynchronously(
        const blink::WebURLRequest& request,
        blink::WebURLLoaderClient* client) override;
    virtual void cancel() override;
    virtual void setDefersLoading(bool value) override;
    virtual void didChangePriority(blink::WebURLRequest::Priority new_priority,
        int intra_priority_value) override;
    virtual bool attachThreadedDataReceiver(
        blink::WebThreadedDataReceiver* threaded_data_receiver) override;

    void onWinINetWillBeDelete();

private:
    net::WebURLLoaderWinINet* m_loaderWinINet;
    CefSchemeLoaderImpl* m_cefSchemeLoader;
};

}  // namespace content

#endif  // WebURLLoaderImpl_h
