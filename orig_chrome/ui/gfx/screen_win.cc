// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/screen_win.h"

#include <windows.h>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/win/display_info.h"
#include "ui/gfx/win/dpi.h"
#include "ui/gfx/win/screen_win_display.h"

namespace {

std::vector<gfx::win::ScreenWinDisplay> DisplayInfosToScreenWinDisplays(
    const std::vector<gfx::win::DisplayInfo>& display_infos)
{
    std::vector<gfx::win::ScreenWinDisplay> screen_win_displays;
    for (const auto& display_info : display_infos)
        screen_win_displays.push_back(gfx::win::ScreenWinDisplay(display_info));

    return screen_win_displays;
}

std::vector<gfx::Display> ScreenWinDisplaysToDisplays(
    const std::vector<gfx::win::ScreenWinDisplay>& screen_win_displays)
{
    std::vector<gfx::Display> displays;
    for (const auto& screen_win_display : screen_win_displays)
        displays.push_back(screen_win_display.display());

    return displays;
}

MONITORINFOEX MonitorInfoFromHMONITOR(HMONITOR monitor)
{
    MONITORINFOEX monitor_info;
    ::ZeroMemory(&monitor_info, sizeof(monitor_info));
    monitor_info.cbSize = sizeof(monitor_info);
    ::GetMonitorInfo(monitor, &monitor_info);
    return monitor_info;
}

BOOL CALLBACK EnumMonitorCallback(HMONITOR monitor,
    HDC hdc,
    LPRECT rect,
    LPARAM data)
{
    std::vector<gfx::win::DisplayInfo>* display_infos = reinterpret_cast<std::vector<gfx::win::DisplayInfo>*>(data);
    DCHECK(display_infos);
    display_infos->push_back(
        gfx::win::DisplayInfo(MonitorInfoFromHMONITOR(monitor),
            gfx::GetDPIScale()));
    return TRUE;
}

std::vector<gfx::win::DisplayInfo> GetDisplayInfosFromSystem()
{
    std::vector<gfx::win::DisplayInfo> display_infos;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorCallback,
        reinterpret_cast<LPARAM>(&display_infos));
    DCHECK_EQ(static_cast<size_t>(::GetSystemMetrics(SM_CMONITORS)),
        display_infos.size());
    return display_infos;
}

} // namespace

namespace gfx {

ScreenWin::ScreenWin()
{
    Initialize();
}

ScreenWin::~ScreenWin() = default;

HWND ScreenWin::GetHWNDFromNativeView(NativeView window) const
{
    NOTREACHED();
    return nullptr;
}

NativeWindow ScreenWin::GetNativeWindowFromHWND(HWND hwnd) const
{
    NOTREACHED();
    return nullptr;
}

gfx::Point ScreenWin::GetCursorScreenPoint()
{
    POINT pt;
    ::GetCursorPos(&pt);
    gfx::Point cursor_pos_pixels(pt);
    return gfx::win::ScreenToDIPPoint(cursor_pos_pixels);
}

gfx::NativeWindow ScreenWin::GetWindowUnderCursor()
{
    POINT cursor_loc;
    HWND hwnd = ::GetCursorPos(&cursor_loc) ? ::WindowFromPoint(cursor_loc) : nullptr;
    return GetNativeWindowFromHWND(hwnd);
}

gfx::NativeWindow ScreenWin::GetWindowAtScreenPoint(const gfx::Point& point)
{
    gfx::Point point_in_pixels = gfx::win::DIPToScreenPoint(point);
    return GetNativeWindowFromHWND(WindowFromPoint(point_in_pixels.ToPOINT()));
}

int ScreenWin::GetNumDisplays() const
{
    return static_cast<int>(screen_win_displays_.size());
}

std::vector<gfx::Display> ScreenWin::GetAllDisplays() const
{
    return ScreenWinDisplaysToDisplays(screen_win_displays_);
}

gfx::Display ScreenWin::GetDisplayNearestWindow(gfx::NativeView window) const
{
    HWND window_hwnd = GetHWNDFromNativeView(window);
    if (!window_hwnd) {
        // When |window| isn't rooted to a display, we should just return the
        // default display so we get some correct display information like the
        // scaling factor.
        return GetPrimaryDisplay();
    }
    gfx::win::ScreenWinDisplay screen_win_display = GetScreenWinDisplayNearestHWND(window_hwnd);
    return screen_win_display.display();
}

gfx::Display ScreenWin::GetDisplayNearestPoint(const gfx::Point& point) const
{
    gfx::Point screen_point(gfx::win::DIPToScreenPoint(point));
    gfx::win::ScreenWinDisplay screen_win_display = GetScreenWinDisplayNearestScreenPoint(screen_point);
    return screen_win_display.display();
}

gfx::Display ScreenWin::GetDisplayMatching(const gfx::Rect& match_rect) const
{
    gfx::win::ScreenWinDisplay screen_win_display = GetScreenWinDisplayNearestScreenRect(match_rect);
    return screen_win_display.display();
}

gfx::Display ScreenWin::GetPrimaryDisplay() const
{
    return GetPrimaryScreenWinDisplay().display();
}

void ScreenWin::AddObserver(DisplayObserver* observer)
{
    change_notifier_.AddObserver(observer);
}

void ScreenWin::RemoveObserver(DisplayObserver* observer)
{
    change_notifier_.RemoveObserver(observer);
}

void ScreenWin::UpdateFromDisplayInfos(
    const std::vector<gfx::win::DisplayInfo>& display_infos)
{
    screen_win_displays_ = DisplayInfosToScreenWinDisplays(display_infos);
}

void ScreenWin::Initialize()
{
    singleton_hwnd_observer_.reset(
        new gfx::SingletonHwndObserver(
            base::Bind(&ScreenWin::OnWndProc, base::Unretained(this))));
    UpdateFromDisplayInfos(GetDisplayInfosFromSystem());
}

MONITORINFOEX ScreenWin::MonitorInfoFromScreenPoint(
    const gfx::Point& screen_point) const
{
    POINT initial_loc = { screen_point.x(), screen_point.y() };
    return MonitorInfoFromHMONITOR(::MonitorFromPoint(initial_loc,
        MONITOR_DEFAULTTONEAREST));
}

MONITORINFOEX ScreenWin::MonitorInfoFromScreenRect(const gfx::Rect& screen_rect)
    const
{
    RECT win_rect = screen_rect.ToRECT();
    return MonitorInfoFromHMONITOR(::MonitorFromRect(&win_rect,
        MONITOR_DEFAULTTONEAREST));
}

MONITORINFOEX ScreenWin::MonitorInfoFromWindow(HWND hwnd,
    DWORD default_options) const
{
    return MonitorInfoFromHMONITOR(::MonitorFromWindow(hwnd, default_options));
}

HWND ScreenWin::GetRootWindow(HWND hwnd) const
{
    return ::GetAncestor(hwnd, GA_ROOT);
}

void ScreenWin::OnWndProc(HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
    if (message != WM_DISPLAYCHANGE)
        return;

    std::vector<gfx::Display> old_displays = GetAllDisplays();
    UpdateFromDisplayInfos(GetDisplayInfosFromSystem());
    change_notifier_.NotifyDisplaysChanged(old_displays, GetAllDisplays());
}

gfx::win::ScreenWinDisplay ScreenWin::GetScreenWinDisplayNearestHWND(HWND hwnd)
    const
{
    return GetScreenWinDisplay(MonitorInfoFromWindow(hwnd,
        MONITOR_DEFAULTTONEAREST));
}

gfx::win::ScreenWinDisplay ScreenWin::GetScreenWinDisplayNearestScreenRect(
    const Rect& screen_rect) const
{
    return GetScreenWinDisplay(MonitorInfoFromScreenRect(screen_rect));
}

gfx::win::ScreenWinDisplay ScreenWin::GetScreenWinDisplayNearestScreenPoint(
    const Point& screen_point) const
{
    return GetScreenWinDisplay(MonitorInfoFromScreenPoint(screen_point));
}

gfx::win::ScreenWinDisplay ScreenWin::GetPrimaryScreenWinDisplay() const
{
    MONITORINFOEX monitor_info = MonitorInfoFromWindow(nullptr,
        MONITOR_DEFAULTTOPRIMARY);
    gfx::win::ScreenWinDisplay screen_win_display = GetScreenWinDisplay(monitor_info);
    gfx::Display display = screen_win_display.display();
    // The Windows primary monitor is defined to have an origin of (0, 0).
    DCHECK_EQ(0, display.bounds().origin().x());
    DCHECK_EQ(0, display.bounds().origin().y());
    return screen_win_display;
}

gfx::win::ScreenWinDisplay ScreenWin::GetScreenWinDisplay(
    const MONITORINFOEX& monitor_info) const
{
    int64_t id = gfx::win::DisplayInfo::DeviceIdFromDeviceName(monitor_info.szDevice);
    for (const auto& screen_win_display : screen_win_displays_) {
        if (screen_win_display.display().id() == id)
            return screen_win_display;
    }
    // There is 1:1 correspondence between MONITORINFOEX and ScreenWinDisplay.
    // If we make it here, it means we have no displays and we should hand out the
    // default display.
    DCHECK_EQ(screen_win_displays_.size(), 0u);
    return gfx::win::ScreenWinDisplay();
}

} // namespace gfx
