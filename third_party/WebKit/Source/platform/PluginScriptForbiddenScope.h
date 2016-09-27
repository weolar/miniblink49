// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PluginScriptForbiddenScope_h
#define PluginScriptForbiddenScope_h

#include "platform/PlatformExport.h"

namespace blink {

// Similar to ScriptForbiddenScope, but more selective. This is intended to help
// reduce the number of places where Flash can run a nested message loop as its
// plugin element is being destroyed. One of the reasons that Flash runs this
// nested message loop is to allow Flash content to synchronously script the
// page when the plugin element is destroyed.
//
// This is problematic for many reasons: the DOM may not be in a consistent
// state, since Blink is in the middle of detaching nodes, nested message loops
// can cause normally impossible conditions to occur (https://crbug.com/367210),
// etc.
//
// When this object is instantiated on the stack, it allows execution of event
// handlers, etc but blocks attempts by plugins to call back into Blink to
// execute script.
//
// Background:
// For historical reasons, Flash has allowed synchronous scripting during
// teardown of the plugin. This is generally problematic, but sites apparently
// rely on this behavior. Over time, Blink has added restrictions on this
// synchronous scripting: for example, past a certain point in Frame detach,
// script execution by Flash is ignored: https://crbug.com/371084.
//
// Unfortunately, there are still ways for plugins to synchronously script
// during Document detach: if an unload handler removes a Flash plugin element,
// that will run the nested message loop, etc. This scoper is intended to block
// those usages, with the eventual goal that Frame detach will never have to run
// a nested message loop.
class PLATFORM_EXPORT PluginScriptForbiddenScope {
public:
    PluginScriptForbiddenScope();
    ~PluginScriptForbiddenScope();

    static bool isForbidden();
};

} // namespace blink

#endif // PluginScriptForbiddenScope_h
