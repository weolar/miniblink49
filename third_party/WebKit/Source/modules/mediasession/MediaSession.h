// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaSession_h
#define MediaSession_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"

namespace blink {

class MediaSession : public GarbageCollected<MediaSession>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static MediaSession* create();

    void activate();
    void deactivate();

    DEFINE_INLINE_TRACE() { }

private:
    MediaSession() = default;
};

} // namespace blink

#endif // MediaSession_h
