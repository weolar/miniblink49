// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/path_service.h"
#include "base/test/launcher/unit_test_launcher.h"
#include "base/test/test_suite.h"
#include "build/build_config.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/base/resource/resource_bundle.h"
#include "ui/base/ui_base_paths.h"

#if defined(OS_ANDROID)
#include "base/android/jni_android.h"
#include "ui/gfx/android/gfx_jni_registrar.h"
#endif

#if defined(OS_MACOSX) && !defined(OS_IOS)
#include "base/test/mock_chrome_application_mac.h"
#endif

#if defined(OS_WIN)
#include "ui/gfx/win/direct_write.h"
#include <windows.h>
#include <winuser.h>
#endif

namespace {

class GfxTestSuite : public base::TestSuite {
public:
    GfxTestSuite(int argc, char** argv)
        : base::TestSuite(argc, argv)
    {
#if defined(OS_WIN)
        reset_antialiasing_on_shutdown_ = false;
#endif
    }

protected:
    void Initialize() override
    {
        base::TestSuite::Initialize();

#if defined(OS_ANDROID)
        gfx::android::RegisterJni(base::android::AttachCurrentThread());
#endif

#if defined(OS_MACOSX) && !defined(OS_IOS)
        mock_cr_app::RegisterMockCrApp();
#endif

        ui::RegisterPathProvider();

        base::FilePath ui_test_pak_path;
        ASSERT_TRUE(PathService::Get(ui::UI_TEST_PAK, &ui_test_pak_path));
        ui::ResourceBundle::InitSharedInstanceWithPakPath(ui_test_pak_path);

#if defined(OS_WIN)
        gfx::win::MaybeInitializeDirectWrite();
        if (gfx::win::IsDirectWriteEnabled()) {
            // Force antialiasing to true if DirectWrite is enabled for font metrics.
            // With antialiasing off, Skia returns GDI compatible metrics which are
            // larger by 1-2 points which cause some tests to fail.
            // TODO(ananta): Investigate and fix.
            BOOL antialiasing = TRUE;
            SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &antialiasing, 0);
            if (!antialiasing) {
                SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, NULL, 0);
                reset_antialiasing_on_shutdown_ = true;
            }
        }
#endif
    }

    void Shutdown() override
    {
        ui::ResourceBundle::CleanupSharedInstance();
        base::TestSuite::Shutdown();
#if defined(OS_WIN)
        if (reset_antialiasing_on_shutdown_)
            SystemParametersInfo(SPI_SETFONTSMOOTHING, FALSE, NULL, 0);
#endif
    }

private:
#if defined(OS_WIN)
    // Set to true if we forced antialiasing to true on Windows for the
    // duration of the test. We reset antialiasing back on shutdown
    bool reset_antialiasing_on_shutdown_;
#endif
    DISALLOW_COPY_AND_ASSIGN(GfxTestSuite);
};

} // namespace

int main(int argc, char** argv)
{
    GfxTestSuite test_suite(argc, argv);

    return base::LaunchUnitTests(
        argc,
        argv,
        base::Bind(&GfxTestSuite::Run, base::Unretained(&test_suite)));
}
