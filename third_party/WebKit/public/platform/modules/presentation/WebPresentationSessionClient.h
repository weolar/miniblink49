// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPresentationSessionClient_h
#define WebPresentationSessionClient_h

#include "public/platform/WebString.h"

namespace blink {

enum class WebPresentationSessionState {
    Connected = 0,
    Disconnected,
};

// The implementation the embedder has to provide for the Presentation API to work.
class WebPresentationSessionClient {
public:
    virtual ~WebPresentationSessionClient() { }

    virtual WebString getId() = 0;
    virtual WebString getUrl() = 0;
};

} // namespace blink

#endif // WebPresentationSessionClient_h
