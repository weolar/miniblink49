// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/sys_color_change_listener.h"

#include <windows.h>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/singleton.h"
#include "base/observer_list.h"
#include "ui/gfx/color_utils.h"
#include "ui/gfx/win/singleton_hwnd_observer.h"

namespace {

bool g_is_inverted_color_scheme = false;
bool g_is_inverted_color_scheme_initialized = false;

void UpdateInvertedColorScheme()
{
    int foreground_luminance = color_utils::GetLuminanceForColor(
        color_utils::GetSysSkColor(COLOR_WINDOWTEXT));
    int background_luminance = color_utils::GetLuminanceForColor(
        color_utils::GetSysSkColor(COLOR_WINDOW));
    HIGHCONTRAST high_contrast = { 0 };
    high_contrast.cbSize = sizeof(HIGHCONTRAST);
    g_is_inverted_color_scheme = SystemParametersInfo(SPI_GETHIGHCONTRAST, 0, &high_contrast, 0) && ((high_contrast.dwFlags & HCF_HIGHCONTRASTON) != 0) && foreground_luminance > background_luminance;
    g_is_inverted_color_scheme_initialized = true;
}

} // namespace

namespace color_utils {

bool IsInvertedColorScheme()
{
    if (!g_is_inverted_color_scheme_initialized)
        UpdateInvertedColorScheme();
    return g_is_inverted_color_scheme;
}

} // namespace color_utils

namespace gfx {

class SysColorChangeObserver {
public:
    static SysColorChangeObserver* GetInstance();

    void AddListener(SysColorChangeListener* listener);
    void RemoveListener(SysColorChangeListener* listener);

private:
    friend struct base::DefaultSingletonTraits<SysColorChangeObserver>;

    SysColorChangeObserver();
    virtual ~SysColorChangeObserver();

    void OnWndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

    base::ObserverList<SysColorChangeListener> listeners_;
    scoped_ptr<gfx::SingletonHwndObserver> singleton_hwnd_observer_;
};

// static
SysColorChangeObserver* SysColorChangeObserver::GetInstance()
{
    return base::Singleton<SysColorChangeObserver>::get();
}

SysColorChangeObserver::SysColorChangeObserver()
    : singleton_hwnd_observer_(new SingletonHwndObserver(
        base::Bind(&SysColorChangeObserver::OnWndProc,
            base::Unretained(this))))
{
}

SysColorChangeObserver::~SysColorChangeObserver() { }

void SysColorChangeObserver::AddListener(SysColorChangeListener* listener)
{
    listeners_.AddObserver(listener);
}

void SysColorChangeObserver::RemoveListener(SysColorChangeListener* listener)
{
    listeners_.RemoveObserver(listener);
}

void SysColorChangeObserver::OnWndProc(HWND hwnd,
    UINT message,
    WPARAM wparam,
    LPARAM lparam)
{
    if (message == WM_SYSCOLORCHANGE || (message == WM_SETTINGCHANGE && wparam == SPI_SETHIGHCONTRAST)) {
        UpdateInvertedColorScheme();
        FOR_EACH_OBSERVER(SysColorChangeListener, listeners_, OnSysColorChange());
    }
}

ScopedSysColorChangeListener::ScopedSysColorChangeListener(
    SysColorChangeListener* listener)
    : listener_(listener)
{
    SysColorChangeObserver::GetInstance()->AddListener(listener_);
}

ScopedSysColorChangeListener::~ScopedSysColorChangeListener()
{
    SysColorChangeObserver::GetInstance()->RemoveListener(listener_);
}

} // namespace gfx
