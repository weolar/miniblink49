/*
 * Copyright (C) 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
#include "core/editing/InputMethodController.h"

#include "core/events/CompositionEvent.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/Range.h"
#include "core/dom/Text.h"
#include "core/editing/Editor.h"
#include "core/editing/TypingCommand.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutObject.h"
#include "core/page/ChromeClient.h"

namespace blink {

InputMethodController::SelectionOffsetsScope::SelectionOffsetsScope(InputMethodController* inputMethodController)
    : m_inputMethodController(inputMethodController)
    , m_offsets(inputMethodController->getSelectionOffsets())
{
}

InputMethodController::SelectionOffsetsScope::~SelectionOffsetsScope()
{
    m_inputMethodController->setSelectionOffsets(m_offsets);
}

// ----------------------------

PassOwnPtrWillBeRawPtr<InputMethodController> InputMethodController::create(LocalFrame& frame)
{
    return adoptPtrWillBeNoop(new InputMethodController(frame));
}

InputMethodController::InputMethodController(LocalFrame& frame)
    : m_frame(&frame)
    , m_compositionStart(0)
    , m_compositionEnd(0)
{
}

InputMethodController::~InputMethodController()
{
}

bool InputMethodController::hasComposition() const
{
    return m_compositionNode && m_compositionNode->isContentEditable();
}

inline Editor& InputMethodController::editor() const
{
    return frame().editor();
}

void InputMethodController::clear()
{
    m_compositionNode = nullptr;
    m_customCompositionUnderlines.clear();
}

bool InputMethodController::insertTextForConfirmedComposition(const String& text)
{
    return frame().eventHandler().handleTextInputEvent(text, 0, TextEventInputComposition);
}

void InputMethodController::selectComposition() const
{
    RefPtrWillBeRawPtr<Range> range = compositionRange();
    if (!range)
        return;

    // The composition can start inside a composed character sequence, so we have to override checks.
    // See <http://bugs.webkit.org/show_bug.cgi?id=15781>
    VisibleSelection selection;
    selection.setWithoutValidation(range->startPosition(), range->endPosition());
    frame().selection().setSelection(selection, 0);
}

bool InputMethodController::confirmComposition()
{
    if (!hasComposition())
        return false;
    return finishComposition(m_compositionNode->data().substring(m_compositionStart, m_compositionEnd - m_compositionStart), ConfirmComposition);
}

bool InputMethodController::confirmComposition(const String& text)
{
    return finishComposition(text, ConfirmComposition);
}

bool InputMethodController::confirmCompositionOrInsertText(const String& text, ConfirmCompositionBehavior confirmBehavior)
{
    if (!hasComposition()) {
        if (!text.length())
            return false;
        editor().insertText(text, 0);
        return true;
    }

    if (text.length()) {
        confirmComposition(text);
        return true;
    }

    if (confirmBehavior != KeepSelection)
        return confirmComposition();

    SelectionOffsetsScope selectionOffsetsScope(this);
    return confirmComposition();
}

void InputMethodController::cancelComposition()
{
    finishComposition(emptyString(), CancelComposition);
}

void InputMethodController::cancelCompositionIfSelectionIsInvalid()
{
    if (!hasComposition() || editor().preventRevealSelection())
        return;

    // Check if selection start and selection end are valid.
    Position start = frame().selection().start();
    Position end = frame().selection().end();
    if (start.containerNode() == m_compositionNode
        && end.containerNode() == m_compositionNode
        && static_cast<unsigned>(start.computeOffsetInContainerNode()) >= m_compositionStart
        && static_cast<unsigned>(end.computeOffsetInContainerNode()) <= m_compositionEnd)
        return;

    cancelComposition();
    frame().chromeClient().didCancelCompositionOnSelectionChange();
}

bool InputMethodController::finishComposition(const String& text, FinishCompositionMode mode)
{
    if (!hasComposition())
        return false;

    ASSERT(mode == ConfirmComposition || mode == CancelComposition);

    Editor::RevealSelectionScope revealSelectionScope(&editor());

    if (mode == CancelComposition)
        ASSERT(text == emptyString());
    else
        selectComposition();

    if (frame().selection().isNone())
        return false;

    // Dispatch a compositionend event to the focused node.
    // We should send this event before sending a TextEvent as written in Section 6.2.2 and 6.2.3 of
    // the DOM Event specification.
    if (Element* target = frame().document()->focusedElement()) {
        RefPtrWillBeRawPtr<CompositionEvent> event = CompositionEvent::create(EventTypeNames::compositionend, frame().domWindow(), text);
        target->dispatchEvent(event, IGNORE_EXCEPTION);
    }

    // If text is empty, then delete the old composition here. If text is non-empty, InsertTextCommand::input
    // will delete the old composition with an optimized replace operation.
    if (text.isEmpty() && mode != CancelComposition) {
        ASSERT(frame().document());
        TypingCommand::deleteSelection(*frame().document(), 0);
    }

    m_compositionNode = nullptr;
    m_customCompositionUnderlines.clear();

    insertTextForConfirmedComposition(text);

    if (mode == CancelComposition) {
        // An open typing command that disagrees about current selection would cause issues with typing later on.
        TypingCommand::closeTyping(m_frame);
    }

    return true;
}

void InputMethodController::setComposition(const String& text, const Vector<CompositionUnderline>& underlines, unsigned selectionStart, unsigned selectionEnd)
{
    Editor::RevealSelectionScope revealSelectionScope(&editor());

    // Updates styles before setting selection for composition to prevent
    // inserting the previous composition text into text nodes oddly.
    // See https://bugs.webkit.org/show_bug.cgi?id=46868
    frame().document()->updateLayoutTreeIfNeeded();

    selectComposition();

    if (frame().selection().isNone())
        return;

    if (Element* target = frame().document()->focusedElement()) {
        // Dispatch an appropriate composition event to the focused node.
        // We check the composition status and choose an appropriate composition event since this
        // function is used for three purposes:
        // 1. Starting a new composition.
        //    Send a compositionstart and a compositionupdate event when this function creates
        //    a new composition node, i.e.
        //    m_compositionNode == 0 && !text.isEmpty().
        //    Sending a compositionupdate event at this time ensures that at least one
        //    compositionupdate event is dispatched.
        // 2. Updating the existing composition node.
        //    Send a compositionupdate event when this function updates the existing composition
        //    node, i.e. m_compositionNode != 0 && !text.isEmpty().
        // 3. Canceling the ongoing composition.
        //    Send a compositionend event when function deletes the existing composition node, i.e.
        //    m_compositionNode != 0 && test.isEmpty().
        RefPtrWillBeRawPtr<CompositionEvent> event = nullptr;
        if (!hasComposition()) {
            // We should send a compositionstart event only when the given text is not empty because this
            // function doesn't create a composition node when the text is empty.
            if (!text.isEmpty()) {
                target->dispatchEvent(CompositionEvent::create(EventTypeNames::compositionstart, frame().domWindow(), frame().selectedText()));
                event = CompositionEvent::create(EventTypeNames::compositionupdate, frame().domWindow(), text);
            }
        } else {
            if (!text.isEmpty())
                event = CompositionEvent::create(EventTypeNames::compositionupdate, frame().domWindow(), text);
            else
                event = CompositionEvent::create(EventTypeNames::compositionend, frame().domWindow(), text);
        }
        if (event.get())
            target->dispatchEvent(event, IGNORE_EXCEPTION);
    }

    // If text is empty, then delete the old composition here. If text is non-empty, InsertTextCommand::input
    // will delete the old composition with an optimized replace operation.
    if (text.isEmpty()) {
        ASSERT(frame().document());
        TypingCommand::deleteSelection(*frame().document(), TypingCommand::PreventSpellChecking);
    }

    m_compositionNode = nullptr;
    m_customCompositionUnderlines.clear();

    if (!text.isEmpty()) {
        ASSERT(frame().document());
        TypingCommand::insertText(*frame().document(), text, TypingCommand::SelectInsertedText | TypingCommand::PreventSpellChecking, TypingCommand::TextCompositionUpdate);

        // Find out what node has the composition now.
        Position base = frame().selection().base().downstream();
        Position extent = frame().selection().extent();
        Node* baseNode = base.deprecatedNode();
        unsigned baseOffset = base.deprecatedEditingOffset();
        Node* extentNode = extent.deprecatedNode();
        unsigned extentOffset = extent.deprecatedEditingOffset();

        if (baseNode && baseNode == extentNode && baseNode->isTextNode() && baseOffset + text.length() == extentOffset) {
            m_compositionNode = toText(baseNode);
            m_compositionStart = baseOffset;
            m_compositionEnd = extentOffset;
            m_customCompositionUnderlines = underlines;
            for (auto& underline : m_customCompositionUnderlines) {
                underline.startOffset += baseOffset;
                underline.endOffset += baseOffset;
            }
            if (baseNode->layoutObject())
                baseNode->layoutObject()->setShouldDoFullPaintInvalidation();

            unsigned start = std::min(baseOffset + selectionStart, extentOffset);
            unsigned end = std::min(std::max(start, baseOffset + selectionEnd), extentOffset);
            RefPtrWillBeRawPtr<Range> selectedRange = Range::create(baseNode->document(), baseNode, start, baseNode, end);
            frame().selection().setSelectedRange(selectedRange.get(), DOWNSTREAM, FrameSelection::NonDirectional, NotUserTriggered);
        }
    }
}

void InputMethodController::setCompositionFromExistingText(const Vector<CompositionUnderline>& underlines, unsigned compositionStart, unsigned compositionEnd)
{
    Element* editable = frame().selection().rootEditableElement();
    Position base = frame().selection().base().downstream();
    Node* baseNode = base.anchorNode();
    if (baseNode && editable->firstChild() == baseNode && editable->lastChild() == baseNode && baseNode->isTextNode()) {
        m_compositionNode = nullptr;
        m_customCompositionUnderlines.clear();

        if (base.anchorType() != PositionAnchorType::OffsetInAnchor)
            return;
        if (baseNode != frame().selection().extent().anchorNode())
            return;

        m_compositionNode = toText(baseNode);
        RefPtrWillBeRawPtr<Range> range = PlainTextRange(compositionStart, compositionEnd).createRange(*editable);
        if (!range)
            return;

        m_compositionStart = range->startOffset();
        m_compositionEnd = range->endOffset();
        m_customCompositionUnderlines = underlines;
        size_t numUnderlines = m_customCompositionUnderlines.size();
        for (size_t i = 0; i < numUnderlines; ++i) {
            m_customCompositionUnderlines[i].startOffset += m_compositionStart;
            m_customCompositionUnderlines[i].endOffset += m_compositionStart;
        }
        if (baseNode->layoutObject())
            baseNode->layoutObject()->setShouldDoFullPaintInvalidation();
        return;
    }

    Editor::RevealSelectionScope revealSelectionScope(&editor());
    SelectionOffsetsScope selectionOffsetsScope(this);
    setSelectionOffsets(PlainTextRange(compositionStart, compositionEnd));
    setComposition(frame().selectedText(), underlines, 0, 0);
}

PassRefPtrWillBeRawPtr<Range> InputMethodController::compositionRange() const
{
    if (!hasComposition())
        return nullptr;
    unsigned length = m_compositionNode->length();
    unsigned start = std::min(m_compositionStart, length);
    unsigned end = std::min(std::max(start, m_compositionEnd), length);
    if (start >= end)
        return nullptr;
    return Range::create(m_compositionNode->document(), m_compositionNode.get(), start, m_compositionNode.get(), end);
}

PlainTextRange InputMethodController::getSelectionOffsets() const
{
    RefPtrWillBeRawPtr<Range> range = frame().selection().selection().firstRange();
    if (!range)
        return PlainTextRange();
    ContainerNode* editable = frame().selection().rootEditableElementOrTreeScopeRootNode();
    ASSERT(editable);
    return PlainTextRange::create(*editable, *range.get());
}

bool InputMethodController::setSelectionOffsets(const PlainTextRange& selectionOffsets)
{
    if (selectionOffsets.isNull())
        return false;
    Element* rootEditableElement = frame().selection().rootEditableElement();
    if (!rootEditableElement)
        return false;

    RefPtrWillBeRawPtr<Range> range = selectionOffsets.createRange(*rootEditableElement);
    if (!range)
        return false;

    return frame().selection().setSelectedRange(range.get(), VP_DEFAULT_AFFINITY, FrameSelection::NonDirectional, FrameSelection::CloseTyping);
}

bool InputMethodController::setEditableSelectionOffsets(const PlainTextRange& selectionOffsets)
{
    if (!editor().canEdit())
        return false;
    return setSelectionOffsets(selectionOffsets);
}

void InputMethodController::extendSelectionAndDelete(int before, int after)
{
    if (!editor().canEdit())
        return;
    PlainTextRange selectionOffsets(getSelectionOffsets());
    if (selectionOffsets.isNull())
        return;

    // A common call of before=1 and after=0 will fail if the last character
    // is multi-code-word UTF-16, including both multi-16bit code-points and
    // Unicode combining character sequences of multiple single-16bit code-
    // points (officially called "compositions"). Try more until success.
    // http://crbug.com/355995
    //
    // FIXME: Note that this is not an ideal solution when this function is
    // called to implement "backspace". In that case, there should be some call
    // that will not delete a full multi-code-point composition but rather
    // only the last code-point so that it's possible for a user to correct
    // a composition without starting it from the beginning.
    // http://crbug.com/37993
    do {
        if (!setSelectionOffsets(PlainTextRange(std::max(static_cast<int>(selectionOffsets.start()) - before, 0), selectionOffsets.end() + after)))
            return;
        if (before == 0)
            break;
        ++before;
    } while (frame().selection().start() == frame().selection().end() && before <= static_cast<int>(selectionOffsets.start()));
    TypingCommand::deleteSelection(*frame().document());
}

DEFINE_TRACE(InputMethodController)
{
    visitor->trace(m_frame);
    visitor->trace(m_compositionNode);
}

} // namespace blink
