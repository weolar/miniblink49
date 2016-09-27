/*
 * Copyright (C) 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2009 Igalia S.L.
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
#include "core/editing/Editor.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/clipboard/Pasteboard.h"
#include "core/css/CSSValueList.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/DocumentFragment.h"
#include "core/editing/CreateLinkCommand.h"
#include "core/editing/FormatBlockCommand.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/IndentOutdentCommand.h"
#include "core/editing/InsertListCommand.h"
#include "core/editing/ReplaceSelectionCommand.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/TypingCommand.h"
#include "core/editing/UnlinkCommand.h"
#include "core/editing/htmlediting.h"
#include "core/editing/markup.h"
#include "core/events/Event.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLFontElement.h"
#include "core/html/HTMLHRElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutBox.h"
#include "core/page/ChromeClient.h"
#include "core/page/EditorClient.h"
#include "platform/KillRing.h"
#include "platform/UserGestureIndicator.h"
#include "platform/scroll/Scrollbar.h"
#include "public/platform/Platform.h"
#include "wtf/text/AtomicString.h"

namespace blink {

using namespace HTMLNames;

class EditorInternalCommand {
public:
    int idForUserMetrics;
    bool (*execute)(LocalFrame&, Event*, EditorCommandSource, const String&);
    bool (*isSupportedFromDOM)(LocalFrame*);
    bool (*isEnabled)(LocalFrame&, Event*, EditorCommandSource);
    TriState (*state)(LocalFrame&, Event*);
    String (*value)(LocalFrame&, Event*);
    bool isTextInsertion;
    bool allowExecutionWhenDisabled;
};

typedef HashMap<String, const EditorInternalCommand*, CaseFoldingHash> CommandMap;

static const bool notTextInsertion = false;
static const bool isTextInsertion = true;

static const bool allowExecutionWhenDisabled = true;
static const bool doNotAllowExecutionWhenDisabled = false;

// Related to Editor::selectionForCommand.
// Certain operations continue to use the target control's selection even if the event handler
// already moved the selection outside of the text control.
static LocalFrame* targetFrame(LocalFrame& frame, Event* event)
{
    if (!event)
        return &frame;
    Node* node = event->target()->toNode();
    if (!node)
        return &frame;
    return node->document().frame();
}

static bool applyCommandToFrame(LocalFrame& frame, EditorCommandSource source, EditAction action, StylePropertySet* style)
{
    // FIXME: We don't call shouldApplyStyle when the source is DOM; is there a good reason for that?
    switch (source) {
    case CommandFromMenuOrKeyBinding:
        frame.editor().applyStyleToSelection(style, action);
        return true;
    case CommandFromDOM:
        frame.editor().applyStyle(style);
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

static bool executeApplyStyle(LocalFrame& frame, EditorCommandSource source, EditAction action, CSSPropertyID propertyID, const String& propertyValue)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> style = MutableStylePropertySet::create();
    style->setProperty(propertyID, propertyValue);
    return applyCommandToFrame(frame, source, action, style.get());
}

static bool executeApplyStyle(LocalFrame& frame, EditorCommandSource source, EditAction action, CSSPropertyID propertyID, CSSValueID propertyValue)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> style = MutableStylePropertySet::create();
    style->setProperty(propertyID, propertyValue);
    return applyCommandToFrame(frame, source, action, style.get());
}

// FIXME: executeToggleStyleInList does not handle complicated cases such as <b><u>hello</u>world</b> properly.
//        This function must use Editor::selectionHasStyle to determine the current style but we cannot fix this
//        until https://bugs.webkit.org/show_bug.cgi?id=27818 is resolved.
static bool executeToggleStyleInList(LocalFrame& frame, EditorCommandSource source, EditAction action, CSSPropertyID propertyID, CSSValue* value)
{
    RefPtrWillBeRawPtr<EditingStyle> selectionStyle = EditingStyle::styleAtSelectionStart(frame.selection().selection());
    if (!selectionStyle || !selectionStyle->style())
        return false;

    RefPtrWillBeRawPtr<CSSValue> selectedCSSValue = selectionStyle->style()->getPropertyCSSValue(propertyID);
    String newStyle("none");
    if (selectedCSSValue->isValueList()) {
        RefPtrWillBeRawPtr<CSSValueList> selectedCSSValueList = toCSSValueList(selectedCSSValue.get());
        if (!selectedCSSValueList->removeAll(value))
            selectedCSSValueList->append(value);
        if (selectedCSSValueList->length())
            newStyle = selectedCSSValueList->cssText();

    } else if (selectedCSSValue->cssText() == "none")
        newStyle = value->cssText();

    // FIXME: We shouldn't be having to convert new style into text.  We should have setPropertyCSSValue.
    RefPtrWillBeRawPtr<MutableStylePropertySet> newMutableStyle = MutableStylePropertySet::create();
    newMutableStyle->setProperty(propertyID, newStyle);
    return applyCommandToFrame(frame, source, action, newMutableStyle.get());
}

static bool executeToggleStyle(LocalFrame& frame, EditorCommandSource source, EditAction action, CSSPropertyID propertyID, const char* offValue, const char* onValue)
{
    // Style is considered present when
    // Mac: present at the beginning of selection
    // other: present throughout the selection

    bool styleIsPresent;
    if (frame.editor().behavior().shouldToggleStyleBasedOnStartOfSelection())
        styleIsPresent = frame.editor().selectionStartHasStyle(propertyID, onValue);
    else
        styleIsPresent = frame.editor().selectionHasStyle(propertyID, onValue) == TrueTriState;

    RefPtrWillBeRawPtr<EditingStyle> style = EditingStyle::create(propertyID, styleIsPresent ? offValue : onValue);
    return applyCommandToFrame(frame, source, action, style->style());
}

static bool executeApplyParagraphStyle(LocalFrame& frame, EditorCommandSource source, EditAction action, CSSPropertyID propertyID, const String& propertyValue)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> style = MutableStylePropertySet::create();
    style->setProperty(propertyID, propertyValue);
    // FIXME: We don't call shouldApplyStyle when the source is DOM; is there a good reason for that?
    switch (source) {
    case CommandFromMenuOrKeyBinding:
        frame.editor().applyParagraphStyleToSelection(style.get(), action);
        return true;
    case CommandFromDOM:
        frame.editor().applyParagraphStyle(style.get());
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

static bool executeInsertFragment(LocalFrame& frame, PassRefPtrWillBeRawPtr<DocumentFragment> fragment)
{
    ASSERT(frame.document());
    ReplaceSelectionCommand::create(*frame.document(), fragment, ReplaceSelectionCommand::PreventNesting, EditActionUnspecified)->apply();
    return true;
}

static bool executeInsertElement(LocalFrame& frame, PassRefPtrWillBeRawPtr<HTMLElement> content)
{
    ASSERT(frame.document());
    RefPtrWillBeRawPtr<DocumentFragment> fragment = DocumentFragment::create(*frame.document());
    TrackExceptionState exceptionState;
    fragment->appendChild(content, exceptionState);
    if (exceptionState.hadException())
        return false;
    return executeInsertFragment(frame, fragment.release());
}

static bool expandSelectionToGranularity(LocalFrame& frame, TextGranularity granularity)
{
    VisibleSelection selection = frame.selection().selection();
    selection.expandUsingGranularity(granularity);
    RefPtrWillBeRawPtr<Range> newRange = selection.toNormalizedRange();
    if (!newRange)
        return false;
    if (newRange->collapsed())
        return false;
    EAffinity affinity = frame.selection().affinity();
    frame.selection().setSelectedRange(newRange.get(), affinity, FrameSelection::NonDirectional, FrameSelection::CloseTyping);
    return true;
}

static TriState selectionListState(const FrameSelection& selection, const QualifiedName& tagName)
{
    if (selection.isCaret()) {
        if (enclosingElementWithTag(selection.selection().start(), tagName))
            return TrueTriState;
    } else if (selection.isRange()) {
        Element* startElement = enclosingElementWithTag(selection.selection().start(), tagName);
        Element* endElement = enclosingElementWithTag(selection.selection().end(), tagName);
        if (startElement && endElement && startElement == endElement)
            return TrueTriState;
    }

    return FalseTriState;
}

static TriState stateStyle(LocalFrame& frame, CSSPropertyID propertyID, const char* desiredValue)
{
    if (frame.editor().behavior().shouldToggleStyleBasedOnStartOfSelection())
        return frame.editor().selectionStartHasStyle(propertyID, desiredValue) ? TrueTriState : FalseTriState;
    return frame.editor().selectionHasStyle(propertyID, desiredValue);
}

static String valueStyle(LocalFrame& frame, CSSPropertyID propertyID)
{
    // FIXME: Rather than retrieving the style at the start of the current selection,
    // we should retrieve the style present throughout the selection for non-Mac platforms.
    return frame.editor().selectionStartCSSPropertyValue(propertyID);
}

static TriState stateTextWritingDirection(LocalFrame& frame, WritingDirection direction)
{
    bool hasNestedOrMultipleEmbeddings;
    WritingDirection selectionDirection = EditingStyle::textDirectionForSelection(frame.selection().selection(),
        frame.selection().typingStyle(), hasNestedOrMultipleEmbeddings);
    // FXIME: We should be returning MixedTriState when selectionDirection == direction && hasNestedOrMultipleEmbeddings
    return (selectionDirection == direction && !hasNestedOrMultipleEmbeddings) ? TrueTriState : FalseTriState;
}

static unsigned verticalScrollDistance(LocalFrame& frame)
{
    Element* focusedElement = frame.document()->focusedElement();
    if (!focusedElement)
        return 0;
    LayoutObject* layoutObject = focusedElement->layoutObject();
    if (!layoutObject || !layoutObject->isBox())
        return 0;
    LayoutBox& layoutBox = toLayoutBox(*layoutObject);
    const ComputedStyle* style = layoutBox.style();
    if (!style)
        return 0;
    if (!(style->overflowY() == OSCROLL || style->overflowY() == OAUTO || focusedElement->hasEditableStyle()))
        return 0;
    int height = std::min<int>(layoutBox.clientHeight(), frame.view()->visibleHeight());
    return static_cast<unsigned>(max(max<int>(height * ScrollableArea::minFractionToStepWhenPaging(), height - ScrollableArea::maxOverlapBetweenPages()), 1));
}

static EphemeralRange unionEphemeralRanges(const EphemeralRange& range1, const EphemeralRange& range2)
{
    const Position startPosition = range1.startPosition().compareTo(range2.startPosition()) <= 0 ? range1.startPosition() : range2.startPosition();
    const Position endPosition = range1.endPosition().compareTo(range2.endPosition()) <= 0 ? range1.endPosition() : range2.endPosition();
    return EphemeralRange(startPosition, endPosition);
}

// Execute command functions

static bool executeBackColor(LocalFrame& frame, Event*, EditorCommandSource source, const String& value)
{
    return executeApplyStyle(frame, source, EditActionSetBackgroundColor, CSSPropertyBackgroundColor, value);
}

static bool executeCopy(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().copy();
    return true;
}

static bool executeCreateLink(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    if (value.isEmpty())
        return false;
    ASSERT(frame.document());
    CreateLinkCommand::create(*frame.document(), value)->apply();
    return true;
}

static bool executeCut(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().cut();
    return true;
}

static bool executeDefaultParagraphSeparator(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    if (equalIgnoringCase(value, "div"))
        frame.editor().setDefaultParagraphSeparator(EditorParagraphSeparatorIsDiv);
    else if (equalIgnoringCase(value, "p"))
        frame.editor().setDefaultParagraphSeparator(EditorParagraphSeparatorIsP);

    return true;
}

static bool executeDelete(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    switch (source) {
    case CommandFromMenuOrKeyBinding: {
        // Doesn't modify the text if the current selection isn't a range.
        frame.editor().performDelete();
        return true;
    }
    case CommandFromDOM:
        // If the current selection is a caret, delete the preceding character. IE performs forwardDelete, but we currently side with Firefox.
        // Doesn't scroll to make the selection visible, or modify the kill ring (this time, siding with IE, not Firefox).
        ASSERT(frame.document());
        TypingCommand::deleteKeyPressed(*frame.document(), frame.selection().granularity() == WordGranularity ? TypingCommand::SmartDelete : 0);
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

static bool executeDeleteBackward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().deleteWithDirection(DirectionBackward, CharacterGranularity, false, true);
    return true;
}

static bool executeDeleteBackwardByDecomposingPreviousCharacter(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    WTF_LOG_ERROR("DeleteBackwardByDecomposingPreviousCharacter is not implemented, doing DeleteBackward instead");
    frame.editor().deleteWithDirection(DirectionBackward, CharacterGranularity, false, true);
    return true;
}

static bool executeDeleteForward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().deleteWithDirection(DirectionForward, CharacterGranularity, false, true);
    return true;
}

static bool executeDeleteToBeginningOfLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().deleteWithDirection(DirectionBackward, LineBoundary, true, false);
    return true;
}

static bool executeDeleteToBeginningOfParagraph(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().deleteWithDirection(DirectionBackward, ParagraphBoundary, true, false);
    return true;
}

static bool executeDeleteToEndOfLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    // Despite its name, this command should delete the newline at the end of
    // a paragraph if you are at the end of a paragraph (like DeleteToEndOfParagraph).
    frame.editor().deleteWithDirection(DirectionForward, LineBoundary, true, false);
    return true;
}

static bool executeDeleteToEndOfParagraph(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    // Despite its name, this command should delete the newline at the end of
    // a paragraph if you are at the end of a paragraph.
    frame.editor().deleteWithDirection(DirectionForward, ParagraphBoundary, true, false);
    return true;
}

static bool executeDeleteToMark(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    // TODO(yosin) We should use |EphemeralRange| version of
    // |VisibleSelection::toNormalizedRange()|.
    RefPtrWillBeRawPtr<Range> mark = frame.editor().mark().toNormalizedRange();
    if (mark) {
        bool selected = frame.selection().setSelectedRange(unionEphemeralRanges(EphemeralRange(mark.get()), frame.editor().selectedRange()), DOWNSTREAM, FrameSelection::NonDirectional, FrameSelection::CloseTyping);
        ASSERT(selected);
        if (!selected)
            return false;
    }
    frame.editor().performDelete();
    frame.editor().setMark(frame.selection().selection());
    return true;
}

static bool executeDeleteWordBackward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().deleteWithDirection(DirectionBackward, WordGranularity, true, false);
    return true;
}

static bool executeDeleteWordForward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().deleteWithDirection(DirectionForward, WordGranularity, true, false);
    return true;
}

static bool executeFindString(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    return frame.editor().findString(value, CaseInsensitive | WrapAround);
}

static bool executeFontName(LocalFrame& frame, Event*, EditorCommandSource source, const String& value)
{
    return executeApplyStyle(frame, source, EditActionSetFont, CSSPropertyFontFamily, value);
}

static bool executeFontSize(LocalFrame& frame, Event*, EditorCommandSource source, const String& value)
{
    CSSValueID size;
    if (!HTMLFontElement::cssValueFromFontSizeNumber(value, size))
        return false;
    return executeApplyStyle(frame, source, EditActionChangeAttributes, CSSPropertyFontSize, size);
}

static bool executeFontSizeDelta(LocalFrame& frame, Event*, EditorCommandSource source, const String& value)
{
    return executeApplyStyle(frame, source, EditActionChangeAttributes, CSSPropertyWebkitFontSizeDelta, value);
}

static bool executeForeColor(LocalFrame& frame, Event*, EditorCommandSource source, const String& value)
{
    return executeApplyStyle(frame, source, EditActionSetColor, CSSPropertyColor, value);
}

static bool executeFormatBlock(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    String tagName = value.lower();
    if (tagName[0] == '<' && tagName[tagName.length() - 1] == '>')
        tagName = tagName.substring(1, tagName.length() - 2);

    AtomicString localName, prefix;
    if (!Document::parseQualifiedName(AtomicString(tagName), prefix, localName, IGNORE_EXCEPTION))
        return false;
    QualifiedName qualifiedTagName(prefix, localName, xhtmlNamespaceURI);

    ASSERT(frame.document());
    RefPtrWillBeRawPtr<FormatBlockCommand> command = FormatBlockCommand::create(*frame.document(), qualifiedTagName);
    command->apply();
    return command->didApply();
}

static bool executeForwardDelete(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    switch (source) {
    case CommandFromMenuOrKeyBinding:
        frame.editor().deleteWithDirection(DirectionForward, CharacterGranularity, false, true);
        return true;
    case CommandFromDOM:
        // Doesn't scroll to make the selection visible, or modify the kill ring.
        // ForwardDelete is not implemented in IE or Firefox, so this behavior is only needed for
        // backward compatibility with ourselves, and for consistency with Delete.
        ASSERT(frame.document());
        TypingCommand::forwardDeleteKeyPressed(*frame.document());
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

static bool executeIgnoreSpelling(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.spellChecker().ignoreSpelling();
    return true;
}

static bool executeIndent(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    IndentOutdentCommand::create(*frame.document(), IndentOutdentCommand::Indent)->apply();
    return true;
}

static bool executeInsertBacktab(LocalFrame& frame, Event* event, EditorCommandSource, const String&)
{
    return targetFrame(frame, event)->eventHandler().handleTextInputEvent("\t", event, TextEventInputBackTab);
}

static bool executeInsertHorizontalRule(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    ASSERT(frame.document());
    RefPtrWillBeRawPtr<HTMLHRElement> rule = HTMLHRElement::create(*frame.document());
    if (!value.isEmpty())
        rule->setIdAttribute(AtomicString(value));
    return executeInsertElement(frame, rule.release());
}

static bool executeInsertHTML(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    ASSERT(frame.document());
    return executeInsertFragment(frame, createFragmentFromMarkup(*frame.document(), value, ""));
}

static bool executeInsertImage(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    ASSERT(frame.document());
    RefPtrWillBeRawPtr<HTMLImageElement> image = HTMLImageElement::create(*frame.document());
    if (!value.isEmpty())
        image->setSrc(value);
    return executeInsertElement(frame, image.release());
}

static bool executeInsertLineBreak(LocalFrame& frame, Event* event, EditorCommandSource source, const String&)
{
    switch (source) {
    case CommandFromMenuOrKeyBinding:
        return targetFrame(frame, event)->eventHandler().handleTextInputEvent("\n", event, TextEventInputLineBreak);
    case CommandFromDOM:
        // Doesn't scroll to make the selection visible, or modify the kill ring.
        // InsertLineBreak is not implemented in IE or Firefox, so this behavior is only needed for
        // backward compatibility with ourselves, and for consistency with other commands.
        ASSERT(frame.document());
        TypingCommand::insertLineBreak(*frame.document(), 0);
        return true;
    }
    ASSERT_NOT_REACHED();
    return false;
}

static bool executeInsertNewline(LocalFrame& frame, Event* event, EditorCommandSource, const String&)
{
    LocalFrame* targetFrame = blink::targetFrame(frame, event);
    return targetFrame->eventHandler().handleTextInputEvent("\n", event, targetFrame->editor().canEditRichly() ? TextEventInputKeyboard : TextEventInputLineBreak);
}

static bool executeInsertNewlineInQuotedContent(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    TypingCommand::insertParagraphSeparatorInQuotedContent(*frame.document());
    return true;
}

static bool executeInsertOrderedList(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    InsertListCommand::create(*frame.document(), InsertListCommand::OrderedList)->apply();
    return true;
}

static bool executeInsertParagraph(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    TypingCommand::insertParagraphSeparator(*frame.document(), 0);
    return true;
}

static bool executeInsertTab(LocalFrame& frame, Event* event, EditorCommandSource, const String&)
{
    return targetFrame(frame, event)->eventHandler().handleTextInputEvent("\t", event);
}

static bool executeInsertText(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    ASSERT(frame.document());
    TypingCommand::insertText(*frame.document(), value, 0);
    return true;
}

static bool executeInsertUnorderedList(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    InsertListCommand::create(*frame.document(), InsertListCommand::UnorderedList)->apply();
    return true;
}

static bool executeJustifyCenter(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeApplyParagraphStyle(frame, source, EditActionCenter, CSSPropertyTextAlign, "center");
}

static bool executeJustifyFull(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeApplyParagraphStyle(frame, source, EditActionJustify, CSSPropertyTextAlign, "justify");
}

static bool executeJustifyLeft(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeApplyParagraphStyle(frame, source, EditActionAlignLeft, CSSPropertyTextAlign, "left");
}

static bool executeJustifyRight(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeApplyParagraphStyle(frame, source, EditActionAlignRight, CSSPropertyTextAlign, "right");
}

static bool executeMakeTextWritingDirectionLeftToRight(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> style = MutableStylePropertySet::create();
    style->setProperty(CSSPropertyUnicodeBidi, CSSValueEmbed);
    style->setProperty(CSSPropertyDirection, CSSValueLtr);
    frame.editor().applyStyle(style.get(), EditActionSetWritingDirection);
    return true;
}

static bool executeMakeTextWritingDirectionNatural(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> style = MutableStylePropertySet::create();
    style->setProperty(CSSPropertyUnicodeBidi, CSSValueNormal);
    frame.editor().applyStyle(style.get(), EditActionSetWritingDirection);
    return true;
}

static bool executeMakeTextWritingDirectionRightToLeft(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    RefPtrWillBeRawPtr<MutableStylePropertySet> style = MutableStylePropertySet::create();
    style->setProperty(CSSPropertyUnicodeBidi, CSSValueEmbed);
    style->setProperty(CSSPropertyDirection, CSSValueRtl);
    frame.editor().applyStyle(style.get(), EditActionSetWritingDirection);
    return true;
}

static bool executeMoveBackward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, CharacterGranularity, UserTriggered);
    return true;
}

static bool executeMoveBackwardAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, CharacterGranularity, UserTriggered);
    return true;
}

static bool executeMoveDown(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, LineGranularity, UserTriggered);
}

static bool executeMoveDownAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, LineGranularity, UserTriggered);
    return true;
}

static bool executeMoveForward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, CharacterGranularity, UserTriggered);
    return true;
}

static bool executeMoveForwardAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, CharacterGranularity, UserTriggered);
    return true;
}

static bool executeMoveLeft(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.selection().modify(FrameSelection::AlterationMove, DirectionLeft, CharacterGranularity, UserTriggered);
}

static bool executeMoveLeftAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionLeft, CharacterGranularity, UserTriggered);
    return true;
}

static bool executeMovePageDown(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    unsigned distance = verticalScrollDistance(frame);
    if (!distance)
        return false;
    return frame.selection().modify(FrameSelection::AlterationMove, distance, FrameSelection::DirectionDown,
        UserTriggered, FrameSelection::AlignCursorOnScrollAlways);
}

static bool executeMovePageDownAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    unsigned distance = verticalScrollDistance(frame);
    if (!distance)
        return false;
    return frame.selection().modify(FrameSelection::AlterationExtend, distance, FrameSelection::DirectionDown,
        UserTriggered, FrameSelection::AlignCursorOnScrollAlways);
}

static bool executeMovePageUp(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    unsigned distance = verticalScrollDistance(frame);
    if (!distance)
        return false;
    return frame.selection().modify(FrameSelection::AlterationMove, distance, FrameSelection::DirectionUp,
        UserTriggered, FrameSelection::AlignCursorOnScrollAlways);
}

static bool executeMovePageUpAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    unsigned distance = verticalScrollDistance(frame);
    if (!distance)
        return false;
    return frame.selection().modify(FrameSelection::AlterationExtend, distance, FrameSelection::DirectionUp,
        UserTriggered, FrameSelection::AlignCursorOnScrollAlways);
}

static bool executeMoveRight(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.selection().modify(FrameSelection::AlterationMove, DirectionRight, CharacterGranularity, UserTriggered);
}

static bool executeMoveRightAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionRight, CharacterGranularity, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfDocument(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, DocumentBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfDocumentAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, DocumentBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfLineAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfParagraph(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, ParagraphBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfParagraphAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, ParagraphBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfSentence(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, SentenceBoundary, UserTriggered);
    return true;
}

static bool executeMoveToBeginningOfSentenceAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, SentenceBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfDocument(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, DocumentBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfDocumentAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, DocumentBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfSentence(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, SentenceBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfSentenceAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, SentenceBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfLineAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfParagraph(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, ParagraphBoundary, UserTriggered);
    return true;
}

static bool executeMoveToEndOfParagraphAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, ParagraphBoundary, UserTriggered);
    return true;
}

static bool executeMoveParagraphBackward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, ParagraphGranularity, UserTriggered);
    return true;
}

static bool executeMoveParagraphBackwardAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, ParagraphGranularity, UserTriggered);
    return true;
}

static bool executeMoveParagraphForward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, ParagraphGranularity, UserTriggered);
    return true;
}

static bool executeMoveParagraphForwardAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, ParagraphGranularity, UserTriggered);
    return true;
}

static bool executeMoveUp(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, LineGranularity, UserTriggered);
}

static bool executeMoveUpAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, LineGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordBackward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionBackward, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordBackwardAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionBackward, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordForward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionForward, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordForwardAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionForward, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordLeft(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionLeft, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordLeftAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionLeft, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordRight(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionRight, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveWordRightAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionRight, WordGranularity, UserTriggered);
    return true;
}

static bool executeMoveToLeftEndOfLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionLeft, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToLeftEndOfLineAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionLeft, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToRightEndOfLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationMove, DirectionRight, LineBoundary, UserTriggered);
    return true;
}

static bool executeMoveToRightEndOfLineAndModifySelection(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().modify(FrameSelection::AlterationExtend, DirectionRight, LineBoundary, UserTriggered);
    return true;
}

static bool executeOutdent(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    IndentOutdentCommand::create(*frame.document(), IndentOutdentCommand::Outdent)->apply();
    return true;
}

static bool executeToggleOverwrite(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().toggleOverwriteModeEnabled();
    return true;
}

static bool executePaste(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().paste();
    return true;
}

static bool executePasteGlobalSelection(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    if (!frame.editor().behavior().supportsGlobalSelection())
        return false;
    ASSERT_UNUSED(source, source == CommandFromMenuOrKeyBinding);

    bool oldSelectionMode = Pasteboard::generalPasteboard()->isSelectionMode();
    Pasteboard::generalPasteboard()->setSelectionMode(true);
    frame.editor().paste();
    Pasteboard::generalPasteboard()->setSelectionMode(oldSelectionMode);
    return true;
}

static bool executePasteAndMatchStyle(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().pasteAsPlainText();
    return true;
}

static bool executePrint(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    FrameHost* host = frame.host();
    if (!host)
        return false;
    host->chromeClient().print(&frame);
    return true;
}

static bool executeRedo(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().redo();
    return true;
}

static bool executeRemoveFormat(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().removeFormattingAndStyle();
    return true;
}

static bool executeScrollPageBackward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.eventHandler().bubblingScroll(ScrollBlockDirectionBackward, ScrollByPage);
}

static bool executeScrollPageForward(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.eventHandler().bubblingScroll(ScrollBlockDirectionForward, ScrollByPage);
}

static bool executeScrollLineUp(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.eventHandler().bubblingScroll(ScrollUpIgnoringWritingMode, ScrollByLine);
}

static bool executeScrollLineDown(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.eventHandler().bubblingScroll(ScrollDownIgnoringWritingMode, ScrollByLine);
}

static bool executeScrollToBeginningOfDocument(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.eventHandler().bubblingScroll(ScrollBlockDirectionBackward, ScrollByDocument);
}

static bool executeScrollToEndOfDocument(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return frame.eventHandler().bubblingScroll(ScrollBlockDirectionForward, ScrollByDocument);
}

static bool executeSelectAll(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().selectAll();
    return true;
}

static bool executeSelectLine(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return expandSelectionToGranularity(frame, LineGranularity);
}

static bool executeSelectParagraph(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return expandSelectionToGranularity(frame, ParagraphGranularity);
}

static bool executeSelectSentence(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return expandSelectionToGranularity(frame, SentenceGranularity);
}

static bool executeSelectToMark(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    // TODO(yosin) We should use |EphemeralRange| version of
    // |VisibleSelection::toNormalizedRange()|.
    RefPtrWillBeRawPtr<Range> mark = frame.editor().mark().toNormalizedRange();
    EphemeralRange selection = frame.editor().selectedRange();
    if (!mark || selection.isNull())
        return false;
    frame.selection().setSelectedRange(unionEphemeralRanges(EphemeralRange(mark.get()), selection), DOWNSTREAM, FrameSelection::NonDirectional, FrameSelection::CloseTyping);
    return true;
}

static bool executeSelectWord(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    return expandSelectionToGranularity(frame, WordGranularity);
}

static bool executeSetMark(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().setMark(frame.selection().selection());
    return true;
}

static bool executeStrikethrough(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> lineThrough = CSSPrimitiveValue::createIdentifier(CSSValueLineThrough);
    return executeToggleStyleInList(frame, source, EditActionUnderline, CSSPropertyWebkitTextDecorationsInEffect, lineThrough.get());
}

static bool executeStyleWithCSS(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    frame.editor().setShouldStyleWithCSS(!equalIgnoringCase(value, "false"));
    return true;
}

static bool executeUseCSS(LocalFrame& frame, Event*, EditorCommandSource, const String& value)
{
    frame.editor().setShouldStyleWithCSS(equalIgnoringCase(value, "false"));
    return true;
}

static bool executeSubscript(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeToggleStyle(frame, source, EditActionSubscript, CSSPropertyVerticalAlign, "baseline", "sub");
}

static bool executeSuperscript(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeToggleStyle(frame, source, EditActionSuperscript, CSSPropertyVerticalAlign, "baseline", "super");
}

static bool executeSwapWithMark(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    const VisibleSelection& mark = frame.editor().mark();
    const VisibleSelection& selection = frame.selection().selection();
    if (mark.isNone() || selection.isNone())
        return false;
    frame.selection().setSelection(mark);
    frame.editor().setMark(selection);
    return true;
}

static bool executeToggleBold(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeToggleStyle(frame, source, EditActionBold, CSSPropertyFontWeight, "normal", "bold");
}

static bool executeToggleItalic(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeToggleStyle(frame, source, EditActionItalics, CSSPropertyFontStyle, "normal", "italic");
}

static bool executeTranspose(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().transpose();
    return true;
}

static bool executeUnderline(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> underline = CSSPrimitiveValue::createIdentifier(CSSValueUnderline);
    return executeToggleStyleInList(frame, source, EditActionUnderline, CSSPropertyWebkitTextDecorationsInEffect, underline.get());
}

static bool executeUndo(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().undo();
    return true;
}

static bool executeUnlink(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    ASSERT(frame.document());
    UnlinkCommand::create(*frame.document())->apply();
    return true;
}

static bool executeUnscript(LocalFrame& frame, Event*, EditorCommandSource source, const String&)
{
    return executeApplyStyle(frame, source, EditActionUnscript, CSSPropertyVerticalAlign, "baseline");
}

static bool executeUnselect(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.selection().clear();
    return true;
}

static bool executeYank(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().insertTextWithoutSendingTextEvent(frame.editor().killRing().yank(), false, 0);
    frame.editor().killRing().setToYankedState();
    return true;
}

static bool executeYankAndSelect(LocalFrame& frame, Event*, EditorCommandSource, const String&)
{
    frame.editor().insertTextWithoutSendingTextEvent(frame.editor().killRing().yank(), true, 0);
    frame.editor().killRing().setToYankedState();
    return true;
}

// Supported functions

static bool supported(LocalFrame*)
{
    return true;
}

static bool supportedFromMenuOrKeyBinding(LocalFrame*)
{
    return false;
}

static bool supportedCopyCut(LocalFrame* frame)
{
    if (!frame)
        return false;

    Settings* settings = frame->settings();
    bool defaultValue = (settings && settings->javaScriptCanAccessClipboard()) || UserGestureIndicator::processingUserGesture();
    return frame->editor().client().canCopyCut(frame, defaultValue);
}

static bool supportedPaste(LocalFrame* frame)
{
    if (!frame)
        return false;

    Settings* settings = frame->settings();
    bool defaultValue = settings && settings->javaScriptCanAccessClipboard() && settings->DOMPasteAllowed();
    return frame->editor().client().canPaste(frame, defaultValue);
}

// Enabled functions

static bool enabled(LocalFrame&, Event*, EditorCommandSource)
{
    return true;
}

static bool enabledVisibleSelection(LocalFrame& frame, Event* event, EditorCommandSource)
{
    // The term "visible" here includes a caret in editable text or a range in any text.
    const VisibleSelection& selection = frame.editor().selectionForCommand(event);
    return (selection.isCaret() && selection.isContentEditable()) || selection.isRange();
}

static bool caretBrowsingEnabled(LocalFrame& frame)
{
    return frame.settings() && frame.settings()->caretBrowsingEnabled();
}

static EditorCommandSource dummyEditorCommandSource = static_cast<EditorCommandSource>(0);

static bool enabledVisibleSelectionOrCaretBrowsing(LocalFrame& frame, Event* event, EditorCommandSource)
{
    // The EditorCommandSource parameter is unused in enabledVisibleSelection, so just pass a dummy variable
    return caretBrowsingEnabled(frame) || enabledVisibleSelection(frame, event, dummyEditorCommandSource);
}

static bool enabledVisibleSelectionAndMark(LocalFrame& frame, Event* event, EditorCommandSource)
{
    const VisibleSelection& selection = frame.editor().selectionForCommand(event);
    return ((selection.isCaret() && selection.isContentEditable()) || selection.isRange())
        && frame.editor().mark().isCaretOrRange();
}

static bool enableCaretInEditableText(LocalFrame& frame, Event* event, EditorCommandSource)
{
    const VisibleSelection& selection = frame.editor().selectionForCommand(event);
    return selection.isCaret() && selection.isContentEditable();
}

static bool enabledCopy(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.editor().canDHTMLCopy() || frame.editor().canCopy();
}

static bool enabledCut(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.editor().canDHTMLCut() || frame.editor().canCut();
}

static bool enabledInEditableText(LocalFrame& frame, Event* event, EditorCommandSource)
{
    return frame.editor().selectionForCommand(event).rootEditableElement();
}

static bool enabledDelete(LocalFrame& frame, Event* event, EditorCommandSource source)
{
    switch (source) {
    case CommandFromMenuOrKeyBinding:
        return frame.editor().canDelete();
    case CommandFromDOM:
        // "Delete" from DOM is like delete/backspace keypress, affects selected range if non-empty,
        // otherwise removes a character
        return enabledInEditableText(frame, event, source);
    }
    ASSERT_NOT_REACHED();
    return false;
}

static bool enabledInEditableTextOrCaretBrowsing(LocalFrame& frame, Event* event, EditorCommandSource)
{
    // The EditorCommandSource parameter is unused in enabledInEditableText, so just pass a dummy variable
    return caretBrowsingEnabled(frame) || enabledInEditableText(frame, event, dummyEditorCommandSource);
}

static bool enabledInRichlyEditableText(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.selection().isCaretOrRange() && frame.selection().isContentRichlyEditable() && frame.selection().rootEditableElement();
}

static bool enabledPaste(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.editor().canPaste();
}

static bool enabledRangeInEditableText(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.selection().isRange() && frame.selection().isContentEditable();
}

static bool enabledRangeInRichlyEditableText(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.selection().isRange() && frame.selection().isContentRichlyEditable();
}

static bool enabledRedo(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.editor().canRedo();
}

static bool enabledUndo(LocalFrame& frame, Event*, EditorCommandSource)
{
    return frame.editor().canUndo();
}

// State functions

static TriState stateNone(LocalFrame&, Event*)
{
    return FalseTriState;
}

static TriState stateBold(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyFontWeight, "bold");
}

static TriState stateItalic(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyFontStyle, "italic");
}

static TriState stateOrderedList(LocalFrame& frame, Event*)
{
    return selectionListState(frame.selection(), olTag);
}

static TriState stateStrikethrough(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyWebkitTextDecorationsInEffect, "line-through");
}

static TriState stateStyleWithCSS(LocalFrame& frame, Event*)
{
    return frame.editor().shouldStyleWithCSS() ? TrueTriState : FalseTriState;
}

static TriState stateSubscript(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyVerticalAlign, "sub");
}

static TriState stateSuperscript(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyVerticalAlign, "super");
}

static TriState stateTextWritingDirectionLeftToRight(LocalFrame& frame, Event*)
{
    return stateTextWritingDirection(frame, LeftToRightWritingDirection);
}

static TriState stateTextWritingDirectionNatural(LocalFrame& frame, Event*)
{
    return stateTextWritingDirection(frame, NaturalWritingDirection);
}

static TriState stateTextWritingDirectionRightToLeft(LocalFrame& frame, Event*)
{
    return stateTextWritingDirection(frame, RightToLeftWritingDirection);
}

static TriState stateUnderline(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyWebkitTextDecorationsInEffect, "underline");
}

static TriState stateUnorderedList(LocalFrame& frame, Event*)
{
    return selectionListState(frame.selection(), ulTag);
}

static TriState stateJustifyCenter(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyTextAlign, "center");
}

static TriState stateJustifyFull(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyTextAlign, "justify");
}

static TriState stateJustifyLeft(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyTextAlign, "left");
}

static TriState stateJustifyRight(LocalFrame& frame, Event*)
{
    return stateStyle(frame, CSSPropertyTextAlign, "right");
}

// Value functions

static String valueNull(LocalFrame&, Event*)
{
    return String();
}

static String valueBackColor(LocalFrame& frame, Event*)
{
    return valueStyle(frame, CSSPropertyBackgroundColor);
}

static String valueDefaultParagraphSeparator(LocalFrame& frame, Event*)
{
    switch (frame.editor().defaultParagraphSeparator()) {
    case EditorParagraphSeparatorIsDiv:
        return divTag.localName();
    case EditorParagraphSeparatorIsP:
        return pTag.localName();
    }

    ASSERT_NOT_REACHED();
    return String();
}

static String valueFontName(LocalFrame& frame, Event*)
{
    return valueStyle(frame, CSSPropertyFontFamily);
}

static String valueFontSize(LocalFrame& frame, Event*)
{
    return valueStyle(frame, CSSPropertyFontSize);
}

static String valueFontSizeDelta(LocalFrame& frame, Event*)
{
    return valueStyle(frame, CSSPropertyWebkitFontSizeDelta);
}

static String valueForeColor(LocalFrame& frame, Event*)
{
    return valueStyle(frame, CSSPropertyColor);
}

static String valueFormatBlock(LocalFrame& frame, Event*)
{
    const VisibleSelection& selection = frame.selection().selection();
    if (!selection.isNonOrphanedCaretOrRange() || !selection.isContentEditable())
        return "";
    Element* formatBlockElement = FormatBlockCommand::elementForFormatBlockCommand(selection.firstRange().get());
    if (!formatBlockElement)
        return "";
    return formatBlockElement->localName();
}

// Map of functions

struct CommandEntry {
    const char* name;
    EditorInternalCommand command;
};

static const CommandMap& createCommandMap()
{
    // If you add new commands, you should assign new Id to each idForUserMetrics and update MappedEditingCommands
    // in chrome/trunk/src/tools/metrics/histograms/histograms.xml.
    static const CommandEntry commands[] = {
        { "AlignCenter", {139, executeJustifyCenter, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "AlignJustified", {1, executeJustifyFull, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "AlignLeft", {2, executeJustifyLeft, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "AlignRight", {3, executeJustifyRight, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "BackColor", {4, executeBackColor, supported, enabledInRichlyEditableText, stateNone, valueBackColor, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "BackwardDelete", {5, executeDeleteBackward, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } }, // FIXME: remove BackwardDelete when Safari for Windows stops using it.
        { "Bold", {6, executeToggleBold, supported, enabledInRichlyEditableText, stateBold, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Copy", {7, executeCopy, supportedCopyCut, enabledCopy, stateNone, valueNull, notTextInsertion, allowExecutionWhenDisabled } },
        { "CreateLink", {8, executeCreateLink, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Cut", {9, executeCut, supportedCopyCut, enabledCut, stateNone, valueNull, notTextInsertion, allowExecutionWhenDisabled } },
        { "DefaultParagraphSeparator", {10, executeDefaultParagraphSeparator, supported, enabled, stateNone, valueDefaultParagraphSeparator, notTextInsertion, doNotAllowExecutionWhenDisabled} },
        { "Delete", {11, executeDelete, supported, enabledDelete, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteBackward", {12, executeDeleteBackward, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteBackwardByDecomposingPreviousCharacter", {13, executeDeleteBackwardByDecomposingPreviousCharacter, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteForward", {14, executeDeleteForward, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteToBeginningOfLine", {15, executeDeleteToBeginningOfLine, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteToBeginningOfParagraph", {16, executeDeleteToBeginningOfParagraph, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteToEndOfLine", {17, executeDeleteToEndOfLine, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteToEndOfParagraph", {18, executeDeleteToEndOfParagraph, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteToMark", {19, executeDeleteToMark, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteWordBackward", {20, executeDeleteWordBackward, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "DeleteWordForward", {21, executeDeleteWordForward, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "FindString", {22, executeFindString, supported, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "FontName", {23, executeFontName, supported, enabledInEditableText, stateNone, valueFontName, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "FontSize", {24, executeFontSize, supported, enabledInEditableText, stateNone, valueFontSize, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "FontSizeDelta", {25, executeFontSizeDelta, supported, enabledInEditableText, stateNone, valueFontSizeDelta, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ForeColor", {26, executeForeColor, supported, enabledInRichlyEditableText, stateNone, valueForeColor, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "FormatBlock", {27, executeFormatBlock, supported, enabledInRichlyEditableText, stateNone, valueFormatBlock, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ForwardDelete", {28, executeForwardDelete, supported, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "HiliteColor", {29, executeBackColor, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "IgnoreSpelling", {30, executeIgnoreSpelling, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Indent", {31, executeIndent, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertBacktab", {32, executeInsertBacktab, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, isTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertHTML", {33, executeInsertHTML, supported, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertHorizontalRule", {34, executeInsertHorizontalRule, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertImage", {35, executeInsertImage, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertLineBreak", {36, executeInsertLineBreak, supported, enabledInEditableText, stateNone, valueNull, isTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertNewline", {37, executeInsertNewline, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, isTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertNewlineInQuotedContent", {38, executeInsertNewlineInQuotedContent, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertOrderedList", {39, executeInsertOrderedList, supported, enabledInRichlyEditableText, stateOrderedList, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertParagraph", {40, executeInsertParagraph, supported, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertTab", {41, executeInsertTab, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, isTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertText", {42, executeInsertText, supported, enabledInEditableText, stateNone, valueNull, isTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "InsertUnorderedList", {43, executeInsertUnorderedList, supported, enabledInRichlyEditableText, stateUnorderedList, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Italic", {44, executeToggleItalic, supported, enabledInRichlyEditableText, stateItalic, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "JustifyCenter", {45, executeJustifyCenter, supported, enabledInRichlyEditableText, stateJustifyCenter, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "JustifyFull", {46, executeJustifyFull, supported, enabledInRichlyEditableText, stateJustifyFull, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "JustifyLeft", {47, executeJustifyLeft, supported, enabledInRichlyEditableText, stateJustifyLeft, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "JustifyNone", {48, executeJustifyLeft, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "JustifyRight", {49, executeJustifyRight, supported, enabledInRichlyEditableText, stateJustifyRight, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MakeTextWritingDirectionLeftToRight", {50, executeMakeTextWritingDirectionLeftToRight, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateTextWritingDirectionLeftToRight, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MakeTextWritingDirectionNatural", {51, executeMakeTextWritingDirectionNatural, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateTextWritingDirectionNatural, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MakeTextWritingDirectionRightToLeft", {52, executeMakeTextWritingDirectionRightToLeft, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateTextWritingDirectionRightToLeft, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveBackward", {53, executeMoveBackward, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveBackwardAndModifySelection", {54, executeMoveBackwardAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveDown", {55, executeMoveDown, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveDownAndModifySelection", {56, executeMoveDownAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveForward", {57, executeMoveForward, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveForwardAndModifySelection", {58, executeMoveForwardAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveLeft", {59, executeMoveLeft, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveLeftAndModifySelection", {60, executeMoveLeftAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MovePageDown", {61, executeMovePageDown, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MovePageDownAndModifySelection", {62, executeMovePageDownAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MovePageUp", {63, executeMovePageUp, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MovePageUpAndModifySelection", {64, executeMovePageUpAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveParagraphBackward", {65, executeMoveParagraphBackward, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveParagraphBackwardAndModifySelection", {66, executeMoveParagraphBackwardAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveParagraphForward", {67, executeMoveParagraphForward, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveParagraphForwardAndModifySelection", {68, executeMoveParagraphForwardAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveRight", {69, executeMoveRight, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveRightAndModifySelection", {70, executeMoveRightAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfDocument", {71, executeMoveToBeginningOfDocument, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfDocumentAndModifySelection", {72, executeMoveToBeginningOfDocumentAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfLine", {73, executeMoveToBeginningOfLine, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfLineAndModifySelection", {74, executeMoveToBeginningOfLineAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfParagraph", {75, executeMoveToBeginningOfParagraph, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfParagraphAndModifySelection", {76, executeMoveToBeginningOfParagraphAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfSentence", {77, executeMoveToBeginningOfSentence, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToBeginningOfSentenceAndModifySelection", {78, executeMoveToBeginningOfSentenceAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfDocument", {79, executeMoveToEndOfDocument, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfDocumentAndModifySelection", {80, executeMoveToEndOfDocumentAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfLine", {81, executeMoveToEndOfLine, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfLineAndModifySelection", {82, executeMoveToEndOfLineAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfParagraph", {83, executeMoveToEndOfParagraph, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfParagraphAndModifySelection", {84, executeMoveToEndOfParagraphAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfSentence", {85, executeMoveToEndOfSentence, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToEndOfSentenceAndModifySelection", {86, executeMoveToEndOfSentenceAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToLeftEndOfLine", {87, executeMoveToLeftEndOfLine, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToLeftEndOfLineAndModifySelection", {88, executeMoveToLeftEndOfLineAndModifySelection, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToRightEndOfLine", {89, executeMoveToRightEndOfLine, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveToRightEndOfLineAndModifySelection", {90, executeMoveToRightEndOfLineAndModifySelection, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveUp", {91, executeMoveUp, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveUpAndModifySelection", {92, executeMoveUpAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordBackward", {93, executeMoveWordBackward, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordBackwardAndModifySelection", {94, executeMoveWordBackwardAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordForward", {95, executeMoveWordForward, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordForwardAndModifySelection", {96, executeMoveWordForwardAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordLeft", {97, executeMoveWordLeft, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordLeftAndModifySelection", {98, executeMoveWordLeftAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordRight", {99, executeMoveWordRight, supportedFromMenuOrKeyBinding, enabledInEditableTextOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "MoveWordRightAndModifySelection", {100, executeMoveWordRightAndModifySelection, supportedFromMenuOrKeyBinding, enabledVisibleSelectionOrCaretBrowsing, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Outdent", {101, executeOutdent, supported, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "OverWrite", {102, executeToggleOverwrite, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Paste", {103, executePaste, supportedPaste, enabledPaste, stateNone, valueNull, notTextInsertion, allowExecutionWhenDisabled } },
        { "PasteAndMatchStyle", {104, executePasteAndMatchStyle, supportedPaste, enabledPaste, stateNone, valueNull, notTextInsertion, allowExecutionWhenDisabled } },
        { "PasteGlobalSelection", {105, executePasteGlobalSelection, supportedFromMenuOrKeyBinding, enabledPaste, stateNone, valueNull, notTextInsertion, allowExecutionWhenDisabled } },
        { "Print", {106, executePrint, supported, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Redo", {107, executeRedo, supported, enabledRedo, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "RemoveFormat", {108, executeRemoveFormat, supported, enabledRangeInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ScrollPageBackward", {109, executeScrollPageBackward, supportedFromMenuOrKeyBinding, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ScrollPageForward", {110, executeScrollPageForward, supportedFromMenuOrKeyBinding, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ScrollLineUp", {111, executeScrollLineUp, supportedFromMenuOrKeyBinding, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ScrollLineDown", {112, executeScrollLineDown, supportedFromMenuOrKeyBinding, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ScrollToBeginningOfDocument", {113, executeScrollToBeginningOfDocument, supportedFromMenuOrKeyBinding, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ScrollToEndOfDocument", {114, executeScrollToEndOfDocument, supportedFromMenuOrKeyBinding, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SelectAll", {115, executeSelectAll, supported, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SelectLine", {116, executeSelectLine, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SelectParagraph", {117, executeSelectParagraph, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SelectSentence", {118, executeSelectSentence, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SelectToMark", {119, executeSelectToMark, supportedFromMenuOrKeyBinding, enabledVisibleSelectionAndMark, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SelectWord", {120, executeSelectWord, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SetMark", {121, executeSetMark, supportedFromMenuOrKeyBinding, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Strikethrough", {122, executeStrikethrough, supported, enabledInRichlyEditableText, stateStrikethrough, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "StyleWithCSS", {123, executeStyleWithCSS, supported, enabled, stateStyleWithCSS, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Subscript", {124, executeSubscript, supported, enabledInRichlyEditableText, stateSubscript, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Superscript", {125, executeSuperscript, supported, enabledInRichlyEditableText, stateSuperscript, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "SwapWithMark", {126, executeSwapWithMark, supportedFromMenuOrKeyBinding, enabledVisibleSelectionAndMark, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ToggleBold", {127, executeToggleBold, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateBold, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ToggleItalic", {128, executeToggleItalic, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateItalic, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "ToggleUnderline", {129, executeUnderline, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateUnderline, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Transpose", {130, executeTranspose, supported, enableCaretInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Underline", {131, executeUnderline, supported, enabledInRichlyEditableText, stateUnderline, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Undo", {132, executeUndo, supported, enabledUndo, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Unlink", {133, executeUnlink, supported, enabledRangeInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Unscript", {134, executeUnscript, supportedFromMenuOrKeyBinding, enabledInRichlyEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Unselect", {135, executeUnselect, supported, enabledVisibleSelection, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "UseCSS", {136, executeUseCSS, supported, enabled, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "Yank", {137, executeYank, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
        { "YankAndSelect", {138, executeYankAndSelect, supportedFromMenuOrKeyBinding, enabledInEditableText, stateNone, valueNull, notTextInsertion, doNotAllowExecutionWhenDisabled } },
    };

    // These unsupported commands are listed here since they appear in the Microsoft
    // documentation used as the starting point for our DOM executeCommand support.
    //
    // 2D-Position (not supported)
    // AbsolutePosition (not supported)
    // BlockDirLTR (not supported)
    // BlockDirRTL (not supported)
    // BrowseMode (not supported)
    // ClearAuthenticationCache (not supported)
    // CreateBookmark (not supported)
    // DirLTR (not supported)
    // DirRTL (not supported)
    // EditMode (not supported)
    // InlineDirLTR (not supported)
    // InlineDirRTL (not supported)
    // InsertButton (not supported)
    // InsertFieldSet (not supported)
    // InsertIFrame (not supported)
    // InsertInputButton (not supported)
    // InsertInputCheckbox (not supported)
    // InsertInputFileUpload (not supported)
    // InsertInputHidden (not supported)
    // InsertInputImage (not supported)
    // InsertInputPassword (not supported)
    // InsertInputRadio (not supported)
    // InsertInputReset (not supported)
    // InsertInputSubmit (not supported)
    // InsertInputText (not supported)
    // InsertMarquee (not supported)
    // InsertSelectDropDown (not supported)
    // InsertSelectListBox (not supported)
    // InsertTextArea (not supported)
    // LiveResize (not supported)
    // MultipleSelection (not supported)
    // Open (not supported)
    // PlayImage (not supported)
    // Refresh (not supported)
    // RemoveParaFormat (not supported)
    // SaveAs (not supported)
    // SizeToControl (not supported)
    // SizeToControlHeight (not supported)
    // SizeToControlWidth (not supported)
    // Stop (not supported)
    // StopImage (not supported)
    // Unbookmark (not supported)

    CommandMap& commandMap = *new CommandMap;
#if ENABLE(ASSERT)
    HashSet<int> idSet;
#endif
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(commands); ++i) {
        const CommandEntry& command = commands[i];
        ASSERT(!commandMap.get(command.name));
        commandMap.set(command.name, &command.command);
#if ENABLE(ASSERT)
        ASSERT(!idSet.contains(command.command.idForUserMetrics));
        idSet.add(command.command.idForUserMetrics);
#endif
    }

    return commandMap;
}

static const EditorInternalCommand* internalCommand(const String& commandName)
{
    static const CommandMap& commandMap = createCommandMap();
    return commandName.isEmpty() ? 0 : commandMap.get(commandName);
}

Editor::Command Editor::command(const String& commandName)
{
    return Command(internalCommand(commandName), CommandFromMenuOrKeyBinding, m_frame);
}

Editor::Command Editor::command(const String& commandName, EditorCommandSource source)
{
    return Command(internalCommand(commandName), source, m_frame);
}

bool Editor::executeCommand(const String& commandName)
{
    // Specially handling commands that Editor::execCommand does not directly
    // support.
    if (commandName == "DeleteToEndOfParagraph") {
        if (!deleteWithDirection(DirectionForward, ParagraphBoundary, true, false))
            deleteWithDirection(DirectionForward, CharacterGranularity, true, false);
        return true;
    }
    if (commandName == "DeleteBackward")
        return command(AtomicString("BackwardDelete")).execute();
    if (commandName == "DeleteForward")
        return command(AtomicString("ForwardDelete")).execute();
    if (commandName == "AdvanceToNextMisspelling") {
        // Wee need to pass false here or else the currently selected word will never be skipped.
        spellChecker().advanceToNextMisspelling(false);
        return true;
    }
    if (commandName == "ToggleSpellPanel") {
        spellChecker().showSpellingGuessPanel();
        return true;
    }
    return command(commandName).execute();
}

bool Editor::executeCommand(const String& commandName, const String& value)
{
    // moveToBeginningOfDocument and moveToEndfDocument are only handled by WebKit for editable nodes.
    if (!canEdit() && commandName == "moveToBeginningOfDocument")
        return frame().eventHandler().bubblingScroll(ScrollUpIgnoringWritingMode, ScrollByDocument);

    if (!canEdit() && commandName == "moveToEndOfDocument")
        return frame().eventHandler().bubblingScroll(ScrollDownIgnoringWritingMode, ScrollByDocument);

    if (commandName == "showGuessPanel") {
        spellChecker().showSpellingGuessPanel();
        return true;
    }

    return command(commandName).execute(value);
}

Editor::Command::Command()
    : m_command(0)
{
}

Editor::Command::Command(const EditorInternalCommand* command, EditorCommandSource source, PassRefPtrWillBeRawPtr<LocalFrame> frame)
    : m_command(command)
    , m_source(source)
    , m_frame(command ? frame : nullptr)
{
    // Use separate assertions so we can tell which bad thing happened.
    if (!command)
        ASSERT(!m_frame);
    else
        ASSERT(m_frame);
}

bool Editor::Command::execute(const String& parameter, Event* triggeringEvent) const
{
    if (!isEnabled(triggeringEvent)) {
        // Let certain commands be executed when performed explicitly even if they are disabled.
        if (!isSupported() || !m_frame || !m_command->allowExecutionWhenDisabled)
            return false;
    }
    frame().document()->updateLayoutIgnorePendingStylesheets();
    Platform::current()->histogramSparse("WebCore.Editing.Commands", m_command->idForUserMetrics);
    return m_command->execute(*m_frame, triggeringEvent, m_source, parameter);
}

bool Editor::Command::execute(Event* triggeringEvent) const
{
    return execute(String(), triggeringEvent);
}

bool Editor::Command::isSupported() const
{
    if (!m_command)
        return false;
    switch (m_source) {
    case CommandFromMenuOrKeyBinding:
        return true;
    case CommandFromDOM:
        return m_command->isSupportedFromDOM(m_frame.get());
    }
    ASSERT_NOT_REACHED();
    return false;
}

bool Editor::Command::isEnabled(Event* triggeringEvent) const
{
    if (!isSupported() || !m_frame)
        return false;
    return m_command->isEnabled(*m_frame, triggeringEvent, m_source);
}

TriState Editor::Command::state(Event* triggeringEvent) const
{
    if (!isSupported() || !m_frame)
        return FalseTriState;
    return m_command->state(*m_frame, triggeringEvent);
}

String Editor::Command::value(Event* triggeringEvent) const
{
    if (!isSupported() || !m_frame)
        return String();
    if (m_command->value == valueNull && m_command->state != stateNone)
        return m_command->state(*m_frame, triggeringEvent) == TrueTriState ? "true" : "false";
    return m_command->value(*m_frame, triggeringEvent);
}

bool Editor::Command::isTextInsertion() const
{
    return m_command && m_command->isTextInsertion;
}

int Editor::Command::idForHistogram() const
{
    return isSupported() ? m_command->idForUserMetrics : 0;
}

} // namespace blink
