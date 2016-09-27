/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/accessibility/AXScrollView.h"

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "modules/accessibility/AXScrollbar.h"

namespace blink {

AXScrollView::AXScrollView(FrameView* view, AXObjectCacheImpl& axObjectCache)
    : AXObject(axObjectCache)
    , m_scrollView(view)
    , m_childrenDirty(false)
{
}

AXScrollView::~AXScrollView()
{
    ASSERT(!m_scrollView);
}

void AXScrollView::detach()
{
    AXObject::detach();
    m_scrollView = nullptr;
}

PassRefPtrWillBeRawPtr<AXScrollView> AXScrollView::create(FrameView* view, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXScrollView(view, axObjectCache));
}

AXObject* AXScrollView::scrollBar(AccessibilityOrientation orientation)
{
    updateScrollbars();

    switch (orientation) {
    case AccessibilityOrientationVertical:
        return m_verticalScrollbar ? m_verticalScrollbar.get() : 0;
    case AccessibilityOrientationHorizontal:
        return m_horizontalScrollbar ? m_horizontalScrollbar.get() : 0;
    case AccessibilityOrientationUndefined:
        return 0;
    }

    return 0;
}

// If this is WebKit1 then the native scroll view needs to return the
// AX information (because there are no scroll bar children in the FrameView object in WK1).
// In WebKit2, the FrameView object will return the AX information (because there are no platform widgets).
bool AXScrollView::isAttachment() const
{
    return false;
}

Widget* AXScrollView::widgetForAttachmentView() const
{
    return m_scrollView;
}

void AXScrollView::updateChildrenIfNecessary()
{
    if (m_childrenDirty)
        clearChildren();

    if (!m_haveChildren)
        addChildren();

    updateScrollbars();
}

void AXScrollView::updateScrollbars()
{
    if (!m_scrollView)
        return;

    if (m_scrollView->horizontalScrollbar() && !m_horizontalScrollbar) {
        m_horizontalScrollbar = addChildScrollbar(m_scrollView->horizontalScrollbar());
    } else if (!m_scrollView->horizontalScrollbar() && m_horizontalScrollbar) {
        removeChildScrollbar(m_horizontalScrollbar.get());
        m_horizontalScrollbar = nullptr;
    }

    if (m_scrollView->verticalScrollbar() && !m_verticalScrollbar) {
        m_verticalScrollbar = addChildScrollbar(m_scrollView->verticalScrollbar());
    } else if (!m_scrollView->verticalScrollbar() && m_verticalScrollbar) {
        removeChildScrollbar(m_verticalScrollbar.get());
        m_verticalScrollbar = nullptr;
    }
}

void AXScrollView::removeChildScrollbar(AXObject* scrollbar)
{
    size_t pos = m_children.find(scrollbar);
    if (pos != kNotFound) {
        m_children[pos]->detachFromParent();
        m_children.remove(pos);
    }
}

AXScrollbar* AXScrollView::addChildScrollbar(Scrollbar* scrollbar)
{
    if (!scrollbar)
        return 0;

    AXScrollbar* scrollBarObject = toAXScrollbar(axObjectCache().getOrCreate(scrollbar));
    scrollBarObject->setParent(this);
    m_children.append(scrollBarObject);
    return scrollBarObject;
}

void AXScrollView::clearChildren()
{
    AXObject::clearChildren();
    m_verticalScrollbar = nullptr;
    m_horizontalScrollbar = nullptr;
}

bool AXScrollView::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    // We just want to match whatever's returned by our web area, which is a child of this
    // object. Normally cached attribute values may only search up the tree. We can't just
    // call accessibilityIsIgnored on the web area, because the web area may search up its
    // ancestors and call this function recursively, and we'd loop until a stack overflow.

    // Instead, we first update the cached accessibilityIsIgnored value for this node to
    // false, call accessibilityIsIgnored on the web area, then return the mathcing value.
    m_cachedIsIgnored = false;
    m_lastModificationCount = axObjectCache().modificationCount();

    AXObject* webArea = webAreaObject();
    if (!webArea)
        return true;

    if (!webArea->accessibilityIsIgnored())
        return false;

    if (ignoredReasons)
        return webArea->computeAccessibilityIsIgnored(ignoredReasons);

    return true;
}

void AXScrollView::addChildren()
{
    ASSERT(!m_haveChildren);
    m_haveChildren = true;

    AXObject* webArea = webAreaObject();
    if (webArea && !webArea->accessibilityIsIgnored())
        m_children.append(webArea);

    updateScrollbars();
}

AXObject* AXScrollView::webAreaObject() const
{
    if (!m_scrollView || !m_scrollView->isFrameView())
        return 0;

    Document* doc = m_scrollView->frame().document();
    if (!doc || !doc->layoutView())
        return 0;

    return axObjectCache().getOrCreate(doc);
}

AXObject* AXScrollView::accessibilityHitTest(const IntPoint& point) const
{
    AXObject* webArea = webAreaObject();
    if (!webArea)
        return 0;

    if (m_horizontalScrollbar && m_horizontalScrollbar->elementRect().contains(point))
        return m_horizontalScrollbar.get();
    if (m_verticalScrollbar && m_verticalScrollbar->elementRect().contains(point))
        return m_verticalScrollbar.get();

    return webArea->accessibilityHitTest(point);
}

LayoutRect AXScrollView::elementRect() const
{
    if (!m_scrollView)
        return LayoutRect();

    return LayoutRect(m_scrollView->frameRect());
}

FrameView* AXScrollView::documentFrameView() const
{
    if (!m_scrollView || !m_scrollView->isFrameView())
        return 0;

    return m_scrollView;
}

AXObject* AXScrollView::computeParent() const
{
    if (!m_scrollView || !m_scrollView->isFrameView())
        return 0;

    // FIXME: Broken for OOPI.
    HTMLFrameOwnerElement* owner = m_scrollView->frame().deprecatedLocalOwner();
    if (owner && owner->layoutObject())
        return axObjectCache().getOrCreate(owner);

    return axObjectCache().getOrCreate(m_scrollView->frame().pagePopupOwner());
}

AXObject* AXScrollView::computeParentIfExists() const
{
    if (!m_scrollView || !m_scrollView->isFrameView())
        return 0;

    HTMLFrameOwnerElement* owner = m_scrollView->frame().deprecatedLocalOwner();
    if (owner && owner->layoutObject())
        return axObjectCache().get(owner);

    return axObjectCache().get(m_scrollView->frame().pagePopupOwner());
}

ScrollableArea* AXScrollView::getScrollableAreaIfScrollable() const
{
    return m_scrollView;
}

DEFINE_TRACE(AXScrollView)
{
    visitor->trace(m_scrollView);
    visitor->trace(m_horizontalScrollbar);
    visitor->trace(m_verticalScrollbar);
    AXObject::trace(visitor);
}

} // namespace blink
