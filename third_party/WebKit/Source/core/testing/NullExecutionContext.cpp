// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/testing/NullExecutionContext.h"

#include "core/dom/ExecutionContextTask.h"
#include "core/events/Event.h"
#include "core/frame/DOMTimer.h"

namespace blink {

namespace {

class NullEventQueue final : public EventQueue {
public:
    NullEventQueue() { }
    virtual ~NullEventQueue() { }
    virtual bool enqueueEvent(PassRefPtrWillBeRawPtr<Event>) override { return true; }
    virtual bool cancelEvent(Event*) override { return true; }
    virtual void close() override { }
};

} // namespace

NullExecutionContext::NullExecutionContext()
    : m_tasksNeedSuspension(false)
    , m_queue(adoptPtrWillBeNoop(new NullEventQueue()))
{
}

void NullExecutionContext::postTask(const WebTraceLocation&, PassOwnPtr<ExecutionContextTask>)
{
}

double NullExecutionContext::timerAlignmentInterval() const
{
    return DOMTimer::visiblePageAlignmentInterval();
}

bool NullExecutionContext::isPrivilegedContext(String& errorMessage, const PrivilegeContextCheck privilegeContextCheck) const
{
    return true;
}

} // namespace blink
