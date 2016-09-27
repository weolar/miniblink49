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

#ifndef SourceBufferList_h
#define SourceBufferList_h

#include "modules/EventTargetModules.h"
#include "platform/heap/Handle.h"

namespace blink {

class SourceBuffer;
class GenericEventQueue;

class SourceBufferList final : public RefCountedGarbageCollectedEventTargetWithInlineData<SourceBufferList> {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(SourceBufferList);
    DEFINE_WRAPPERTYPEINFO();
public:
    static SourceBufferList* create(ExecutionContext* context, GenericEventQueue* asyncEventQueue)
    {
        return new SourceBufferList(context, asyncEventQueue);
    }
    ~SourceBufferList() override;

    unsigned length() const { return m_list.size(); }
    SourceBuffer* item(unsigned index) const { return (index < m_list.size()) ? m_list[index].get() : 0; }

    void add(SourceBuffer*);
    void insert(size_t position, SourceBuffer*);
    void remove(SourceBuffer*);
    size_t find(SourceBuffer* buffer) { return m_list.find(buffer); }
    bool contains(SourceBuffer* buffer) { return m_list.find(buffer) != kNotFound; }
    void clear();

    // EventTarget interface
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    DECLARE_VIRTUAL_TRACE();

private:
    SourceBufferList(ExecutionContext*, GenericEventQueue*);

    void scheduleEvent(const AtomicString&);

    RawPtrWillBeMember<ExecutionContext> m_executionContext;
    RawPtrWillBeMember<GenericEventQueue> m_asyncEventQueue;

    HeapVector<Member<SourceBuffer>> m_list;
};

} // namespace blink

#endif // SourceBufferList_h
