/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
 *           (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 * Copyright (C) 2007 Samuel Weinig (sam@webkit.org)
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
#include "core/html/HTMLTextAreaElement.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/events/BeforeTextInsertedEvent.h"
#include "core/events/Event.h"
#include "core/frame/FrameHost.h"
#include "core/frame/LocalFrame.h"
#include "core/html/FormDataList.h"
#include "core/html/forms/FormController.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/layout/LayoutTextControlMultiLine.h"
#include "core/page/ChromeClient.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

static const unsigned defaultRows = 2;
static const unsigned defaultCols = 20;

// On submission, LF characters are converted into CRLF.
// This function returns number of characters considering this.
static unsigned numberOfLineBreaks(const String& text)
{
    unsigned length = text.length();
    unsigned count = 0;
    for (unsigned i = 0; i < length; i++) {
        if (text[i] == '\n')
            count++;
    }
    return count;
}

static inline unsigned computeLengthForSubmission(const String& text)
{
    return text.length() + numberOfLineBreaks(text);
}

HTMLTextAreaElement::HTMLTextAreaElement(Document& document, HTMLFormElement* form)
    : HTMLTextFormControlElement(textareaTag, document, form)
    , m_rows(defaultRows)
    , m_cols(defaultCols)
    , m_wrap(SoftWrap)
    , m_isDirty(false)
    , m_valueIsUpToDate(true)
{
}

PassRefPtrWillBeRawPtr<HTMLTextAreaElement> HTMLTextAreaElement::create(Document& document, HTMLFormElement* form)
{
    RefPtrWillBeRawPtr<HTMLTextAreaElement> textArea = adoptRefWillBeNoop(new HTMLTextAreaElement(document, form));
    textArea->ensureUserAgentShadowRoot();
    return textArea.release();
}

void HTMLTextAreaElement::didAddUserAgentShadowRoot(ShadowRoot& root)
{
    root.appendChild(TextControlInnerEditorElement::create(document()));
}

const AtomicString& HTMLTextAreaElement::formControlType() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, textarea, ("textarea", AtomicString::ConstructFromLiteral));
    return textarea;
}

FormControlState HTMLTextAreaElement::saveFormControlState() const
{
    return m_isDirty ? FormControlState(value()) : FormControlState();
}

void HTMLTextAreaElement::restoreFormControlState(const FormControlState& state)
{
    setValue(state[0]);
}

void HTMLTextAreaElement::childrenChanged(const ChildrenChange& change)
{
    HTMLElement::childrenChanged(change);
    setLastChangeWasNotUserEdit();
    if (m_isDirty)
        setInnerEditorValue(value());
    else
        setNonDirtyValue(defaultValue());
}

bool HTMLTextAreaElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == alignAttr) {
        // Don't map 'align' attribute.  This matches what Firefox, Opera and IE do.
        // See http://bugs.webkit.org/show_bug.cgi?id=7075
        return false;
    }

    if (name == wrapAttr)
        return true;
    return HTMLTextFormControlElement::isPresentationAttribute(name);
}

void HTMLTextAreaElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == wrapAttr) {
        if (shouldWrapText()) {
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWhiteSpace, CSSValuePreWrap);
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWordWrap, CSSValueBreakWord);
        } else {
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWhiteSpace, CSSValuePre);
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWordWrap, CSSValueNormal);
        }
    } else {
        HTMLTextFormControlElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

void HTMLTextAreaElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == rowsAttr) {
        unsigned rows = 0;
        if (value.isEmpty() || !parseHTMLNonNegativeInteger(value, rows) || rows <= 0)
            rows = defaultRows;
        if (m_rows != rows) {
            m_rows = rows;
            if (layoutObject())
                layoutObject()->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
        }
    } else if (name == colsAttr) {
        unsigned cols = 0;
        if (value.isEmpty() || !parseHTMLNonNegativeInteger(value, cols) || cols <= 0)
            cols = defaultCols;
        if (m_cols != cols) {
            m_cols = cols;
            if (LayoutObject* layoutObject = this->layoutObject())
                layoutObject->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
        }
    } else if (name == wrapAttr) {
        // The virtual/physical values were a Netscape extension of HTML 3.0, now deprecated.
        // The soft/hard /off values are a recommendation for HTML 4 extension by IE and NS 4.
        WrapMethod wrap;
        if (equalIgnoringCase(value, "physical") || equalIgnoringCase(value, "hard") || equalIgnoringCase(value, "on"))
            wrap = HardWrap;
        else if (equalIgnoringCase(value, "off"))
            wrap = NoWrap;
        else
            wrap = SoftWrap;
        if (wrap != m_wrap) {
            m_wrap = wrap;
            if (LayoutObject* layoutObject = this->layoutObject())
                layoutObject->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
        }
    } else if (name == accesskeyAttr) {
        // ignore for the moment
    } else if (name == maxlengthAttr) {
        setNeedsValidityCheck();
    } else if (name == minlengthAttr) {
        setNeedsValidityCheck();
    } else {
        HTMLTextFormControlElement::parseAttribute(name, value);
    }
}

LayoutObject* HTMLTextAreaElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutTextControlMultiLine(this);
}

bool HTMLTextAreaElement::appendFormData(FormDataList& encoding, bool)
{
    if (name().isEmpty())
        return false;

    document().updateLayout();

    const String& text = (m_wrap == HardWrap) ? valueWithHardLineBreaks() : value();
    encoding.appendData(name(), text);

    const AtomicString& dirnameAttrValue = fastGetAttribute(dirnameAttr);
    if (!dirnameAttrValue.isNull())
        encoding.appendData(dirnameAttrValue, directionForFormData());
    return true;
}

void HTMLTextAreaElement::resetImpl()
{
    setNonDirtyValue(defaultValue());
}

bool HTMLTextAreaElement::hasCustomFocusLogic() const
{
    return true;
}

bool HTMLTextAreaElement::isKeyboardFocusable() const
{
    // If a given text area can be focused at all, then it will always be keyboard focusable.
    return isFocusable();
}

bool HTMLTextAreaElement::shouldShowFocusRingOnMouseFocus() const
{
    return true;
}

void HTMLTextAreaElement::updateFocusAppearance(bool restorePreviousSelection)
{
    if (!restorePreviousSelection)
        setSelectionRange(0, 0, SelectionHasNoDirection, NotDispatchSelectEvent);
    else
        restoreCachedSelection();

    if (document().frame())
        document().frame()->selection().revealSelection();
}

void HTMLTextAreaElement::defaultEventHandler(Event* event)
{
    if (layoutObject() && (event->isMouseEvent() || event->isDragEvent() || event->hasInterface(EventNames::WheelEvent) || event->type() == EventTypeNames::blur))
        forwardEvent(event);
    else if (layoutObject() && event->isBeforeTextInsertedEvent())
        handleBeforeTextInsertedEvent(static_cast<BeforeTextInsertedEvent*>(event));

    HTMLTextFormControlElement::defaultEventHandler(event);
}

void HTMLTextAreaElement::handleFocusEvent(Element*, WebFocusType)
{
    if (LocalFrame* frame = document().frame())
        frame->spellChecker().didBeginEditing(this);
}

void HTMLTextAreaElement::subtreeHasChanged()
{
    setChangedSinceLastFormControlChangeEvent(true);
    m_valueIsUpToDate = false;
    setNeedsValidityCheck();
    setAutofilled(false);
    updatePlaceholderVisibility(false);

    if (!focused())
        return;

    // When typing in a textarea, childrenChanged is not called, so we need to force the directionality check.
    calculateAndAdjustDirectionality();

    ASSERT(document().isActive());
    document().frameHost()->chromeClient().didChangeValueInTextField(*this);
}

void HTMLTextAreaElement::handleBeforeTextInsertedEvent(BeforeTextInsertedEvent* event) const
{
    ASSERT(event);
    ASSERT(layoutObject());
    int signedMaxLength = maxLength();
    if (signedMaxLength < 0)
        return;
    unsigned unsignedMaxLength = static_cast<unsigned>(signedMaxLength);

    const String& currentValue = innerEditorValue();
    unsigned currentLength = computeLengthForSubmission(currentValue);
    if (currentLength + computeLengthForSubmission(event->text()) < unsignedMaxLength)
        return;

    // selectionLength represents the selection length of this text field to be
    // removed by this insertion.
    // If the text field has no focus, we don't need to take account of the
    // selection length. The selection is the source of text drag-and-drop in
    // that case, and nothing in the text field will be removed.
    unsigned selectionLength = 0;
    if (focused()) {
        Position start, end;
        document().frame()->selection().selection().toNormalizedPositions(start, end);
        selectionLength = computeLengthForSubmission(plainText(start, end));
    }
    ASSERT(currentLength >= selectionLength);
    unsigned baseLength = currentLength - selectionLength;
    unsigned appendableLength = unsignedMaxLength > baseLength ? unsignedMaxLength - baseLength : 0;
    event->setText(sanitizeUserInputValue(event->text(), appendableLength));
}

String HTMLTextAreaElement::sanitizeUserInputValue(const String& proposedValue, unsigned maxLength)
{
    if (maxLength > 0 && U16_IS_LEAD(proposedValue[maxLength - 1]))
        --maxLength;
    return proposedValue.left(maxLength);
}

void HTMLTextAreaElement::updateValue() const
{
    if (m_valueIsUpToDate)
        return;

    m_value = innerEditorValue();
    const_cast<HTMLTextAreaElement*>(this)->m_valueIsUpToDate = true;
    const_cast<HTMLTextAreaElement*>(this)->notifyFormStateChanged();
    m_isDirty = true;
    const_cast<HTMLTextAreaElement*>(this)->updatePlaceholderVisibility(false);
}

String HTMLTextAreaElement::value() const
{
    updateValue();
    return m_value;
}

void HTMLTextAreaElement::setValue(const String& value, TextFieldEventBehavior eventBehavior)
{
    RefPtrWillBeRawPtr<HTMLTextAreaElement> protector(this);
    setValueCommon(value, eventBehavior);
    m_isDirty = true;
    if (document().focusedElement() == this)
        document().frameHost()->chromeClient().didUpdateTextOfFocusedElementByNonUserInput();
}

void HTMLTextAreaElement::setNonDirtyValue(const String& value)
{
    setValueCommon(value, DispatchNoEvent, SetSeletion);
    m_isDirty = false;
}

void HTMLTextAreaElement::setValueCommon(const String& newValue, TextFieldEventBehavior eventBehavior, SetValueCommonOption setValueOption)
{
    // Code elsewhere normalizes line endings added by the user via the keyboard or pasting.
    // We normalize line endings coming from JavaScript here.
    String normalizedValue = newValue.isNull() ? "" : newValue;
    normalizedValue.replace("\r\n", "\n");
    normalizedValue.replace('\r', '\n');

    // Return early because we don't want to trigger other side effects
    // when the value isn't changing.
    // FIXME: Simple early return doesn't match the Firefox ever.
    // Remove these lines.
    if (normalizedValue == value()) {
        if (setValueOption == SetSeletion) {
            setNeedsValidityCheck();
            if (isFinishedParsingChildren()) {
                // Set the caret to the end of the text value except for initialize.
                unsigned endOfString = m_value.length();
                setSelectionRange(endOfString, endOfString, SelectionHasNoDirection, NotDispatchSelectEvent, ChangeSelectionIfFocused);
            }
        }
        return;
    }

    m_value = normalizedValue;
    setInnerEditorValue(m_value);
    if (eventBehavior == DispatchNoEvent)
        setLastChangeWasNotUserEdit();
    updatePlaceholderVisibility(false);
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::ControlValue));
    m_suggestedValue = String();
    setNeedsValidityCheck();
    if (isFinishedParsingChildren()) {
        // Set the caret to the end of the text value except for initialize.
        unsigned endOfString = m_value.length();
        setSelectionRange(endOfString, endOfString, SelectionHasNoDirection, NotDispatchSelectEvent, ChangeSelectionIfFocused);
    }

    notifyFormStateChanged();
    if (eventBehavior == DispatchNoEvent) {
        setTextAsOfLastFormControlChangeEvent(normalizedValue);
    } else {
        if (eventBehavior == DispatchInputAndChangeEvent)
            dispatchFormControlInputEvent();
        dispatchFormControlChangeEvent();
    }
}

void HTMLTextAreaElement::setInnerEditorValue(const String& value)
{
    HTMLTextFormControlElement::setInnerEditorValue(value);
    m_valueIsUpToDate = true;
}

String HTMLTextAreaElement::defaultValue() const
{
    StringBuilder value;

    // Since there may be comments, ignore nodes other than text nodes.
    for (Node* n = firstChild(); n; n = n->nextSibling()) {
        if (n->isTextNode())
            value.append(toText(n)->data());
    }

    return value.toString();
}

void HTMLTextAreaElement::setDefaultValue(const String& defaultValue)
{
    RefPtrWillBeRawPtr<Node> protectFromMutationEvents(this);

    // To preserve comments, remove only the text nodes, then add a single text node.
    WillBeHeapVector<RefPtrWillBeMember<Node>> textNodes;
    for (Node* n = firstChild(); n; n = n->nextSibling()) {
        if (n->isTextNode())
            textNodes.append(n);
    }
    size_t size = textNodes.size();
    for (size_t i = 0; i < size; ++i)
        removeChild(textNodes[i].get(), IGNORE_EXCEPTION);

    // Normalize line endings.
    String value = defaultValue;
    value.replace("\r\n", "\n");
    value.replace('\r', '\n');

    insertBefore(document().createTextNode(value), firstChild(), IGNORE_EXCEPTION);

    if (!m_isDirty)
        setNonDirtyValue(value);
}

int HTMLTextAreaElement::maxLength() const
{
    int value;
    if (!parseHTMLInteger(getAttribute(maxlengthAttr), value))
        return -1;
    return value >= 0 ? value : -1;
}

int HTMLTextAreaElement::minLength() const
{
    int value;
    if (!parseHTMLInteger(getAttribute(minlengthAttr), value))
        return -1;
    return value >= 0 ? value : -1;
}

void HTMLTextAreaElement::setMaxLength(int newValue, ExceptionState& exceptionState)
{
    int min = minLength();
    if (newValue < 0)
        exceptionState.throwDOMException(IndexSizeError, "The value provided (" + String::number(newValue) + ") is not positive or 0.");
    else if (min >= 0 && newValue < min)
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMinimumBound("maxLength", newValue, min));
    else
        setIntegralAttribute(maxlengthAttr, newValue);
}

void HTMLTextAreaElement::setMinLength(int newValue, ExceptionState& exceptionState)
{
    int max = maxLength();
    if (newValue < 0)
        exceptionState.throwDOMException(IndexSizeError, "The value provided (" + String::number(newValue) + ") is not positive or 0.");
    else if (max >= 0 && newValue > max)
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("minLength", newValue, max));
    else
        setIntegralAttribute(minlengthAttr, newValue);
}

String HTMLTextAreaElement::suggestedValue() const
{
    return m_suggestedValue;
}

void HTMLTextAreaElement::setSuggestedValue(const String& value)
{
    m_suggestedValue = value;

    if (!value.isNull())
        setInnerEditorValue(m_suggestedValue);
    else
        setInnerEditorValue(m_value);
    updatePlaceholderVisibility(false);
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::ControlValue));
}

String HTMLTextAreaElement::validationMessage() const
{
    if (!willValidate())
        return String();

    if (customError())
        return customValidationMessage();

    if (valueMissing())
        return locale().queryString(WebLocalizedString::ValidationValueMissing);

    if (tooLong())
        return locale().validationMessageTooLongText(computeLengthForSubmission(value()), maxLength());

    if (tooShort())
        return locale().validationMessageTooShortText(computeLengthForSubmission(value()), minLength());

    return String();
}

bool HTMLTextAreaElement::valueMissing() const
{
    // We should not call value() for performance.
    return willValidate() && valueMissing(nullptr);
}

bool HTMLTextAreaElement::valueMissing(const String* value) const
{
    return isRequiredFormControl() && !isDisabledOrReadOnly() && (value ? *value : this->value()).isEmpty();
}

bool HTMLTextAreaElement::tooLong() const
{
    // We should not call value() for performance.
    return willValidate() && tooLong(nullptr, CheckDirtyFlag);
}

bool HTMLTextAreaElement::tooShort() const
{
    // We should not call value() for performance.
    return willValidate() && tooShort(nullptr, CheckDirtyFlag);
}

bool HTMLTextAreaElement::tooLong(const String* value, NeedsToCheckDirtyFlag check) const
{
    // Return false for the default value or value set by script even if it is
    // longer than maxLength.
    if (check == CheckDirtyFlag && !lastChangeWasUserEdit())
        return false;

    int max = maxLength();
    if (max < 0)
        return false;
    return computeLengthForSubmission(value ? *value : this->value()) > static_cast<unsigned>(max);
}

bool HTMLTextAreaElement::tooShort(const String* value, NeedsToCheckDirtyFlag check) const
{
    // Return false for the default value or value set by script even if it is
    // shorter than minLength.
    if (check == CheckDirtyFlag && !lastChangeWasUserEdit())
        return false;

    int min = minLength();
    if (min <= 0)
        return false;
    // An empty string is excluded from minlength check.
    unsigned len = computeLengthForSubmission(value ? *value : this->value());
    return len > 0 && len < static_cast<unsigned>(min);
}

bool HTMLTextAreaElement::isValidValue(const String& candidate) const
{
    return !valueMissing(&candidate) && !tooLong(&candidate, IgnoreDirtyFlag) && !tooShort(&candidate, IgnoreDirtyFlag);
}

void HTMLTextAreaElement::accessKeyAction(bool)
{
    focus();
}

void HTMLTextAreaElement::setCols(unsigned cols)
{
    setUnsignedIntegralAttribute(colsAttr, cols);
}

void HTMLTextAreaElement::setRows(unsigned rows)
{
    setUnsignedIntegralAttribute(rowsAttr, rows);
}

bool HTMLTextAreaElement::matchesReadOnlyPseudoClass() const
{
    return isReadOnly();
}

bool HTMLTextAreaElement::matchesReadWritePseudoClass() const
{
    return !isReadOnly();
}

void HTMLTextAreaElement::updatePlaceholderText()
{
    HTMLElement* placeholder = placeholderElement();
    const AtomicString& placeholderText = fastGetAttribute(placeholderAttr);
    if (placeholderText.isEmpty()) {
        if (placeholder)
            userAgentShadowRoot()->removeChild(placeholder);
        return;
    }
    if (!placeholder) {
        RefPtrWillBeRawPtr<HTMLDivElement> newElement = HTMLDivElement::create(document());
        placeholder = newElement.get();
        placeholder->setShadowPseudoId(AtomicString("-webkit-input-placeholder", AtomicString::ConstructFromLiteral));
        placeholder->setAttribute(idAttr, ShadowElementNames::placeholder());
        userAgentShadowRoot()->insertBefore(placeholder, innerEditorElement()->nextSibling());
    }
    placeholder->setTextContent(placeholderText);
}

bool HTMLTextAreaElement::isInteractiveContent() const
{
    return true;
}

bool HTMLTextAreaElement::supportsAutofocus() const
{
    return true;
}

const AtomicString& HTMLTextAreaElement::defaultAutocapitalize() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, sentences, ("sentences", AtomicString::ConstructFromLiteral));
    return sentences;
}

void HTMLTextAreaElement::copyNonAttributePropertiesFromElement(const Element& source)
{
    const HTMLTextAreaElement& sourceElement = static_cast<const HTMLTextAreaElement&>(source);
    setValueCommon(sourceElement.value(), DispatchNoEvent, SetSeletion);
    m_isDirty = sourceElement.m_isDirty;
    HTMLTextFormControlElement::copyNonAttributePropertiesFromElement(source);
}

} // namespace blink
