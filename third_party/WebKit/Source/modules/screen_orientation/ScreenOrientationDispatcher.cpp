// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/screen_orientation/ScreenOrientationDispatcher.h"

#include "public/platform/Platform.h"

namespace blink {

ScreenOrientationDispatcher& ScreenOrientationDispatcher::instance()
{
    DEFINE_STATIC_LOCAL(Persistent<ScreenOrientationDispatcher>, screenOrientationDispatcher, (new ScreenOrientationDispatcher()));
    return *screenOrientationDispatcher;
}

ScreenOrientationDispatcher::ScreenOrientationDispatcher()
{
}

DEFINE_TRACE(ScreenOrientationDispatcher)
{
    PlatformEventDispatcher::trace(visitor);
}

void ScreenOrientationDispatcher::startListening()
{
    Platform::current()->startListening(WebPlatformEventScreenOrientation, 0);
}

void ScreenOrientationDispatcher::stopListening()
{
    Platform::current()->stopListening(WebPlatformEventScreenOrientation);
}

} // namespace blink
