// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/PluginScriptForbiddenScope.h"

#include "wtf/Assertions.h"
#include "wtf/MainThread.h"

namespace blink {

static unsigned s_pluginScriptForbiddenCount = 0;

PluginScriptForbiddenScope::PluginScriptForbiddenScope()
{
    ASSERT(isMainThread());
    ++s_pluginScriptForbiddenCount;
}

PluginScriptForbiddenScope::~PluginScriptForbiddenScope()
{
    ASSERT(isMainThread());
    ASSERT(s_pluginScriptForbiddenCount);
    --s_pluginScriptForbiddenCount;
}

bool PluginScriptForbiddenScope::isForbidden()
{
    ASSERT(isMainThread());
    return s_pluginScriptForbiddenCount > 0;
}

} // namespace blink
