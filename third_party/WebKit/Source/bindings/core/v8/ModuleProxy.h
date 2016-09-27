// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ModuleProxy_h
#define ModuleProxy_h

#include <v8.h>

namespace blink {

// A proxy class to invoke functions implemented in bindings/modules
// from bindings/core.
class ModuleProxy {
public:
    static ModuleProxy& moduleProxy();

private:
    ModuleProxy() { }
};

} // namespace blink

#endif // ModuleProxy_h
