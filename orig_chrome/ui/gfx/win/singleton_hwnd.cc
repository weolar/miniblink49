// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/win/singleton_hwnd.h"

#include "base/memory/singleton.h"
#include "base/message_loop/message_loop.h"
#include "ui/gfx/win/singleton_hwnd_observer.h"

namespace gfx {

// static
SingletonHwnd* SingletonHwnd::GetInstance()
{
    return base::Singleton<SingletonHwnd>::get();
}

BOOL SingletonHwnd::ProcessWindowMessage(HWND window,
    UINT message,
    WPARAM wparam,
    LPARAM lparam,
    LRESULT& result,
    DWORD msg_map_id)
{
    FOR_EACH_OBSERVER(SingletonHwndObserver,
        observer_list_,
        OnWndProc(window, message, wparam, lparam));
    return false;
}

SingletonHwnd::SingletonHwnd()
{
    if (!base::MessageLoopForUI::IsCurrent()) {
        // Creating this window in (e.g.) a renderer inhibits shutdown on
        // Windows. See http://crbug.com/230122 and http://crbug.com/236039.
        DLOG(ERROR) << "Cannot create windows on non-UI thread!";
        return;
    }
    WindowImpl::Init(NULL, Rect());
}

SingletonHwnd::~SingletonHwnd()
{
    // WindowImpl will clean up the hwnd value on WM_NCDESTROY.
    if (hwnd())
        DestroyWindow(hwnd());

    // Tell all of our current observers to clean themselves up.
    FOR_EACH_OBSERVER(SingletonHwndObserver, observer_list_, ClearWndProc());
}

void SingletonHwnd::AddObserver(SingletonHwndObserver* observer)
{
    observer_list_.AddObserver(observer);
}

void SingletonHwnd::RemoveObserver(SingletonHwndObserver* observer)
{
    observer_list_.RemoveObserver(observer);
}

} // namespace gfx
