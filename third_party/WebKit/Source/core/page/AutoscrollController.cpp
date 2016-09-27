/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2012 Digia Plc. and/or its subsidiary(-ies)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/AutoscrollController.h"

#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutListBox.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "wtf/CurrentTime.h"

namespace blink {

// Delay time in second for start autoscroll if pointer is in border edge of scrollable element.
static double autoscrollDelay = 0.2;

PassOwnPtr<AutoscrollController> AutoscrollController::create(Page& page)
{
    return adoptPtr(new AutoscrollController(page));
}

AutoscrollController::AutoscrollController(Page& page)
    : m_page(page)
    , m_autoscrollLayoutObject(nullptr)
    , m_autoscrollType(NoAutoscroll)
    , m_dragAndDropAutoscrollStartTime(0)
{
}

bool AutoscrollController::autoscrollInProgress() const
{
    return m_autoscrollType == AutoscrollForSelection;
}

bool AutoscrollController::autoscrollInProgress(const LayoutBox* layoutObject) const
{
    return m_autoscrollLayoutObject == layoutObject;
}

void AutoscrollController::startAutoscrollForSelection(LayoutObject* layoutObject)
{
    // We don't want to trigger the autoscroll or the panScroll if it's already active
    if (m_autoscrollType != NoAutoscroll)
        return;
    LayoutBox* scrollable = LayoutBox::findAutoscrollable(layoutObject);
    if (!scrollable)
        scrollable = layoutObject->isListBox() ? toLayoutListBox(layoutObject) : nullptr;
    if (!scrollable)
        return;
    m_autoscrollType = AutoscrollForSelection;
    m_autoscrollLayoutObject = scrollable;
    startAutoscroll();
}

void AutoscrollController::stopAutoscroll()
{
    LayoutBox* scrollable = m_autoscrollLayoutObject;
    m_autoscrollLayoutObject = nullptr;

    if (!scrollable)
        return;

    scrollable->stopAutoscroll();
#if OS(WIN)
    if (panScrollInProgress()) {
        if (FrameView* view = scrollable->frame()->view()) {
            view->setCursor(pointerCursor());
        }
    }
#endif

    m_autoscrollType = NoAutoscroll;
}

void AutoscrollController::stopAutoscrollIfNeeded(LayoutObject* layoutObject)
{
    if (m_autoscrollLayoutObject != layoutObject)
        return;
    m_autoscrollLayoutObject = nullptr;
    m_autoscrollType = NoAutoscroll;
}

void AutoscrollController::updateAutoscrollLayoutObject()
{
    if (!m_autoscrollLayoutObject)
        return;

    LayoutObject* layoutObject = m_autoscrollLayoutObject;

#if OS(WIN)
    HitTestResult hitTest = layoutObject->frame()->eventHandler().hitTestResultAtPoint(m_panScrollStartPos, HitTestRequest::ReadOnly | HitTestRequest::Active);

    if (Node* nodeAtPoint = hitTest.innerNode())
        layoutObject = nodeAtPoint->layoutObject();
#endif

    while (layoutObject && !(layoutObject->isBox() && toLayoutBox(layoutObject)->canAutoscroll()))
        layoutObject = layoutObject->parent();
    m_autoscrollLayoutObject = layoutObject && layoutObject->isBox() ? toLayoutBox(layoutObject) : nullptr;
}

void AutoscrollController::updateDragAndDrop(Node* dropTargetNode, const IntPoint& eventPosition, double eventTime)
{
    if (!dropTargetNode || !dropTargetNode->layoutObject()) {
        stopAutoscroll();
        return;
    }

    if (m_autoscrollLayoutObject && m_autoscrollLayoutObject->frame() != dropTargetNode->layoutObject()->frame())
        return;

    LayoutBox* scrollable = LayoutBox::findAutoscrollable(dropTargetNode->layoutObject());
    if (!scrollable) {
        stopAutoscroll();
        return;
    }

    Page* page = scrollable->frame() ? scrollable->frame()->page() : nullptr;
    if (!page) {
        stopAutoscroll();
        return;
    }

    IntSize offset = scrollable->calculateAutoscrollDirection(eventPosition);
    if (offset.isZero()) {
        stopAutoscroll();
        return;
    }

    m_dragAndDropAutoscrollReferencePosition = eventPosition + offset;

    if (m_autoscrollType == NoAutoscroll) {
        m_autoscrollType = AutoscrollForDragAndDrop;
        m_autoscrollLayoutObject = scrollable;
        m_dragAndDropAutoscrollStartTime = eventTime;
        startAutoscroll();
    } else if (m_autoscrollLayoutObject != scrollable) {
        m_dragAndDropAutoscrollStartTime = eventTime;
        m_autoscrollLayoutObject = scrollable;
    }
}

#if OS(WIN)
void AutoscrollController::handleMouseReleaseForPanScrolling(LocalFrame* frame, const PlatformMouseEvent& mouseEvent)
{
    if (!frame->isMainFrame())
        return;
    switch (m_autoscrollType) {
    case AutoscrollForPan:
        if (mouseEvent.button() == MiddleButton)
            m_autoscrollType = AutoscrollForPanCanStop;
        break;
    case AutoscrollForPanCanStop:
        stopAutoscroll();
        break;
    }
}

bool AutoscrollController::panScrollInProgress() const
{
    return m_autoscrollType == AutoscrollForPanCanStop || m_autoscrollType == AutoscrollForPan;
}

void AutoscrollController::startPanScrolling(LayoutBox* scrollable, const IntPoint& lastKnownMousePosition)
{
    // We don't want to trigger the autoscroll or the panScroll if it's already active
    if (m_autoscrollType != NoAutoscroll)
        return;

    m_autoscrollType = AutoscrollForPan;
    m_autoscrollLayoutObject = scrollable;
    m_panScrollStartPos = lastKnownMousePosition;

    startAutoscroll();
}
#else
bool AutoscrollController::panScrollInProgress() const
{
    return false;
}
#endif

// FIXME: This would get get better animation fidelity if it used the monotonicFrameBeginTime instead
// of WTF::currentTime().
void AutoscrollController::animate(double)
{
    if (!m_autoscrollLayoutObject) {
        stopAutoscroll();
        return;
    }

    EventHandler& eventHandler = m_autoscrollLayoutObject->frame()->eventHandler();
    switch (m_autoscrollType) {
    case AutoscrollForDragAndDrop:
        if (WTF::currentTime() - m_dragAndDropAutoscrollStartTime > autoscrollDelay)
            m_autoscrollLayoutObject->autoscroll(m_dragAndDropAutoscrollReferencePosition);
        break;
    case AutoscrollForSelection:
        if (!eventHandler.mousePressed()) {
            stopAutoscroll();
            return;
        }
        eventHandler.updateSelectionForMouseDrag();
        m_autoscrollLayoutObject->autoscroll(eventHandler.lastKnownMousePosition());
        break;
    case NoAutoscroll:
        break;
#if OS(WIN)
    case AutoscrollForPanCanStop:
    case AutoscrollForPan:
        if (!panScrollInProgress()) {
            stopAutoscroll();
            return;
        }
        if (FrameView* view = m_autoscrollLayoutObject->frame()->view())
            updatePanScrollState(view, eventHandler.lastKnownMousePosition());
        m_autoscrollLayoutObject->panScroll(m_panScrollStartPos);
        break;
#endif
    }
    if (m_autoscrollType != NoAutoscroll)
        m_page.chromeClient().scheduleAnimation();
}

void AutoscrollController::startAutoscroll()
{
    m_page.chromeClient().scheduleAnimation();
}

#if OS(WIN)
void AutoscrollController::updatePanScrollState(FrameView* view, const IntPoint& lastKnownMousePosition)
{
    // At the original click location we draw a 4 arrowed icon. Over this icon there won't be any scroll
    // So we don't want to change the cursor over this area
    bool east = m_panScrollStartPos.x() < (lastKnownMousePosition.x() - noPanScrollRadius);
    bool west = m_panScrollStartPos.x() > (lastKnownMousePosition.x() + noPanScrollRadius);
    bool north = m_panScrollStartPos.y() > (lastKnownMousePosition.y() + noPanScrollRadius);
    bool south = m_panScrollStartPos.y() < (lastKnownMousePosition.y() - noPanScrollRadius);

    if (m_autoscrollType == AutoscrollForPan && (east || west || north || south))
        m_autoscrollType = AutoscrollForPanCanStop;

    if (north) {
        if (east)
            view->setCursor(northEastPanningCursor());
        else if (west)
            view->setCursor(northWestPanningCursor());
        else
            view->setCursor(northPanningCursor());
    } else if (south) {
        if (east)
            view->setCursor(southEastPanningCursor());
        else if (west)
            view->setCursor(southWestPanningCursor());
        else
            view->setCursor(southPanningCursor());
    } else if (east)
        view->setCursor(eastPanningCursor());
    else if (west)
        view->setCursor(westPanningCursor());
    else
        view->setCursor(middlePanningCursor());
}
#endif

} // namespace blink
