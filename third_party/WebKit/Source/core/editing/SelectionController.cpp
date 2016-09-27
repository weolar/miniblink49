/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2012 Digia Plc. and/or its subsidiary(-ies)
 * Copyright (C) 2015 Google Inc. All rights reserved.
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
#include "core/editing/SelectionController.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/events/Event.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutView.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {
PassOwnPtrWillBeRawPtr<SelectionController> SelectionController::create(LocalFrame& frame)
{
    return adoptPtrWillBeNoop(new SelectionController(frame));
}

SelectionController::SelectionController(LocalFrame& frame)
    : m_frame(&frame)
    , m_mouseDownMayStartSelect(false)
    , m_mouseDownWasSingleClickInSelection(false)
    , m_selectionState(SelectionState::HaveNotStartedSelection)
{
}

DEFINE_TRACE(SelectionController)
{
    visitor->trace(m_frame);
}

namespace {

void setSelectionIfNeeded(FrameSelection& selection, const VisibleSelection& newSelection)
{
    if (VisibleSelection::InDOMTree::equalSelections(selection.selection(), newSelection))
        return;
    selection.setSelection(newSelection);
}

bool dispatchSelectStart(Node* node)
{
    if (!node || !node->layoutObject())
        return true;

    return node->dispatchEvent(Event::createCancelableBubble(EventTypeNames::selectstart));
}

template <typename Strategy>
VisibleSelection expandSelectionToRespectUserSelectAllAlgorithm(Node* targetNode, const VisibleSelection& selection)
{
    using PositionType = typename Strategy::PositionType;

    Node* rootUserSelectAll = PositionType::rootUserSelectAllForNode(targetNode);
    if (!rootUserSelectAll)
        return selection;

    VisibleSelection newSelection(selection);
    newSelection.setBase(PositionType::beforeNode(rootUserSelectAll).upstream(CanCrossEditingBoundary));
    newSelection.setExtent(PositionType::afterNode(rootUserSelectAll).downstream(CanCrossEditingBoundary));

    return newSelection;
}

VisibleSelection expandSelectionToRespectUserSelectAll(Node* targetNode, const VisibleSelection& selection)
{
    if (RuntimeEnabledFeatures::selectionForComposedTreeEnabled())
        return expandSelectionToRespectUserSelectAllAlgorithm<VisibleSelection::InComposedTree>(targetNode, selection);
    return expandSelectionToRespectUserSelectAllAlgorithm<VisibleSelection::InDOMTree>(targetNode, selection);
}

bool expandSelectionUsingGranularity(VisibleSelection& selection, TextGranularity granularity)
{
    if (RuntimeEnabledFeatures::selectionForComposedTreeEnabled())
        return selection.expandUsingGranularityInComposedTree(granularity);
    return selection.expandUsingGranularity(granularity);
}

template <typename Strategy>
static int textDistance(const PositionAlgorithm<Strategy>& start, const PositionAlgorithm<Strategy>& end)
{
    return TextIteratorAlgorithm<Strategy>::rangeLength(start, end, true);
}

bool canMouseDownStartSelect(Node* node)
{
    if (!node || !node->layoutObject())
        return true;

    if (!node->canStartSelection())
        return false;

    return true;
}

} // namespace

template <typename Strategy>
bool SelectionController::handleMousePressEventSingleClickAlgorithm(const MouseEventWithHitTestResults& event)
{
    TRACE_EVENT0("blink", "SelectionController::handleMousePressEventSingleClick");
    using PositionType = typename Strategy::PositionType;

    m_frame->document()->updateLayoutIgnorePendingStylesheets();
    Node* innerNode = event.innerNode();
    if (!(innerNode && innerNode->layoutObject() && m_mouseDownMayStartSelect))
        return false;

    // Extend the selection if the Shift key is down, unless the click is in a link.
    bool extendSelection = event.event().shiftKey() && !event.isOverLink();

    // Don't restart the selection when the mouse is pressed on an
    // existing selection so we can allow for text dragging.
    if (FrameView* view = m_frame->view()) {
        LayoutPoint vPoint = view->rootFrameToContents(event.event().position());
        if (!extendSelection && selection().contains(vPoint)) {
            m_mouseDownWasSingleClickInSelection = true;
            return false;
        }
    }

    PositionWithAffinity eventPos = innerNode->layoutObject()->positionForPoint(event.localPoint());
    VisiblePosition visiblePos(Strategy::toPositionType(eventPos.position()), eventPos.affinity());
    if (visiblePos.isNull())
        visiblePos = VisiblePosition(firstPositionInOrBeforeNode(innerNode), DOWNSTREAM);
    PositionType pos = Strategy::toPositionType(visiblePos.deepEquivalent());

    VisibleSelection newSelection = selection().selection();
    TextGranularity granularity = CharacterGranularity;

    if (extendSelection && newSelection.isCaretOrRange()) {
        VisibleSelection selectionInUserSelectAll(expandSelectionToRespectUserSelectAll(innerNode, VisibleSelection(VisiblePosition(pos))));
        if (selectionInUserSelectAll.isRange()) {
            if (Strategy::selectionStart(selectionInUserSelectAll).compareTo(Strategy::selectionStart(newSelection)) < 0)
                pos = Strategy::selectionStart(selectionInUserSelectAll);
            else if (Strategy::selectionEnd(newSelection).compareTo(Strategy::selectionEnd(selectionInUserSelectAll)) < 0)
                pos = Strategy::selectionEnd(selectionInUserSelectAll);
        }

        if (!m_frame->editor().behavior().shouldConsiderSelectionAsDirectional()) {
            if (pos.isNotNull()) {
                // See <rdar://problem/3668157> REGRESSION (Mail): shift-click deselects when selection
                // was created right-to-left
                PositionType start = Strategy::selectionStart(newSelection);
                PositionType end = Strategy::selectionEnd(newSelection);
                int distanceToStart = textDistance(start, pos);
                int distanceToEnd = textDistance(pos, end);
                if (distanceToStart <= distanceToEnd)
                    newSelection = VisibleSelection(end, pos);
                else
                    newSelection = VisibleSelection(start, pos);
            }
        } else {
            newSelection.setExtent(pos);
        }

        if (selection().granularity() != CharacterGranularity) {
            granularity = selection().granularity();
            expandSelectionUsingGranularity(newSelection, selection().granularity());
        }
    } else {
        newSelection = expandSelectionToRespectUserSelectAll(innerNode, VisibleSelection(visiblePos));
    }

    // Updating the selection is considered side-effect of the event and so it doesn't impact the handled state.
    updateSelectionForMouseDownDispatchingSelectStart(innerNode, newSelection, granularity);
    return false;
}

template <typename Strategy>
void SelectionController::updateSelectionForMouseDragAlgorithm(const HitTestResult& hitTestResult, Node* mousePressNode, const LayoutPoint& dragStartPos, const IntPoint& lastKnownMousePosition)
{
    using PositionType = typename Strategy::PositionType;

    if (!m_mouseDownMayStartSelect)
        return;

    Node* target = hitTestResult.innerNode();
    if (!target)
        return;

    PositionWithAffinity rawTargetPosition = selection().selection().positionRespectingEditingBoundary(hitTestResult.localPoint(), target);
    VisiblePosition targetPosition = VisiblePosition(Strategy::toPositionType(rawTargetPosition.position()), rawTargetPosition.affinity());
    // Don't modify the selection if we're not on a node.
    if (targetPosition.isNull())
        return;

    // Restart the selection if this is the first mouse move. This work is usually
    // done in handleMousePressEvent, but not if the mouse press was on an existing selection.
    VisibleSelection newSelection = selection().selection();

    // Special case to limit selection to the containing block for SVG text.
    // FIXME: Isn't there a better non-SVG-specific way to do this?
    if (Node* selectionBaseNode = Strategy::selectionBase(newSelection).deprecatedNode()) {
        if (LayoutObject* selectionBaseLayoutObject = selectionBaseNode->layoutObject()) {
            if (selectionBaseLayoutObject->isSVGText()) {
                if (target->layoutObject()->containingBlock() != selectionBaseLayoutObject->containingBlock())
                    return;
            }
        }
    }

    if (m_selectionState == SelectionState::HaveNotStartedSelection && !dispatchSelectStart(target))
        return;

    if (m_selectionState != SelectionState::ExtendedSelection) {
        // Always extend selection here because it's caused by a mouse drag
        m_selectionState = SelectionState::ExtendedSelection;
        newSelection = VisibleSelection(targetPosition);
    }

    if (RuntimeEnabledFeatures::userSelectAllEnabled()) {
        Node* rootUserSelectAllForMousePressNode = Position::rootUserSelectAllForNode(mousePressNode);
        if (rootUserSelectAllForMousePressNode && rootUserSelectAllForMousePressNode == Position::rootUserSelectAllForNode(target)) {
            newSelection.setBase(PositionType::beforeNode(rootUserSelectAllForMousePressNode).upstream(CanCrossEditingBoundary));
            newSelection.setExtent(PositionType::afterNode(rootUserSelectAllForMousePressNode).downstream(CanCrossEditingBoundary));
        } else {
            // Reset base for user select all when base is inside user-select-all area and extent < base.
            if (rootUserSelectAllForMousePressNode) {
                PositionType eventPosition = Strategy::toPositionType(target->layoutObject()->positionForPoint(hitTestResult.localPoint()).position());
                PositionType dragStartPosition = Strategy::toPositionType(mousePressNode->layoutObject()->positionForPoint(dragStartPos).position());
                if (eventPosition.compareTo(dragStartPosition) < 0)
                    newSelection.setBase(PositionType::afterNode(rootUserSelectAllForMousePressNode).downstream(CanCrossEditingBoundary));
            }

            Node* rootUserSelectAllForTarget = Position::rootUserSelectAllForNode(target);
            if (rootUserSelectAllForTarget && mousePressNode->layoutObject() && Strategy::toPositionType(target->layoutObject()->positionForPoint(hitTestResult.localPoint()).position()).compareTo(Strategy::toPositionType(mousePressNode->layoutObject()->positionForPoint(dragStartPos).position())) < 0)
                newSelection.setExtent(PositionType::beforeNode(rootUserSelectAllForTarget).upstream(CanCrossEditingBoundary));
            else if (rootUserSelectAllForTarget && mousePressNode->layoutObject())
                newSelection.setExtent(PositionType::afterNode(rootUserSelectAllForTarget).downstream(CanCrossEditingBoundary));
            else
                newSelection.setExtent(targetPosition);
        }
    } else {
        newSelection.setExtent(targetPosition);
    }

    if (selection().granularity() != CharacterGranularity)
        expandSelectionUsingGranularity(newSelection, selection().granularity());

    selection().setNonDirectionalSelectionIfNeeded(newSelection, selection().granularity(),
        FrameSelection::AdjustEndpointsAtBidiBoundary);
}

bool SelectionController::updateSelectionForMouseDownDispatchingSelectStart(Node* targetNode, const VisibleSelection& selection, TextGranularity granularity)
{
    if (Position::nodeIsUserSelectNone(targetNode))
        return false;

    if (!dispatchSelectStart(targetNode))
        return false;

    if (selection.isRange()) {
        m_selectionState = SelectionState::ExtendedSelection;
    } else {
        granularity = CharacterGranularity;
        m_selectionState = SelectionState::PlacedCaret;
    }

    this->selection().setNonDirectionalSelectionIfNeeded(selection, granularity);

    return true;
}

void SelectionController::selectClosestWordFromHitTestResult(const HitTestResult& result, AppendTrailingWhitespace appendTrailingWhitespace)
{
    Node* innerNode = result.innerNode();
    VisibleSelection newSelection;

    if (!innerNode || !innerNode->layoutObject())
        return;

    VisiblePosition pos(innerNode->layoutObject()->positionForPoint(result.localPoint()));
    if (pos.isNotNull()) {
        newSelection = VisibleSelection(pos);
        expandSelectionUsingGranularity(newSelection, WordGranularity);
    }

    if (appendTrailingWhitespace == AppendTrailingWhitespace::ShouldAppend && newSelection.isRange())
        newSelection.appendTrailingWhitespace();

    updateSelectionForMouseDownDispatchingSelectStart(innerNode, expandSelectionToRespectUserSelectAll(innerNode, newSelection), WordGranularity);
}

void SelectionController::selectClosestMisspellingFromHitTestResult(const HitTestResult& result, AppendTrailingWhitespace appendTrailingWhitespace)
{
    Node* innerNode = result.innerNode();
    VisibleSelection newSelection;

    if (!innerNode || !innerNode->layoutObject())
        return;

    VisiblePosition pos(innerNode->layoutObject()->positionForPoint(result.localPoint()));
    Position start = pos.deepEquivalent();
    Position end = pos.deepEquivalent();
    if (pos.isNotNull()) {
        DocumentMarkerVector markers = innerNode->document().markers().markersInRange(makeRange(pos, pos).get(), DocumentMarker::MisspellingMarkers());
        if (markers.size() == 1) {
            start.moveToOffset(markers[0]->startOffset());
            end.moveToOffset(markers[0]->endOffset());
            newSelection = VisibleSelection(start, end);
        }
    }

    if (appendTrailingWhitespace == AppendTrailingWhitespace::ShouldAppend && newSelection.isRange())
        newSelection.appendTrailingWhitespace();

    updateSelectionForMouseDownDispatchingSelectStart(innerNode, expandSelectionToRespectUserSelectAll(innerNode, newSelection), WordGranularity);
}

void SelectionController::selectClosestWordFromMouseEvent(const MouseEventWithHitTestResults& result)
{
    if (!m_mouseDownMayStartSelect)
        return;

    selectClosestWordFromHitTestResult(result.hitTestResult(),
        (result.event().clickCount() == 2 && m_frame->editor().isSelectTrailingWhitespaceEnabled()) ? AppendTrailingWhitespace::ShouldAppend : AppendTrailingWhitespace::DontAppend);
}

void SelectionController::selectClosestMisspellingFromMouseEvent(const MouseEventWithHitTestResults& result)
{
    if (!m_mouseDownMayStartSelect)
        return;

    selectClosestMisspellingFromHitTestResult(result.hitTestResult(),
        (result.event().clickCount() == 2 && m_frame->editor().isSelectTrailingWhitespaceEnabled()) ? AppendTrailingWhitespace::ShouldAppend : AppendTrailingWhitespace::DontAppend);

}

void SelectionController::selectClosestWordOrLinkFromMouseEvent(const MouseEventWithHitTestResults& result)
{
    if (!result.hitTestResult().isLiveLink())
        return selectClosestWordFromMouseEvent(result);

    Node* innerNode = result.innerNode();

    if (!innerNode || !innerNode->layoutObject() || !m_mouseDownMayStartSelect)
        return;

    VisibleSelection newSelection;
    Element* URLElement = result.hitTestResult().URLElement();
    VisiblePosition pos(innerNode->layoutObject()->positionForPoint(result.localPoint()));
    if (pos.isNotNull() && pos.deepEquivalent().deprecatedNode()->isDescendantOf(URLElement))
        newSelection = VisibleSelection::selectionFromContentsOfNode(URLElement);

    updateSelectionForMouseDownDispatchingSelectStart(innerNode, expandSelectionToRespectUserSelectAll(innerNode, newSelection), WordGranularity);
}

bool SelectionController::handleMousePressEventDoubleClick(const MouseEventWithHitTestResults& event)
{
    TRACE_EVENT0("blink", "SelectionController::handleMousePressEventDoubleClick");

    if (event.event().button() != LeftButton)
        return false;

    if (selection().isRange()) {
        // A double-click when range is already selected
        // should not change the selection.  So, do not call
        // selectClosestWordFromMouseEvent, but do set
        // m_beganSelectingText to prevent handleMouseReleaseEvent
        // from setting caret selection.
        m_selectionState = SelectionState::ExtendedSelection;
    } else {
        selectClosestWordFromMouseEvent(event);
    }
    return true;
}

bool SelectionController::handleMousePressEventTripleClick(const MouseEventWithHitTestResults& event)
{
    TRACE_EVENT0("blink", "SelectionController::handleMousePressEventTripleClick");

    if (event.event().button() != LeftButton)
        return false;

    Node* innerNode = event.innerNode();
    if (!(innerNode && innerNode->layoutObject() && m_mouseDownMayStartSelect))
        return false;

    VisibleSelection newSelection;
    VisiblePosition pos(innerNode->layoutObject()->positionForPoint(event.localPoint()));
    if (pos.isNotNull()) {
        newSelection = VisibleSelection(pos);
        expandSelectionUsingGranularity(newSelection, ParagraphGranularity);
    }

    return updateSelectionForMouseDownDispatchingSelectStart(innerNode, expandSelectionToRespectUserSelectAll(innerNode, newSelection), ParagraphGranularity);
}

bool SelectionController::handleMousePressEventSingleClick(const MouseEventWithHitTestResults& event)
{
    if (RuntimeEnabledFeatures::selectionForComposedTreeEnabled())
        return handleMousePressEventSingleClickAlgorithm<VisibleSelection::InComposedTree>(event);
    return handleMousePressEventSingleClickAlgorithm<VisibleSelection::InDOMTree>(event);
}

void SelectionController::handleMousePressEvent(const MouseEventWithHitTestResults& event)
{
    // If we got the event back, that must mean it wasn't prevented,
    // so it's allowed to start a drag or selection if it wasn't in a scrollbar.
    m_mouseDownMayStartSelect = canMouseDownStartSelect(event.innerNode()) && !event.scrollbar();
    m_mouseDownWasSingleClickInSelection = false;
}

void SelectionController::handleMouseDraggedEvent(const MouseEventWithHitTestResults& event, const IntPoint& mouseDownPos, const LayoutPoint& dragStartPos, Node* mousePressNode, const IntPoint& lastKnownMousePosition)
{
    if (m_selectionState != SelectionState::ExtendedSelection) {
        HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::Active);
        HitTestResult result(request, mouseDownPos);
        m_frame->document()->layoutView()->hitTest(result);

        updateSelectionForMouseDrag(result, mousePressNode, dragStartPos, lastKnownMousePosition);
    }
    updateSelectionForMouseDrag(event.hitTestResult(), mousePressNode, dragStartPos, lastKnownMousePosition);
}

void SelectionController::updateSelectionForMouseDrag(Node* mousePressNode, const LayoutPoint& dragStartPos, const IntPoint& lastKnownMousePosition)
{
    FrameView* view = m_frame->view();
    if (!view)
        return;
    LayoutView* layoutObject = m_frame->contentLayoutObject();
    if (!layoutObject)
        return;

    HitTestRequest request(HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::Move);
    HitTestResult result(request, view->rootFrameToContents(lastKnownMousePosition));
    layoutObject->hitTest(result);
    updateSelectionForMouseDrag(result, mousePressNode, dragStartPos, lastKnownMousePosition);
}

void SelectionController::updateSelectionForMouseDrag(const HitTestResult& hitTestResult, Node* mousePressNode, const LayoutPoint& dragStartPos, const IntPoint& lastKnownMousePosition)
{
    if (RuntimeEnabledFeatures::selectionForComposedTreeEnabled())
        return updateSelectionForMouseDragAlgorithm<VisibleSelection::InComposedTree>(hitTestResult, mousePressNode, dragStartPos, lastKnownMousePosition);
    updateSelectionForMouseDragAlgorithm<VisibleSelection::InDOMTree>(hitTestResult, mousePressNode, dragStartPos, lastKnownMousePosition);
}

bool SelectionController::handleMouseReleaseEvent(const MouseEventWithHitTestResults& event, const LayoutPoint& dragStartPos)
{
    bool handled = false;
    m_mouseDownMayStartSelect = false;
    // Clear the selection if the mouse didn't move after the last mouse
    // press and it's not a context menu click.  We do this so when clicking
    // on the selection, the selection goes away.  However, if we are
    // editing, place the caret.
    if (m_mouseDownWasSingleClickInSelection && m_selectionState != SelectionState::ExtendedSelection
        && dragStartPos == event.event().position()
        && selection().isRange()
        && event.event().button() != RightButton) {
        VisibleSelection newSelection;
        Node* node = event.innerNode();
        bool caretBrowsing = m_frame->settings() && m_frame->settings()->caretBrowsingEnabled();
        if (node && node->layoutObject() && (caretBrowsing || node->hasEditableStyle())) {
            VisiblePosition pos = VisiblePosition(node->layoutObject()->positionForPoint(event.localPoint()));
            newSelection = VisibleSelection(pos);
        }

        setSelectionIfNeeded(selection(), newSelection);

        handled = true;
    }

    selection().notifyLayoutObjectOfSelectionChange(UserTriggered);

    selection().selectFrameElementInParentIfFullySelected();

    if (event.event().button() == MiddleButton && !event.isOverLink()) {
        // Ignore handled, since we want to paste to where the caret was placed anyway.
        handled = handlePasteGlobalSelection(event.event()) || handled;
    }

    return handled;
}


bool SelectionController::handlePasteGlobalSelection(const PlatformMouseEvent& mouseEvent)
{
    // If the event was a middle click, attempt to copy global selection in after
    // the newly set caret position.
    //
    // This code is called from either the mouse up or mouse down handling. There
    // is some debate about when the global selection is pasted:
    //   xterm: pastes on up.
    //   GTK: pastes on down.
    //   Qt: pastes on up.
    //   Firefox: pastes on up.
    //   Chromium: pastes on up.
    //
    // There is something of a webcompat angle to this well, as highlighted by
    // crbug.com/14608. Pages can clear text boxes 'onclick' and, if we paste on
    // down then the text is pasted just before the onclick handler runs and
    // clears the text box. So it's important this happens after the event
    // handlers have been fired.
    if (mouseEvent.type() != PlatformEvent::MouseReleased)
        return false;

    if (!m_frame->page())
        return false;
    Frame* focusFrame = m_frame->page()->focusController().focusedOrMainFrame();
    // Do not paste here if the focus was moved somewhere else.
    if (m_frame == focusFrame && m_frame->editor().behavior().supportsGlobalSelection())
        return m_frame->editor().command("PasteGlobalSelection").execute();

    return false;
}

bool SelectionController::handleGestureLongPress(const PlatformGestureEvent& gestureEvent, const HitTestResult& hitTestResult)
{
#if OS(ANDROID)
    bool shouldLongPressSelectWord = true;
#else
    bool shouldLongPressSelectWord = m_frame->settings() && m_frame->settings()->touchEditingEnabled();
#endif
    if (!shouldLongPressSelectWord)
        return false;

    Node* innerNode = hitTestResult.innerNode();
    if (hitTestResult.isLiveLink() || !innerNode || !(innerNode->isContentEditable() || innerNode->isTextNode()
#if OS(ANDROID)
        || innerNode->canStartSelection()
#endif
        ))
        return false;

    selectClosestWordFromHitTestResult(hitTestResult, AppendTrailingWhitespace::DontAppend);
    if (!selection().isRange())
        return false;
    return true;
}

void SelectionController::sendContextMenuEvent(const MouseEventWithHitTestResults& mev, const LayoutPoint& position)
{
    if (selection().contains(position)
        || mev.scrollbar()
        // FIXME: In the editable case, word selection sometimes selects content that isn't underneath the mouse.
        // If the selection is non-editable, we do word selection to make it easier to use the contextual menu items
        // available for text selections.  But only if we're above text.
        || !(selection().isContentEditable() || (mev.innerNode() && mev.innerNode()->isTextNode())))
        return;

    m_mouseDownMayStartSelect = true; // context menu events are always allowed to perform a selection

    if (mev.hitTestResult().isMisspelled())
        selectClosestMisspellingFromMouseEvent(mev);
    else if (m_frame->editor().behavior().shouldSelectOnContextualMenuClick())
        selectClosestWordOrLinkFromMouseEvent(mev);
}

void SelectionController::passMousePressEventToSubframe(const MouseEventWithHitTestResults& mev)
{
    // If we're clicking into a frame that is selected, the frame will appear
    // greyed out even though we're clicking on the selection.  This looks
    // really strange (having the whole frame be greyed out), so we deselect the
    // selection.
    IntPoint p = m_frame->view()->rootFrameToContents(mev.event().position());
    if (!selection().contains(p))
        return;

    VisiblePosition visiblePos(
        mev.innerNode()->layoutObject()->positionForPoint(mev.localPoint()));
    VisibleSelection newSelection(visiblePos);
    selection().setSelection(newSelection);
}

void SelectionController::initializeSelectionState()
{
    m_selectionState = SelectionState::HaveNotStartedSelection;
}

void SelectionController::setMouseDownMayStartSelect(bool mayStartSelect)
{
    m_mouseDownMayStartSelect = mayStartSelect;
}

bool SelectionController::mouseDownMayStartSelect() const
{
    return m_mouseDownMayStartSelect;
}

bool SelectionController::mouseDownWasSingleClickInSelection() const
{
    return m_mouseDownWasSingleClickInSelection;
}

FrameSelection& SelectionController::selection() const
{
    return m_frame->selection();
}

} // namespace blink
