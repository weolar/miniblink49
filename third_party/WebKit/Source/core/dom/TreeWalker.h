/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2000 Frederik Holljen (frederik.holljen@hig.no)
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2004, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef TreeWalker_h
#define TreeWalker_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/NodeFilter.h"
#include "core/dom/NodeIteratorBase.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class ExceptionState;

class TreeWalker final : public RefCountedWillBeGarbageCollected<TreeWalker>, public ScriptWrappable, public NodeIteratorBase {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(TreeWalker);
public:
    static PassRefPtrWillBeRawPtr<TreeWalker> create(PassRefPtrWillBeRawPtr<Node> rootNode, unsigned whatToShow, PassRefPtrWillBeRawPtr<NodeFilter> filter)
    {
        return adoptRefWillBeNoop(new TreeWalker(rootNode, whatToShow, filter));
    }

    Node* currentNode() const { return m_current.get(); }
    void setCurrentNode(PassRefPtrWillBeRawPtr<Node>, ExceptionState&);

    Node* parentNode(ExceptionState&);
    Node* firstChild(ExceptionState&);
    Node* lastChild(ExceptionState&);
    Node* previousSibling(ExceptionState&);
    Node* nextSibling(ExceptionState&);
    Node* previousNode(ExceptionState&);
    Node* nextNode(ExceptionState&);

    DECLARE_TRACE();

private:
    TreeWalker(PassRefPtrWillBeRawPtr<Node>, unsigned whatToShow, PassRefPtrWillBeRawPtr<NodeFilter>);

    Node* setCurrent(PassRefPtrWillBeRawPtr<Node>);

    RefPtrWillBeMember<Node> m_current;
};

} // namespace blink

#endif // TreeWalker_h
