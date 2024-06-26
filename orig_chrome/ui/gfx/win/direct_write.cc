// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/win/direct_write.h"

#include "base/command_line.h"
#include "base/metrics/field_trial.h"
#include "base/win/registry.h"
#include "base/win/scoped_comptr.h"
#include "base/win/windows_version.h"
#include "skia/ext/fontmgr_default_win.h"
#include "third_party/skia/include/ports/SkTypeface_win.h"
#include "ui/gfx/platform_font_win.h"
#include "ui/gfx/switches.h"

namespace gfx {
namespace win {

    namespace {

        static bool dwrite_enabled = false;

    }

    bool ShouldUseDirectWrite()
    {
        // If the flag is currently on, and we're on Win7 or above, we enable
        // DirectWrite. Skia does not require the additions to DirectWrite in QFE
        // 2670838, but a simple 'better than XP' check is not enough.
        if (base::win::GetVersion() < base::win::VERSION_WIN7)
            return false;

        base::win::OSInfo::VersionNumber os_version = base::win::OSInfo::GetInstance()->version_number();
        if ((os_version.major == 6) && (os_version.minor == 1)) {
            // We can't use DirectWrite for pre-release versions of Windows 7.
            if (os_version.build < 7600)
                return false;
        }
        // If forced off, don't use it.
        const base::CommandLine& command_line = *base::CommandLine::ForCurrentProcess();
        return !command_line.HasSwitch(switches::kDisableDirectWrite);
    }

    void CreateDWriteFactory(IDWriteFactory** factory)
    {
        if (!ShouldUseDirectWrite() || base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kDisableDirectWriteForUI)) {
            return;
        }

        using DWriteCreateFactoryProc = decltype(DWriteCreateFactory)*;
        HMODULE dwrite_dll = LoadLibraryW(L"dwrite.dll");
        if (!dwrite_dll)
            return;

        DWriteCreateFactoryProc dwrite_create_factory_proc = reinterpret_cast<DWriteCreateFactoryProc>(
            GetProcAddress(dwrite_dll, "DWriteCreateFactory"));
        // Not finding the DWriteCreateFactory function indicates a corrupt dll.
        if (!dwrite_create_factory_proc)
            return;

        // Failure to create the DirectWrite factory indicates a corrupt dll.
        base::win::ScopedComPtr<IUnknown> factory_unknown;
        if (FAILED(dwrite_create_factory_proc(DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory),
                factory_unknown.Receive()))) {
            return;
        }
        factory_unknown.QueryInterface<IDWriteFactory>(factory);
    }

    void MaybeInitializeDirectWrite()
    {
        static bool tried_dwrite_initialize = false;
        if (tried_dwrite_initialize)
            return;
        tried_dwrite_initialize = true;

        base::win::ScopedComPtr<IDWriteFactory> factory;
        CreateDWriteFactory(factory.Receive());

        if (factory == nullptr)
            return;

        // The skia call to create a new DirectWrite font manager instance can fail
        // if we are unable to get the system font collection from the DirectWrite
        // factory. The GetSystemFontCollection method in the IDWriteFactory
        // interface fails with E_INVALIDARG on certain Windows 7 gold versions
        // (6.1.7600.*). We should just use GDI in these cases.
        SkFontMgr* direct_write_font_mgr = SkFontMgr_New_DirectWrite(factory.get());
        if (!direct_write_font_mgr)
            return;
        dwrite_enabled = true;
        SetDefaultSkiaFactory(direct_write_font_mgr);
        gfx::PlatformFontWin::SetDirectWriteFactory(factory.get());
    }

    bool IsDirectWriteEnabled()
    {
        return dwrite_enabled;
    }

} // namespace win
} // namespace gfx
