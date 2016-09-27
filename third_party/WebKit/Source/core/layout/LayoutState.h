/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutState_h
#define LayoutState_h

#include "platform/geometry/LayoutRect.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"

namespace blink {

class ForceHorriblySlowRectMapping;
class LayoutBox;
class LayoutFlowThread;
class LayoutObject;
class LayoutView;

class LayoutState {
    WTF_MAKE_NONCOPYABLE(LayoutState);
public:
    // Constructor for root LayoutState created by LayoutView
    LayoutState(LayoutUnit pageLogicalHeight, bool pageLogicalHeightChanged, LayoutView&);
    // Constructor for sub-tree layout
    explicit LayoutState(LayoutObject& root);

    LayoutState(LayoutBox&, const LayoutSize& offset, LayoutUnit pageLogicalHeight = 0, bool pageHeightLogicalChanged = false, bool containingBlockLogicalWidthChanged = false);

    ~LayoutState();

    void clearPaginationInformation();
    bool isPaginated() const { return m_isPaginated; }

    // The page logical offset is the object's offset from the top of the page in the page progression
    // direction (so an x-offset in vertical text and a y-offset for horizontal text).
    LayoutUnit pageLogicalOffset(const LayoutBox&, const LayoutUnit& childLogicalOffset) const;

    const LayoutSize& layoutOffset() const { return m_layoutOffset; }
    const LayoutSize& pageOffset() const { return m_pageOffset; }
    LayoutUnit pageLogicalHeight() const { return m_pageLogicalHeight; }
    bool pageLogicalHeightChanged() const { return m_pageLogicalHeightChanged; }
    bool containingBlockLogicalWidthChanged() const { return m_containingBlockLogicalWidthChanged; }

    LayoutState* next() const { return m_next; }

    bool needsBlockDirectionLocationSetBeforeLayout() const { return m_isPaginated && m_pageLogicalHeight; }

    LayoutFlowThread* flowThread() const { return m_flowThread; }

    LayoutObject& layoutObject() const { return m_layoutObject; }

private:
    friend class ForceHorriblySlowRectMapping;

    // Do not add anything apart from bitfields until after m_flowThread. See https://bugs.webkit.org/show_bug.cgi?id=100173
    bool m_isPaginated : 1;
    // If our page height has changed, this will force all blocks to relayout.
    bool m_pageLogicalHeightChanged : 1;
    bool m_containingBlockLogicalWidthChanged : 1;

    LayoutFlowThread* m_flowThread;

    LayoutState* m_next;

    // x/y offset from container. Does not include relative positioning or scroll offsets.
    LayoutSize m_layoutOffset;

    // The current page height for the pagination model that encloses us.
    LayoutUnit m_pageLogicalHeight;
    // The offset of the start of the first page in the nearest enclosing pagination model.
    LayoutSize m_pageOffset;

    LayoutObject& m_layoutObject;
};

} // namespace blink

#endif // LayoutState_h
