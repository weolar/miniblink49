/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
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

#ifndef EventPath_h
#define EventPath_h

#include "core/CoreExport.h"
#include "core/events/NodeEventContext.h"
#include "core/events/TreeScopeEventContext.h"
#include "core/events/WindowEventContext.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/Vector.h"

namespace blink {

class Event;
class EventTarget;
class Node;
class TouchEvent;
class TouchList;
class TreeScope;

class CORE_EXPORT EventPath final : public NoBaseWillBeGarbageCollectedFinalized<EventPath> {
public:
    explicit EventPath(Node&, Event* = nullptr);

    void initializeWith(Node&, Event*);

    NodeEventContext& operator[](size_t index) { return m_nodeEventContexts[index]; }
    const NodeEventContext& operator[](size_t index) const { return m_nodeEventContexts[index]; }
    NodeEventContext& at(size_t index) { return m_nodeEventContexts[index]; }
    NodeEventContext& last() { return m_nodeEventContexts[size() - 1]; }

    WindowEventContext& windowEventContext() { ASSERT(m_windowEventContext); return *m_windowEventContext; }
    void ensureWindowEventContext();

    bool isEmpty() const { return m_nodeEventContexts.isEmpty(); }
    size_t size() const { return m_nodeEventContexts.size(); }

    void adjustForRelatedTarget(Node&, EventTarget* relatedTarget);
    void adjustForTouchEvent(TouchEvent&);

    static EventTarget* eventTargetRespectingTargetRules(Node&);

    DECLARE_TRACE();
    void clear()
    {
        m_nodeEventContexts.clear();
        m_treeScopeEventContexts.clear();
    }

private:
    EventPath();

    void initialize();
    void calculatePath();
    void calculateAdjustedTargets();
    void calculateTreeScopePrePostOrderNumbers();

    void shrink(size_t newSize) { ASSERT(!m_windowEventContext); m_nodeEventContexts.shrink(newSize); }
    void shrinkIfNeeded(const Node& target, const EventTarget& relatedTarget);

    void adjustTouchList(const TouchList*, WillBeHeapVector<RawPtrWillBeMember<TouchList>> adjustedTouchList, const WillBeHeapVector<RawPtrWillBeMember<TreeScope>>& treeScopes);

    typedef WillBeHeapHashMap<RawPtrWillBeMember<TreeScope>, RefPtrWillBeMember<TreeScopeEventContext>> TreeScopeEventContextMap;
    TreeScopeEventContext* ensureTreeScopeEventContext(Node* currentTarget, TreeScope*, TreeScopeEventContextMap&);

    typedef WillBeHeapHashMap<RawPtrWillBeMember<TreeScope>, RawPtrWillBeMember<EventTarget>> RelatedTargetMap;

    static void buildRelatedNodeMap(const Node&, RelatedTargetMap&);
    static EventTarget* findRelatedNode(TreeScope&, RelatedTargetMap&);

#if ENABLE(ASSERT)
    static void checkReachability(TreeScope&, TouchList&);
#endif

    const NodeEventContext& topNodeEventContext();

    WillBeHeapVector<NodeEventContext> m_nodeEventContexts;
    RawPtrWillBeMember<Node> m_node;
    RawPtrWillBeMember<Event> m_event;
    WillBeHeapVector<RefPtrWillBeMember<TreeScopeEventContext>> m_treeScopeEventContexts;
    OwnPtrWillBeMember<WindowEventContext> m_windowEventContext;
};

} // namespace

#endif
