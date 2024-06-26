// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/screen_win.h"

#include <inttypes.h>
#include <stddef.h>
#include <windows.h>

#include <cwchar>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "base/macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/display.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/screen.h"
#include "ui/gfx/test/display_util.h"
#include "ui/gfx/win/display_info.h"
#include "ui/gfx/win/dpi.h"
#include "ui/gfx/win/screen_win_display.h"

namespace gfx {

namespace {

    MONITORINFOEX CreateMonitorInfo(gfx::Rect monitor,
        gfx::Rect work,
        std::wstring device_name)
    {
        MONITORINFOEX monitor_info;
        ::ZeroMemory(&monitor_info, sizeof(monitor_info));
        monitor_info.cbSize = sizeof(monitor_info);
        monitor_info.rcMonitor = monitor.ToRECT();
        monitor_info.rcWork = work.ToRECT();
        size_t device_char_count = ARRAYSIZE(monitor_info.szDevice);
        wcsncpy(monitor_info.szDevice, device_name.c_str(), device_char_count);
        monitor_info.szDevice[device_char_count - 1] = L'\0';
        return monitor_info;
    }

    class TestScreenWin : public gfx::ScreenWin {
    public:
        TestScreenWin(const std::vector<gfx::win::DisplayInfo>& display_infos,
            const std::vector<MONITORINFOEX>& monitor_infos,
            const std::unordered_map<HWND, gfx::Rect>& hwnd_map)
            : monitor_infos_(monitor_infos)
            , hwnd_map_(hwnd_map)
        {
            UpdateFromDisplayInfos(display_infos);
        }

        ~TestScreenWin() override = default;

    protected:
        // gfx::ScreenWin:
        HWND GetHWNDFromNativeView(NativeView window) const override
        {
            // NativeView is only used as an identifier in this tests, so interchange
            // NativeView with an HWND for convenience.
            return reinterpret_cast<HWND>(window);
        }

        NativeWindow GetNativeWindowFromHWND(HWND hwnd) const override
        {
            // NativeWindow is only used as an identifier in this tests, so interchange
            // an HWND for a NativeWindow for convenience.
            return reinterpret_cast<NativeWindow>(hwnd);
        }

    private:
        void Initialize() override { }

        MONITORINFOEX MonitorInfoFromScreenPoint(const gfx::Point& screen_point) const
            override
        {
            for (const MONITORINFOEX& monitor_info : monitor_infos_) {
                if (gfx::Rect(monitor_info.rcMonitor).Contains(screen_point))
                    return monitor_info;
            }
            NOTREACHED();
            return monitor_infos_[0];
        }

        MONITORINFOEX MonitorInfoFromScreenRect(const gfx::Rect& screen_rect) const
            override
        {
            MONITORINFOEX candidate = monitor_infos_[0];
            int largest_area = 0;
            for (const MONITORINFOEX& monitor_info : monitor_infos_) {
                gfx::Rect bounds(monitor_info.rcMonitor);
                if (bounds.Intersects(screen_rect)) {
                    bounds.Intersect(screen_rect);
                    int area = bounds.height() * bounds.width();
                    if (largest_area < area) {
                        candidate = monitor_info;
                        largest_area = area;
                    }
                }
            }
            EXPECT_NE(largest_area, 0);
            return candidate;
        }

        MONITORINFOEX MonitorInfoFromWindow(HWND hwnd, DWORD default_options)
            const override
        {
            auto search = hwnd_map_.find(hwnd);
            if (search != hwnd_map_.end())
                return MonitorInfoFromScreenRect(search->second);

            EXPECT_EQ(default_options, static_cast<DWORD>(MONITOR_DEFAULTTOPRIMARY));
            for (const auto& monitor_info : monitor_infos_) {
                if (monitor_info.rcMonitor.left == 0 && monitor_info.rcMonitor.top == 0) {
                    return monitor_info;
                }
            }
            NOTREACHED();
            return monitor_infos_[0];
        }

        HWND GetRootWindow(HWND hwnd) const override
        {
            return hwnd;
        }

        std::vector<MONITORINFOEX> monitor_infos_;
        std::unordered_map<HWND, gfx::Rect> hwnd_map_;

        DISALLOW_COPY_AND_ASSIGN(TestScreenWin);
    };

    Screen* GetScreen()
    {
        return gfx::Screen::GetScreen();
    }

} // namespace

// Allows tests to specify the screen and associated state.
class TestScreenWinInitializer {
public:
    virtual void AddMonitor(const gfx::Rect& pixel_bounds,
        const gfx::Rect& pixel_work,
        const wchar_t* device_name,
        float device_scale_factor)
        = 0;

    virtual HWND CreateFakeHwnd(const gfx::Rect& bounds) = 0;
};

class TestScreenWinManager : public TestScreenWinInitializer {
public:
    TestScreenWinManager() = default;

    ~TestScreenWinManager()
    {
        gfx::Screen::SetScreenInstance(nullptr);
    }

    void AddMonitor(const gfx::Rect& pixel_bounds,
        const gfx::Rect& pixel_work,
        const wchar_t* device_name,
        float device_scale_factor) override
    {
        MONITORINFOEX monitor_info = CreateMonitorInfo(pixel_bounds,
            pixel_work,
            device_name);
        monitor_infos_.push_back(monitor_info);
        display_infos_.push_back(gfx::win::DisplayInfo(monitor_info,
            device_scale_factor,
            gfx::Display::ROTATE_0));
    }

    HWND CreateFakeHwnd(const gfx::Rect& bounds) override
    {
        EXPECT_EQ(screen_win_, nullptr);
        hwnd_map_.insert(std::pair<HWND, gfx::Rect>(++hwndLast_, bounds));
        return hwndLast_;
    }

    void InitializeScreenWin()
    {
        ASSERT_EQ(screen_win_, nullptr);
        screen_win_.reset(new TestScreenWin(display_infos_,
            monitor_infos_,
            hwnd_map_));
        gfx::Screen::SetScreenInstance(screen_win_.get());
    }

    ScreenWin* GetScreenWin()
    {
        return screen_win_.get();
    }

private:
    HWND hwndLast_ = nullptr;
    scoped_ptr<ScreenWin> screen_win_;
    std::vector<MONITORINFOEX> monitor_infos_;
    std::vector<gfx::win::DisplayInfo> display_infos_;
    std::unordered_map<HWND, gfx::Rect> hwnd_map_;

    DISALLOW_COPY_AND_ASSIGN(TestScreenWinManager);
};

class ScreenWinTest : public testing::Test {
protected:
    ScreenWinTest() = default;

    void SetUp() override
    {
        testing::Test::SetUp();
        gfx::SetDefaultDeviceScaleFactor(1.0);
        screen_win_initializer_.reset(new TestScreenWinManager());
        SetUpScreen(screen_win_initializer_.get());
        screen_win_initializer_->InitializeScreenWin();
    }

    void TearDown() override
    {
        screen_win_initializer_.reset();
        gfx::SetDefaultDeviceScaleFactor(1.0);
        testing::Test::TearDown();
    }

    virtual void SetUpScreen(TestScreenWinInitializer* initializer) = 0;

    NativeWindow GetNativeWindowFromHWND(HWND hwnd) const
    {
        ScreenWin* screen_win = screen_win_initializer_->GetScreenWin();
        return screen_win->GetNativeWindowFromHWND(hwnd);
        ;
    }

private:
    scoped_ptr<TestScreenWinManager> screen_win_initializer_;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTest);
};

// Single Display of 1.0 Device Scale Factor.
class ScreenWinTestSingleDisplay1x : public ScreenWinTest {
public:
    ScreenWinTestSingleDisplay1x() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        initializer->AddMonitor(gfx::Rect(0, 0, 1920, 1200),
            gfx::Rect(0, 0, 1920, 1100),
            L"primary",
            1.0);
        fake_hwnd_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 1920, 1100));
    }

    HWND GetFakeHwnd()
    {
        return fake_hwnd_;
    }

private:
    HWND fake_hwnd_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestSingleDisplay1x);
};

TEST_F(ScreenWinTestSingleDisplay1x, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(1u, displays.size());
    EXPECT_EQ(gfx::Rect(0, 0, 1920, 1200), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 1920, 1100), displays[0].work_area());
}

TEST_F(ScreenWinTestSingleDisplay1x, GetNumDisplays)
{
    EXPECT_EQ(1, GetScreen()->GetNumDisplays());
}

TEST_F(ScreenWinTestSingleDisplay1x, GetDisplayNearestWindowPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(screen->GetPrimaryDisplay(),
        screen->GetDisplayNearestWindow(nullptr));
}

TEST_F(ScreenWinTestSingleDisplay1x, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    gfx::NativeWindow native_window = GetNativeWindowFromHWND(GetFakeHwnd());
    EXPECT_EQ(screen->GetAllDisplays()[0],
        screen->GetDisplayNearestWindow(native_window));
}

TEST_F(ScreenWinTestSingleDisplay1x, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(250, 952)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(1919, 1199)));
}

TEST_F(ScreenWinTestSingleDisplay1x, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));
}

TEST_F(ScreenWinTestSingleDisplay1x, GetPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(gfx::Point(0, 0), screen->GetPrimaryDisplay().bounds().origin());
}

// Single Display of 1.25 Device Scale Factor.
class ScreenWinTestSingleDisplay1_25x : public ScreenWinTest {
public:
    ScreenWinTestSingleDisplay1_25x() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        gfx::SetDefaultDeviceScaleFactor(1.25);
        // Add Monitor of Scale Factor 1.0 since gfx::GetDPIScale performs the
        // clamping and not ScreenWin.
        initializer->AddMonitor(gfx::Rect(0, 0, 1920, 1200),
            gfx::Rect(0, 0, 1920, 1100),
            L"primary",
            1.0);
        fake_hwnd_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 1920, 1100));
    }

    HWND GetFakeHwnd()
    {
        return fake_hwnd_;
    }

private:
    HWND fake_hwnd_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestSingleDisplay1_25x);
};

TEST_F(ScreenWinTestSingleDisplay1_25x, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(1u, displays.size());
    // On Windows, scale factors of 1.25 or lower are clamped to 1.0.
    EXPECT_EQ(gfx::Rect(0, 0, 1920, 1200), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 1920, 1100), displays[0].work_area());
}

TEST_F(ScreenWinTestSingleDisplay1_25x, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    gfx::NativeWindow native_window = GetNativeWindowFromHWND(GetFakeHwnd());
    EXPECT_EQ(screen->GetAllDisplays()[0],
        screen->GetDisplayNearestWindow(native_window));
}

TEST_F(ScreenWinTestSingleDisplay1_25x, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(250, 952)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(1919, 1199)));
}

TEST_F(ScreenWinTestSingleDisplay1_25x, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));
}
TEST_F(ScreenWinTestSingleDisplay1_25x, GetPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(gfx::Point(0, 0), screen->GetPrimaryDisplay().bounds().origin());
}

// Single Display of 1.25 Device Scale Factor.
class ScreenWinTestSingleDisplay1_5x : public ScreenWinTest {
public:
    ScreenWinTestSingleDisplay1_5x() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        gfx::SetDefaultDeviceScaleFactor(1.5);
        initializer->AddMonitor(gfx::Rect(0, 0, 1920, 1200),
            gfx::Rect(0, 0, 1920, 1100),
            L"primary",
            1.5);
        fake_hwnd_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 1920, 1100));
    }

    HWND GetFakeHwnd()
    {
        return fake_hwnd_;
    }

private:
    HWND fake_hwnd_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestSingleDisplay1_5x);
};

TEST_F(ScreenWinTestSingleDisplay1_5x, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(1u, displays.size());
    EXPECT_EQ(gfx::Rect(0, 0, 1280, 800), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 1280, 734), displays[0].work_area());
}

TEST_F(ScreenWinTestSingleDisplay1_5x, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    gfx::NativeWindow native_window = GetNativeWindowFromHWND(GetFakeHwnd());
    EXPECT_EQ(screen->GetAllDisplays()[0],
        screen->GetDisplayNearestWindow(native_window));
}

TEST_F(ScreenWinTestSingleDisplay1_5x, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(250, 524)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(1279, 733)));
}

TEST_F(ScreenWinTestSingleDisplay1_5x, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));
}
TEST_F(ScreenWinTestSingleDisplay1_5x, GetPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(gfx::Point(0, 0), screen->GetPrimaryDisplay().bounds().origin());
}

// Single Display of 2.0 Device Scale Factor.
class ScreenWinTestSingleDisplay2x : public ScreenWinTest {
public:
    ScreenWinTestSingleDisplay2x() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        gfx::SetDefaultDeviceScaleFactor(2.0);
        initializer->AddMonitor(gfx::Rect(0, 0, 1920, 1200),
            gfx::Rect(0, 0, 1920, 1100),
            L"primary",
            2.0);
        fake_hwnd_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 1920, 1100));
    }

    HWND GetFakeHwnd()
    {
        return fake_hwnd_;
    }

private:
    HWND fake_hwnd_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestSingleDisplay2x);
};

TEST_F(ScreenWinTestSingleDisplay2x, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(1u, displays.size());
    EXPECT_EQ(gfx::Rect(0, 0, 960, 600), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 960, 550), displays[0].work_area());
}

TEST_F(ScreenWinTestSingleDisplay2x, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    gfx::NativeWindow native_window = GetNativeWindowFromHWND(GetFakeHwnd());
    EXPECT_EQ(screen->GetAllDisplays()[0],
        screen->GetDisplayNearestWindow(native_window));
}

TEST_F(ScreenWinTestSingleDisplay2x, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(125, 476)));
    EXPECT_EQ(display, screen->GetDisplayNearestPoint(gfx::Point(959, 599)));
}

TEST_F(ScreenWinTestSingleDisplay2x, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display display = screen->GetAllDisplays()[0];
    EXPECT_EQ(display, screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));
}

// Two Displays of 1.0 Device Scale Factor.
class ScreenWinTestTwoDisplays1x : public ScreenWinTest {
public:
    ScreenWinTestTwoDisplays1x() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        initializer->AddMonitor(gfx::Rect(0, 0, 1920, 1200),
            gfx::Rect(0, 0, 1920, 1100),
            L"primary",
            1.0);
        initializer->AddMonitor(gfx::Rect(1920, 0, 800, 600),
            gfx::Rect(1920, 0, 800, 600),
            L"secondary",
            1.0);
        fake_hwnd_left_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 1920, 1100));
        fake_hwnd_right_ = initializer->CreateFakeHwnd(gfx::Rect(1920, 0, 800, 600));
    }

    HWND GetLeftFakeHwnd()
    {
        return fake_hwnd_left_;
    }

    HWND GetRightFakeHwnd()
    {
        return fake_hwnd_right_;
    }

private:
    HWND fake_hwnd_left_ = nullptr;
    HWND fake_hwnd_right_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestTwoDisplays1x);
};

TEST_F(ScreenWinTestTwoDisplays1x, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(2u, displays.size());
    EXPECT_EQ(gfx::Rect(0, 0, 1920, 1200), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 1920, 1100), displays[0].work_area());
    EXPECT_EQ(gfx::Rect(1920, 0, 800, 600), displays[1].bounds());
    EXPECT_EQ(gfx::Rect(1920, 0, 800, 600), displays[1].work_area());
}

TEST_F(ScreenWinTestTwoDisplays1x, GetNumDisplays)
{
    EXPECT_EQ(2, GetScreen()->GetNumDisplays());
}

TEST_F(ScreenWinTestTwoDisplays1x, GetDisplayNearestWindowPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(screen->GetPrimaryDisplay(),
        screen->GetDisplayNearestWindow(nullptr));
}

TEST_F(ScreenWinTestTwoDisplays1x, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    gfx::NativeWindow left_window = GetNativeWindowFromHWND(GetLeftFakeHwnd());
    EXPECT_EQ(left_display, screen->GetDisplayNearestWindow(left_window));

    gfx::NativeWindow right_window = GetNativeWindowFromHWND(GetRightFakeHwnd());
    EXPECT_EQ(right_display, screen->GetDisplayNearestWindow(right_window));
}

TEST_F(ScreenWinTestTwoDisplays1x, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    EXPECT_EQ(left_display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(left_display, screen->GetDisplayNearestPoint(gfx::Point(250, 952)));
    EXPECT_EQ(left_display,
        screen->GetDisplayNearestPoint(gfx::Point(1919, 1199)));

    EXPECT_EQ(right_display, screen->GetDisplayNearestPoint(gfx::Point(1920, 0)));
    EXPECT_EQ(right_display,
        screen->GetDisplayNearestPoint(gfx::Point(2000, 400)));
    EXPECT_EQ(right_display,
        screen->GetDisplayNearestPoint(gfx::Point(2719, 599)));
}

TEST_F(ScreenWinTestTwoDisplays1x, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    EXPECT_EQ(left_display,
        screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(left_display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));

    EXPECT_EQ(right_display,
        screen->GetDisplayMatching(gfx::Rect(1920, 0, 100, 100)));
    EXPECT_EQ(right_display,
        screen->GetDisplayMatching(gfx::Rect(2619, 499, 100, 100)));
}

TEST_F(ScreenWinTestTwoDisplays1x, GetPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display primary = screen->GetPrimaryDisplay();
    EXPECT_EQ(gfx::Point(0, 0), primary.bounds().origin());
}

// Two Displays of 2.0 Device Scale Factor.
class ScreenWinTestTwoDisplays2x : public ScreenWinTest {
public:
    ScreenWinTestTwoDisplays2x() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        gfx::SetDefaultDeviceScaleFactor(2.0);
        initializer->AddMonitor(gfx::Rect(0, 0, 1920, 1200),
            gfx::Rect(0, 0, 1920, 1100),
            L"primary",
            2.0);
        initializer->AddMonitor(gfx::Rect(1920, 0, 800, 600),
            gfx::Rect(1920, 0, 800, 600),
            L"secondary",
            2.0);
        fake_hwnd_left_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 1920, 1100));
        fake_hwnd_right_ = initializer->CreateFakeHwnd(gfx::Rect(1920, 0, 800, 600));
    }

    HWND GetLeftFakeHwnd()
    {
        return fake_hwnd_left_;
    }

    HWND GetRightFakeHwnd()
    {
        return fake_hwnd_right_;
    }

private:
    HWND fake_hwnd_left_ = nullptr;
    HWND fake_hwnd_right_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestTwoDisplays2x);
};

TEST_F(ScreenWinTestTwoDisplays2x, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(2u, displays.size());
    EXPECT_EQ(gfx::Rect(0, 0, 960, 600), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 960, 550), displays[0].work_area());
    EXPECT_EQ(gfx::Rect(960, 0, 400, 300), displays[1].bounds());
    EXPECT_EQ(gfx::Rect(960, 0, 400, 300), displays[1].work_area());
}

TEST_F(ScreenWinTestTwoDisplays2x, GetDisplayNearestWindowPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(screen->GetPrimaryDisplay(),
        screen->GetDisplayNearestWindow(nullptr));
}

TEST_F(ScreenWinTestTwoDisplays2x, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    gfx::NativeWindow left_window = GetNativeWindowFromHWND(GetLeftFakeHwnd());
    EXPECT_EQ(left_display, screen->GetDisplayNearestWindow(left_window));

    gfx::NativeWindow right_window = GetNativeWindowFromHWND(GetRightFakeHwnd());
    EXPECT_EQ(right_display, screen->GetDisplayNearestWindow(right_window));
}

TEST_F(ScreenWinTestTwoDisplays2x, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    EXPECT_EQ(left_display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(left_display, screen->GetDisplayNearestPoint(gfx::Point(125, 476)));
    EXPECT_EQ(left_display,
        screen->GetDisplayNearestPoint(gfx::Point(959, 599)));

    EXPECT_EQ(right_display, screen->GetDisplayNearestPoint(gfx::Point(960, 0)));
    EXPECT_EQ(right_display,
        screen->GetDisplayNearestPoint(gfx::Point(1000, 200)));
    EXPECT_EQ(right_display,
        screen->GetDisplayNearestPoint(gfx::Point(1359, 299)));
}

TEST_F(ScreenWinTestTwoDisplays2x, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    EXPECT_EQ(left_display,
        screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(left_display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));

    EXPECT_EQ(right_display,
        screen->GetDisplayMatching(gfx::Rect(1920, 0, 100, 100)));
    EXPECT_EQ(right_display,
        screen->GetDisplayMatching(gfx::Rect(2619, 499, 100, 100)));
}

TEST_F(ScreenWinTestTwoDisplays2x, GetPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display primary = screen->GetPrimaryDisplay();
    EXPECT_EQ(gfx::Point(0, 0), primary.bounds().origin());
}

// Two Displays of 2.0 (Left) and 1.0 (Right) Device Scale Factor under
// Windows DPI Virtualization. Note that the displays do not form a euclidean
// space.
class ScreenWinTestTwoDisplays2x1xVirtualized : public ScreenWinTest {
public:
    ScreenWinTestTwoDisplays2x1xVirtualized() = default;

    void SetUpScreen(TestScreenWinInitializer* initializer) override
    {
        gfx::SetDefaultDeviceScaleFactor(2.0);
        initializer->AddMonitor(gfx::Rect(0, 0, 3200, 1600),
            gfx::Rect(0, 0, 3200, 1500),
            L"primary",
            2.0);
        initializer->AddMonitor(gfx::Rect(6400, 0, 3840, 2400),
            gfx::Rect(6400, 0, 3840, 2400),
            L"secondary",
            2.0);
        fake_hwnd_left_ = initializer->CreateFakeHwnd(gfx::Rect(0, 0, 3200, 1500));
        fake_hwnd_right_ = initializer->CreateFakeHwnd(gfx::Rect(6400, 0, 3840, 2400));
    }

    HWND GetLeftFakeHwnd()
    {
        return fake_hwnd_left_;
    }

    HWND GetRightFakeHwnd()
    {
        return fake_hwnd_right_;
    }

private:
    HWND fake_hwnd_left_ = nullptr;
    HWND fake_hwnd_right_ = nullptr;

    DISALLOW_COPY_AND_ASSIGN(ScreenWinTestTwoDisplays2x1xVirtualized);
};

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized, GetDisplays)
{
    std::vector<gfx::Display> displays = GetScreen()->GetAllDisplays();
    ASSERT_EQ(2u, displays.size());
    EXPECT_EQ(gfx::Rect(0, 0, 1600, 800), displays[0].bounds());
    EXPECT_EQ(gfx::Rect(0, 0, 1600, 750), displays[0].work_area());
    EXPECT_EQ(gfx::Rect(3200, 0, 1920, 1200), displays[1].bounds());
    EXPECT_EQ(gfx::Rect(3200, 0, 1920, 1200), displays[1].work_area());
}

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized, GetNumDisplays)
{
    EXPECT_EQ(2, GetScreen()->GetNumDisplays());
}

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized,
    GetDisplayNearestWindowPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    EXPECT_EQ(screen->GetPrimaryDisplay(),
        screen->GetDisplayNearestWindow(nullptr));
}

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized, GetDisplayNearestWindow)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    gfx::NativeWindow left_window = GetNativeWindowFromHWND(GetLeftFakeHwnd());
    EXPECT_EQ(left_display, screen->GetDisplayNearestWindow(left_window));

    gfx::NativeWindow right_window = GetNativeWindowFromHWND(GetRightFakeHwnd());
    EXPECT_EQ(right_display, screen->GetDisplayNearestWindow(right_window));
}

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized, GetDisplayNearestPoint)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    EXPECT_EQ(left_display, screen->GetDisplayNearestPoint(gfx::Point(0, 0)));
    EXPECT_EQ(left_display, screen->GetDisplayNearestPoint(gfx::Point(125, 476)));
    EXPECT_EQ(left_display,
        screen->GetDisplayNearestPoint(gfx::Point(1599, 799)));

    EXPECT_EQ(right_display, screen->GetDisplayNearestPoint(gfx::Point(3200, 0)));
    EXPECT_EQ(right_display,
        screen->GetDisplayNearestPoint(gfx::Point(4000, 400)));
    EXPECT_EQ(right_display,
        screen->GetDisplayNearestPoint(gfx::Point(5119, 1199)));
}

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized, GetDisplayMatching)
{
    gfx::Screen* screen = GetScreen();
    const gfx::Display left_display = screen->GetAllDisplays()[0];
    const gfx::Display right_display = screen->GetAllDisplays()[1];

    EXPECT_EQ(left_display,
        screen->GetDisplayMatching(gfx::Rect(0, 0, 100, 100)));
    EXPECT_EQ(left_display,
        screen->GetDisplayMatching(gfx::Rect(1819, 1099, 100, 100)));

    EXPECT_EQ(right_display,
        screen->GetDisplayMatching(gfx::Rect(6400, 0, 100, 100)));
    EXPECT_EQ(right_display,
        screen->GetDisplayMatching(gfx::Rect(10139, 2299, 100, 100)));
}

TEST_F(ScreenWinTestTwoDisplays2x1xVirtualized, GetPrimaryDisplay)
{
    gfx::Screen* screen = GetScreen();
    gfx::Display primary = screen->GetPrimaryDisplay();
    EXPECT_EQ(gfx::Point(0, 0), primary.bounds().origin());
}

} // namespace gfx
