// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_BASE_EVENT_UTILS_H_
#define UI_EVENTS_BASE_EVENT_UTILS_H_

#include <stdint.h>

#include "ui/events/events_base_export.h"

// Common functions to be used for all platforms.
namespace ui {

// Generate an unique identifier for events.
EVENTS_BASE_EXPORT uint32_t GetNextTouchEventId();

// Checks if |flags| contains system key modifiers.
EVENTS_BASE_EXPORT bool IsSystemKeyModifier(int flags);

} // namespace ui

#endif // UI_EVENTS_BASE_EVENT_UTILS_H_
