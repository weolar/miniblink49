/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Collabora, Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef PluginStream_h
#define PluginStream_h

#include "content/web_impl_win/npapi/PluginQuirkSet.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/Source/platform/network/ResourceRequest.h"
#include "third_party/WebKit/Source/platform/network/ResourceResponse.h"
#include "third_party/WebKit/Source/platform/heap/Handle.h"
#include "third_party/WebKit/public/platform/WebURLLoaderClient.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/platform/WebURLRequest.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/npapi/bindings/npapi.h"
#include <wtf/HashMap.h>
#include <wtf/RefCounted.h>
#include <wtf/Vector.h>
#include <wtf/text/CString.h>
#include <wtf/text/StringHash.h>
#include <wtf/text/WTFString.h>
#include <vector>

namespace blink {
class LocalFrame;
}

namespace content {
class PluginStream;

enum PluginStreamState { StreamBeforeStarted, StreamStarted, StreamStopped };

class PluginStreamClient {
public:
    virtual ~PluginStreamClient() {}
    virtual void streamDidFinishLoading(PluginStream*) {}
};

class PluginStream : public blink::GarbageCollectedFinalized<PluginStream>, public blink::WebURLLoaderClient {
public:
    static PluginStream* create(PluginStreamClient* client, blink::LocalFrame* frame, const blink::ResourceRequest& request, bool sendNotification, void* notifyData, const NPPluginFuncs* functions, NPP instance, const PluginQuirkSet& quirks)
    {
        return (new PluginStream(client, frame, request, sendNotification, notifyData, functions, instance, quirks));
    }
    virtual ~PluginStream();

    void start();
    void stop();

    void startStream();

    void setLoadManually(bool loadManually) { m_loadManually = loadManually; }

    void sendJavaScriptStream(const blink::KURL& requestURL, const WTF::CString& resultString);
    void cancelAndDestroyStream(NPReason);

    static NPP ownerForStream(NPStream*);

    void ref();
    void deref();
    DECLARE_VIRTUAL_TRACE();

private:
    PluginStream(PluginStreamClient*, blink::LocalFrame*, const blink::ResourceRequest&, bool sendNotification, void* notifyData, const NPPluginFuncs*, NPP instance, const PluginQuirkSet&);

    void deliverData();
    void destroyStream(NPReason);
    void destroyStream();

    // WebURLLoaderClient
    virtual void willSendRequest(blink::WebURLLoader*, blink::WebURLRequest& newRequest, const blink::WebURLResponse& redirectResponse) override;
    virtual void didReceiveResponse(blink::WebURLLoader*, const blink::WebURLResponse&) override;
    virtual void didReceiveData(blink::WebURLLoader*, const char* data, int dataLength, int encodedDataLength) override;
    virtual void didFail(blink::WebURLLoader* loader, const blink::WebURLError&) override;
    virtual void didFinishLoading(blink::WebURLLoader* loader, double finishTime, int64_t totalEncodedDataLength) override;
    bool wantsAllStreams() const;

    blink::WebURLRequest m_resourceRequest;
    blink::WebURLResponse m_resourceResponse;

    PluginStreamClient* m_client;
    blink::LocalFrame* m_frame;
    OwnPtr<blink::WebURLLoader> m_loader;
    void* m_notifyData;
    bool m_sendNotification;
    PluginStreamState m_streamState;
    bool m_loadManually;

    blink::Timer<PluginStream> m_delayDeliveryTimer;
    void delayDeliveryTimerFired(blink::Timer<PluginStream>*);

    OwnPtr<std::vector<char>> m_deliveryData;

    HANDLE m_tempFileHandle;

    const NPPluginFuncs* m_pluginFuncs;
    NPP m_instance;
    uint16_t m_transferMode;
    int32_t m_offset;
    CString m_headers;
    String m_path;
    NPReason m_reason;
    NPStream m_stream;
    PluginQuirkSet m_quirks;

    int m_ref;
    blink::Persistent<PluginStream> m_keepAlive;

    friend class WebPluginImpl;
};

} // namespace content

#endif
