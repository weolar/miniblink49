// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RemoteFrameClient_h
#define RemoteFrameClient_h

#include "core/frame/FrameClient.h"
#include "core/frame/FrameTypes.h"
#include "core/loader/FrameLoaderTypes.h"

namespace blink {

class Event;
class ResourceRequest;

class RemoteFrameClient : public FrameClient {
public:
    virtual ~RemoteFrameClient() { }

    virtual void navigate(const ResourceRequest&, bool shouldReplaceCurrentEntry) = 0;
    virtual void reload(FrameLoadType, ClientRedirectPolicy) = 0;
    virtual unsigned backForwardLength() = 0;

    // FIXME: Remove this method once we have input routing in the browser
    // process. See http://crbug.com/339659.
    virtual void forwardInputEvent(Event*) = 0;
};

} // namespace blink

#endif // RemoteFrameClient_h
