// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/x/x11_switches.h"
#include "build/build_config.h"

namespace switches {

#if !defined(OS_CHROMEOS)
// When enabled, tries to get a transparent X11 visual so that we can have
// per-pixel alpha in windows.
//
// TODO(erg): Remove this switch once we've stabilized the code
// path. http://crbug.com/369209
const char kEnableTransparentVisuals[] = "enable-transparent-visuals";

// Color bit depth of the main window created in the browser process and matches
// XWindowAttributes.depth.
const char kWindowDepth[] = "window-depth";

// Which X11 display to connect to. Emulates the GTK+ "--display=" command line
// argument.
const char kX11Display[] = "display";
#endif

} // namespace switches
