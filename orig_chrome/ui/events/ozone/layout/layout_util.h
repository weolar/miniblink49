// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_OZONE_LAYOUT_LAYOUT_UTIL_H_
#define UI_EVENTS_OZONE_LAYOUT_LAYOUT_UTIL_H_

// TODO(kpschoedel): consider moving this out of Ozone.

#include "base/strings/string16.h"
#include "ui/events/keycodes/dom/dom_key.h"
#include "ui/events/keycodes/keyboard_codes.h"
#include "ui/events/ozone/layout/events_ozone_layout_export.h"

namespace ui {

// Returns the ui::EventFlags value associated with a modifier key,
// or 0 (EF_NONE) if the key is not a modifier.
EVENTS_OZONE_LAYOUT_EXPORT int ModifierDomKeyToEventFlag(DomKey key);

} // namespace ui

#endif // UI_EVENTS_OZONE_LAYOUT_LAYOUT_UTIL_H_
