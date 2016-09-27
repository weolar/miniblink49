// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LocalFrameLifecycleNotifier_h
#define LocalFrameLifecycleNotifier_h

#include "core/CoreExport.h"
#include "platform/LifecycleNotifier.h"
#include "wtf/Forward.h"

namespace blink {

class LocalFrameLifecycleObserver;
class LocalFrame;

class CORE_EXPORT LocalFrameLifecycleNotifier : public LifecycleNotifier<LocalFrame, LocalFrameLifecycleObserver> {
public:
    void notifyWillDetachFrameHost();
};

} // namespace blink

#endif // LocalFrameLifecycleNotifier_h
