// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/FrameViewAutoSizeInfo.h"

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutView.h"

namespace blink {

FrameViewAutoSizeInfo::FrameViewAutoSizeInfo(FrameView* view)
    : m_frameView(view)
    , m_inAutoSize(false)
    , m_didRunAutosize(false)
{
    ASSERT(m_frameView);
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(FrameViewAutoSizeInfo);

DEFINE_TRACE(FrameViewAutoSizeInfo)
{
    visitor->trace(m_frameView);
}

void FrameViewAutoSizeInfo::configureAutoSizeMode(const IntSize& minSize, const IntSize& maxSize)
{
    ASSERT(!minSize.isEmpty());
    ASSERT(minSize.width() <= maxSize.width());
    ASSERT(minSize.height() <= maxSize.height());

    if (m_minAutoSize == minSize && m_maxAutoSize == maxSize)
        return;

    m_minAutoSize = minSize;
    m_maxAutoSize = maxSize;
    m_didRunAutosize = false;
}

void FrameViewAutoSizeInfo::autoSizeIfNeeded()
{
    if (m_inAutoSize)
        return;

    TemporaryChange<bool> changeInAutoSize(m_inAutoSize, true);

    Document* document = m_frameView->frame().document();
    if (!document || !document->isActive())
        return;

    Element* documentElement = document->documentElement();
    if (!documentElement)
        return;

    // If this is the first time we run autosize, start from small height and
    // allow it to grow.
    if (!m_didRunAutosize)
        m_frameView->resize(m_frameView->frameRect().width(), m_minAutoSize.height());

    IntSize size = m_frameView->frameRect().size();

    // Do the resizing twice. The first time is basically a rough calculation using the preferred width
    // which may result in a height change during the second iteration.
    for (int i = 0; i < 2; i++) {
        // Update various sizes including contentsSize, scrollHeight, etc.
        document->updateLayoutIgnorePendingStylesheets();

        LayoutView* layoutView = document->layoutView();
        if (!layoutView)
            return;

        int width = layoutView->minPreferredLogicalWidth();

        LayoutBox* documentLayoutBox = documentElement->layoutBox();
        if (!documentLayoutBox)
            return;

        int height = documentLayoutBox->scrollHeight();
        IntSize newSize(width, height);

        // Check to see if a scrollbar is needed for a given dimension and
        // if so, increase the other dimension to account for the scrollbar.
        // Since the dimensions are only for the view rectangle, once a
        // dimension exceeds the maximum, there is no need to increase it further.
        if (newSize.width() > m_maxAutoSize.width()) {
            RefPtrWillBeRawPtr<Scrollbar> localHorizontalScrollbar = m_frameView->horizontalScrollbar();
            if (!localHorizontalScrollbar)
                localHorizontalScrollbar = m_frameView->createScrollbar(HorizontalScrollbar);
            if (!localHorizontalScrollbar->isOverlayScrollbar())
                newSize.setHeight(newSize.height() + localHorizontalScrollbar->height());

            // Don't bother checking for a vertical scrollbar because the width is at
            // already greater the maximum.
        } else if (newSize.height() > m_maxAutoSize.height()) {
            RefPtrWillBeRawPtr<Scrollbar> localVerticalScrollbar = m_frameView->verticalScrollbar();
            if (!localVerticalScrollbar)
                localVerticalScrollbar = m_frameView->createScrollbar(VerticalScrollbar);
            if (!localVerticalScrollbar->isOverlayScrollbar())
                newSize.setWidth(newSize.width() + localVerticalScrollbar->width());

            // Don't bother checking for a horizontal scrollbar because the height is
            // already greater the maximum.
        }

        // Ensure the size is at least the min bounds.
        newSize = newSize.expandedTo(m_minAutoSize);

        // Bound the dimensions by the max bounds and determine what scrollbars to show.
        ScrollbarMode horizonalScrollbarMode = ScrollbarAlwaysOff;
        if (newSize.width() > m_maxAutoSize.width()) {
            newSize.setWidth(m_maxAutoSize.width());
            horizonalScrollbarMode = ScrollbarAlwaysOn;
        }
        ScrollbarMode verticalScrollbarMode = ScrollbarAlwaysOff;
        if (newSize.height() > m_maxAutoSize.height()) {
            newSize.setHeight(m_maxAutoSize.height());
            verticalScrollbarMode = ScrollbarAlwaysOn;
        }

        if (newSize == size)
            continue;

        // While loading only allow the size to increase (to avoid twitching during intermediate smaller states)
        // unless autoresize has just been turned on or the maximum size is smaller than the current size.
        if (m_didRunAutosize && size.height() <= m_maxAutoSize.height() && size.width() <= m_maxAutoSize.width()
            && !m_frameView->frame().document()->loadEventFinished() && (newSize.height() < size.height() || newSize.width() < size.width()))
            break;

        m_frameView->resize(newSize.width(), newSize.height());
        // Force the scrollbar state to avoid the scrollbar code adding them and causing them to be needed. For example,
        // a vertical scrollbar may cause text to wrap and thus increase the height (which is the only reason the scollbar is needed).
        m_frameView->setVerticalScrollbarLock(false);
        m_frameView->setHorizontalScrollbarLock(false);
        m_frameView->setScrollbarModes(horizonalScrollbarMode, verticalScrollbarMode, true, true);
    }
    m_didRunAutosize = true;
}

} // namespace blink
