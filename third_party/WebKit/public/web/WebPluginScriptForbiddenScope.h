// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPluginScriptForbiddenScope_h
#define WebPluginScriptForbiddenScope_h

#include "public/platform/WebCommon.h"

namespace blink {

class BLINK_EXPORT WebPluginScriptForbiddenScope {
public:
    WebPluginScriptForbiddenScope() = delete;
    static bool isForbidden();
};

} // namespace blink

#endif // WebPluginScriptForbiddenScope_h
