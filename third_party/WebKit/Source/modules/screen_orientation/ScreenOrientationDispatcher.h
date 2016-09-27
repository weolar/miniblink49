// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScreenOrientationDispatcher_h
#define ScreenOrientationDispatcher_h

#include "core/frame/PlatformEventDispatcher.h"
#include "platform/heap/Handle.h"

namespace blink {

// ScreenOrientationDispatcher is a singleton that handles whether the current
// Blink instance should be listening to the screen orientation platform events.
// It is not a common implementation of PlatformEventDispatcher in the sense
// that it doesn't actually dispatch events but simply start/stop listening. The
// reason being that screen orientation events are always sent to the WebFrame's
// but some platforms require to poll to have an accurate reporting. When
// ScreenOrientationDispatcher is listening, that means that the platform should
// be polling if required.
class ScreenOrientationDispatcher final : public GarbageCollectedFinalized<ScreenOrientationDispatcher>, public PlatformEventDispatcher {
    USING_GARBAGE_COLLECTED_MIXIN(ScreenOrientationDispatcher);
public:
    static ScreenOrientationDispatcher& instance();

    DECLARE_VIRTUAL_TRACE();

private:
    ScreenOrientationDispatcher();

    // Inherited from PlatformEventDispatcher.
    void startListening() override;
    void stopListening() override;
};

} // namespace blink

#endif // ScreenOrientationDispatcher_h
