/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/mediasource/SourceBufferList.h"

#include "core/dom/ExecutionContext.h"
#include "core/events/GenericEventQueue.h"
#include "modules/EventModules.h"
#include "modules/mediasource/SourceBuffer.h"

namespace blink {

SourceBufferList::SourceBufferList(ExecutionContext* context, GenericEventQueue* asyncEventQueue)
    : m_executionContext(context)
    , m_asyncEventQueue(asyncEventQueue)
{
}

SourceBufferList::~SourceBufferList()
{
#if !ENABLE(OILPAN)
    ASSERT(m_list.isEmpty());
#endif
}

void SourceBufferList::add(SourceBuffer* buffer)
{
    m_list.append(buffer);
    scheduleEvent(EventTypeNames::addsourcebuffer);
}

void SourceBufferList::insert(size_t position, SourceBuffer* buffer)
{
    m_list.insert(position, buffer);
    scheduleEvent(EventTypeNames::addsourcebuffer);
}

void SourceBufferList::remove(SourceBuffer* buffer)
{
    size_t index = m_list.find(buffer);
    if (index == kNotFound)
        return;
    m_list.remove(index);
    scheduleEvent(EventTypeNames::removesourcebuffer);
}

void SourceBufferList::clear()
{
    m_list.clear();
    scheduleEvent(EventTypeNames::removesourcebuffer);
}

void SourceBufferList::scheduleEvent(const AtomicString& eventName)
{
    ASSERT(m_asyncEventQueue);

    RefPtrWillBeRawPtr<Event> event = Event::create(eventName);
    event->setTarget(this);

    m_asyncEventQueue->enqueueEvent(event.release());
}

const AtomicString& SourceBufferList::interfaceName() const
{
    return EventTargetNames::SourceBufferList;
}

ExecutionContext* SourceBufferList::executionContext() const
{
    return m_executionContext;
}

DEFINE_TRACE(SourceBufferList)
{
    visitor->trace(m_executionContext);
    visitor->trace(m_asyncEventQueue);
    visitor->trace(m_list);
    RefCountedGarbageCollectedEventTargetWithInlineData<SourceBufferList>::trace(visitor);
}

} // namespace blink
