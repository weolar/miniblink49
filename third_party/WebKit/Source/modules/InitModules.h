// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InitModules_h
#define InitModules_h

#include "core/Init.h"
#include "modules/ModulesExport.h"

namespace blink {

class MODULES_EXPORT ModulesInitializer : public CoreInitializer {
public:
    void init() override;
    static void terminateThreads();
};

} // namespace blink

#endif // InitModules_h
