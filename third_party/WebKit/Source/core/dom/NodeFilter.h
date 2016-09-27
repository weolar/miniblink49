/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2000 Frederik Holljen (frederik.holljen@hig.no)
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2004, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef NodeFilter_h
#define NodeFilter_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/dom/NodeFilterCondition.h"
#include "platform/heap/Handle.h"
#include "wtf/RefPtr.h"

namespace blink {

class NodeFilter final : public RefCountedWillBeGarbageCollected<NodeFilter>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    /**
     * The following constants are returned by the acceptNode()
     * method:
     */
    enum {
        FILTER_ACCEPT = 1,
        FILTER_REJECT = 2,
        FILTER_SKIP   = 3
    };

    /**
     * These are the available values for the whatToShow parameter.
     * They are the same as the set of possible types for Node, and
     * their values are derived by using a bit position corresponding
     * to the value of NodeType for the equivalent node type.
     */
    enum {
        SHOW_ALL                       = 0xFFFFFFFF,
        SHOW_ELEMENT                   = 0x00000001,
        SHOW_ATTRIBUTE                 = 0x00000002,
        SHOW_TEXT                      = 0x00000004,
        SHOW_CDATA_SECTION             = 0x00000008,
        SHOW_ENTITY_REFERENCE          = 0x00000010,
        SHOW_ENTITY                    = 0x00000020,
        SHOW_PROCESSING_INSTRUCTION    = 0x00000040,
        SHOW_COMMENT                   = 0x00000080,
        SHOW_DOCUMENT                  = 0x00000100,
        SHOW_DOCUMENT_TYPE             = 0x00000200,
        SHOW_DOCUMENT_FRAGMENT         = 0x00000400,
        SHOW_NOTATION                  = 0x00000800
    };

    static PassRefPtrWillBeRawPtr<NodeFilter> create(PassRefPtrWillBeRawPtr<NodeFilterCondition> condition)
    {
        return adoptRefWillBeNoop(new NodeFilter(condition));
    }

    static PassRefPtrWillBeRawPtr<NodeFilter> create()
    {
        return adoptRefWillBeNoop(new NodeFilter());
    }

    unsigned acceptNode(Node*, ExceptionState&) const;

    void setCondition(PassRefPtrWillBeRawPtr<NodeFilterCondition> condition)
    {
        m_condition = condition;
    }

    DECLARE_TRACE();

private:
    explicit NodeFilter(PassRefPtrWillBeRawPtr<NodeFilterCondition> condition) : m_condition(condition) { }

    NodeFilter() { }

    RefPtrWillBeMember<NodeFilterCondition> m_condition;
};

} // namespace blink

#endif // NodeFilter_h
