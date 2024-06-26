// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/color_profile.h"

#include <map>
#include <stddef.h>
#include <windows.h>

#include "base/files/file_util.h"
#include "base/lazy_instance.h"
#include "base/macros.h"
#include "base/synchronization/lock.h"

namespace gfx {

class ColorProfileCache {
public:
    // A thread-safe cache of color profiles keyed by windows device name.
    ColorProfileCache() { }

    bool Find(const std::wstring& device, std::vector<char>* profile)
    {
        base::AutoLock lock(lock_);
        DeviceColorProfile::const_iterator it = cache_.find(device);
        if (it == cache_.end())
            return false;
        *profile = it->second;
        return true;
    }

    void Insert(const std::wstring& device, const std::vector<char>& profile)
    {
        base::AutoLock lock(lock_);
        cache_[device] = profile;
    }

    bool Erase(const std::wstring& device)
    {
        base::AutoLock lock(lock_);
        DeviceColorProfile::iterator it = cache_.find(device);
        if (it == cache_.end())
            return false;
        cache_.erase(device);
        return true;
    }

    void Clear()
    {
        base::AutoLock lock(lock_);
        cache_.clear();
    }

private:
    typedef std::map<std::wstring, std::vector<char>> DeviceColorProfile;

    DeviceColorProfile cache_;
    base::Lock lock_;

    DISALLOW_COPY_AND_ASSIGN(ColorProfileCache);
};

base::LazyInstance<ColorProfileCache>::Leaky g_color_profile_cache = LAZY_INSTANCE_INITIALIZER;

inline ColorProfileCache& GetColorProfileCache()
{
    return g_color_profile_cache.Get();
}

bool GetDisplayColorProfile(const gfx::Rect& bounds,
    std::vector<char>* profile)
{
    DCHECK(profile->empty());

    RECT rect = bounds.ToRECT();
    HMONITOR handle = ::MonitorFromRect(&rect, MONITOR_DEFAULTTONULL);
    if (bounds.IsEmpty() || !handle)
        return false;

    MONITORINFOEX monitor;
    monitor.cbSize = sizeof(MONITORINFOEX);
    CHECK(::GetMonitorInfo(handle, &monitor));
    if (GetColorProfileCache().Find(monitor.szDevice, profile))
        return true;

    HDC hdc = ::CreateDC(monitor.szDevice, NULL, NULL, NULL);
    DWORD path_length = MAX_PATH;
    WCHAR path[MAX_PATH + 1];
    BOOL result = ::GetICMProfile(hdc, &path_length, path);
    ::DeleteDC(hdc);
    if (!result)
        return false;

    base::FilePath file_name = base::FilePath(path).BaseName();
    if (file_name != base::FilePath(L"sRGB Color Space Profile.icm")) {
        std::string data;
        if (base::ReadFileToString(base::FilePath(path), &data))
            profile->assign(data.data(), data.data() + data.size());
        size_t length = profile->size();
        if (gfx::InvalidColorProfileLength(length))
            profile->clear();
    }

    GetColorProfileCache().Insert(monitor.szDevice, *profile);
    return true;
}

void ReadColorProfile(std::vector<char>* profile)
{
    // TODO: support multiple monitors.
    HDC screen_dc = GetDC(NULL);
    DWORD path_len = MAX_PATH;
    WCHAR path[MAX_PATH + 1];

    BOOL result = GetICMProfile(screen_dc, &path_len, path);
    ReleaseDC(NULL, screen_dc);
    if (!result)
        return;
    std::string profileData;
    if (!base::ReadFileToString(base::FilePath(path), &profileData))
        return;
    size_t length = profileData.size();
    if (gfx::InvalidColorProfileLength(length))
        return;
    profile->assign(profileData.data(), profileData.data() + length);
}

} // namespace gfx
