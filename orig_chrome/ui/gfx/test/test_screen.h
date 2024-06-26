// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/macros.h"
#include "ui/gfx/display.h"
#include "ui/gfx/screen.h"

namespace gfx {
namespace test {

    // A dummy implementation of gfx::Screen that contains a single gfx::Display
    // only. The contained gfx::Display can be accessed and modified via
    // TestScreen::display().
    //
    // NOTE: Adding and removing gfx::DisplayOberver's are no-ops and observers will
    // NOT be notified ever.
    class TestScreen : public gfx::Screen {
    public:
        TestScreen();
        ~TestScreen() override;

        gfx::Display* display() { return &display_; }

        // gfx::Screen:
        gfx::Point GetCursorScreenPoint() override;
        gfx::NativeWindow GetWindowUnderCursor() override;
        gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override;
        int GetNumDisplays() const override;
        std::vector<gfx::Display> GetAllDisplays() const override;
        gfx::Display GetDisplayNearestWindow(gfx::NativeView view) const override;
        gfx::Display GetDisplayNearestPoint(const gfx::Point& point) const override;
        gfx::Display GetDisplayMatching(const gfx::Rect& match_rect) const override;
        gfx::Display GetPrimaryDisplay() const override;
        void AddObserver(gfx::DisplayObserver* observer) override;
        void RemoveObserver(gfx::DisplayObserver* observer) override;

    private:
        // The only display.
        gfx::Display display_;

        DISALLOW_COPY_AND_ASSIGN(TestScreen);
    };

} // namespace test
} // namespace gfx
