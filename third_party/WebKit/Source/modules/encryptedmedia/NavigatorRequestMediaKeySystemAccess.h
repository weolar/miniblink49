// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorRequestMediaKeySystemAccess_h
#define NavigatorRequestMediaKeySystemAccess_h

#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/modules/v8/V8MediaKeySystemConfiguration.h"
#include "core/frame/Navigator.h"
#include "modules/encryptedmedia/MediaKeySystemConfiguration.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class NavigatorRequestMediaKeySystemAccess {
public:
    static ScriptPromise requestMediaKeySystemAccess(
        ScriptState*,
        Navigator&,
        const String& keySystem,
        const HeapVector<MediaKeySystemConfiguration>& supportedConfigurations);

private:
    NavigatorRequestMediaKeySystemAccess();
};

} // namespace blink

#endif // NavigatorRequestMediaKeySystemAccess_h
