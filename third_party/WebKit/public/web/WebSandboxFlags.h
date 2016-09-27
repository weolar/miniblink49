// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef WebSandboxFlags_h
#define WebSandboxFlags_h

#include "../platform/WebCommon.h"

namespace blink {

// See http://www.whatwg.org/specs/web-apps/current-work/#attr-iframe-sandbox
// for a list of the sandbox flags.  This enum should be kept in sync with
// Source/core/dom/SandboxFlags.h.  Enforced in AssertMatchingEnums.cpp
enum class WebSandboxFlags : int {
    None = 0,
    Navigation = 1,
    Plugins = 1 << 1,
    Origin = 1 << 2,
    Forms = 1 << 3,
    Scripts = 1 << 4,
    TopNavigation = 1 << 5,
    Popups = 1 << 6,
    AutomaticFeatures = 1 << 7,
    PointerLock = 1 << 8,
    DocumentDomain = 1 << 9,
    OrientationLock = 1 << 10,
    PropagatesToAuxiliaryBrowsingContexts = 1 << 11,
    Modals = 1 << 12,
    All = -1
};

inline WebSandboxFlags operator&(WebSandboxFlags a, WebSandboxFlags b)
{
    return static_cast<WebSandboxFlags>(static_cast<int>(a) & static_cast<int>(b));
}

inline WebSandboxFlags operator~(WebSandboxFlags flags)
{
    return static_cast<WebSandboxFlags>(~static_cast<int>(flags));
}

} // namespace blink

#endif // WebSandboxFlags_h
