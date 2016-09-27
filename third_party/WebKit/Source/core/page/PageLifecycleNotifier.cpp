/*
 * Copyright (C) 2013 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "config.h"
#include "core/page/PageLifecycleNotifier.h"

#include "core/page/PageLifecycleObserver.h"

namespace blink {

void PageLifecycleNotifier::notifyPageVisibilityChanged()
{
    TemporaryChange<IterationType> scope(m_iterating, IteratingOverAll);
#if !ENABLE(OILPAN)
    // Notifications perform unknown amounts of heap allocations,
    // which might trigger (conservative) GCs. This will flush out
    // dead observers, causing the _non-heap_ set be updated. Snapshot
    // the observers and explicitly check if they're still alive before
    // notifying.
    Vector<RawPtr<PageLifecycleObserver>> snapshotOfObservers;
    copyToVector(m_observers, snapshotOfObservers);
    for (PageLifecycleObserver* observer : snapshotOfObservers) {
        if (m_observers.contains(observer))
            observer->pageVisibilityChanged();
    }
#else
    for (PageLifecycleObserver* observer : m_observers)
        observer->pageVisibilityChanged();
#endif
}

void PageLifecycleNotifier::notifyDidCommitLoad(LocalFrame* frame)
{
    TemporaryChange<IterationType> scope(m_iterating, IteratingOverAll);
#if !ENABLE(OILPAN)
    // Notifications perform unknown amounts of heap allocations,
    // which might trigger (conservative) GCs. This will flush out
    // dead observers, causing the _non-heap_ set be updated. Snapshot
    // the observers and explicitly check if they're still alive before
    // notifying.
    Vector<RawPtr<PageLifecycleObserver>> snapshotOfObservers;
    copyToVector(m_observers, snapshotOfObservers);
    for (PageLifecycleObserver* observer : snapshotOfObservers) {
        if (m_observers.contains(observer))
            observer->didCommitLoad(frame);
    }
#else
    for (PageLifecycleObserver* observer : m_observers)
        observer->didCommitLoad(frame);
#endif
}

} // namespace blink
