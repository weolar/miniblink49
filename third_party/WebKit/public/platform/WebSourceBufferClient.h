// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSourceBufferClient_h
#define WebSourceBufferClient_h

namespace blink {

class WebSourceBufferClient {
public:
    virtual ~WebSourceBufferClient() { }

    // FIXME: Add a track collection parameter here.
    virtual void initializationSegmentReceived() = 0;
};

} // namespace blink

#endif // WebSourceBufferClient_h
