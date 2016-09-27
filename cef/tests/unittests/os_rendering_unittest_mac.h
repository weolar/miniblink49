// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license
// that can be found in the LICENSE file.

#ifndef CEF_TESTS_UNITTESTS_OS_RENDERING_UNITTEST_MAC_H_
#define CEF_TESTS_UNITTESTS_OS_RENDERING_UNITTEST_MAC_H_

#include "include/cef_base.h"
#include "ui/events/keycodes/keyboard_codes.h"

namespace osr_unittests {

CefWindowHandle GetFakeView();
void GetKeyEvent(CefKeyEvent& event, ui::KeyboardCode keyCode, int modifiers);

}  // namespace osr_unittests

#endif
