/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2009 Adam Barth. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NavigationScheduler_h
#define NavigationScheduler_h

#include "core/CoreExport.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class FormSubmission;
class LocalFrame;
class NavigationScheduler;
class ScheduledNavigation;

class NavigationDisablerForBeforeUnload {
    WTF_MAKE_NONCOPYABLE(NavigationDisablerForBeforeUnload);
    STACK_ALLOCATED();
public:
    NavigationDisablerForBeforeUnload()
    {
        s_navigationDisableCount++;
    }
    ~NavigationDisablerForBeforeUnload()
    {
        ASSERT(s_navigationDisableCount);
        s_navigationDisableCount--;
    }
    static bool isNavigationAllowed() { return !s_navigationDisableCount; }

private:
    static unsigned s_navigationDisableCount;
};

class FrameNavigationDisabler {
    WTF_MAKE_NONCOPYABLE(FrameNavigationDisabler);
    STACK_ALLOCATED();
public:
    explicit FrameNavigationDisabler(LocalFrame*);
    ~FrameNavigationDisabler();

private:
    FrameNavigationDisabler() = delete;

    NavigationScheduler& m_navigationScheduler;
};

class CORE_EXPORT NavigationScheduler final {
    WTF_MAKE_NONCOPYABLE(NavigationScheduler);
    DISALLOW_ALLOCATION();
public:
    explicit NavigationScheduler(LocalFrame*);
    ~NavigationScheduler();

    bool locationChangePending();

    void scheduleRedirect(double delay, const String& url);
    void scheduleLocationChange(Document*, const String& url, bool lockBackForwardList = true);
    void schedulePageBlock(Document*);
    void scheduleFormSubmission(Document*, PassRefPtrWillBeRawPtr<FormSubmission>);
    void scheduleReload();

    void startTimer();
    void cancel();

    DECLARE_TRACE();

private:
    friend class FrameNavigationDisabler;

    void disableFrameNavigation() { ++m_navigationDisableCount; }
    void enableFrameNavigation() { --m_navigationDisableCount; }
    bool isFrameNavigationAllowed() const { return !m_navigationDisableCount; }

    bool shouldScheduleReload() const;
    bool shouldScheduleNavigation(const String& url) const;

    void timerFired(Timer<NavigationScheduler>*);
    void schedule(PassOwnPtrWillBeRawPtr<ScheduledNavigation>);

    static bool mustLockBackForwardList(LocalFrame* targetFrame);

    RawPtrWillBeMember<LocalFrame> m_frame;
    Timer<NavigationScheduler> m_timer;
    OwnPtrWillBeMember<ScheduledNavigation> m_redirect;
    int m_navigationDisableCount;
};

} // namespace blink

#endif // NavigationScheduler_h
