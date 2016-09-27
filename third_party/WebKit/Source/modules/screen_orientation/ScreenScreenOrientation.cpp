// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/screen_orientation/ScreenScreenOrientation.h"

#include "bindings/core/v8/ScriptState.h"
#include "core/frame/Screen.h"
#include "modules/screen_orientation/ScreenOrientation.h"

namespace blink {

// static
ScreenScreenOrientation& ScreenScreenOrientation::from(Screen& screen)
{
    ScreenScreenOrientation* supplement = static_cast<ScreenScreenOrientation*>(HeapSupplement<Screen>::from(screen, supplementName()));
    if (!supplement) {
        supplement = new ScreenScreenOrientation();
        provideTo(screen, supplementName(), supplement);
    }
    return *supplement;
}

// static
ScreenOrientation* ScreenScreenOrientation::orientation(ScriptState* state, Screen& screen)
{
    ScreenScreenOrientation& self = ScreenScreenOrientation::from(screen);
    if (!screen.frame())
        return nullptr;

    if (!self.m_orientation)
        self.m_orientation = ScreenOrientation::create(screen.frame());

    return self.m_orientation;
}

const char* ScreenScreenOrientation::supplementName()
{
    return "ScreenScreenOrientation";
}

DEFINE_TRACE(ScreenScreenOrientation)
{
    visitor->trace(m_orientation);
    HeapSupplement<Screen>::trace(visitor);
}

} // namespace blink
