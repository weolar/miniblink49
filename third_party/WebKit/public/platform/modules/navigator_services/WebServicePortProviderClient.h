// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServicePortProviderClient_h
#define WebServicePortProviderClient_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebMessagePortChannel.h"
#include "public/platform/modules/navigator_services/WebServicePort.h"

namespace blink {

class WebString;

// Interface implemented by blink, used for communication back from embedding
// code to blink. An instance of this interface is passed to the embedder when
// a WebServicePortProvider is created.
class BLINK_PLATFORM_EXPORT WebServicePortProviderClient {
public:
    // Post a message to one of the ports owned by a ServicePortCollection.
    virtual void postMessage(WebServicePortID, const WebString&, const WebMessagePortChannelArray&) = 0;

protected:
    virtual ~WebServicePortProviderClient() {}
};

} // namespace blink

#endif // WebServicePortProviderClient_h
