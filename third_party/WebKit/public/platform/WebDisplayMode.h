// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebDisplayMode_h
#define WebDisplayMode_h

namespace blink {

enum WebDisplayMode {
    WebDisplayModeUndefined, // User for override setting (ie. not set).
    WebDisplayModeBrowser,
    WebDisplayModeMinimalUi,
    WebDisplayModeStandalone,
    WebDisplayModeFullscreen,
    WebDisplayModeLast = WebDisplayModeFullscreen
};

} // namespace blink

#endif // WebDisplayMode_h
