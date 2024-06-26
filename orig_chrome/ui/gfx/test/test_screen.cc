// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "ui/gfx/test/test_screen.h"

namespace gfx {
namespace test {

    TestScreen::TestScreen()
    {
    }

    TestScreen::~TestScreen()
    {
    }

    gfx::Point TestScreen::GetCursorScreenPoint()
    {
        return gfx::Point();
    }

    gfx::NativeWindow TestScreen::GetWindowUnderCursor()
    {
        return nullptr;
    }

    gfx::NativeWindow TestScreen::GetWindowAtScreenPoint(const gfx::Point& point)
    {
        return nullptr;
    }

    int TestScreen::GetNumDisplays() const
    {
        return 1;
    }

    std::vector<gfx::Display> TestScreen::GetAllDisplays() const
    {
        return std::vector<gfx::Display>(1, display_);
    }

    gfx::Display TestScreen::GetDisplayNearestWindow(gfx::NativeView view) const
    {
        return display_;
    }

    gfx::Display TestScreen::GetDisplayNearestPoint(const gfx::Point& point) const
    {
        return display_;
    }

    gfx::Display TestScreen::GetDisplayMatching(const gfx::Rect& match_rect) const
    {
        return display_;
    }

    gfx::Display TestScreen::GetPrimaryDisplay() const
    {
        return display_;
    }

    void TestScreen::AddObserver(gfx::DisplayObserver* observer)
    {
    }

    void TestScreen::RemoveObserver(gfx::DisplayObserver* observer)
    {
    }

} // namespace test
} // namespace gfx
