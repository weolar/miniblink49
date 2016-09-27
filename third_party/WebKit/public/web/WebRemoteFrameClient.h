// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebRemoteFrameClient_h
#define WebRemoteFrameClient_h

#include "public/platform/WebSecurityOrigin.h"
#include "public/web/WebDOMMessageEvent.h"

namespace blink {
class WebInputEvent;
class WebLocalFrame;
class WebRemoteFrame;
struct WebRect;

class WebRemoteFrameClient {
public:
    // Specifies the reason for the detachment.
    enum class DetachType { Remove, Swap };

    // Notify the embedder that it should remove this frame from the frame tree
    // and release any resources associated with it.
    virtual void frameDetached(DetachType) { }

    // Notifies the embedder that a postMessage was issued to a remote frame.
    virtual void postMessageEvent(
        WebLocalFrame* sourceFrame,
        WebRemoteFrame* targetFrame,
        WebSecurityOrigin targetOrigin,
        WebDOMMessageEvent) { }

    // Send initial drawing parameters to a child frame that is being rendered
    // out of process.
    virtual void initializeChildFrame(
        const WebRect& frameRect,
        float scaleFactor) { }

    // A remote frame was asked to start a navigation.
    virtual void navigate(const WebURLRequest& request, bool shouldReplaceCurrentEntry) { }
    virtual void reload(bool ignoreCache, bool isClientRedirect) { }

    // FIXME: Remove this method once we have input routing in the browser
    // process. See http://crbug.com/339659.
    virtual void forwardInputEvent(const WebInputEvent*) { }
};

} // namespace blink

#endif // WebRemoteFrameClient_h
