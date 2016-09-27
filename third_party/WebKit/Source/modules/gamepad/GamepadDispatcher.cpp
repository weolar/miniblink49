// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/gamepad/GamepadDispatcher.h"

#include "modules/gamepad/NavigatorGamepad.h"
#include "public/platform/Platform.h"
#include "wtf/TemporaryChange.h"

namespace blink {

GamepadDispatcher& GamepadDispatcher::instance()
{
    DEFINE_STATIC_LOCAL(Persistent<GamepadDispatcher>, gamepadDispatcher, (new GamepadDispatcher()));
    return *gamepadDispatcher;
}

void GamepadDispatcher::sampleGamepads(WebGamepads& gamepads)
{
    Platform::current()->sampleGamepads(gamepads);
}

GamepadDispatcher::GamepadDispatcher()
{
}

GamepadDispatcher::~GamepadDispatcher()
{
}

DEFINE_TRACE(GamepadDispatcher)
{
    PlatformEventDispatcher::trace(visitor);
}

void GamepadDispatcher::didConnectGamepad(unsigned index, const WebGamepad& gamepad)
{
    dispatchDidConnectOrDisconnectGamepad(index, gamepad, true);
}

void GamepadDispatcher::didDisconnectGamepad(unsigned index, const WebGamepad& gamepad)
{
    dispatchDidConnectOrDisconnectGamepad(index, gamepad, false);
}

void GamepadDispatcher::dispatchDidConnectOrDisconnectGamepad(unsigned index, const WebGamepad& gamepad, bool connected)
{
    ASSERT(index < WebGamepads::itemsLengthCap);
    ASSERT(connected == gamepad.connected);

    m_latestChange.pad = gamepad;
    m_latestChange.index = index;
    notifyControllers();
}

void GamepadDispatcher::startListening()
{
    Platform::current()->startListening(WebPlatformEventGamepad, this);
}

void GamepadDispatcher::stopListening()
{
    Platform::current()->stopListening(WebPlatformEventGamepad);
}

} // namespace blink
