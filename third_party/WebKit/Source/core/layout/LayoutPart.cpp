/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
 * Copyright (C) 2004, 2005, 2006, 2009 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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

#include "config.h"
#include "core/layout/LayoutPart.h"

#include "core/dom/AXObjectCache.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/LayoutView.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/PartPainter.h"
#include "core/plugins/PluginView.h"

namespace blink {

LayoutPart::LayoutPart(Element* element)
    : LayoutReplaced(element)
    // Reference counting is used to prevent the part from being destroyed
    // while inside the Widget code, which might not be able to handle that.
    , m_refCount(1)
{
    ASSERT(element);
    frameView()->addPart(this);
    setInline(false);
}

void LayoutPart::deref()
{
    if (--m_refCount <= 0)
        delete this;
}

void LayoutPart::willBeDestroyed()
{
    frameView()->removePart(this);

    if (AXObjectCache* cache = document().existingAXObjectCache()) {
        cache->childrenChanged(this->parent());
        cache->remove(this);
    }

    Element* element = toElement(node());
    if (element && element->isFrameOwnerElement())
        toHTMLFrameOwnerElement(element)->setWidget(nullptr);

    LayoutReplaced::willBeDestroyed();
}

void LayoutPart::destroy()
{
    willBeDestroyed();
    // We call clearNode here because LayoutPart is ref counted. This call to destroy
    // may not actually destroy the layout object. We can keep it around because of
    // references from the FrameView class. (The actual destruction of the class happens
    // in postDestroy() which is called from deref()).
    //
    // But, we've told the system we've destroyed the layoutObject, which happens when
    // the DOM node is destroyed. So there is a good change the DOM node this object
    // points too is invalid, so we have to clear the node so we make sure we don't
    // access it in the future.
    clearNode();
    deref();
}

LayoutPart::~LayoutPart()
{
    ASSERT(m_refCount <= 0);
}

Widget* LayoutPart::widget() const
{
    // Plugin widgets are stored in their DOM node. This includes HTMLAppletElement.
    Element* element = toElement(node());

    if (element && element->isFrameOwnerElement())
        return toHTMLFrameOwnerElement(element)->ownedWidget();

    return nullptr;
}

DeprecatedPaintLayerType LayoutPart::layerTypeRequired() const
{
    DeprecatedPaintLayerType type = LayoutReplaced::layerTypeRequired();
    if (type != NoDeprecatedPaintLayer)
        return type;
    return ForcedDeprecatedPaintLayer;
}

bool LayoutPart::requiresAcceleratedCompositing() const
{
    // There are two general cases in which we can return true. First, if this is a plugin
    // LayoutObject and the plugin has a layer, then we need a layer. Second, if this is
    // a LayoutObject with a contentDocument and that document needs a layer, then we need
    // a layer.
    if (widget() && widget()->isPluginView() && toPluginView(widget())->platformLayer())
        return true;

    if (!node() || !node()->isFrameOwnerElement())
        return false;

    HTMLFrameOwnerElement* element = toHTMLFrameOwnerElement(node());
    if (element->contentFrame() && element->contentFrame()->isRemoteFrame())
        return true;

    if (Document* contentDocument = element->contentDocument()) {
        if (LayoutView* view = contentDocument->layoutView())
            return view->usesCompositing();
    }

    return false;
}

bool LayoutPart::needsPreferredWidthsRecalculation() const
{
    if (LayoutReplaced::needsPreferredWidthsRecalculation())
        return true;
    return embeddedContentBox();
}

bool LayoutPart::nodeAtPointOverWidget(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction action)
{
    bool hadResult = result.innerNode();
    bool inside = LayoutReplaced::nodeAtPoint(result, locationInContainer, accumulatedOffset, action);

    // Check to see if we are really over the widget itself (and not just in the border/padding area).
    if ((inside || result.isRectBasedTest()) && !hadResult && result.innerNode() == node())
        result.setIsOverWidget(contentBoxRect().contains(result.localPoint()));
    return inside;
}

bool LayoutPart::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction action)
{
    if (!widget() || !widget()->isFrameView() || !result.hitTestRequest().allowsChildFrameContent())
        return nodeAtPointOverWidget(result, locationInContainer, accumulatedOffset, action);

    FrameView* childFrameView = toFrameView(widget());
    LayoutView* childRoot = childFrameView->layoutView();

    if (visibleToHitTestRequest(result.hitTestRequest()) && childRoot) {
        LayoutPoint adjustedLocation = accumulatedOffset + location();
        LayoutPoint contentOffset = LayoutPoint(borderLeft() + paddingLeft(), borderTop() + paddingTop()) - LayoutSize(childFrameView->scrollOffset());
        HitTestLocation newHitTestLocation(locationInContainer, -adjustedLocation - contentOffset);
        HitTestRequest newHitTestRequest(result.hitTestRequest().type() | HitTestRequest::ChildFrameHitTest);
        HitTestResult childFrameResult(newHitTestRequest, newHitTestLocation);

        // The frame's layout and style must be up-to-date if we reach here.
        bool isInsideChildFrame = childRoot->hitTestNoLifecycleUpdate(childFrameResult);

        if (result.hitTestRequest().listBased()) {
            result.append(childFrameResult);
        } else if (isInsideChildFrame) {
            // Force the result not to be cacheable because the parent
            // frame should not cache this result; as it won't be notified of
            // changes in the child.
            childFrameResult.setCacheable(false);
            result = childFrameResult;
        }

        if (isInsideChildFrame)
            return true;
    }

    return nodeAtPointOverWidget(result, locationInContainer, accumulatedOffset, action);
}

CompositingReasons LayoutPart::additionalCompositingReasons() const
{
    if (requiresAcceleratedCompositing())
        return CompositingReasonIFrame;
    return CompositingReasonNone;
}

void LayoutPart::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutReplaced::styleDidChange(diff, oldStyle);
    Widget* widget = this->widget();

    if (!widget)
        return;

    // If the iframe has custom scrollbars, recalculate their style.
    if (widget && widget->isFrameView())
        toFrameView(widget)->recalculateCustomScrollbarStyle();

    if (style()->visibility() != VISIBLE) {
        widget->hide();
    } else {
        widget->show();
    }
}

void LayoutPart::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);
    clearNeedsLayout();
}

void LayoutPart::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    PartPainter(*this).paint(paintInfo, paintOffset);
}

void LayoutPart::paintContents(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    PartPainter(*this).paintContents(paintInfo, paintOffset);
}

CursorDirective LayoutPart::getCursor(const LayoutPoint& point, Cursor& cursor) const
{
    if (widget() && widget()->isPluginView()) {
        // A plugin is responsible for setting the cursor when the pointer is over it.
        return DoNotSetCursor;
    }
    return LayoutReplaced::getCursor(point, cursor);
}

void LayoutPart::updateOnWidgetChange()
{
    Widget* widget = this->widget();
    if (!widget)
        return;

    if (!style())
        return;

    if (!needsLayout())
        updateWidgetGeometry();

    if (style()->visibility() != VISIBLE) {
        widget->hide();
    } else {
        widget->show();
        // FIXME: Why do we issue a full paint invalidation in this case, but not the other?
        setShouldDoFullPaintInvalidation();
    }
}

void LayoutPart::updateWidgetPosition()
{
    Widget* widget = this->widget();
    if (!widget || !node()) // Check the node in case destroy() has been called.
        return;

    bool boundsChanged = updateWidgetGeometry();

    // If the frame bounds got changed, or if view needs layout (possibly indicating
    // content size is wrong) we have to do a layout to set the right widget size.
    if (widget && widget->isFrameView()) {
        FrameView* frameView = toFrameView(widget);
        // Check the frame's page to make sure that the frame isn't in the process of being destroyed.
        if ((boundsChanged || frameView->needsLayout()) && frameView->frame().page())
            frameView->layout();
    }
}

void LayoutPart::widgetPositionsUpdated()
{
    Widget* widget = this->widget();
    if (!widget)
        return;
    widget->widgetPositionsUpdated();
}

bool LayoutPart::updateWidgetGeometry()
{
    Widget* widget = this->widget();
    ASSERT(widget);

    LayoutRect contentBox = contentBoxRect();
    LayoutRect absoluteContentBox(localToAbsoluteQuad(FloatQuad(contentBox)).boundingBox());
    if (widget->isFrameView()) {
        contentBox.setLocation(absoluteContentBox.location());
        return setWidgetGeometry(contentBox);
    }

    return setWidgetGeometry(absoluteContentBox);
}

// Widgets are always placed on integer boundaries, so rounding the size is actually
// the desired behavior. This function is here because it's otherwise seldom what we
// want to do with a LayoutRect.
static inline IntRect roundedIntRect(const LayoutRect& rect)
{
    return IntRect(roundedIntPoint(rect.location()), roundedIntSize(rect.size()));
}

bool LayoutPart::setWidgetGeometry(const LayoutRect& frame)
{
    if (!node())
        return false;

    Widget* widget = this->widget();
    ASSERT(widget);

    IntRect newFrame = roundedIntRect(frame);

    if (widget->frameRect() == newFrame)
        return false;

    RefPtr<LayoutPart> protector(this);
    RefPtrWillBeRawPtr<Node> protectedNode(node());
    widget->setFrameRect(newFrame);
    return widget->frameRect().size() != newFrame.size();
}

void LayoutPart::invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState& paintInvalidationState)
{
    if (widget() && widget()->isFrameView()) {
        FrameView* childFrameView = toFrameView(widget());
        PaintInvalidationState childViewPaintInvalidationState(*childFrameView->layoutView(), paintInvalidationState);
        toFrameView(widget())->invalidateTreeIfNeeded(childViewPaintInvalidationState);
    }

    LayoutReplaced::invalidatePaintOfSubtreesIfNeeded(paintInvalidationState);
}

}
