// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.


#include "config.h"

#include "bindings/modules/v8/V8DataTransferItemPartial.h"
#include "bindings/modules/v8/V8DedicatedWorkerGlobalScopePartial.h"
#include "bindings/modules/v8/V8DevToolsHostPartial.h"
#include "bindings/modules/v8/V8HTMLInputElementPartial.h"
#include "bindings/modules/v8/V8HTMLMediaElementPartial.h"
#include "bindings/modules/v8/V8HTMLVideoElementPartial.h"
#include "bindings/modules/v8/V8MouseEventPartial.h"
#include "bindings/modules/v8/V8NavigatorPartial.h"
#include "bindings/modules/v8/V8ScreenPartial.h"
#include "bindings/modules/v8/V8SharedWorkerGlobalScopePartial.h"
#include "bindings/modules/v8/V8URLPartial.h"
#include "bindings/modules/v8/V8WindowPartial.h"
#include "bindings/modules/v8/V8WorkerGlobalScopePartial.h"
#include "bindings/modules/v8/V8WorkerNavigatorPartial.h"

namespace blink {

void initPartialInterfacesInModules()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    V8DedicatedWorkerGlobalScopePartial::initialize();
    V8DevToolsHostPartial::initialize();
    V8HTMLInputElementPartial::initialize();
    V8HTMLMediaElementPartial::initialize();
    V8HTMLVideoElementPartial::initialize();
    V8MouseEventPartial::initialize();
    V8NavigatorPartial::initialize();
    V8ScreenPartial::initialize();
    V8SharedWorkerGlobalScopePartial::initialize();
    V8URLPartial::initialize();
    V8WindowPartial::initialize();
    V8WorkerGlobalScopePartial::initialize();
    V8WorkerNavigatorPartial::initialize();
#else
    V8DataTransferItemPartial::initialize();
    V8NavigatorPartial::initialize();
    V8WindowPartial::initialize();
    V8DevToolsHostPartial::initialize();
    V8ScreenPartial::initialize();
    V8HTMLInputElementPartial::initialize();
    V8MouseEventPartial::initialize();
    V8HTMLMediaElementPartial::initialize();
    V8HTMLVideoElementPartial::initialize();
    V8URLPartial::initialize();
    V8DedicatedWorkerGlobalScopePartial::initialize();
    V8SharedWorkerGlobalScopePartial::initialize();
    V8WorkerGlobalScopePartial::initialize();
    V8WorkerNavigatorPartial::initialize();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

} // namespace blink
