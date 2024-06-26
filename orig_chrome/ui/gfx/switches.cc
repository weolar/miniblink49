// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/switches.h"
#include "build/build_config.h"

namespace switches {

// Overrides the device scale factor for the browser UI and the contents.
const char kForceDeviceScaleFactor[] = "force-device-scale-factor";

#if defined(OS_WIN)
// Disables the DirectWrite font rendering system on windows.
const char kDisableDirectWrite[] = "disable-direct-write";

// Disables DirectWrite font rendering for general UI elements.
const char kDisableDirectWriteForUI[] = "disable-directwrite-for-ui";
#endif

#if defined(OS_MACOSX)
// Enables the HarfBuzz port of RenderText on Mac (it's already used only for
// text editing; this enables it for everything else).
const char kEnableHarfBuzzRenderText[] = "enable-harfbuzz-rendertext";
#endif

} // namespace switches
