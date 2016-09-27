/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef DOMTimer_h
#define DOMTimer_h

#include "bindings/core/v8/ScheduledAction.h"
#include "core/CoreExport.h"
#include "core/frame/SuspendableTimer.h"
#include "platform/UserGestureIndicator.h"
#include "platform/heap/Handle.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"

namespace blink {

class ExecutionContext;

class CORE_EXPORT DOMTimer final : public RefCountedWillBeGarbageCollectedFinalized<DOMTimer>, public SuspendableTimer {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMTimer);
public:
    // Creates a new timer owned by the ExecutionContext, starts it and returns its ID.
    static int install(ExecutionContext*, PassOwnPtrWillBeRawPtr<ScheduledAction>, int timeout, bool singleShot);
    static void removeByID(ExecutionContext*, int timeoutID);

    virtual ~DOMTimer();

    // ActiveDOMObject
    virtual void stop() override;

    // The following are essentially constants. All intervals are in seconds.
    static double hiddenPageAlignmentInterval();
    static double visiblePageAlignmentInterval();

    // Eager finalization is needed to promptly stop this Timer object.
    // Otherwise timer events might fire at an object that's slated for destruction
    // (when lazily swept), but some of its members (m_action) may already have
    // been finalized & must not be accessed.
    EAGERLY_FINALIZE();
    DECLARE_VIRTUAL_TRACE();

    void disposeTimer();

private:
    friend class DOMTimerCoordinator; // For create().

    static PassRefPtrWillBeRawPtr<DOMTimer> create(ExecutionContext* context, PassOwnPtrWillBeRawPtr<ScheduledAction> action, int timeout, bool singleShot, int timeoutID)
    {
        return adoptRefWillBeNoop(new DOMTimer(context, action, timeout, singleShot, timeoutID));
    }

    DOMTimer(ExecutionContext*, PassOwnPtrWillBeRawPtr<ScheduledAction>, int interval, bool singleShot, int timeoutID);
    virtual void fired() override;

    // Retuns timer fire time rounded to the next multiple of timer alignment interval.
    virtual double alignedFireTime(double) const override;

    int m_timeoutID;
    int m_nestingLevel;
    OwnPtrWillBeMember<ScheduledAction> m_action;
    RefPtr<UserGestureToken> m_userGestureToken;
};

} // namespace blink

#endif // DOMTimer_h
