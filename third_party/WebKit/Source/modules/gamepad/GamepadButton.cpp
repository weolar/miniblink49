// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/gamepad/Gamepad.h"

namespace blink {

GamepadButton* GamepadButton::create()
{
    return new GamepadButton();
}

GamepadButton::GamepadButton()
    : m_value(0.)
    , m_pressed(false)
{
}

} // namespace blink
