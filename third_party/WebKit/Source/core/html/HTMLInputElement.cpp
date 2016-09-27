/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 *           (C) 2006 Alexey Proskuryakov (ap@nypop.com)
 * Copyright (C) 2007 Samuel Weinig (sam@webkit.org)
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2012 Samsung Electronics. All rights reserved.
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
#include "core/html/HTMLInputElement.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptEventListener.h"
#include "bindings/core/v8/V8DOMActivityLogger.h"
#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Document.h"
#include "core/dom/IdTargetObserver.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/SpellChecker.h"
#include "core/events/BeforeTextInsertedEvent.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/ScopedEventQueue.h"
#include "core/events/TouchEvent.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDataListOptionsCollection.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLImageLoader.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/forms/ColorChooser.h"
#include "core/html/forms/DateTimeChooser.h"
#include "core/html/forms/FileInputType.h"
#include "core/html/forms/FormController.h"
#include "core/html/forms/InputType.h"
#include "core/html/forms/SearchInputType.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutTextControlSingleLine.h"
#include "core/layout/LayoutTheme.h"
#include "core/page/ChromeClient.h"
#include "platform/Language.h"
#include "platform/PlatformMouseEvent.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/MathExtras.h"

namespace blink {

using namespace HTMLNames;

class ListAttributeTargetObserver : public IdTargetObserver {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(ListAttributeTargetObserver);
public:
    static PassOwnPtrWillBeRawPtr<ListAttributeTargetObserver> create(const AtomicString& id, HTMLInputElement*);
    DECLARE_VIRTUAL_TRACE();
    void idTargetChanged() override;

private:
    ListAttributeTargetObserver(const AtomicString& id, HTMLInputElement*);

    RawPtrWillBeMember<HTMLInputElement> m_element;
};

// FIXME: According to HTML4, the length attribute's value can be arbitrarily
// large. However, due to https://bugs.webkit.org/show_bug.cgi?id=14536 things
// get rather sluggish when a text field has a larger number of characters than
// this, even when just clicking in the text field.
const int HTMLInputElement::maximumLength = 524288;
const int defaultSize = 20;
const int maxSavedResults = 256;

HTMLInputElement::HTMLInputElement(Document& document, HTMLFormElement* form, bool createdByParser)
    : HTMLTextFormControlElement(inputTag, document, form)
    , m_size(defaultSize)
    , m_maxLength(maximumLength)
    , m_minLength(0)
    , m_maxResults(-1)
    , m_isChecked(false)
    , m_reflectsCheckedAttribute(true)
    , m_isIndeterminate(false)
    , m_isActivatedSubmit(false)
    , m_autocomplete(Uninitialized)
    , m_hasNonEmptyList(false)
    , m_stateRestored(false)
    , m_parsingInProgress(createdByParser)
    , m_valueAttributeWasUpdatedAfterParsing(false)
    , m_canReceiveDroppedFiles(false)
    , m_hasTouchEventHandler(false)
    , m_shouldRevealPassword(false)
    , m_needsToUpdateViewValue(true)
    // |m_inputType| is lazily created when constructed by the parser to avoid
    // constructing unnecessarily a text inputType and its shadow subtree, just
    // to destroy them when the |type| attribute gets set by the parser to
    // something else than 'text'.
    , m_inputType(createdByParser ? nullptr : InputType::createText(*this))
    , m_inputTypeView(m_inputType)
{
    setHasCustomStyleCallbacks();
}

PassRefPtrWillBeRawPtr<HTMLInputElement> HTMLInputElement::create(Document& document, HTMLFormElement* form, bool createdByParser)
{
    RefPtrWillBeRawPtr<HTMLInputElement> inputElement = adoptRefWillBeNoop(new HTMLInputElement(document, form, createdByParser));
    if (!createdByParser)
        inputElement->ensureUserAgentShadowRoot();
    return inputElement.release();
}

DEFINE_TRACE(HTMLInputElement)
{
    visitor->trace(m_inputType);
    visitor->trace(m_inputTypeView);
    visitor->trace(m_listAttributeTargetObserver);
    visitor->trace(m_imageLoader);
    HTMLTextFormControlElement::trace(visitor);
}

HTMLImageLoader& HTMLInputElement::ensureImageLoader()
{
    if (!m_imageLoader)
        m_imageLoader = HTMLImageLoader::create(this);
    return *m_imageLoader;
}

void HTMLInputElement::didAddUserAgentShadowRoot(ShadowRoot&)
{
    m_inputTypeView->createShadowSubtree();
}

void HTMLInputElement::willAddFirstAuthorShadowRoot()
{
    m_inputTypeView->destroyShadowSubtree();
    m_inputTypeView = InputTypeView::create(*this);
    lazyReattachIfAttached();
}

HTMLInputElement::~HTMLInputElement()
{
#if !ENABLE(OILPAN)
    // Need to remove form association while this is still an HTMLInputElement
    // so that virtual functions are called correctly.
    setForm(0);
    // setForm(0) may register this to a document-level radio button group.
    // We should unregister it to avoid accessing a deleted object.
    if (type() == InputTypeNames::radio)
        document().formController().radioButtonGroupScope().removeButton(this);
    if (m_hasTouchEventHandler && document().frameHost())
        document().frameHost()->eventHandlerRegistry().didRemoveEventHandler(*this, EventHandlerRegistry::TouchEvent);
#endif
}

const AtomicString& HTMLInputElement::name() const
{
    return m_name.isNull() ? emptyAtom : m_name;
}

Vector<FileChooserFileInfo> HTMLInputElement::filesFromFileInputFormControlState(const FormControlState& state)
{
    return FileInputType::filesFromFormControlState(state);
}

bool HTMLInputElement::shouldAutocomplete() const
{
    if (m_autocomplete != Uninitialized)
        return m_autocomplete == On;
    return HTMLTextFormControlElement::shouldAutocomplete();
}

bool HTMLInputElement::isValidValue(const String& value) const
{
    if (!m_inputType->canSetStringValue()) {
        ASSERT_NOT_REACHED();
        return false;
    }
    return !m_inputType->typeMismatchFor(value)
        && !m_inputType->stepMismatch(value)
        && !m_inputType->rangeUnderflow(value)
        && !m_inputType->rangeOverflow(value)
        && !tooLong(value, IgnoreDirtyFlag)
        && !tooShort(value, IgnoreDirtyFlag)
        && !m_inputType->patternMismatch(value)
        && !m_inputType->valueMissing(value);
}

bool HTMLInputElement::tooLong() const
{
    return willValidate() && tooLong(value(), CheckDirtyFlag);
}

bool HTMLInputElement::tooShort() const
{
    return willValidate() && tooShort(value(), CheckDirtyFlag);
}

bool HTMLInputElement::typeMismatch() const
{
    return willValidate() && m_inputType->typeMismatch();
}

bool HTMLInputElement::valueMissing() const
{
    return willValidate() && m_inputType->valueMissing(value());
}

bool HTMLInputElement::hasBadInput() const
{
    return willValidate() && m_inputType->hasBadInput();
}

bool HTMLInputElement::patternMismatch() const
{
    return willValidate() && m_inputType->patternMismatch(value());
}

bool HTMLInputElement::tooLong(const String& value, NeedsToCheckDirtyFlag check) const
{
    return m_inputType->tooLong(value, check);
}

bool HTMLInputElement::tooShort(const String& value, NeedsToCheckDirtyFlag check) const
{
    return m_inputType->tooShort(value, check);
}

bool HTMLInputElement::rangeUnderflow() const
{
    return willValidate() && m_inputType->rangeUnderflow(value());
}

bool HTMLInputElement::rangeOverflow() const
{
    return willValidate() && m_inputType->rangeOverflow(value());
}

String HTMLInputElement::validationMessage() const
{
    if (!willValidate())
        return String();

    if (customError())
        return customValidationMessage();

    return m_inputType->validationMessage();
}

double HTMLInputElement::minimum() const
{
    return m_inputType->minimum();
}

double HTMLInputElement::maximum() const
{
    return m_inputType->maximum();
}

bool HTMLInputElement::stepMismatch() const
{
    return willValidate() && m_inputType->stepMismatch(value());
}

bool HTMLInputElement::getAllowedValueStep(Decimal* step) const
{
    return m_inputType->getAllowedValueStep(step);
}

StepRange HTMLInputElement::createStepRange(AnyStepHandling anyStepHandling) const
{
    return m_inputType->createStepRange(anyStepHandling);
}

Decimal HTMLInputElement::findClosestTickMarkValue(const Decimal& value)
{
    return m_inputType->findClosestTickMarkValue(value);
}

void HTMLInputElement::stepUp(int n, ExceptionState& exceptionState)
{
    m_inputType->stepUp(n, exceptionState);
}

void HTMLInputElement::stepDown(int n, ExceptionState& exceptionState)
{
    m_inputType->stepUp(-n, exceptionState);
}

void HTMLInputElement::blur()
{
    m_inputTypeView->blur();
}

void HTMLInputElement::defaultBlur()
{
    HTMLTextFormControlElement::blur();
}

bool HTMLInputElement::hasCustomFocusLogic() const
{
    return m_inputTypeView->hasCustomFocusLogic();
}

bool HTMLInputElement::isKeyboardFocusable() const
{
    return m_inputType->isKeyboardFocusable();
}

bool HTMLInputElement::shouldShowFocusRingOnMouseFocus() const
{
    return m_inputType->shouldShowFocusRingOnMouseFocus();
}

void HTMLInputElement::updateFocusAppearance(bool restorePreviousSelection)
{
    if (isTextField()) {
        if (!restorePreviousSelection)
            select(NotDispatchSelectEvent);
        else
            restoreCachedSelection();
        if (document().frame())
            document().frame()->selection().revealSelection();
    } else {
        HTMLTextFormControlElement::updateFocusAppearance(restorePreviousSelection);
    }
}

void HTMLInputElement::beginEditing()
{
    ASSERT(document().isActive());
    if (!document().isActive())
        return;

    if (!isTextField())
        return;

    document().frame()->spellChecker().didBeginEditing(this);
}

void HTMLInputElement::endEditing()
{
    ASSERT(document().isActive());
    if (!document().isActive())
        return;

    if (!isTextField())
        return;

    LocalFrame* frame = document().frame();
    frame->spellChecker().didEndEditingOnTextField(this);
    frame->host()->chromeClient().didEndEditingOnTextField(*this);
}

void HTMLInputElement::handleFocusEvent(Element* oldFocusedElement, WebFocusType type)
{
    m_inputTypeView->handleFocusEvent(oldFocusedElement, type);
    m_inputType->enableSecureTextInput();
}

void HTMLInputElement::dispatchFocusInEvent(const AtomicString& eventType, Element* oldFocusedElement, WebFocusType type)
{
    if (eventType == EventTypeNames::DOMFocusIn)
        m_inputTypeView->handleFocusInEvent(oldFocusedElement, type);
    HTMLFormControlElementWithState::dispatchFocusInEvent(eventType, oldFocusedElement, type);
}

void HTMLInputElement::handleBlurEvent()
{
    m_inputType->disableSecureTextInput();
    m_inputTypeView->handleBlurEvent();
}

void HTMLInputElement::setType(const AtomicString& type)
{
    setAttribute(typeAttr, type);
}

void HTMLInputElement::updateTouchEventHandlerRegistry()
{
    ASSERT(m_inputTypeView);

    bool hasTouchEventHandler = m_inputTypeView->hasTouchEventHandler();
    if (hasTouchEventHandler == !!m_hasTouchEventHandler)
        return;
    // If the Document is being or has been stopped, don't register any handlers.
    if (document().frameHost() && document().lifecycle().state() < DocumentLifecycle::Stopping) {
        EventHandlerRegistry& registry = document().frameHost()->eventHandlerRegistry();
        if (hasTouchEventHandler)
            registry.didAddEventHandler(*this, EventHandlerRegistry::TouchEvent);
        else
            registry.didRemoveEventHandler(*this, EventHandlerRegistry::TouchEvent);
    }
    m_hasTouchEventHandler = hasTouchEventHandler;
}

void HTMLInputElement::initializeTypeInParsing()
{
    ASSERT(m_parsingInProgress);
    ASSERT(!m_inputType);
    ASSERT(!m_inputTypeView);

    const AtomicString& newTypeName = InputType::normalizeTypeName(fastGetAttribute(typeAttr));
    m_inputType = InputType::create(*this, newTypeName);
    m_inputTypeView = m_inputType;
    ensureUserAgentShadowRoot();

    updateTouchEventHandlerRegistry();

    setNeedsWillValidateCheck();

    m_inputType->warnIfValueIsInvalid(fastGetAttribute(valueAttr).string());

    m_inputTypeView->updateView();
    setTextAsOfLastFormControlChangeEvent(value());
    setChangedSinceLastFormControlChangeEvent(false);
}

void HTMLInputElement::updateType()
{
    ASSERT(m_inputType);
    ASSERT(m_inputTypeView);

    const AtomicString& newTypeName = InputType::normalizeTypeName(fastGetAttribute(typeAttr));
    if (m_inputType->formControlType() == newTypeName)
        return;

    RefPtrWillBeRawPtr<InputType> newType = InputType::create(*this, newTypeName);
    removeFromRadioButtonGroup();

    bool didStoreValue = m_inputType->storesValueSeparateFromAttribute();
    bool didRespectHeightAndWidth = m_inputType->shouldRespectHeightAndWidthAttributes();

    m_inputTypeView->destroyShadowSubtree();
    lazyReattachIfAttached();

    m_inputType = newType.release();
    if (hasOpenShadowRoot())
        m_inputTypeView = InputTypeView::create(*this);
    else
        m_inputTypeView = m_inputType;
    m_inputTypeView->createShadowSubtree();

    updateTouchEventHandlerRegistry();

    setNeedsWillValidateCheck();

    bool willStoreValue = m_inputType->storesValueSeparateFromAttribute();

    if (didStoreValue && !willStoreValue && hasDirtyValue()) {
        setAttribute(valueAttr, AtomicString(m_valueIfDirty));
        m_valueIfDirty = String();
    }
    if (!didStoreValue && willStoreValue) {
        AtomicString valueString = fastGetAttribute(valueAttr);
        m_inputType->warnIfValueIsInvalid(valueString);
        m_valueIfDirty = sanitizeValue(valueString);
    } else {
        if (!hasDirtyValue())
            m_inputType->warnIfValueIsInvalid(fastGetAttribute(valueAttr).string());
        updateValueIfNeeded();
    }

    m_needsToUpdateViewValue = true;
    m_inputTypeView->updateView();

    if (didRespectHeightAndWidth != m_inputType->shouldRespectHeightAndWidthAttributes()) {
        ASSERT(elementData());
        AttributeCollection attributes = attributesWithoutUpdate();
        if (const Attribute* height = attributes.find(heightAttr))
            attributeChanged(heightAttr, height->value());
        if (const Attribute* width = attributes.find(widthAttr))
            attributeChanged(widthAttr, width->value());
        if (const Attribute* align = attributes.find(alignAttr))
            attributeChanged(alignAttr, align->value());
    }

    if (document().focusedElement() == this)
        document().updateFocusAppearanceSoon(true /* restore selection */);

    setTextAsOfLastFormControlChangeEvent(value());
    setChangedSinceLastFormControlChangeEvent(false);

    addToRadioButtonGroup();

    setNeedsValidityCheck();
    notifyFormStateChanged();
}

void HTMLInputElement::subtreeHasChanged()
{
    m_inputTypeView->subtreeHasChanged();
    // When typing in an input field, childrenChanged is not called, so we need to force the directionality check.
    calculateAndAdjustDirectionality();
}

const AtomicString& HTMLInputElement::formControlType() const
{
    return m_inputType->formControlType();
}

bool HTMLInputElement::shouldSaveAndRestoreFormControlState() const
{
    if (!m_inputType->shouldSaveAndRestoreFormControlState())
        return false;
    return HTMLTextFormControlElement::shouldSaveAndRestoreFormControlState();
}

FormControlState HTMLInputElement::saveFormControlState() const
{
    return m_inputType->saveFormControlState();
}

void HTMLInputElement::restoreFormControlState(const FormControlState& state)
{
    m_inputType->restoreFormControlState(state);
    m_stateRestored = true;
}

bool HTMLInputElement::canStartSelection() const
{
    if (!isTextField())
        return false;
    return HTMLTextFormControlElement::canStartSelection();
}

int HTMLInputElement::selectionStartForBinding(ExceptionState& exceptionState) const
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return 0;
    }
    return HTMLTextFormControlElement::selectionStart();
}

int HTMLInputElement::selectionEndForBinding(ExceptionState& exceptionState) const
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return 0;
    }
    return HTMLTextFormControlElement::selectionEnd();
}

String HTMLInputElement::selectionDirectionForBinding(ExceptionState& exceptionState) const
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return String();
    }
    return HTMLTextFormControlElement::selectionDirection();
}

void HTMLInputElement::setSelectionStartForBinding(int start, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }
    HTMLTextFormControlElement::setSelectionStart(start);
}

void HTMLInputElement::setSelectionEndForBinding(int end, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }
    HTMLTextFormControlElement::setSelectionEnd(end);
}

void HTMLInputElement::setSelectionDirectionForBinding(const String& direction, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }
    HTMLTextFormControlElement::setSelectionDirection(direction);
}

void HTMLInputElement::setSelectionRangeForBinding(int start, int end, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }
    HTMLTextFormControlElement::setSelectionRange(start, end);
}

void HTMLInputElement::setSelectionRangeForBinding(int start, int end, const String& direction, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }
    HTMLTextFormControlElement::setSelectionRange(start, end, direction);
}

void HTMLInputElement::accessKeyAction(bool sendMouseEvents)
{
    m_inputType->accessKeyAction(sendMouseEvents);
}

bool HTMLInputElement::isPresentationAttribute(const QualifiedName& name) const
{
    // FIXME: Remove type check.
    if (name == vspaceAttr || name == hspaceAttr || name == alignAttr || name == widthAttr || name == heightAttr || (name == borderAttr && type() == InputTypeNames::image))
        return true;
    return HTMLTextFormControlElement::isPresentationAttribute(name);
}

void HTMLInputElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == vspaceAttr) {
        addHTMLLengthToStyle(style, CSSPropertyMarginTop, value);
        addHTMLLengthToStyle(style, CSSPropertyMarginBottom, value);
    } else if (name == hspaceAttr) {
        addHTMLLengthToStyle(style, CSSPropertyMarginLeft, value);
        addHTMLLengthToStyle(style, CSSPropertyMarginRight, value);
    } else if (name == alignAttr) {
        if (m_inputType->shouldRespectAlignAttribute())
            applyAlignmentAttributeToStyle(value, style);
    } else if (name == widthAttr) {
        if (m_inputType->shouldRespectHeightAndWidthAttributes())
            addHTMLLengthToStyle(style, CSSPropertyWidth, value);
    } else if (name == heightAttr) {
        if (m_inputType->shouldRespectHeightAndWidthAttributes())
            addHTMLLengthToStyle(style, CSSPropertyHeight, value);
    } else if (name == borderAttr && type() == InputTypeNames::image) { // FIXME: Remove type check.
        applyBorderAttributeToStyle(value, style);
    } else {
        HTMLTextFormControlElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

void HTMLInputElement::attributeWillChange(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == formactionAttr && inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("input");
            argv.append(formactionAttr.toString());
            argv.append(oldValue);
            argv.append(newValue);
            activityLogger->logEvent("blinkSetAttribute", argv.size(), argv.data());
        }
    }
    HTMLTextFormControlElement::attributeWillChange(name, oldValue, newValue);
}

void HTMLInputElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    ASSERT(m_inputType);
    ASSERT(m_inputTypeView);

    if (name == nameAttr) {
        removeFromRadioButtonGroup();
        m_name = value;
        addToRadioButtonGroup();
        HTMLTextFormControlElement::parseAttribute(name, value);
    } else if (name == autocompleteAttr) {
        if (equalIgnoringCase(value, "off")) {
            m_autocomplete = Off;
        } else {
            if (value.isEmpty())
                m_autocomplete = Uninitialized;
            else
                m_autocomplete = On;
        }
    } else if (name == typeAttr) {
        updateType();
    } else if (name == valueAttr) {
        // We only need to setChanged if the form is looking at the default value right now.
        if (!hasDirtyValue()) {
            updatePlaceholderVisibility(false);
            setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(valueAttr));
        }
        m_needsToUpdateViewValue = true;
        setNeedsValidityCheck();
        m_valueAttributeWasUpdatedAfterParsing = !m_parsingInProgress;
        m_inputType->warnIfValueIsInvalidAndElementIsVisible(value);
        m_inputTypeView->valueAttributeChanged();
    } else if (name == checkedAttr) {
        // Another radio button in the same group might be checked by state
        // restore. We shouldn't call setChecked() even if this has the checked
        // attribute. So, delay the setChecked() call until
        // finishParsingChildren() is called if parsing is in progress.
        if (!m_parsingInProgress && m_reflectsCheckedAttribute) {
            setChecked(!value.isNull());
            m_reflectsCheckedAttribute = true;
        }
    } else if (name == maxlengthAttr) {
        parseMaxLengthAttribute(value);
    } else if (name == minlengthAttr) {
        parseMinLengthAttribute(value);
    } else if (name == sizeAttr) {
        int oldSize = m_size;
        m_size = defaultSize;
        int valueAsInteger;
        if (!value.isEmpty() && parseHTMLInteger(value, valueAsInteger) && valueAsInteger > 0)
            m_size = valueAsInteger;
        if (m_size != oldSize && layoutObject())
            layoutObject()->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
    } else if (name == altAttr) {
        m_inputTypeView->altAttributeChanged();
    } else if (name == srcAttr) {
        m_inputTypeView->srcAttributeChanged();
    } else if (name == usemapAttr || name == accesskeyAttr) {
        // FIXME: ignore for the moment
    } else if (name == onsearchAttr) {
        // Search field and slider attributes all just cause updateFromElement to be called through style recalcing.
        setAttributeEventListener(EventTypeNames::search, createAttributeEventListener(this, name, value, eventParameterName()));
    } else if (name == resultsAttr) {
        int oldResults = m_maxResults;
        m_maxResults = !value.isNull() ? std::min(value.toInt(), maxSavedResults) : -1;
        // FIXME: Detaching just for maxResults change is not ideal.  We should figure out the right
        // time to relayout for this change.
        if (m_maxResults != oldResults && (m_maxResults <= 0 || oldResults <= 0))
            lazyReattachIfAttached();
        setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(resultsAttr));
        UseCounter::count(document(), UseCounter::ResultsAttribute);
    } else if (name == incrementalAttr) {
        setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(incrementalAttr));
        UseCounter::count(document(), UseCounter::IncrementalAttribute);
    } else if (name == minAttr) {
        m_inputTypeView->minOrMaxAttributeChanged();
        m_inputType->sanitizeValueInResponseToMinOrMaxAttributeChange();
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::MinAttribute);
    } else if (name == maxAttr) {
        m_inputTypeView->minOrMaxAttributeChanged();
        m_inputType->sanitizeValueInResponseToMinOrMaxAttributeChange();
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::MaxAttribute);
    } else if (name == multipleAttr) {
        m_inputTypeView->multipleAttributeChanged();
        setNeedsValidityCheck();
    } else if (name == stepAttr) {
        m_inputTypeView->stepAttributeChanged();
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::StepAttribute);
    } else if (name == patternAttr) {
        setNeedsValidityCheck();
        UseCounter::count(document(), UseCounter::PatternAttribute);
    } else if (name == disabledAttr) {
        HTMLTextFormControlElement::parseAttribute(name, value);
        m_inputTypeView->disabledAttributeChanged();
    } else if (name == readonlyAttr) {
        HTMLTextFormControlElement::parseAttribute(name, value);
        m_inputTypeView->readonlyAttributeChanged();
    } else if (name == listAttr) {
        m_hasNonEmptyList = !value.isEmpty();
        if (m_hasNonEmptyList) {
            resetListAttributeTargetObserver();
            listAttributeTargetChanged();
        }
        UseCounter::count(document(), UseCounter::ListAttribute);
    } else if (name == webkitdirectoryAttr) {
        HTMLTextFormControlElement::parseAttribute(name, value);
        UseCounter::count(document(), UseCounter::PrefixedDirectoryAttribute);
    } else {
        HTMLTextFormControlElement::parseAttribute(name, value);
    }
    m_inputTypeView->attributeChanged();
}

void HTMLInputElement::parserDidSetAttributes()
{
    ASSERT(m_parsingInProgress);
    initializeTypeInParsing();
}

void HTMLInputElement::finishParsingChildren()
{
    m_parsingInProgress = false;
    ASSERT(m_inputType);
    ASSERT(m_inputTypeView);
    HTMLTextFormControlElement::finishParsingChildren();
    if (!m_stateRestored) {
        bool checked = hasAttribute(checkedAttr);
        if (checked)
            setChecked(checked);
        m_reflectsCheckedAttribute = true;
    }
}

bool HTMLInputElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    return m_inputType->layoutObjectIsNeeded() && HTMLTextFormControlElement::layoutObjectIsNeeded(style);
}

LayoutObject* HTMLInputElement::createLayoutObject(const ComputedStyle& style)
{
    return m_inputTypeView->createLayoutObject(style);
}

void HTMLInputElement::attach(const AttachContext& context)
{
    HTMLTextFormControlElement::attach(context);

    m_inputTypeView->startResourceLoading();
    m_inputType->countUsage();

    if (document().focusedElement() == this)
        document().updateFocusAppearanceSoon(true /* restore selection */);
}

void HTMLInputElement::detach(const AttachContext& context)
{
    HTMLTextFormControlElement::detach(context);
    m_needsToUpdateViewValue = true;
    m_inputTypeView->closePopupView();
}

String HTMLInputElement::altText() const
{
    // http://www.w3.org/TR/1998/REC-html40-19980424/appendix/notes.html#altgen
    // also heavily discussed by Hixie on bugzilla
    // note this is intentionally different to HTMLImageElement::altText()
    String alt = fastGetAttribute(altAttr);
    // fall back to title attribute
    if (alt.isNull())
        alt = fastGetAttribute(titleAttr);
    if (alt.isNull())
        alt = fastGetAttribute(valueAttr);
    if (alt.isEmpty())
        alt = locale().queryString(WebLocalizedString::InputElementAltText);
    return alt;
}

bool HTMLInputElement::canBeSuccessfulSubmitButton() const
{
    return m_inputType->canBeSuccessfulSubmitButton();
}

bool HTMLInputElement::isActivatedSubmit() const
{
    return m_isActivatedSubmit;
}

void HTMLInputElement::setActivatedSubmit(bool flag)
{
    m_isActivatedSubmit = flag;
}

bool HTMLInputElement::appendFormData(FormDataList& encoding, bool multipart)
{
    return m_inputType->isFormDataAppendable() && m_inputType->appendFormData(encoding, multipart);
}

String HTMLInputElement::resultForDialogSubmit()
{
    return m_inputType->resultForDialogSubmit();
}

void HTMLInputElement::resetImpl()
{
    if (m_inputType->storesValueSeparateFromAttribute()) {
        setValue(String());
        setNeedsValidityCheck();
    }

    setChecked(hasAttribute(checkedAttr));
    m_reflectsCheckedAttribute = true;
}

bool HTMLInputElement::isTextField() const
{
    return m_inputType->isTextField();
}

void HTMLInputElement::dispatchChangeEventIfNeeded()
{
    if (inDocument() && m_inputType->shouldSendChangeEventAfterCheckedChanged())
        dispatchFormControlChangeEvent();
}

void HTMLInputElement::setChecked(bool nowChecked, TextFieldEventBehavior eventBehavior)
{
    if (checked() == nowChecked)
        return;

    RefPtrWillBeRawPtr<HTMLInputElement> protector(this);
    m_reflectsCheckedAttribute = false;
    m_isChecked = nowChecked;

    if (RadioButtonGroupScope* scope = radioButtonGroupScope())
        scope->updateCheckedState(this);
    if (layoutObject())
        LayoutTheme::theme().controlStateChanged(*layoutObject(), CheckedControlState);

    setNeedsValidityCheck();

    // Ideally we'd do this from the layout tree (matching
    // LayoutTextView), but it's not possible to do it at the moment
    // because of the way the code is structured.
    if (layoutObject()) {
        if (AXObjectCache* cache = layoutObject()->document().existingAXObjectCache())
            cache->checkedStateChanged(this);
    }

    // Only send a change event for items in the document (avoid firing during
    // parsing) and don't send a change event for a radio button that's getting
    // unchecked to match other browsers. DOM is not a useful standard for this
    // because it says only to fire change events at "lose focus" time, which is
    // definitely wrong in practice for these types of elements.
    if (eventBehavior != DispatchNoEvent && inDocument() && m_inputType->shouldSendChangeEventAfterCheckedChanged()) {
        setTextAsOfLastFormControlChangeEvent(String());
        if (eventBehavior == DispatchInputAndChangeEvent)
            dispatchFormControlInputEvent();
    }

    pseudoStateChanged(CSSSelector::PseudoChecked);
}

void HTMLInputElement::setIndeterminate(bool newValue)
{
    if (indeterminate() == newValue)
        return;

    m_isIndeterminate = newValue;

    pseudoStateChanged(CSSSelector::PseudoIndeterminate);

    if (layoutObject())
        LayoutTheme::theme().controlStateChanged(*layoutObject(), CheckedControlState);
}

int HTMLInputElement::size() const
{
    return m_size;
}

bool HTMLInputElement::sizeShouldIncludeDecoration(int& preferredSize) const
{
    return m_inputTypeView->sizeShouldIncludeDecoration(defaultSize, preferredSize);
}

void HTMLInputElement::copyNonAttributePropertiesFromElement(const Element& source)
{
    const HTMLInputElement& sourceElement = static_cast<const HTMLInputElement&>(source);

    m_valueIfDirty = sourceElement.m_valueIfDirty;
    setChecked(sourceElement.m_isChecked);
    m_reflectsCheckedAttribute = sourceElement.m_reflectsCheckedAttribute;
    m_isIndeterminate = sourceElement.m_isIndeterminate;

    HTMLTextFormControlElement::copyNonAttributePropertiesFromElement(source);

    m_needsToUpdateViewValue = true;
    m_inputTypeView->updateView();
}

String HTMLInputElement::value() const
{
    String value;
    if (m_inputType->getTypeSpecificValue(value))
        return value;

    value = m_valueIfDirty;
    if (!value.isNull())
        return value;

    AtomicString valueString = fastGetAttribute(valueAttr);
    value = sanitizeValue(valueString);
    if (!value.isNull())
        return value;

    return m_inputType->fallbackValue();
}

String HTMLInputElement::valueWithDefault() const
{
    String value = this->value();
    if (!value.isNull())
        return value;

    return m_inputType->defaultValue();
}

void HTMLInputElement::setValueForUser(const String& value)
{
    // Call setValue and make it send a change event.
    setValue(value, DispatchChangeEvent);
}

const String& HTMLInputElement::suggestedValue() const
{
    return m_suggestedValue;
}

void HTMLInputElement::setSuggestedValue(const String& value)
{
    if (!m_inputType->canSetSuggestedValue())
        return;
    m_needsToUpdateViewValue = true;
    m_suggestedValue = sanitizeValue(value);
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::ControlValue));
    m_inputTypeView->updateView();
}

void HTMLInputElement::setEditingValue(const String& value)
{
    if (!layoutObject() || !isTextField())
        return;
    setInnerEditorValue(value);
    subtreeHasChanged();

    unsigned max = value.length();
    if (focused())
        setSelectionRange(max, max, SelectionHasNoDirection, NotDispatchSelectEvent);
    else
        cacheSelectionInResponseToSetValue(max);

    dispatchInputEvent();
}

void HTMLInputElement::setInnerEditorValue(const String& value)
{
    HTMLTextFormControlElement::setInnerEditorValue(value);
    m_needsToUpdateViewValue = false;
}

void HTMLInputElement::setValue(const String& value, ExceptionState& exceptionState, TextFieldEventBehavior eventBehavior)
{
    // FIXME: Remove type check.
    if (type() == InputTypeNames::file && !value.isEmpty()) {
        exceptionState.throwDOMException(InvalidStateError, "This input element accepts a filename, which may only be programmatically set to the empty string.");
        return;
    }
    setValue(value, eventBehavior);
}

void HTMLInputElement::setValue(const String& value, TextFieldEventBehavior eventBehavior)
{
    m_inputType->warnIfValueIsInvalidAndElementIsVisible(value);
    if (!m_inputType->canSetValue(value))
        return;

    RefPtrWillBeRawPtr<HTMLInputElement> protector(this);
    EventQueueScope scope;
    String sanitizedValue = sanitizeValue(value);
    bool valueChanged = sanitizedValue != this->value();

    setLastChangeWasNotUserEdit();
    m_needsToUpdateViewValue = true;
    m_suggestedValue = String(); // Prevent TextFieldInputType::setValue from using the suggested value.

    m_inputType->setValue(sanitizedValue, valueChanged, eventBehavior);

    if (valueChanged && eventBehavior == DispatchNoEvent)
        setTextAsOfLastFormControlChangeEvent(sanitizedValue.isNull() ? defaultValue() : sanitizedValue);

    if (!valueChanged)
        return;

    notifyFormStateChanged();
}

void HTMLInputElement::setValueInternal(const String& sanitizedValue, TextFieldEventBehavior eventBehavior)
{
    m_valueIfDirty = sanitizedValue;
    setNeedsValidityCheck();
    if (document().focusedElement() == this)
        document().frameHost()->chromeClient().didUpdateTextOfFocusedElementByNonUserInput();
}

void HTMLInputElement::updateView()
{
    m_inputTypeView->updateView();
}

double HTMLInputElement::valueAsDate(bool& isNull) const
{
    double date =  m_inputType->valueAsDate();
    isNull = !std::isfinite(date);
    return date;
}

void HTMLInputElement::setValueAsDate(double value, ExceptionState& exceptionState)
{
    m_inputType->setValueAsDate(value, exceptionState);
}

double HTMLInputElement::valueAsNumber() const
{
    return m_inputType->valueAsDouble();
}

void HTMLInputElement::setValueAsNumber(double newValue, ExceptionState& exceptionState, TextFieldEventBehavior eventBehavior)
{
    // http://www.whatwg.org/specs/web-apps/current-work/multipage/common-input-element-attributes.html#dom-input-valueasnumber
    // On setting, if the new value is infinite, then throw a TypeError exception.
    if (std::isinf(newValue)) {
        exceptionState.throwTypeError(ExceptionMessages::notAFiniteNumber(newValue));
        return;
    }
    m_inputType->setValueAsDouble(newValue, eventBehavior, exceptionState);
}

void HTMLInputElement::setValueFromRenderer(const String& value)
{
    // File upload controls will never use this.
    ASSERT(type() != InputTypeNames::file);

    m_suggestedValue = String();

    // Renderer and our event handler are responsible for sanitizing values.
    ASSERT(value == sanitizeValue(value) || sanitizeValue(value).isEmpty());

    m_valueIfDirty = value;
    m_needsToUpdateViewValue = false;

    // Input event is fired by the Node::defaultEventHandler for editable controls.
    if (!isTextField())
        dispatchInputEvent();
    notifyFormStateChanged();

    setNeedsValidityCheck();

    // Clear autofill flag (and yellow background) on user edit.
    setAutofilled(false);
}

void* HTMLInputElement::preDispatchEventHandler(Event* event)
{
    if (event->type() == EventTypeNames::textInput && m_inputTypeView->shouldSubmitImplicitly(event)) {
        event->stopPropagation();
        return nullptr;
    }
    if (event->type() != EventTypeNames::click)
        return nullptr;
    if (!event->isMouseEvent() || toMouseEvent(event)->button() != LeftButton)
        return nullptr;
#if ENABLE(OILPAN)
    return m_inputTypeView->willDispatchClick();
#else
    // FIXME: Check whether there are any cases where this actually ends up leaking.
    return m_inputTypeView->willDispatchClick().leakPtr();
#endif
}

void HTMLInputElement::postDispatchEventHandler(Event* event, void* dataFromPreDispatch)
{
    OwnPtrWillBeRawPtr<ClickHandlingState> state = adoptPtrWillBeNoop(static_cast<ClickHandlingState*>(dataFromPreDispatch));
    if (!state)
        return;
    m_inputTypeView->didDispatchClick(event, *state);
}

void HTMLInputElement::defaultEventHandler(Event* evt)
{
    if (evt->isMouseEvent() && evt->type() == EventTypeNames::click && toMouseEvent(evt)->button() == LeftButton) {
        m_inputTypeView->handleClickEvent(toMouseEvent(evt));
        if (evt->defaultHandled())
            return;
    }

    if (evt->isTouchEvent() && m_inputTypeView->hasTouchEventHandler()) {
        m_inputTypeView->handleTouchEvent(toTouchEvent(evt));
        if (evt->defaultHandled())
            return;
    }

    if (evt->isKeyboardEvent() && evt->type() == EventTypeNames::keydown) {
        m_inputTypeView->handleKeydownEvent(toKeyboardEvent(evt));
        if (evt->defaultHandled())
            return;
    }

    // Call the base event handler before any of our own event handling for almost all events in text fields.
    // Makes editing keyboard handling take precedence over the keydown and keypress handling in this function.
    bool callBaseClassEarly = isTextField() && (evt->type() == EventTypeNames::keydown || evt->type() == EventTypeNames::keypress);
    if (callBaseClassEarly) {
        HTMLTextFormControlElement::defaultEventHandler(evt);
        if (evt->defaultHandled())
            return;
    }

    // DOMActivate events cause the input to be "activated" - in the case of image and submit inputs, this means
    // actually submitting the form. For reset inputs, the form is reset. These events are sent when the user clicks
    // on the element, or presses enter while it is the active element. JavaScript code wishing to activate the element
    // must dispatch a DOMActivate event - a click event will not do the job.
    if (evt->type() == EventTypeNames::DOMActivate) {
        m_inputType->handleDOMActivateEvent(evt);
        if (evt->defaultHandled())
            return;
    }

    // Use key press event here since sending simulated mouse events
    // on key down blocks the proper sending of the key press event.
    if (evt->isKeyboardEvent() && evt->type() == EventTypeNames::keypress) {
        m_inputTypeView->handleKeypressEvent(toKeyboardEvent(evt));
        if (evt->defaultHandled())
            return;
    }

    if (evt->isKeyboardEvent() && evt->type() == EventTypeNames::keyup) {
        m_inputTypeView->handleKeyupEvent(toKeyboardEvent(evt));
        if (evt->defaultHandled())
            return;
    }

    if (m_inputTypeView->shouldSubmitImplicitly(evt)) {
        // FIXME: Remove type check.
        if (type() == InputTypeNames::search)
            onSearch();
        // Form submission finishes editing, just as loss of focus does.
        // If there was a change, send the event now.
        if (wasChangedSinceLastFormControlChangeEvent())
            dispatchFormControlChangeEvent();

        RefPtrWillBeRawPtr<HTMLFormElement> formForSubmission = m_inputTypeView->formForSubmission();
        // Form may never have been present, or may have been destroyed by code responding to the change event.
        if (formForSubmission)
            formForSubmission->submitImplicitly(evt, canTriggerImplicitSubmission());

        evt->setDefaultHandled();
        return;
    }

    if (evt->isBeforeTextInsertedEvent())
        m_inputTypeView->handleBeforeTextInsertedEvent(static_cast<BeforeTextInsertedEvent*>(evt));

    if (evt->isMouseEvent() && evt->type() == EventTypeNames::mousedown) {
        m_inputTypeView->handleMouseDownEvent(toMouseEvent(evt));
        if (evt->defaultHandled())
            return;
    }

    m_inputTypeView->forwardEvent(evt);

    if (!callBaseClassEarly && !evt->defaultHandled())
        HTMLTextFormControlElement::defaultEventHandler(evt);
}

bool HTMLInputElement::willRespondToMouseClickEvents()
{
    // FIXME: Consider implementing willRespondToMouseClickEvents() in InputType if more accurate results are necessary.
    if (!isDisabledFormControl())
        return true;

    return HTMLTextFormControlElement::willRespondToMouseClickEvents();
}

bool HTMLInputElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == srcAttr || attribute.name() == formactionAttr || HTMLTextFormControlElement::isURLAttribute(attribute);
}

bool HTMLInputElement::hasLegalLinkAttribute(const QualifiedName& name) const
{
    return m_inputType->hasLegalLinkAttribute(name) || HTMLTextFormControlElement::hasLegalLinkAttribute(name);
}

const QualifiedName& HTMLInputElement::subResourceAttributeName() const
{
    return m_inputType->subResourceAttributeName();
}

const AtomicString& HTMLInputElement::defaultValue() const
{
    return fastGetAttribute(valueAttr);
}

static inline bool isRFC2616TokenCharacter(UChar ch)
{
    return isASCII(ch) && ch > ' ' && ch != '"' && ch != '(' && ch != ')' && ch != ',' && ch != '/' && (ch < ':' || ch > '@') && (ch < '[' || ch > ']') && ch != '{' && ch != '}' && ch != 0x7f;
}

static bool isValidMIMEType(const String& type)
{
    size_t slashPosition = type.find('/');
    if (slashPosition == kNotFound || !slashPosition || slashPosition == type.length() - 1)
        return false;
    for (size_t i = 0; i < type.length(); ++i) {
        if (!isRFC2616TokenCharacter(type[i]) && i != slashPosition)
            return false;
    }
    return true;
}

static bool isValidFileExtension(const String& type)
{
    if (type.length() < 2)
        return false;
    return type[0] == '.';
}

static Vector<String> parseAcceptAttribute(const String& acceptString, bool (*predicate)(const String&))
{
    Vector<String> types;
    if (acceptString.isEmpty())
        return types;

    Vector<String> splitTypes;
    acceptString.split(',', false, splitTypes);
    for (const String& splitType : splitTypes) {
        String trimmedType = stripLeadingAndTrailingHTMLSpaces(splitType);
        if (trimmedType.isEmpty())
            continue;
        if (!predicate(trimmedType))
            continue;
        types.append(trimmedType.lower());
    }

    return types;
}

Vector<String> HTMLInputElement::acceptMIMETypes()
{
    return parseAcceptAttribute(fastGetAttribute(acceptAttr), isValidMIMEType);
}

Vector<String> HTMLInputElement::acceptFileExtensions()
{
    return parseAcceptAttribute(fastGetAttribute(acceptAttr), isValidFileExtension);
}

const AtomicString& HTMLInputElement::alt() const
{
    return fastGetAttribute(altAttr);
}

int HTMLInputElement::maxLength() const
{
    return m_maxLength;
}

int HTMLInputElement::minLength() const
{
    return m_minLength;
}

void HTMLInputElement::setMaxLength(int maxLength, ExceptionState& exceptionState)
{
    if (maxLength < 0)
        exceptionState.throwDOMException(IndexSizeError, "The value provided (" + String::number(maxLength) + ") is negative.");
    else if (maxLength < m_minLength)
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMinimumBound("maxLength", maxLength, m_minLength));
    else
        setIntegralAttribute(maxlengthAttr, maxLength);
}

void HTMLInputElement::setMinLength(int minLength, ExceptionState& exceptionState)
{
    if (minLength < 0)
        exceptionState.throwDOMException(IndexSizeError, "The value provided (" + String::number(minLength) + ") is negative.");
    else if (minLength > m_maxLength)
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("minLength", minLength, m_maxLength));
    else
        setIntegralAttribute(minlengthAttr, minLength);
}

bool HTMLInputElement::multiple() const
{
    return fastHasAttribute(multipleAttr);
}

void HTMLInputElement::setSize(unsigned size)
{
    setUnsignedIntegralAttribute(sizeAttr, size);
}

void HTMLInputElement::setSize(unsigned size, ExceptionState& exceptionState)
{
    if (!size)
        exceptionState.throwDOMException(IndexSizeError, "The value provided is 0, which is an invalid size.");
    else
        setSize(size);
}

KURL HTMLInputElement::src() const
{
    return document().completeURL(fastGetAttribute(srcAttr));
}

FileList* HTMLInputElement::files()
{
    return m_inputType->files();
}

void HTMLInputElement::setFiles(FileList* files)
{
    m_inputType->setFiles(files);
}

bool HTMLInputElement::receiveDroppedFiles(const DragData* dragData)
{
    return m_inputType->receiveDroppedFiles(dragData);
}

String HTMLInputElement::droppedFileSystemId()
{
    return m_inputType->droppedFileSystemId();
}

bool HTMLInputElement::canReceiveDroppedFiles() const
{
    return m_canReceiveDroppedFiles;
}

void HTMLInputElement::setCanReceiveDroppedFiles(bool canReceiveDroppedFiles)
{
    if (!!m_canReceiveDroppedFiles == canReceiveDroppedFiles)
        return;
    m_canReceiveDroppedFiles = canReceiveDroppedFiles;
    if (layoutObject())
        layoutObject()->updateFromElement();
}

String HTMLInputElement::sanitizeValue(const String& proposedValue) const
{
    if (proposedValue.isNull())
        return proposedValue;
    return m_inputType->sanitizeValue(proposedValue);
}

String HTMLInputElement::localizeValue(const String& proposedValue) const
{
    if (proposedValue.isNull())
        return proposedValue;
    return m_inputType->localizeValue(proposedValue);
}

bool HTMLInputElement::isInRange() const
{
    return m_inputType->isInRange(value());
}

bool HTMLInputElement::isOutOfRange() const
{
    return m_inputType->isOutOfRange(value());
}

bool HTMLInputElement::isRequiredFormControl() const
{
    return m_inputType->supportsRequired() && isRequired();
}

bool HTMLInputElement::matchesReadOnlyPseudoClass() const
{
    return m_inputType->supportsReadOnly() && isReadOnly();
}

bool HTMLInputElement::matchesReadWritePseudoClass() const
{
    return m_inputType->supportsReadOnly() && !isReadOnly();
}

void HTMLInputElement::onSearch()
{
    // FIXME: Remove type check, and static_cast.
    ASSERT(type() == InputTypeNames::search);
    if (m_inputType)
        static_cast<SearchInputType*>(m_inputType.get())->stopSearchEventTimer();
    dispatchEvent(Event::createBubble(EventTypeNames::search));
}

void HTMLInputElement::updateClearButtonVisibility()
{
    m_inputTypeView->updateClearButtonVisibility();
}

void HTMLInputElement::willChangeForm()
{
    removeFromRadioButtonGroup();
    HTMLTextFormControlElement::willChangeForm();
}

void HTMLInputElement::didChangeForm()
{
    HTMLTextFormControlElement::didChangeForm();
    addToRadioButtonGroup();
}

Node::InsertionNotificationRequest HTMLInputElement::insertedInto(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("input");
            argv.append(fastGetAttribute(typeAttr));
            argv.append(fastGetAttribute(formactionAttr));
            activityLogger->logEvent("blinkAddElement", argv.size(), argv.data());
        }
    }
    HTMLTextFormControlElement::insertedInto(insertionPoint);
    if (insertionPoint->inDocument() && !form())
        addToRadioButtonGroup();
    resetListAttributeTargetObserver();
    return InsertionShouldCallDidNotifySubtreeInsertions;
}

void HTMLInputElement::removedFrom(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument() && !form())
        removeFromRadioButtonGroup();
    HTMLTextFormControlElement::removedFrom(insertionPoint);
    ASSERT(!inDocument());
    resetListAttributeTargetObserver();
}

void HTMLInputElement::didMoveToNewDocument(Document& oldDocument)
{
    if (imageLoader())
        imageLoader()->elementDidMoveToNewDocument();

    // FIXME: Remove type check.
    if (type() == InputTypeNames::radio)
        oldDocument.formController().radioButtonGroupScope().removeButton(this);

    HTMLTextFormControlElement::didMoveToNewDocument(oldDocument);
}

void HTMLInputElement::removeAllEventListeners()
{
    HTMLTextFormControlElement::removeAllEventListeners();
    m_hasTouchEventHandler = false;
}

bool HTMLInputElement::recalcWillValidate() const
{
    return m_inputType->supportsValidation() && HTMLTextFormControlElement::recalcWillValidate();
}

void HTMLInputElement::requiredAttributeChanged()
{
    HTMLTextFormControlElement::requiredAttributeChanged();
    if (RadioButtonGroupScope* scope = radioButtonGroupScope())
        scope->requiredAttributeChanged(this);
    m_inputTypeView->requiredAttributeChanged();
}

void HTMLInputElement::selectColorInColorChooser(const Color& color)
{
    if (ColorChooserClient* client = m_inputType->colorChooserClient())
        client->didChooseColor(color);
}

void HTMLInputElement::endColorChooser()
{
    if (ColorChooserClient* client = m_inputType->colorChooserClient())
        client->didEndChooser();
}

HTMLElement* HTMLInputElement::list() const
{
    return dataList();
}

HTMLDataListElement* HTMLInputElement::dataList() const
{
    if (!m_hasNonEmptyList)
        return nullptr;

    if (!m_inputType->shouldRespectListAttribute())
        return nullptr;

    Element* element = treeScope().getElementById(fastGetAttribute(listAttr));
    if (!element)
        return nullptr;
    if (!isHTMLDataListElement(*element))
        return nullptr;

    return toHTMLDataListElement(element);
}

bool HTMLInputElement::hasValidDataListOptions() const
{
    HTMLDataListElement* dataList = this->dataList();
    if (!dataList)
        return false;
    RefPtrWillBeRawPtr<HTMLDataListOptionsCollection> options = dataList->options();
    for (unsigned i = 0; HTMLOptionElement* option = options->item(i); ++i) {
        if (isValidValue(option->value()))
            return true;
    }
    return false;
}

void HTMLInputElement::setListAttributeTargetObserver(PassOwnPtrWillBeRawPtr<ListAttributeTargetObserver> newObserver)
{
    if (m_listAttributeTargetObserver)
        m_listAttributeTargetObserver->unregister();
    m_listAttributeTargetObserver = newObserver;
}

void HTMLInputElement::resetListAttributeTargetObserver()
{
    if (inDocument())
        setListAttributeTargetObserver(ListAttributeTargetObserver::create(fastGetAttribute(listAttr), this));
    else
        setListAttributeTargetObserver(nullptr);
}

void HTMLInputElement::listAttributeTargetChanged()
{
    m_inputTypeView->listAttributeTargetChanged();
}

bool HTMLInputElement::isSteppable() const
{
    return m_inputType->isSteppable();
}

bool HTMLInputElement::isTextButton() const
{
    return m_inputType->isTextButton();
}

bool HTMLInputElement::isImage() const
{
    return m_inputType->isImage();
}

bool HTMLInputElement::isEnumeratable() const
{
    return m_inputType->isEnumeratable();
}

bool HTMLInputElement::supportLabels() const
{
    return m_inputType->isInteractiveContent();
}

bool HTMLInputElement::shouldAppearChecked() const
{
    return checked() && m_inputType->isCheckable();
}

bool HTMLInputElement::supportsPlaceholder() const
{
    return m_inputType->supportsPlaceholder();
}

void HTMLInputElement::updatePlaceholderText()
{
    return m_inputTypeView->updatePlaceholderText();
}

void HTMLInputElement::parseMaxLengthAttribute(const AtomicString& value)
{
    int maxLength;
    if (!parseHTMLInteger(value, maxLength))
        maxLength = maximumLength;
    if (maxLength < 0 || maxLength > maximumLength)
        maxLength = maximumLength;
    int oldMaxLength = m_maxLength;
    m_maxLength = maxLength;
    if (oldMaxLength != maxLength)
        updateValueIfNeeded();
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(maxlengthAttr));
    setNeedsValidityCheck();
}

void HTMLInputElement::parseMinLengthAttribute(const AtomicString& value)
{
    int minLength;
    if (!parseHTMLInteger(value, minLength))
        minLength = 0;
    if (minLength < 0)
        minLength = 0;
    m_minLength = minLength;
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(minlengthAttr));
    setNeedsValidityCheck();
}

void HTMLInputElement::updateValueIfNeeded()
{
    String newValue = sanitizeValue(m_valueIfDirty);
    ASSERT(!m_valueIfDirty.isNull() || newValue.isNull());
    if (newValue != m_valueIfDirty)
        setValue(newValue);
}

bool HTMLInputElement::supportsAutocapitalize() const
{
    return m_inputType->supportsAutocapitalize();
}

const AtomicString& HTMLInputElement::defaultAutocapitalize() const
{
    return m_inputType->defaultAutocapitalize();
}

String HTMLInputElement::defaultToolTip() const
{
    return m_inputType->defaultToolTip();
}

bool HTMLInputElement::shouldAppearIndeterminate() const
{
    return m_inputType->shouldAppearIndeterminate();
}

bool HTMLInputElement::isInRequiredRadioButtonGroup()
{
    // FIXME: Remove type check.
    ASSERT(type() == InputTypeNames::radio);
    if (RadioButtonGroupScope* scope = radioButtonGroupScope())
        return scope->isInRequiredGroup(this);
    return false;
}

HTMLInputElement* HTMLInputElement::checkedRadioButtonForGroup()
{
    if (checked())
        return this;
    if (RadioButtonGroupScope* scope = radioButtonGroupScope())
        return scope->checkedButtonForGroup(name());
    return nullptr;
}

RadioButtonGroupScope* HTMLInputElement::radioButtonGroupScope() const
{
    // FIXME: Remove type check.
    if (type() != InputTypeNames::radio)
        return nullptr;
    if (HTMLFormElement* formElement = form())
        return &formElement->radioButtonGroupScope();
    if (inDocument())
        return &document().formController().radioButtonGroupScope();
    return nullptr;
}

inline void HTMLInputElement::addToRadioButtonGroup()
{
    if (RadioButtonGroupScope* scope = radioButtonGroupScope())
        scope->addButton(this);
}

inline void HTMLInputElement::removeFromRadioButtonGroup()
{
    if (RadioButtonGroupScope* scope = radioButtonGroupScope())
        scope->removeButton(this);
}

unsigned HTMLInputElement::height() const
{
    return m_inputType->height();
}

unsigned HTMLInputElement::width() const
{
    return m_inputType->width();
}

void HTMLInputElement::setHeight(unsigned height)
{
    setUnsignedIntegralAttribute(heightAttr, height);
}

void HTMLInputElement::setWidth(unsigned width)
{
    setUnsignedIntegralAttribute(widthAttr, width);
}

PassOwnPtrWillBeRawPtr<ListAttributeTargetObserver> ListAttributeTargetObserver::create(const AtomicString& id, HTMLInputElement* element)
{
    return adoptPtrWillBeNoop(new ListAttributeTargetObserver(id, element));
}

ListAttributeTargetObserver::ListAttributeTargetObserver(const AtomicString& id, HTMLInputElement* element)
    : IdTargetObserver(element->treeScope().idTargetObserverRegistry(), id)
    , m_element(element)
{
}

DEFINE_TRACE(ListAttributeTargetObserver)
{
    visitor->trace(m_element);
    IdTargetObserver::trace(visitor);
}

void ListAttributeTargetObserver::idTargetChanged()
{
    m_element->listAttributeTargetChanged();
}

void HTMLInputElement::setRangeText(const String& replacement, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }

    HTMLTextFormControlElement::setRangeText(replacement, exceptionState);
}

void HTMLInputElement::setRangeText(const String& replacement, unsigned start, unsigned end, const String& selectionMode, ExceptionState& exceptionState)
{
    if (!m_inputType->supportsSelectionAPI()) {
        exceptionState.throwDOMException(InvalidStateError, "The input element's type ('" + m_inputType->formControlType() + "') does not support selection.");
        return;
    }

    HTMLTextFormControlElement::setRangeText(replacement, start, end, selectionMode, exceptionState);
}

bool HTMLInputElement::setupDateTimeChooserParameters(DateTimeChooserParameters& parameters)
{
    if (!document().view())
        return false;

    parameters.type = type();
    parameters.minimum = minimum();
    parameters.maximum = maximum();
    parameters.required = isRequired();
    if (!RuntimeEnabledFeatures::langAttributeAwareFormControlUIEnabled()) {
        parameters.locale = defaultLanguage();
    } else {
        AtomicString computedLocale = computeInheritedLanguage();
        parameters.locale = computedLocale.isEmpty() ? defaultLanguage() : computedLocale;
    }

    StepRange stepRange = createStepRange(RejectAny);
    if (stepRange.hasStep()) {
        parameters.step = stepRange.step().toDouble();
        parameters.stepBase = stepRange.stepBase().toDouble();
    } else {
        parameters.step = 1.0;
        parameters.stepBase = 0;
    }

    parameters.anchorRectInRootFrame = document().view()->contentsToRootFrame(pixelSnappedBoundingBox());
    parameters.anchorRectInScreen = document().view()->contentsToScreen(pixelSnappedBoundingBox());
    parameters.currentValue = value();
    parameters.doubleValue = m_inputType->valueAsDouble();
    parameters.isAnchorElementRTL = m_inputType->computedTextDirection() == RTL;
    if (HTMLDataListElement* dataList = this->dataList()) {
        RefPtrWillBeRawPtr<HTMLDataListOptionsCollection> options = dataList->options();
        for (unsigned i = 0; HTMLOptionElement* option = options->item(i); ++i) {
            if (!isValidValue(option->value()))
                continue;
            DateTimeSuggestion suggestion;
            suggestion.value = m_inputType->parseToNumber(option->value(), Decimal::nan()).toDouble();
            if (std::isnan(suggestion.value))
                continue;
            suggestion.localizedValue = localizeValue(option->value());
            suggestion.label = option->value() == option->label() ? String() : option->label();
            parameters.suggestions.append(suggestion);
        }
    }
    return true;
}

bool HTMLInputElement::supportsInputModeAttribute() const
{
    return m_inputType->supportsInputModeAttribute();
}

void HTMLInputElement::setShouldRevealPassword(bool value)
{
    if (!!m_shouldRevealPassword == value)
        return;
    m_shouldRevealPassword = value;
    lazyReattachIfAttached();
}

bool HTMLInputElement::isInteractiveContent() const
{
    return m_inputType->isInteractiveContent();
}

bool HTMLInputElement::supportsAutofocus() const
{
    return m_inputType->isInteractiveContent();
}

PassRefPtr<ComputedStyle> HTMLInputElement::customStyleForLayoutObject()
{
    return m_inputTypeView->customStyleForLayoutObject(originalStyleForLayoutObject());
}

bool HTMLInputElement::shouldDispatchFormControlChangeEvent(String& oldValue, String& newValue)
{
    return m_inputType->shouldDispatchFormControlChangeEvent(oldValue, newValue);
}

void HTMLInputElement::didNotifySubtreeInsertionsToDocument()
{
    listAttributeTargetChanged();
}

AXObject* HTMLInputElement::popupRootAXObject()
{
    return m_inputTypeView->popupRootAXObject();
}

void HTMLInputElement::ensureFallbackContent()
{
    m_inputTypeView->ensureFallbackContent();
}

void HTMLInputElement::ensurePrimaryContent()
{
    m_inputTypeView->ensurePrimaryContent();
}

bool HTMLInputElement::hasFallbackContent() const
{
    return m_inputTypeView->hasFallbackContent();
}
} // namespace
