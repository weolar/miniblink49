// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/LocalFrameLifecycleNotifier.h"

#include "core/frame/LocalFrameLifecycleObserver.h"

namespace blink {

void LocalFrameLifecycleNotifier::notifyWillDetachFrameHost()
{
    TemporaryChange<IterationType> scope(m_iterating, IteratingOverAll);
#if !ENABLE(OILPAN)
    // Notifications perform unknown amounts of heap allocations,
    // which might trigger (conservative) GCs. This will flush out
    // dead observers, causing the _non-heap_ set be updated. Snapshot
    // the observers and explicitly check if they're still alive before
    // notifying.
    Vector<RawPtr<LocalFrameLifecycleObserver>> snapshotOfObservers;
    copyToVector(m_observers, snapshotOfObservers);
    for (LocalFrameLifecycleObserver* observer : snapshotOfObservers) {
        if (m_observers.contains(observer))
            observer->willDetachFrameHost();
    }
#else
    for (LocalFrameLifecycleObserver* observer : m_observers)
        observer->willDetachFrameHost();
#endif
}

} // namespace blink
