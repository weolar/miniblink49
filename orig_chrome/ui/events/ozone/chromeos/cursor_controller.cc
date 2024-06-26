// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/chromeos/cursor_controller.h"

namespace ui {

namespace {

    void TransformCursorMove(gfx::Display::Rotation rotation,
        float scale,
        gfx::Vector2dF* delta)
    {
        float dx;
        float dy;

        switch (rotation) {
        case gfx::Display::ROTATE_90:
            dx = -delta->y();
            dy = delta->x();
            break;
        case gfx::Display::ROTATE_180:
            dx = -delta->x();
            dy = -delta->y();
            break;
        case gfx::Display::ROTATE_270:
            dx = delta->y();
            dy = -delta->x();
            break;
        default: // gfx::Display::ROTATE_0
            dx = delta->x();
            dy = delta->y();
            break;
        }

        delta->set_x(dx * scale);
        delta->set_y(dy * scale);
    }

} // namespace

// static
CursorController* CursorController::GetInstance()
{
    return base::Singleton<CursorController>::get();
}

void CursorController::SetCursorConfigForWindow(gfx::AcceleratedWidget widget,
    gfx::Display::Rotation rotation,
    float scale)
{
    base::AutoLock lock(window_to_cursor_configuration_map_lock_);
    PerWindowCursorConfiguration config = { rotation, scale };
    window_to_cursor_configuration_map_[widget] = config;
}

void CursorController::ClearCursorConfigForWindow(
    gfx::AcceleratedWidget widget)
{
    window_to_cursor_configuration_map_.erase(widget);
}

void CursorController::ApplyCursorConfigForWindow(gfx::AcceleratedWidget widget,
    gfx::Vector2dF* delta) const
{
    base::AutoLock lock(window_to_cursor_configuration_map_lock_);
    auto it = window_to_cursor_configuration_map_.find(widget);
    if (it != window_to_cursor_configuration_map_.end())
        TransformCursorMove(it->second.rotation, it->second.scale, delta);
}

CursorController::CursorController()
{
}

CursorController::~CursorController()
{
}

} // namespace ui
