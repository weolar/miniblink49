// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MarkingVisitor_h
#define MarkingVisitor_h

#include "platform/heap/MarkingVisitorImpl.h"

namespace blink {

template <Visitor::MarkingMode Mode>
class MarkingVisitor final : public Visitor, public MarkingVisitorImpl<MarkingVisitor<Mode>> {
public:
    using Impl = MarkingVisitorImpl<MarkingVisitor<Mode>>;
    friend class MarkingVisitorImpl<MarkingVisitor<Mode>>;

    MarkingVisitor()
        : Visitor(Mode)
    {
    }

    void markHeader(HeapObjectHeader* header, TraceCallback callback) override
    {
        Impl::markHeader(header, header->payload(), callback);
    }

    void mark(const void* objectPointer, TraceCallback callback) override
    {
        Impl::mark(objectPointer, callback);
    }

    void registerDelayedMarkNoTracing(const void* object) override
    {
        Impl::registerDelayedMarkNoTracing(object);
    }

    void registerWeakMembers(const void* closure, const void* objectPointer, WeakCallback callback) override
    {
        Impl::registerWeakMembers(closure, objectPointer, callback);
    }

    virtual void registerWeakTable(const void* closure, EphemeronCallback iterationCallback, EphemeronCallback iterationDoneCallback)
    {
        Impl::registerWeakTable(closure, iterationCallback, iterationDoneCallback);
    }

#if ENABLE(ASSERT)
    virtual bool weakTableRegistered(const void* closure)
    {
        return Impl::weakTableRegistered(closure);
    }
#endif

    bool ensureMarked(const void* objectPointer) override
    {
        return Impl::ensureMarked(objectPointer);
    }

protected:
    void registerWeakCellWithCallback(void** cell, WeakCallback callback) override
    {
        Impl::registerWeakCellWithCallback(cell, callback);
    }

    inline bool shouldMarkObject(const void* objectPointer)
    {
        if (Mode != ThreadLocalMarking)
            return true;

        BasePage* page = pageFromObject(objectPointer);
        ASSERT(!page->orphaned());
        // When doing a thread local GC, the marker checks if
        // the object resides in another thread's heap. If it
        // does, the object should not be marked & traced.
        return page->terminating();
    }
};

} // namespace blink

#endif
