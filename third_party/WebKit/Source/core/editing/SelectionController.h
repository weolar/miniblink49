/*
 * Copyright (C) 2006, 2007, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef SelectionController_h
#define SelectionController_h

#include "core/CoreExport.h"
#include "core/editing/TextGranularity.h"
#include "core/page/EventWithHitTestResults.h"
#include "platform/heap/Handle.h"

namespace blink {

class FrameSelection;
class HitTestResult;
class LocalFrame;
class VisibleSelection;

class SelectionController final : public NoBaseWillBeGarbageCollected<SelectionController> {
    WTF_MAKE_NONCOPYABLE(SelectionController);
public:
    static PassOwnPtrWillBeRawPtr<SelectionController> create(LocalFrame&);
    DECLARE_TRACE();

    bool updateSelectionForMouseDownDispatchingSelectStart(Node*, const VisibleSelection&, TextGranularity);
    void handleMousePressEvent(const MouseEventWithHitTestResults&);
    bool handleMousePressEventSingleClick(const MouseEventWithHitTestResults&);
    bool handleMousePressEventDoubleClick(const MouseEventWithHitTestResults&);
    bool handleMousePressEventTripleClick(const MouseEventWithHitTestResults&);
    void handleMouseDraggedEvent(const MouseEventWithHitTestResults&, const IntPoint&, const LayoutPoint&, Node*, const IntPoint&);
    bool handleMouseReleaseEvent(const MouseEventWithHitTestResults&, const LayoutPoint&);
    bool handlePasteGlobalSelection(const PlatformMouseEvent&);
    bool handleGestureLongPress(const PlatformGestureEvent&, const HitTestResult&);

    void updateSelectionForMouseDrag(Node*, const LayoutPoint&, const IntPoint&);
    void updateSelectionForMouseDrag(const HitTestResult&, Node*, const LayoutPoint&, const IntPoint&);
    void sendContextMenuEvent(const MouseEventWithHitTestResults&, const LayoutPoint&);
    void passMousePressEventToSubframe(const MouseEventWithHitTestResults&);

    void initializeSelectionState();
    void setMouseDownMayStartSelect(bool);
    bool mouseDownMayStartSelect() const;
    bool mouseDownWasSingleClickInSelection() const;

private:
    explicit SelectionController(LocalFrame&);

    template <typename Strategy>
    bool handleMousePressEventSingleClickAlgorithm(const MouseEventWithHitTestResults&);

    template <typename Strategy>
    void updateSelectionForMouseDragAlgorithm(const HitTestResult&, Node*, const LayoutPoint&, const IntPoint&);

    enum class AppendTrailingWhitespace { ShouldAppend, DontAppend };
    void selectClosestWordFromHitTestResult(const HitTestResult&, AppendTrailingWhitespace);
    void selectClosestMisspellingFromHitTestResult(const HitTestResult&, AppendTrailingWhitespace);
    void selectClosestWordFromMouseEvent(const MouseEventWithHitTestResults&);
    void selectClosestMisspellingFromMouseEvent(const MouseEventWithHitTestResults&);
    void selectClosestWordOrLinkFromMouseEvent(const MouseEventWithHitTestResults&);

    FrameSelection& selection() const;

    RawPtrWillBeMember<LocalFrame> const m_frame;
    bool m_mouseDownMayStartSelect;
    bool m_mouseDownWasSingleClickInSelection;
    enum class SelectionState { HaveNotStartedSelection, PlacedCaret, ExtendedSelection };
    SelectionState m_selectionState;
};

} // namespace blink
#endif // SelectionController_h
