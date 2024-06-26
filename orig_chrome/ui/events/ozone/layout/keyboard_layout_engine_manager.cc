// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"

#include "base/logging.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"

namespace ui {

// static
KeyboardLayoutEngineManager* KeyboardLayoutEngineManager::instance_ = nullptr;

KeyboardLayoutEngineManager::KeyboardLayoutEngineManager(
    KeyboardLayoutEngine* engine)
    : keyboard_layout_engine_(engine)
{
    CHECK(!instance_) << "Only one keyboard layout manager can be created.";
    instance_ = this;
}

KeyboardLayoutEngineManager::~KeyboardLayoutEngineManager()
{
    CHECK_EQ(this, instance_);
    instance_ = NULL;
}

void KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
    scoped_ptr<KeyboardLayoutEngine> engine)
{
    if (instance_)
        instance_->keyboard_layout_engine_.reset(engine.release());
    else
        new KeyboardLayoutEngineManager(engine.release());
}

KeyboardLayoutEngine* KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()
{
    // TODO(kpschoedel): crbug.com/430194 This lazy initialization is a
    // workaround for not yet initializing KeyboardLayoutEngineManager
    // expliclity in all tests that need it.
    if (!instance_)
        new KeyboardLayoutEngineManager(new StubKeyboardLayoutEngine());
    return instance_->keyboard_layout_engine_.get();
}

} // namespace ui
