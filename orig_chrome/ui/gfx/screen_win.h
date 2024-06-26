// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GFX_SCREEN_WIN_H_
#define UI_GFX_SCREEN_WIN_H_

#include <windows.h>

#include <vector>

#include "base/macros.h"
#include "ui/gfx/display_change_notifier.h"
#include "ui/gfx/gfx_export.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/win/singleton_hwnd_observer.h"

namespace gfx {

class Display;
class Point;
class Rect;

namespace win {

    class DisplayInfo;
    class ScreenWinDisplay;

} // namespace win

class GFX_EXPORT ScreenWin : public Screen {
public:
    ScreenWin();
    ~ScreenWin() override;

    // Returns the HWND associated with the NativeView.
    virtual HWND GetHWNDFromNativeView(NativeView window) const;

    // Returns the NativeView associated with the HWND.
    virtual NativeWindow GetNativeWindowFromHWND(HWND hwnd) const;

protected:
    // gfx::Screen:
    gfx::Point GetCursorScreenPoint() override;
    gfx::NativeWindow GetWindowUnderCursor() override;
    gfx::NativeWindow GetWindowAtScreenPoint(const gfx::Point& point) override;
    int GetNumDisplays() const override;
    std::vector<gfx::Display> GetAllDisplays() const override;
    gfx::Display GetDisplayNearestWindow(gfx::NativeView window) const override;
    gfx::Display GetDisplayNearestPoint(const gfx::Point& point) const override;
    gfx::Display GetDisplayMatching(const gfx::Rect& match_rect) const override;
    gfx::Display GetPrimaryDisplay() const override;
    void AddObserver(DisplayObserver* observer) override;
    void RemoveObserver(DisplayObserver* observer) override;

    void UpdateFromDisplayInfos(
        const std::vector<gfx::win::DisplayInfo>& display_infos);

    // Virtual to support mocking by unit tests.
    virtual void Initialize();
    virtual MONITORINFOEX MonitorInfoFromScreenPoint(
        const gfx::Point& screen_point) const;
    virtual MONITORINFOEX MonitorInfoFromScreenRect(const gfx::Rect& screen_rect)
        const;
    virtual MONITORINFOEX MonitorInfoFromWindow(HWND hwnd, DWORD default_options)
        const;
    virtual HWND GetRootWindow(HWND hwnd) const;

private:
    void OnWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    // Returns the ScreenWinDisplay closest to or enclosing |hwnd|.
    gfx::win::ScreenWinDisplay GetScreenWinDisplayNearestHWND(HWND hwnd) const;

    // Returns the ScreenWinDisplay closest to or enclosing |screen_rect|.
    gfx::win::ScreenWinDisplay GetScreenWinDisplayNearestScreenRect(
        const Rect& screen_rect) const;

    // Returns the ScreenWinDisplay closest to or enclosing |screen_point|.
    gfx::win::ScreenWinDisplay GetScreenWinDisplayNearestScreenPoint(
        const Point& screen_point) const;

    // Returns the ScreenWinDisplay corresponding to the primary monitor.
    gfx::win::ScreenWinDisplay GetPrimaryScreenWinDisplay() const;

    gfx::win::ScreenWinDisplay GetScreenWinDisplay(
        const MONITORINFOEX& monitor_info) const;

    // Helper implementing the DisplayObserver handling.
    gfx::DisplayChangeNotifier change_notifier_;

    scoped_ptr<SingletonHwndObserver> singleton_hwnd_observer_;

    // Current list of ScreenWinDisplays.
    std::vector<gfx::win::ScreenWinDisplay> screen_win_displays_;

    DISALLOW_COPY_AND_ASSIGN(ScreenWin);
};

} // namespace gfx

#endif // UI_GFX_SCREEN_WIN_H_
