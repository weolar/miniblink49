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

#ifndef NodeChildRemovalTracker_h
#define NodeChildRemovalTracker_h

#include "core/dom/Node.h"
#include "platform/heap/Handle.h"

namespace blink {

class NodeChildRemovalTracker {
    STACK_ALLOCATED();
public:
    explicit NodeChildRemovalTracker(Node&);
    ~NodeChildRemovalTracker();

    static bool isBeingRemoved(Node*);

private:
    Node& node() const { return *m_node; }
    NodeChildRemovalTracker* previous() { return m_previous; }

    RawPtrWillBeMember<Node> m_node;
    // Using raw pointers are safe because these NodeChildRemovalTrackers are
    // guaranteed to be on a stack.
    NodeChildRemovalTracker* m_previous;
    static NodeChildRemovalTracker* s_last;
};

inline NodeChildRemovalTracker::NodeChildRemovalTracker(Node& node)
    : m_node(node)
    , m_previous(s_last)
{
    s_last = this;
}

inline NodeChildRemovalTracker::~NodeChildRemovalTracker()
{
    s_last = m_previous;
}

inline bool NodeChildRemovalTracker::isBeingRemoved(Node* node)
{
    for (NodeChildRemovalTracker* removal = s_last; removal; removal = removal->previous()) {
        if (removal->node().containsIncludingShadowDOM(node))
            return true;
    }

    return false;
}

} // namespace

#endif
