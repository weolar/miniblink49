// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/mouse_button_map_evdev.h"

#include <linux/input.h>

#include "base/logging.h"

namespace ui {

namespace {

    // Defines the range of button codes that we support.
    //
    // Check linux/input.h for more info.
    const MouseButtonMapEvdev::Button kMinMouseButtonCode = BTN_MISC;
    const MouseButtonMapEvdev::Button kMaxMouseButtonCode = BTN_GEAR_UP;

    bool IsMouseButton(const MouseButtonMapEvdev::Button button)
    {
        return (button >= kMinMouseButtonCode && button <= kMaxMouseButtonCode);
    }

} // namespace

MouseButtonMapEvdev::MouseButtonMapEvdev()
{
    ResetButtonMap();
}

MouseButtonMapEvdev::~MouseButtonMapEvdev()
{
}

void MouseButtonMapEvdev::UpdateButtonMap(Button button_from,
    Button button_to)
{
    DCHECK(IsMouseButton(button_from) && IsMouseButton(button_to));
    button_map_[button_from] = button_to;
}

void MouseButtonMapEvdev::ResetButtonMap()
{
    button_map_.clear();
    for (Button i = kMinMouseButtonCode; i <= kMaxMouseButtonCode; ++i)
        button_map_[i] = i;
}

int MouseButtonMapEvdev::GetMappedButton(const Button button) const
{
    ButtonMap::const_iterator it = button_map_.find(button);
    DCHECK(it != button_map_.end());
    return it->second;
}

} // namespace ui
