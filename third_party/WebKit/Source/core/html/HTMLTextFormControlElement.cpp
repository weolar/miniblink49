/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
 *           (C) 2006 Alexey Proskuryakov (ap@nypop.com)
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
#include "core/html/HTMLTextFormControlElement.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeList.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/CharacterIterator.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/events/Event.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLBRElement.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutTheme.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "platform/heap/Handle.h"
#include "platform/text/TextBoundaries.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

HTMLTextFormControlElement::HTMLTextFormControlElement(const QualifiedName& tagName, Document& doc, HTMLFormElement* form)
    : HTMLFormControlElementWithState(tagName, doc, form)
    , m_lastChangeWasUserEdit(false)
    , m_cachedSelectionStart(0)
    , m_cachedSelectionEnd(0)
    , m_cachedSelectionDirection(SelectionHasNoDirection)
{
}

HTMLTextFormControlElement::~HTMLTextFormControlElement()
{
}

Node::InsertionNotificationRequest HTMLTextFormControlElement::insertedInto(ContainerNode* insertionPoint)
{
    HTMLFormControlElementWithState::insertedInto(insertionPoint);
    if (!insertionPoint->inDocument())
        return InsertionDone;
    String initialValue = value();
    setTextAsOfLastFormControlChangeEvent(initialValue.isNull() ? emptyString() : initialValue);
    return InsertionDone;
}

void HTMLTextFormControlElement::dispatchFocusEvent(Element* oldFocusedElement, WebFocusType type)
{
    if (supportsPlaceholder())
        updatePlaceholderVisibility(false);
    handleFocusEvent(oldFocusedElement, type);
    HTMLFormControlElementWithState::dispatchFocusEvent(oldFocusedElement, type);
}

void HTMLTextFormControlElement::dispatchBlurEvent(Element* newFocusedElement, WebFocusType type)
{
    if (supportsPlaceholder())
        updatePlaceholderVisibility(false);
    handleBlurEvent();
    HTMLFormControlElementWithState::dispatchBlurEvent(newFocusedElement, type);
}

void HTMLTextFormControlElement::defaultEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::webkitEditableContentChanged && layoutObject() && layoutObject()->isTextControl()) {
        m_lastChangeWasUserEdit = true;
        subtreeHasChanged();
        return;
    }

    HTMLFormControlElementWithState::defaultEventHandler(event);
}

void HTMLTextFormControlElement::forwardEvent(Event* event)
{
    if (event->type() == EventTypeNames::blur || event->type() == EventTypeNames::focus)
        return;
    innerEditorElement()->defaultEventHandler(event);
}

String HTMLTextFormControlElement::strippedPlaceholder() const
{
    // According to the HTML5 specification, we need to remove CR and LF from
    // the attribute value.
    const AtomicString& attributeValue = fastGetAttribute(placeholderAttr);
    if (!attributeValue.contains(newlineCharacter) && !attributeValue.contains(carriageReturnCharacter))
        return attributeValue;

    StringBuilder stripped;
    unsigned length = attributeValue.length();
    stripped.reserveCapacity(length);
    for (unsigned i = 0; i < length; ++i) {
        UChar character = attributeValue[i];
        if (character == newlineCharacter || character == carriageReturnCharacter)
            continue;
        stripped.append(character);
    }
    return stripped.toString();
}

static bool isNotLineBreak(UChar ch) { return ch != newlineCharacter && ch != carriageReturnCharacter; }

bool HTMLTextFormControlElement::isPlaceholderEmpty() const
{
    const AtomicString& attributeValue = fastGetAttribute(placeholderAttr);
    return attributeValue.string().find(isNotLineBreak) == kNotFound;
}

bool HTMLTextFormControlElement::placeholderShouldBeVisible() const
{
    return supportsPlaceholder()
        && isEmptyValue()
        && isEmptySuggestedValue()
        && !isPlaceholderEmpty()
        && (document().focusedElement() != this || (LayoutTheme::theme().shouldShowPlaceholderWhenFocused()))
        && (!layoutObject() || layoutObject()->style()->visibility() == VISIBLE);
}

HTMLElement* HTMLTextFormControlElement::placeholderElement() const
{
    return toHTMLElement(userAgentShadowRoot()->getElementById(ShadowElementNames::placeholder()));
}

void HTMLTextFormControlElement::updatePlaceholderVisibility(bool placeholderValueChanged)
{
    if (!supportsPlaceholder())
        return;
    if (!placeholderElement() || placeholderValueChanged)
        updatePlaceholderText();
    HTMLElement* placeholder = placeholderElement();
    if (!placeholder)
        return;

    placeholder->setInlineStyleProperty(CSSPropertyDisplay, placeholderShouldBeVisible() ? CSSValueBlock : CSSValueNone);
}

void HTMLTextFormControlElement::setSelectionStart(int start)
{
    setSelectionRange(start, std::max(start, selectionEnd()), selectionDirection());
}

void HTMLTextFormControlElement::setSelectionEnd(int end)
{
    setSelectionRange(std::min(end, selectionStart()), end, selectionDirection());
}

void HTMLTextFormControlElement::setSelectionDirection(const String& direction)
{
    setSelectionRange(selectionStart(), selectionEnd(), direction);
}

void HTMLTextFormControlElement::select(NeedToDispatchSelectEvent eventBehaviour)
{
    document().updateLayoutIgnorePendingStylesheets();
    setSelectionRange(0, std::numeric_limits<int>::max(), SelectionHasNoDirection, eventBehaviour, isFocusable() ? ChangeSelectionAndFocus : NotChangeSelection);
}

bool HTMLTextFormControlElement::shouldDispatchFormControlChangeEvent(String& oldValue, String& newValue)
{
    return !equalIgnoringNullity(oldValue, newValue);
}

void HTMLTextFormControlElement::dispatchFormControlChangeEvent()
{
    String newValue = value();
    if (shouldDispatchFormControlChangeEvent(m_textAsOfLastFormControlChangeEvent, newValue)) {
        setTextAsOfLastFormControlChangeEvent(newValue);
        dispatchChangeEvent();
    }
    setChangedSinceLastFormControlChangeEvent(false);
}

void HTMLTextFormControlElement::setRangeText(const String& replacement, ExceptionState& exceptionState)
{
    setRangeText(replacement, selectionStart(), selectionEnd(), String(), exceptionState);
}

void HTMLTextFormControlElement::setRangeText(const String& replacement, unsigned start, unsigned end, const String& selectionMode, ExceptionState& exceptionState)
{
    if (start > end) {
        exceptionState.throwDOMException(IndexSizeError, "The provided start value (" + String::number(start) + ") is larger than the provided end value (" + String::number(end) + ").");
        return;
    }
    if (hasOpenShadowRoot())
        return;

    String text = innerEditorValue();
    unsigned textLength = text.length();
    unsigned replacementLength = replacement.length();
    unsigned newSelectionStart = selectionStart();
    unsigned newSelectionEnd = selectionEnd();

    start = std::min(start, textLength);
    end = std::min(end, textLength);

    if (start < end)
        text.replace(start, end - start, replacement);
    else
        text.insert(replacement, start);

    setInnerEditorValue(text);

    // FIXME: What should happen to the value (as in value()) if there's no layoutObject?
    if (!layoutObject())
        return;

    subtreeHasChanged();

    if (equalIgnoringCase(selectionMode, "select")) {
        newSelectionStart = start;
        newSelectionEnd = start + replacementLength;
    } else if (equalIgnoringCase(selectionMode, "start")) {
        newSelectionStart = newSelectionEnd = start;
    } else if (equalIgnoringCase(selectionMode, "end")) {
        newSelectionStart = newSelectionEnd = start + replacementLength;
    } else {
        // Default is "preserve".
        long delta = replacementLength - (end - start);

        if (newSelectionStart > end)
            newSelectionStart += delta;
        else if (newSelectionStart > start)
            newSelectionStart = start;

        if (newSelectionEnd > end)
            newSelectionEnd += delta;
        else if (newSelectionEnd > start)
            newSelectionEnd = start + replacementLength;
    }

    setSelectionRange(newSelectionStart, newSelectionEnd, SelectionHasNoDirection);
}

void HTMLTextFormControlElement::setSelectionRange(int start, int end, const String& directionString)
{
    TextFieldSelectionDirection direction = SelectionHasNoDirection;
    if (directionString == "forward")
        direction = SelectionHasForwardDirection;
    else if (directionString == "backward")
        direction = SelectionHasBackwardDirection;

    if (direction == SelectionHasNoDirection && document().frame() && document().frame()->editor().behavior().shouldConsiderSelectionAsDirectional())
        direction = SelectionHasForwardDirection;

    return setSelectionRange(start, end, direction);
}

static Position positionForIndex(HTMLElement* innerEditor, int index)
{
    ASSERT(index >= 0);
    if (index == 0) {
        Node* node = NodeTraversal::next(*innerEditor, innerEditor);
        if (node && node->isTextNode())
            return Position(node, 0);
        return Position(innerEditor, 0);
    }
    int remainingCharactersToMoveForward = index;
    Node* lastBrOrText = innerEditor;
    for (Node& node : NodeTraversal::descendantsOf(*innerEditor)) {
        ASSERT(remainingCharactersToMoveForward >= 0);
        if (node.hasTagName(brTag)) {
            if (remainingCharactersToMoveForward == 0)
                return positionBeforeNode(&node);
            --remainingCharactersToMoveForward;
            lastBrOrText = &node;
            continue;
        }

        if (node.isTextNode()) {
            Text& text = toText(node);
            if (remainingCharactersToMoveForward < static_cast<int>(text.length()))
                return Position(&text, remainingCharactersToMoveForward);
            remainingCharactersToMoveForward -= text.length();
            lastBrOrText = &node;
            continue;
        }

        ASSERT_NOT_REACHED();
    }
    return lastPositionInOrAfterNode(lastBrOrText);
}

static int indexForPosition(HTMLElement* innerEditor, const Position& passedPosition)
{
    if (!innerEditor || !innerEditor->contains(passedPosition.anchorNode()) || passedPosition.isNull())
        return 0;

    if (positionBeforeNode(innerEditor) == passedPosition)
        return 0;

    int index = 0;
    Node* startNode = passedPosition.computeNodeBeforePosition();
    if (!startNode)
        startNode = passedPosition.containerNode();
    ASSERT(startNode);
    ASSERT(innerEditor->contains(startNode));

    for (Node* node = startNode; node; node = NodeTraversal::previous(*node, innerEditor)) {
        if (node->isTextNode()) {
            int length = toText(*node).length();
            if (node == passedPosition.containerNode())
                index += std::min(length, passedPosition.offsetInContainerNode());
            else
                index += length;
        } else if (node->hasTagName(brTag)) {
            ++index;
        }
    }

    ASSERT(index >= 0);
    return index;
}

void HTMLTextFormControlElement::setSelectionRange(int start, int end, TextFieldSelectionDirection direction, NeedToDispatchSelectEvent eventBehaviour, SelectionOption selectionOption)
{
    if (hasOpenShadowRoot() || !isTextFormControl() || !inDocument())
        return;

    const int editorValueLength = static_cast<int>(innerEditorValue().length());
    ASSERT(editorValueLength >= 0);
    end = std::max(std::min(end, editorValueLength), 0);
    start = std::min(std::max(start, 0), end);
    cacheSelection(start, end, direction);

    if (selectionOption == NotChangeSelection || (selectionOption == ChangeSelectionIfFocused && document().focusedElement() != this)) {
        if (eventBehaviour == DispatchSelectEvent)
            scheduleSelectEvent();
        return;
    }

    LocalFrame* frame = document().frame();
    HTMLElement* innerEditor = innerEditorElement();
    if (!frame || !innerEditor)
        return;

    Position startPosition = positionForIndex(innerEditor, start);
    Position endPosition = start == end ? startPosition : positionForIndex(innerEditor, end);

    ASSERT(start == indexForPosition(innerEditor, startPosition));
    ASSERT(end == indexForPosition(innerEditor, endPosition));

    // startPosition and endPosition can be null position for example when
    // "-webkit-user-select: none" style attribute is specified.
    if (startPosition.isNotNull() && endPosition.isNotNull()) {
        ASSERT(startPosition.anchorNode()->shadowHost() == this
            && endPosition.anchorNode()->shadowHost() == this);
    }
    VisibleSelection newSelection;
    if (direction == SelectionHasBackwardDirection)
        newSelection.setWithoutValidation(endPosition, startPosition);
    else
        newSelection.setWithoutValidation(startPosition, endPosition);
    newSelection.setIsDirectional(direction != SelectionHasNoDirection);

    frame->selection().setSelection(newSelection, FrameSelection::CloseTyping | FrameSelection::ClearTypingStyle | (selectionOption == ChangeSelectionAndFocus ? 0 : FrameSelection::DoNotSetFocus));
    if (eventBehaviour == DispatchSelectEvent)
        scheduleSelectEvent();
}

VisiblePosition HTMLTextFormControlElement::visiblePositionForIndex(int index) const
{
    if (index <= 0)
        return VisiblePosition(firstPositionInNode(innerEditorElement()), DOWNSTREAM);
    Position start, end;
    bool selected = Range::selectNodeContents(innerEditorElement(), start, end);
    if (!selected)
        return VisiblePosition();
    CharacterIterator it(start, end);
    it.advance(index - 1);
    return VisiblePosition(it.endPosition(), UPSTREAM);
}

int HTMLTextFormControlElement::indexForVisiblePosition(const VisiblePosition& pos) const
{
    Position indexPosition = pos.deepEquivalent().parentAnchoredEquivalent();
    if (enclosingTextFormControl(indexPosition) != this)
        return 0;
    ASSERT(indexPosition.document());
    RefPtrWillBeRawPtr<Range> range = Range::create(*indexPosition.document());
    range->setStart(innerEditorElement(), 0, ASSERT_NO_EXCEPTION);
    range->setEnd(indexPosition.containerNode(), indexPosition.offsetInContainerNode(), ASSERT_NO_EXCEPTION);
    return TextIterator::rangeLength(range->startPosition(), range->endPosition());
}

int HTMLTextFormControlElement::selectionStart() const
{
    if (!isTextFormControl())
        return 0;
    if (document().focusedElement() != this)
        return m_cachedSelectionStart;

    return computeSelectionStart();
}

int HTMLTextFormControlElement::computeSelectionStart() const
{
    ASSERT(isTextFormControl());
    LocalFrame* frame = document().frame();
    if (!frame)
        return 0;

    return indexForPosition(innerEditorElement(), frame->selection().start());
}

int HTMLTextFormControlElement::selectionEnd() const
{
    if (!isTextFormControl())
        return 0;
    if (document().focusedElement() != this)
        return m_cachedSelectionEnd;
    return computeSelectionEnd();
}

int HTMLTextFormControlElement::computeSelectionEnd() const
{
    ASSERT(isTextFormControl());
    LocalFrame* frame = document().frame();
    if (!frame)
        return 0;

    return indexForPosition(innerEditorElement(), frame->selection().end());
}

static const AtomicString& directionString(TextFieldSelectionDirection direction)
{
    DEFINE_STATIC_LOCAL(const AtomicString, none, ("none", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, forward, ("forward", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, backward, ("backward", AtomicString::ConstructFromLiteral));

    switch (direction) {
    case SelectionHasNoDirection:
        return none;
    case SelectionHasForwardDirection:
        return forward;
    case SelectionHasBackwardDirection:
        return backward;
    }

    ASSERT_NOT_REACHED();
    return none;
}

const AtomicString& HTMLTextFormControlElement::selectionDirection() const
{
    if (!isTextFormControl())
        return directionString(SelectionHasNoDirection);
    if (document().focusedElement() != this)
        return directionString(m_cachedSelectionDirection);

    return directionString(computeSelectionDirection());
}

TextFieldSelectionDirection HTMLTextFormControlElement::computeSelectionDirection() const
{
    ASSERT(isTextFormControl());
    LocalFrame* frame = document().frame();
    if (!frame)
        return SelectionHasNoDirection;

    const VisibleSelection& selection = frame->selection().selection();
    return selection.isDirectional() ? (selection.isBaseFirst() ? SelectionHasForwardDirection : SelectionHasBackwardDirection) : SelectionHasNoDirection;
}

static inline void setContainerAndOffsetForRange(Node* node, int offset, Node*& containerNode, int& offsetInContainer)
{
    if (node->isTextNode()) {
        containerNode = node;
        offsetInContainer = offset;
    } else {
        containerNode = node->parentNode();
        offsetInContainer = node->nodeIndex() + offset;
    }
}

PassRefPtrWillBeRawPtr<Range> HTMLTextFormControlElement::selection() const
{
    if (!layoutObject() || !isTextFormControl())
        return nullptr;

    int start = m_cachedSelectionStart;
    int end = m_cachedSelectionEnd;

    ASSERT(start <= end);
    HTMLElement* innerText = innerEditorElement();
    if (!innerText)
        return nullptr;

    if (!innerText->hasChildren())
        return Range::create(document(), innerText, 0, innerText, 0);

    int offset = 0;
    Node* startNode = 0;
    Node* endNode = 0;
    for (Node& node : NodeTraversal::descendantsOf(*innerText)) {
        ASSERT(!node.hasChildren());
        ASSERT(node.isTextNode() || isHTMLBRElement(node));
        int length = node.isTextNode() ? lastOffsetInNode(&node) : 1;

        if (offset <= start && start <= offset + length)
            setContainerAndOffsetForRange(&node, start - offset, startNode, start);

        if (offset <= end && end <= offset + length) {
            setContainerAndOffsetForRange(&node, end - offset, endNode, end);
            break;
        }

        offset += length;
    }

    if (!startNode || !endNode)
        return nullptr;

    return Range::create(document(), startNode, start, endNode, end);
}

const AtomicString& HTMLTextFormControlElement::autocapitalize() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, off, ("off", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, none, ("none", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, characters, ("characters", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, words, ("words", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, sentences, ("sentences", AtomicString::ConstructFromLiteral));

    const AtomicString& value = fastGetAttribute(autocapitalizeAttr);
    if (equalIgnoringCase(value, none) || equalIgnoringCase(value, off))
        return none;
    if (equalIgnoringCase(value, characters))
        return characters;
    if (equalIgnoringCase(value, words))
        return words;
    if (equalIgnoringCase(value, sentences))
        return sentences;

    // Invalid or missing value.
    return defaultAutocapitalize();
}

void HTMLTextFormControlElement::setAutocapitalize(const AtomicString& autocapitalize)
{
    setAttribute(autocapitalizeAttr, autocapitalize);
}

void HTMLTextFormControlElement::restoreCachedSelection()
{
    setSelectionRange(m_cachedSelectionStart, m_cachedSelectionEnd, m_cachedSelectionDirection, NotDispatchSelectEvent);
}

void HTMLTextFormControlElement::selectionChanged(bool userTriggered)
{
    if (!layoutObject() || !isTextFormControl())
        return;

    // selectionStart() or selectionEnd() will return cached selection when this node doesn't have focus
    cacheSelection(computeSelectionStart(), computeSelectionEnd(), computeSelectionDirection());

    if (LocalFrame* frame = document().frame()) {
        if (frame->selection().isRange() && userTriggered)
            dispatchEvent(Event::createBubble(EventTypeNames::select));
    }
}

void HTMLTextFormControlElement::scheduleSelectEvent()
{
    RefPtrWillBeRawPtr<Event> event = Event::createBubble(EventTypeNames::select);
    event->setTarget(this);
    document().enqueueUniqueAnimationFrameEvent(event.release());
}

void HTMLTextFormControlElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == autocapitalizeAttr)
        UseCounter::count(document(), UseCounter::AutocapitalizeAttribute);

    if (name == placeholderAttr) {
        updatePlaceholderVisibility(true);
        UseCounter::count(document(), UseCounter::PlaceholderAttribute);
    } else {
        HTMLFormControlElementWithState::parseAttribute(name, value);
    }
}

bool HTMLTextFormControlElement::lastChangeWasUserEdit() const
{
    if (!isTextFormControl())
        return false;
    return m_lastChangeWasUserEdit;
}

void HTMLTextFormControlElement::setInnerEditorValue(const String& value)
{
    ASSERT(!hasOpenShadowRoot());
    if (!isTextFormControl() || hasOpenShadowRoot())
        return;

    bool textIsChanged = value != innerEditorValue();
    if (textIsChanged || !innerEditorElement()->hasChildren()) {
        if (textIsChanged && layoutObject()) {
            if (AXObjectCache* cache = document().existingAXObjectCache())
                cache->handleTextFormControlChanged(this);
        }
        innerEditorElement()->setInnerText(value, ASSERT_NO_EXCEPTION);

        if (value.endsWith('\n') || value.endsWith('\r'))
            innerEditorElement()->appendChild(HTMLBRElement::create(document()));
    }
}

static String finishText(StringBuilder& result)
{
    // Remove one trailing newline; there's always one that's collapsed out by layoutObject.
    size_t size = result.length();
    if (size && result[size - 1] == '\n')
        result.resize(--size);
    return result.toString();
}

String HTMLTextFormControlElement::innerEditorValue() const
{
    ASSERT(!hasOpenShadowRoot());
    HTMLElement* innerEditor = innerEditorElement();
    if (!innerEditor || !isTextFormControl())
        return emptyString();

    StringBuilder result;
    for (Node& node : NodeTraversal::inclusiveDescendantsOf(*innerEditor)) {
        if (isHTMLBRElement(node))
            result.append(newlineCharacter);
        else if (node.isTextNode())
            result.append(toText(node).data());
    }
    return finishText(result);
}

static void getNextSoftBreak(RootInlineBox*& line, Node*& breakNode, unsigned& breakOffset)
{
    RootInlineBox* next;
    for (; line; line = next) {
        next = line->nextRootBox();
        if (next && !line->endsWithBreak()) {
            ASSERT(line->lineBreakObj());
            breakNode = line->lineBreakObj()->node();
            breakOffset = line->lineBreakPos();
            line = next;
            return;
        }
    }
    breakNode = 0;
    breakOffset = 0;
}

String HTMLTextFormControlElement::valueWithHardLineBreaks() const
{
    // FIXME: It's not acceptable to ignore the HardWrap setting when there is no layoutObject.
    // While we have no evidence this has ever been a practical problem, it would be best to fix it some day.
    HTMLElement* innerText = innerEditorElement();
    if (!innerText || !isTextFormControl())
        return value();

    LayoutBlockFlow* layoutObject = toLayoutBlockFlow(innerText->layoutObject());
    if (!layoutObject)
        return value();

    Node* breakNode;
    unsigned breakOffset;
    RootInlineBox* line = layoutObject->firstRootBox();
    if (!line)
        return value();

    getNextSoftBreak(line, breakNode, breakOffset);

    StringBuilder result;
    for (Node& node : NodeTraversal::descendantsOf(*innerText)) {
        if (isHTMLBRElement(node)) {
            result.append(newlineCharacter);
        } else if (node.isTextNode()) {
            String data = toText(node).data();
            unsigned length = data.length();
            unsigned position = 0;
            while (breakNode == node && breakOffset <= length) {
                if (breakOffset > position) {
                    result.append(data, position, breakOffset - position);
                    position = breakOffset;
                    result.append(newlineCharacter);
                }
                getNextSoftBreak(line, breakNode, breakOffset);
            }
            result.append(data, position, length - position);
        }
        while (breakNode == node)
            getNextSoftBreak(line, breakNode, breakOffset);
    }
    return finishText(result);
}

HTMLTextFormControlElement* enclosingTextFormControl(const Position& position)
{
    ASSERT(position.isNull() || position.anchorType() == PositionAnchorType::OffsetInAnchor
        || position.containerNode() || !position.anchorNode()->shadowHost()
        || (position.anchorNode()->parentNode() && position.anchorNode()->parentNode()->isShadowRoot()));
    return enclosingTextFormControl(position.containerNode());
}

HTMLTextFormControlElement* enclosingTextFormControl(Node* container)
{
    if (!container)
        return nullptr;
    Element* ancestor = container->shadowHost();
    return ancestor && isHTMLTextFormControlElement(*ancestor) && container->containingShadowRoot()->type() == ShadowRootType::UserAgent ? toHTMLTextFormControlElement(ancestor) : 0;
}

String HTMLTextFormControlElement::directionForFormData() const
{
    for (const HTMLElement* element = this; element; element = Traversal<HTMLElement>::firstAncestor(*element)) {
        const AtomicString& dirAttributeValue = element->fastGetAttribute(dirAttr);
        if (dirAttributeValue.isNull())
            continue;

        if (equalIgnoringCase(dirAttributeValue, "rtl") || equalIgnoringCase(dirAttributeValue, "ltr"))
            return dirAttributeValue;

        if (equalIgnoringCase(dirAttributeValue, "auto")) {
            bool isAuto;
            TextDirection textDirection = element->directionalityIfhasDirAutoAttribute(isAuto);
            return textDirection == RTL ? "rtl" : "ltr";
        }
    }

    return "ltr";
}

HTMLElement* HTMLTextFormControlElement::innerEditorElement() const
{
    return toHTMLElement(userAgentShadowRoot()->getElementById(ShadowElementNames::innerEditor()));
}

static Position innerNodePosition(const Position& innerPosition)
{
    ASSERT(innerPosition.anchorType() != PositionAnchorType::BeforeAnchor);
    ASSERT(innerPosition.anchorType() != PositionAnchorType::AfterAnchor);
    HTMLElement* element = toHTMLElement(innerPosition.anchorNode());
    ASSERT(element);
    RefPtrWillBeRawPtr<NodeList> childNodes = element->childNodes();
    if (!childNodes->length())
        return Position(element, 0);

    unsigned offset = 0;

    switch (innerPosition.anchorType()) {
    case PositionAnchorType::OffsetInAnchor:
        offset = std::max(0, std::min(innerPosition.offsetInContainerNode(), static_cast<int>(childNodes->length())));
        break;
    case PositionAnchorType::AfterChildren:
        offset = childNodes->length();
        break;
    default:
        break;
    }

    if (offset == childNodes->length())
        return Position(element->lastChild(), PositionAnchorType::AfterAnchor);

    Node* node = childNodes->item(offset);
    if (node->isTextNode())
        return Position(toText(node), 0);

    return Position(node, PositionAnchorType::BeforeAnchor);
}

enum FindOption {
    FindStart,
    FindEnd
};

static Position findWordBoundary(const HTMLElement* innerEditor, const Position& startPosition, const Position& endPosition, FindOption findOption)
{
    StringBuilder concatTexts;
    Vector<unsigned> lengthList;
    Vector<Text*> textList;

    if (startPosition.anchorNode()->isTextNode())
        ASSERT(startPosition.anchorType() == PositionAnchorType::OffsetInAnchor);
    if (endPosition.anchorNode()->isTextNode())
        ASSERT(endPosition.anchorType() == PositionAnchorType::OffsetInAnchor);

    // Traverse text nodes.
    for (Node* node = startPosition.anchorNode(); node; node = NodeTraversal::next(*node, innerEditor)) {
        bool isStartNode = node == startPosition.anchorNode();
        bool isEndNode = node == endPosition.anchorNode();
        if (node->isTextNode()) {
            Text* text = toText(node);
            const unsigned start = isStartNode ? startPosition.offsetInContainerNode() : 0;
            const unsigned end = isEndNode ? endPosition.offsetInContainerNode() : text->data().length();
            const unsigned length = end - start;

            concatTexts.append(text->data(), start, length);
            lengthList.append(length);
            textList.append(text);
        }

        if (isEndNode)
            break;
    }

    if (concatTexts.length() == 0)
        return startPosition;

    int start, end;
    if (findOption == FindEnd && concatTexts[0] == '\n') {
        // findWordBoundary("\ntext", 0, &start, &end) assigns 1 to |end| but
        // we expect 0 at the case.
        start = 0;
        end = 0;
    } else {
        Vector<UChar> characters;
        concatTexts.toString().appendTo(characters);
        findWordBoundary(characters.data(), characters.size(), findOption == FindStart ? characters.size() : 0, &start, &end);
    }
    ASSERT(start >= 0);
    ASSERT(end >= 0);
    unsigned remainingOffset = findOption == FindStart ? start : end;
    // Find position.
    for (unsigned i = 0; i < lengthList.size(); ++i) {
        if (remainingOffset <= lengthList[i])
            return Position(textList[i], (textList[i] == startPosition.anchorNode()) ? remainingOffset + startPosition.offsetInContainerNode() : remainingOffset);
        remainingOffset -= lengthList[i];
    }

    ASSERT_NOT_REACHED();
    return Position();
}

Position HTMLTextFormControlElement::startOfWord(const Position& position)
{
    const HTMLTextFormControlElement* textFormControl = enclosingTextFormControl(position);
    ASSERT(textFormControl);
    HTMLElement* innerEditor = textFormControl->innerEditorElement();

    const Position startPosition = startOfSentence(position);
    if (startPosition == position)
        return position;
    const Position endPosition = (position.anchorNode() == innerEditor) ? innerNodePosition(position) : position;

    return findWordBoundary(innerEditor, startPosition, endPosition, FindStart);
}

Position HTMLTextFormControlElement::endOfWord(const Position& position)
{
    const HTMLTextFormControlElement* textFormControl = enclosingTextFormControl(position);
    ASSERT(textFormControl);
    HTMLElement* innerEditor = textFormControl->innerEditorElement();

    const Position endPosition = endOfSentence(position);
    if (endPosition == position)
        return position;
    const Position startPosition = (position.anchorNode() == innerEditor) ? innerNodePosition(position) : position;

    return findWordBoundary(innerEditor, startPosition, endPosition, FindEnd);
}

static Position endOfPrevious(const Node& node, HTMLElement* innerEditor)
{
    Node* previousNode = NodeTraversal::previous(node, innerEditor);
    if (!previousNode)
        return Position();

    if (isHTMLBRElement(previousNode))
        return Position(previousNode, PositionAnchorType::AfterAnchor);

    if (previousNode->isTextNode())
        return Position(toText(previousNode), toText(previousNode)->length());

    return Position();
}

static Position previousIfPositionIsAfterLineBreak(const Position& position, HTMLElement* innerEditor)
{
    if (position.isNull())
        return Position();

    // Move back if position is just after line break.
    if (isHTMLBRElement(*position.anchorNode())) {
        switch (position.anchorType()) {
        case PositionAnchorType::AfterAnchor:
            return Position(position.anchorNode(), PositionAnchorType::BeforeAnchor);
        case PositionAnchorType::BeforeAnchor:
            return previousIfPositionIsAfterLineBreak(endOfPrevious(*position.anchorNode(), innerEditor), innerEditor);
        default:
            ASSERT_NOT_REACHED();
        }
    } else if (position.anchorNode()->isTextNode()) {
        Text* textNode = toText(position.anchorNode());
        unsigned offset = position.offsetInContainerNode();
        if (textNode->length() == 0 || offset == 0) {
            return previousIfPositionIsAfterLineBreak(endOfPrevious(*position.anchorNode(), innerEditor), innerEditor);
        }

        if (offset <= textNode->length() && textNode->data()[offset - 1] == '\n') {
            return Position(textNode, offset - 1);
        }
    }

    return position;
}

static inline Position startOfInnerText(const HTMLTextFormControlElement* textFormControl)
{
    return Position(textFormControl->innerEditorElement(), 0);
}

Position HTMLTextFormControlElement::startOfSentence(const Position& position)
{
    HTMLTextFormControlElement* textFormControl = enclosingTextFormControl(position);
    ASSERT(textFormControl);

    HTMLElement* innerEditor = textFormControl->innerEditorElement();
    if (!innerEditor->childNodes()->length())
        return startOfInnerText(textFormControl);

    const Position innerPosition = position.anchorNode() == innerEditor ? innerNodePosition(position) : position;
    const Position pivotPosition = previousIfPositionIsAfterLineBreak(innerPosition, innerEditor);
    if (pivotPosition.isNull())
        return startOfInnerText(textFormControl);

    for (Node* node = pivotPosition.anchorNode(); node; node = NodeTraversal::previous(*node, innerEditor)) {
        bool isPivotNode = (node == pivotPosition.anchorNode());

        if (isHTMLBRElement(node) && (!isPivotNode || pivotPosition.anchorType() == PositionAnchorType::AfterAnchor))
            return Position(node, PositionAnchorType::AfterAnchor);

        if (node->isTextNode()) {
            Text* textNode = toText(node);
            size_t lastLineBreak = textNode->data().substring(0, isPivotNode ? pivotPosition.offsetInContainerNode() : textNode->length()).reverseFind('\n');
            if (lastLineBreak != kNotFound)
                return Position(textNode, lastLineBreak + 1);
        }
    }
    return startOfInnerText(textFormControl);
}

static Position endOfInnerText(const HTMLTextFormControlElement* textFormControl)
{
    HTMLElement* innerEditor = textFormControl->innerEditorElement();
    return Position(innerEditor, innerEditor->childNodes()->length());
}

Position HTMLTextFormControlElement::endOfSentence(const Position& position)
{
    HTMLTextFormControlElement* textFormControl = enclosingTextFormControl(position);
    ASSERT(textFormControl);

    HTMLElement* innerEditor = textFormControl->innerEditorElement();
    if (innerEditor->childNodes()->length() == 0)
        return startOfInnerText(textFormControl);

    const Position pivotPosition = position.anchorNode() == innerEditor ? innerNodePosition(position) : position;
    if (pivotPosition.isNull())
        return startOfInnerText(textFormControl);

    for (Node* node = pivotPosition.anchorNode(); node; node = NodeTraversal::next(*node, innerEditor)) {
        bool isPivotNode = node == pivotPosition.anchorNode();

        if (isHTMLBRElement(node))
            return Position(node, PositionAnchorType::AfterAnchor);

        if (node->isTextNode()) {
            Text* textNode = toText(node);
            size_t firstLineBreak = textNode->data().find('\n', isPivotNode ? pivotPosition.offsetInContainerNode() : 0);
            if (firstLineBreak != kNotFound)
                return Position(textNode, firstLineBreak + 1);
        }
    }
    return endOfInnerText(textFormControl);
}

void HTMLTextFormControlElement::copyNonAttributePropertiesFromElement(const Element& source)
{
    const HTMLTextFormControlElement& sourceElement = static_cast<const HTMLTextFormControlElement&>(source);
    m_lastChangeWasUserEdit = sourceElement.m_lastChangeWasUserEdit;
    HTMLFormControlElement::copyNonAttributePropertiesFromElement(source);
}

} // namespace blink
