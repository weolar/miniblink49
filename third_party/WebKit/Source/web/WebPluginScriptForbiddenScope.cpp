// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/web/WebPluginScriptForbiddenScope.h"

#include "platform/PluginScriptForbiddenScope.h"

namespace blink {

bool WebPluginScriptForbiddenScope::isForbidden()
{
    return PluginScriptForbiddenScope::isForbidden();
}

} // namespace blink
