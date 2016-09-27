// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InlinedGlobalMarkingVisitor_h
#define InlinedGlobalMarkingVisitor_h

#include "platform/heap/MarkingVisitorImpl.h"

namespace blink {

class InlinedGlobalMarkingVisitor final : public VisitorHelper<InlinedGlobalMarkingVisitor>, public MarkingVisitorImpl<InlinedGlobalMarkingVisitor> {
public:
    friend class VisitorHelper<InlinedGlobalMarkingVisitor>;
    using Helper = VisitorHelper<InlinedGlobalMarkingVisitor>;
    friend class MarkingVisitorImpl<InlinedGlobalMarkingVisitor>;
    using Impl = MarkingVisitorImpl<InlinedGlobalMarkingVisitor>;

    explicit InlinedGlobalMarkingVisitor(Visitor* visitor)
        : m_visitor(visitor)
    {
        ASSERT(visitor->markingMode() == Visitor::GlobalMarking);
    }

    // Hack to unify interface to visitor->trace().
    // Without this hack, we need to use visitor.trace() for
    // trace(InlinedGlobalMarkingVisitor) and visitor->trace() for trace(Visitor*).
    InlinedGlobalMarkingVisitor* operator->() { return this; }

    using Impl::mark;
    using Impl::ensureMarked;
    using Impl::registerDelayedMarkNoTracing;
    using Impl::registerWeakTable;
    using Impl::registerWeakMembers;
#if ENABLE(ASSERT)
    using Impl::weakTableRegistered;
#endif

    template<typename T>
    void mark(T* t)
    {
        Helper::mark(t);
    }

    template<typename T, void (T::*method)(Visitor*)>
    void registerWeakMembers(const T* obj)
    {
        Helper::template registerWeakMembers<T, method>(obj);
    }

    Visitor* getUninlined() { return m_visitor; }
protected:
    // Methods to be called from MarkingVisitorImpl.

    inline bool shouldMarkObject(const void*)
    {
        // As this is global marking visitor, we need to mark all objects.
        return true;
    }

    inline Visitor::MarkingMode markingMode() const
    {
        return m_visitor->markingMode();
    }

private:
    static InlinedGlobalMarkingVisitor fromHelper(Helper* helper)
    {
        return *static_cast<InlinedGlobalMarkingVisitor*>(helper);
    }

    Visitor* m_visitor;
};

inline void GarbageCollectedMixin::trace(InlinedGlobalMarkingVisitor)
{
}

} // namespace blink

#endif
