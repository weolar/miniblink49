// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CefSchemeLoader_h
#define CefSchemeLoader_h

#include "include/internal/cef_ptr.h"
#include "include/cef_resource_handler.h"

#include "third_party/WebKit/public/platform/WebURLLoader.h"

class CefRequestHeadCallback;
class CefRequestDataCallback;
class CefResponse;

class CefSchemeLoaderImpl {
public:
    CefSchemeLoaderImpl(blink::WebURLLoader* loader, blink::WebURLLoaderClient* client);
    virtual ~CefSchemeLoaderImpl();

    static bool CanLoad(const blink::WebURLRequest& request, blink::WebURLLoaderClient* client);
    static CefSchemeLoaderImpl* LoadAsynchronously(
        const blink::WebURLRequest& request, 
        blink::WebURLLoaderClient* client, 
        blink::WebURLLoader* loader,
        bool& canLoad);

    void LoadSynchronously(
        const blink::WebURLRequest& request,
        blink::WebURLLoaderClient* client,
        blink::WebURLLoader* loader,
        blink::WebURLResponse& response,
        blink::WebURLError& error,
        blink::WebData& data);

    void Cancel();

    void SetDefersLoading(bool value);
    void DidChangePriority(blink::WebURLRequest::Priority new_priority, int intra_priority_value);
    bool AttachThreadedDataReceiver(blink::WebThreadedDataReceiver* threaded_data_receiver);

    void OnLoaderWillBeDelete();

    void OnHeadAvailable();
    void OnDataAvailable();

private:
    blink::WebURLLoader* m_loader;
    blink::WebURLLoaderClient* m_client;

    CefRefPtr<CefResourceHandler> m_resourceHandler;
    CefRefPtr<CefRequestHeadCallback> m_headCallback;
    CefRefPtr<CefRequestDataCallback> m_dataCallback;
    CefRefPtr<CefResponse> m_response;
    int64 m_responseLength;
    bool m_headAvailable;
    int64_t m_totalEncodedDataLength;
};

#endif  // CefSchemeLoader_h
