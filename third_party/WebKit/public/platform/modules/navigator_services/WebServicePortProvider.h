// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServicePortProvider_h
#define WebServicePortProvider_h

#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/WebVector.h"
#include "public/platform/modules/navigator_services/WebServicePort.h"
#include "public/platform/modules/navigator_services/WebServicePortCallbacks.h"

namespace blink {

class WebString;
class WebURL;


// One instance of this class is associated with each ServicePortCollection.
// When a ServicePortCollection is destroyed the WebServicePortProvider
// associated with it is also destroyed. All communication from ServicePorts and
// the ServicePortCollection to embedding code goes via this interface.
class WebServicePortProvider {
public:
    virtual void destroy() { BLINK_ASSERT_NOT_REACHED(); }

    // Initiates a connection from the given origin to the given URL. When
    // successful the service can communicate with the client over the given
    // channel. The origin isn't passed as WebSecurityOrigin because that would
    // be a layering violation (platform/ code shouldn't depend on web/ code).
    // Ownership of the WebServicePortConnectCallbacks is transferred to the provider.
    virtual void connect(const WebURL&, const WebString& origin, WebServicePortConnectCallbacks*) { BLINK_ASSERT_NOT_REACHED(); }

    // Called when javascript code calls postMessage on a ServicePort that is owned
    // by the ServicePortCollection this provider is associated with.
    // Passes ownership of WebMessagePortChannelArray.
    virtual void postMessage(WebServicePortID, const WebString&, WebMessagePortChannelArray*) { BLINK_ASSERT_NOT_REACHED(); }

    // Called when a ServicePort owned by the ServicePortCollection this
    // provider is associated with is closed (explicitly or via garbage
    // collection).
    virtual void closePort(WebServicePortID) { BLINK_ASSERT_NOT_REACHED(); }

protected:
    virtual ~WebServicePortProvider() {}
};

} // namespace blink

#if INSIDE_BLINK

namespace WTF {

template<typename T> struct OwnedPtrDeleter;
template<> struct OwnedPtrDeleter<blink::WebServicePortProvider> {
    static void deletePtr(blink::WebServicePortProvider* provider)
    {
        if (provider)
            provider->destroy();
    }
};

} // namespace WTF

#endif // INSIDE_BLINK

#endif // WebServicePortProvider_h
