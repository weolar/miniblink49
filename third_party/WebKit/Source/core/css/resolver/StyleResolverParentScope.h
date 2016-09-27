// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleResolverParentScope_h
#define StyleResolverParentScope_h

#include "core/css/resolver/StyleResolver.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/shadow/ShadowRoot.h"

namespace blink {

// Maintains the parent element stack (and bloom filter) inside recalcStyle.
class StyleResolverParentScope final {
    STACK_ALLOCATED();
public:
    explicit StyleResolverParentScope(Node& parent);
    ~StyleResolverParentScope();

    static void ensureParentStackIsPushed();

private:
    void pushParentIfNeeded();
    Node& parent() const { return *m_parent; }

    RawPtrWillBeMember<Node> m_parent;
    bool m_pushed;
    StyleResolverParentScope* m_previous;
    StyleResolver& m_resolver;

    static StyleResolverParentScope* s_currentScope;
};

inline StyleResolverParentScope::StyleResolverParentScope(Node& parent)
    : m_parent(parent)
    , m_pushed(false)
    , m_previous(s_currentScope)
    , m_resolver(*parent.document().styleResolver())
{
    ASSERT(parent.document().inStyleRecalc());
    ASSERT(parent.isElementNode() || parent.isShadowRoot());
    s_currentScope = this;
    m_resolver.increaseStyleSharingDepth();
}

inline StyleResolverParentScope::~StyleResolverParentScope()
{
    s_currentScope = m_previous;
    m_resolver.decreaseStyleSharingDepth();
    if (!m_pushed)
        return;
    if (parent().isElementNode())
        m_resolver.popParentElement(toElement(parent()));
}

inline void StyleResolverParentScope::ensureParentStackIsPushed()
{
    if (s_currentScope)
        s_currentScope->pushParentIfNeeded();
}

inline void StyleResolverParentScope::pushParentIfNeeded()
{
    if (m_pushed)
        return;
    if (m_previous)
        m_previous->pushParentIfNeeded();
    if (parent().isElementNode())
        m_resolver.pushParentElement(toElement(parent()));
    m_pushed = true;
}

} // namespace blink

#endif // StyleResolverParentScope_h
