// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ChildFrameDisconnector_h
#define ChildFrameDisconnector_h

#include "platform/heap/Handle.h"

namespace blink {

class ElementShadow;
class HTMLFrameOwnerElement;
class Node;

class ChildFrameDisconnector {
    STACK_ALLOCATED();
public:
    enum DisconnectPolicy {
        RootAndDescendants,
        DescendantsOnly
    };

    explicit ChildFrameDisconnector(Node& root)
        : m_root(root)
    {
    }

    void disconnect(DisconnectPolicy = RootAndDescendants);

private:
    void collectFrameOwners(Node&);
    void collectFrameOwners(ElementShadow&);
    void disconnectCollectedFrameOwners();
    Node& root() const { return *m_root; }

    WillBeHeapVector<RefPtrWillBeMember<HTMLFrameOwnerElement>, 10> m_frameOwners;
    RawPtrWillBeMember<Node> m_root;
};

} // namespace blink

#endif // ChildFrameDisconnector_h
