// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPresentationController_h
#define WebPresentationController_h

#include "public/platform/WebCommon.h"

namespace blink {

class WebPresentationSessionClient;
class WebString;
enum class WebPresentationSessionState;

// The delegate Blink provides to WebPresentationClient in order to get updates.
class BLINK_PLATFORM_EXPORT WebPresentationController {
public:
    virtual ~WebPresentationController() { }

    // Called when the presentation session is started by the embedder using
    // the default presentation URL and id.
    virtual void didStartDefaultSession(WebPresentationSessionClient*) = 0;

    // Called when the state of a session changes.
    virtual void didChangeSessionState(WebPresentationSessionClient*, WebPresentationSessionState) = 0;

    // Called when a text message of a session is received.
    virtual void didReceiveSessionTextMessage(WebPresentationSessionClient*, const WebString& message) = 0;

    // Called when a binary message of a session is received.
    virtual void didReceiveSessionBinaryMessage(WebPresentationSessionClient*, const uint8_t* data, size_t length) = 0;
};

} // namespace blink

#endif // WebPresentationController_h
