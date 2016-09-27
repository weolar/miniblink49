/*
 * Copyright (C) 2006 Apple Computer, Inc.
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
 */

#ifndef FrameTree_h
#define FrameTree_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class Frame;
class TreeScope;

class CORE_EXPORT FrameTree final {
    WTF_MAKE_NONCOPYABLE(FrameTree);
    DISALLOW_ALLOCATION();
public:
    explicit FrameTree(Frame* thisFrame);
    ~FrameTree();

    const AtomicString& name() const { return m_name; }
    const AtomicString& uniqueName() const { return m_uniqueName; }
    // If |name| is not empty, |fallbackName| is ignored. Otherwise,
    // |fallbackName| is used as a source of uniqueName.
    void setName(const AtomicString& name, const AtomicString& fallbackName = nullAtom);

    Frame* parent() const;
    Frame* top() const;
    Frame* previousSibling() const;
    Frame* nextSibling() const;
    Frame* firstChild() const;
    Frame* lastChild() const;

    bool isDescendantOf(const Frame* ancestor) const;
    Frame* traversePreviousWithWrap(bool) const;
    Frame* traverseNext(const Frame* stayWithin = nullptr) const;
    Frame* traverseNextWithWrap(bool) const;

    Frame* child(const AtomicString& name) const;
    Frame* find(const AtomicString& name) const;
    unsigned childCount() const;

    Frame* scopedChild(unsigned index) const;
    Frame* scopedChild(const AtomicString& name) const;
    unsigned scopedChildCount() const;
    void invalidateScopedChildCount();

    DECLARE_TRACE();

private:
    Frame* deepLastChild() const;
    AtomicString uniqueChildName(const AtomicString& requestedName) const;
    bool uniqueNameExists(const AtomicString& name) const;
    unsigned scopedChildCount(TreeScope*) const;

    RawPtrWillBeMember<Frame> m_thisFrame;

    AtomicString m_name; // The actual frame name (may be empty).
    AtomicString m_uniqueName;

    mutable unsigned m_scopedChildCount;
};

} // namespace blink

#ifndef NDEBUG
// Outside the WebCore namespace for ease of invocation from gdb.
void showFrameTree(const blink::Frame*);
#endif

#endif // FrameTree_h
