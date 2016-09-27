/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2010 Apple Inc. All rights reserved.
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

#ifndef HTMLInputElement_h
#define HTMLInputElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLTextFormControlElement.h"
#include "core/html/forms/StepRange.h"
#include "platform/FileChooser.h"

namespace blink {

class AXObject;
class DragData;
class ExceptionState;
class FileList;
class HTMLDataListElement;
class HTMLImageLoader;
class InputType;
class InputTypeView;
class KURL;
class ListAttributeTargetObserver;
class RadioButtonGroupScope;
struct DateTimeChooserParameters;

class CORE_EXPORT HTMLInputElement : public HTMLTextFormControlElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLInputElement> create(Document&, HTMLFormElement*, bool createdByParser);
    ~HTMLInputElement() override;
    DECLARE_VIRTUAL_TRACE();

    DEFINE_ATTRIBUTE_EVENT_LISTENER(webkitspeechchange);

    bool shouldAutocomplete() const final;

    // For ValidityState
    bool hasBadInput() const final;
    bool patternMismatch() const final;
    bool rangeUnderflow() const final;
    bool rangeOverflow() const final;
    bool stepMismatch() const final;
    bool tooLong() const final;
    bool tooShort() const final;
    bool typeMismatch() const final;
    bool valueMissing() const final;
    String validationMessage() const final;

    // Returns the minimum value for type=date, number, or range.  Don't call this for other types.
    double minimum() const;
    // Returns the maximum value for type=date, number, or range.  Don't call this for other types.
    // This always returns a value which is >= minimum().
    double maximum() const;
    // Sets the "allowed value step" defined in the HTML spec to the specified double pointer.
    // Returns false if there is no "allowed value step."
    bool getAllowedValueStep(Decimal*) const;
    StepRange createStepRange(AnyStepHandling) const;

    Decimal findClosestTickMarkValue(const Decimal&);

    // Implementations of HTMLInputElement::stepUp() and stepDown().
    void stepUp(int, ExceptionState&);
    void stepDown(int, ExceptionState&);
    // stepUp()/stepDown() for user-interaction.
    bool isSteppable() const;

    // Returns true if the type is button, reset, or submit.
    bool isTextButton() const;
    // Returns true if the type is email, number, password, search, tel, text,
    // or url.
    bool isTextField() const;

    bool isImage() const;

    bool checked() const { return m_isChecked; }
    void setChecked(bool, TextFieldEventBehavior = DispatchNoEvent);
    void dispatchChangeEventIfNeeded();

    // 'indeterminate' is a state independent of the checked state that causes the control to draw in a way that hides the actual state.
    bool indeterminate() const { return m_isIndeterminate; }
    void setIndeterminate(bool);
    // shouldAppearChecked is used by the layout tree/CSS while checked() is used by JS to determine checked state
    bool shouldAppearChecked() const;
    bool shouldAppearIndeterminate() const override;

    int size() const;
    bool sizeShouldIncludeDecoration(int& preferredSize) const;

    void setType(const AtomicString&);

    String value() const override;
    void setValue(const String&, ExceptionState&, TextFieldEventBehavior = DispatchNoEvent);
    void setValue(const String&, TextFieldEventBehavior = DispatchNoEvent);
    void setValueForUser(const String&);
    // Checks if the specified string would be a valid value.
    // We should not call this for types with no string value such as CHECKBOX and RADIO.
    bool isValidValue(const String&) const;
    bool hasDirtyValue() const { return !m_valueIfDirty.isNull(); }

    String sanitizeValue(const String&) const;

    String localizeValue(const String&) const;

    const String& suggestedValue() const;
    void setSuggestedValue(const String&);

    void setEditingValue(const String&);

    double valueAsDate(bool& isNull) const;
    void setValueAsDate(double, ExceptionState&);

    double valueAsNumber() const;
    void setValueAsNumber(double, ExceptionState&, TextFieldEventBehavior = DispatchNoEvent);

    String valueWithDefault() const;

    void setValueFromRenderer(const String&);

    int selectionStartForBinding(ExceptionState&) const;
    int selectionEndForBinding(ExceptionState&) const;
    String selectionDirectionForBinding(ExceptionState&) const;
    void setSelectionStartForBinding(int, ExceptionState&);
    void setSelectionEndForBinding(int, ExceptionState&);
    void setSelectionDirectionForBinding(const String&, ExceptionState&);
    void setSelectionRangeForBinding(int start, int end, ExceptionState&);
    void setSelectionRangeForBinding(int start, int end, const String& direction, ExceptionState&);

    bool layoutObjectIsNeeded(const ComputedStyle&) final;
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    void detach(const AttachContext& = AttachContext()) final;
    void updateFocusAppearance(bool restorePreviousSelection) final;

    // FIXME: For isActivatedSubmit and setActivatedSubmit, we should use the NVI-idiom here by making
    // it private virtual in all classes and expose a public method in HTMLFormControlElement to call
    // the private virtual method.
    bool isActivatedSubmit() const final;
    void setActivatedSubmit(bool flag) final;

    String altText() const final;

    int maxResults() const { return m_maxResults; }

    const AtomicString& defaultValue() const;

    Vector<String> acceptMIMETypes();
    Vector<String> acceptFileExtensions();
    const AtomicString& alt() const;

    void setSize(unsigned);
    void setSize(unsigned, ExceptionState&);

    KURL src() const;

    int maxLength() const;
    int minLength() const;
    void setMaxLength(int, ExceptionState&);
    void setMinLength(int, ExceptionState&);

    bool multiple() const;

    FileList* files();
    void setFiles(FileList*);

    // Returns true if the given DragData has more than one dropped files.
    bool receiveDroppedFiles(const DragData*);

    String droppedFileSystemId();

    // These functions are used for laying out the input active during a
    // drag-and-drop operation.
    bool canReceiveDroppedFiles() const;
    void setCanReceiveDroppedFiles(bool);

    void onSearch();

    void updateClearButtonVisibility();

    bool willRespondToMouseClickEvents() override;

    HTMLElement* list() const;
    HTMLDataListElement* dataList() const;
    bool hasValidDataListOptions() const;
    void listAttributeTargetChanged();

    HTMLInputElement* checkedRadioButtonForGroup();
    bool isInRequiredRadioButtonGroup();

    // Functions for InputType classes.
    void setValueInternal(const String&, TextFieldEventBehavior);
    bool valueAttributeWasUpdatedAfterParsing() const { return m_valueAttributeWasUpdatedAfterParsing; }
    void updateView();
    bool needsToUpdateViewValue() const { return m_needsToUpdateViewValue; }
    void setInnerEditorValue(const String&) override;

    void cacheSelectionInResponseToSetValue(int caretOffset) { cacheSelection(caretOffset, caretOffset, SelectionHasNoDirection); }

    // For test purposes.
    void selectColorInColorChooser(const Color&);
    void endColorChooser();

    String defaultToolTip() const;

    static const int maximumLength;

    unsigned height() const;
    unsigned width() const;
    void setHeight(unsigned);
    void setWidth(unsigned);

    void blur() final;
    void defaultBlur();

    const AtomicString& name() const final;

    void beginEditing();
    void endEditing();

    static Vector<FileChooserFileInfo> filesFromFileInputFormControlState(const FormControlState&);

    bool matchesReadOnlyPseudoClass() const final;
    bool matchesReadWritePseudoClass() const final;
    void setRangeText(const String& replacement, ExceptionState&) final;
    void setRangeText(const String& replacement, unsigned start, unsigned end, const String& selectionMode, ExceptionState&) final;

    HTMLImageLoader* imageLoader() const { return m_imageLoader.get(); }
    HTMLImageLoader& ensureImageLoader();

    bool setupDateTimeChooserParameters(DateTimeChooserParameters&);

    bool supportsInputModeAttribute() const;

    void setShouldRevealPassword(bool value);
    bool shouldRevealPassword() const { return m_shouldRevealPassword; }
    AXObject* popupRootAXObject();
    void didNotifySubtreeInsertionsToDocument() override;

    virtual void ensureFallbackContent();
    virtual void ensurePrimaryContent();
    bool hasFallbackContent() const;
protected:
    HTMLInputElement(Document&, HTMLFormElement*, bool createdByParser);

    void defaultEventHandler(Event*) override;

private:
    enum AutoCompleteSetting { Uninitialized, On, Off };

    void didAddUserAgentShadowRoot(ShadowRoot&) final;
    void willAddFirstAuthorShadowRoot() final;

    void willChangeForm() final;
    void didChangeForm() final;
    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) final;
    void didMoveToNewDocument(Document& oldDocument) final;
    void removeAllEventListeners() final;

    bool hasCustomFocusLogic() const final;
    bool isKeyboardFocusable() const final;
    bool shouldShowFocusRingOnMouseFocus() const final;
    bool isEnumeratable() const final;
    bool isInteractiveContent() const final;
    bool supportLabels() const final;

    bool isTextFormControl() const final { return isTextField(); }

    bool canTriggerImplicitSubmission() const final { return isTextField(); }

    const AtomicString& formControlType() const final;

    bool shouldSaveAndRestoreFormControlState() const final;
    FormControlState saveFormControlState() const final;
    void restoreFormControlState(const FormControlState&) final;

    bool canStartSelection() const final;

    void accessKeyAction(bool sendMouseEvents) final;

    void attributeWillChange(const QualifiedName&, const AtomicString& oldValue, const AtomicString& newValue) override;
    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    bool isPresentationAttribute(const QualifiedName&) const final;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) final;
    void finishParsingChildren() final;
    void parserDidSetAttributes() final;

    void copyNonAttributePropertiesFromElement(const Element&) final;

    void attach(const AttachContext& = AttachContext()) final;

    bool appendFormData(FormDataList&, bool) final;
    String resultForDialogSubmit() final;

    bool canBeSuccessfulSubmitButton() const final;

    void resetImpl() final;
    bool supportsAutofocus() const final;

    void* preDispatchEventHandler(Event*) final;
    void postDispatchEventHandler(Event*, void* dataFromPreDispatch) final;

    bool isURLAttribute(const Attribute&) const final;
    bool hasLegalLinkAttribute(const QualifiedName&) const final;
    const QualifiedName& subResourceAttributeName() const final;
    bool isInRange() const final;
    bool isOutOfRange() const final;

    bool tooLong(const String&, NeedsToCheckDirtyFlag) const;
    bool tooShort(const String&, NeedsToCheckDirtyFlag) const;

    bool supportsPlaceholder() const final;
    void updatePlaceholderText() final;
    bool isEmptyValue() const final { return innerEditorValue().isEmpty(); }
    bool isEmptySuggestedValue() const final { return suggestedValue().isEmpty(); }
    void handleFocusEvent(Element* oldFocusedElement, WebFocusType) final;
    void handleBlurEvent() final;
    void dispatchFocusInEvent(const AtomicString& eventType, Element* oldFocusedElement, WebFocusType) final;
    bool supportsAutocapitalize() const final;
    const AtomicString& defaultAutocapitalize() const final;

    bool isOptionalFormControl() const final { return !isRequiredFormControl(); }
    bool isRequiredFormControl() const final;
    bool recalcWillValidate() const final;
    void requiredAttributeChanged() final;

    void updateTouchEventHandlerRegistry();
    void initializeTypeInParsing();
    void updateType();

    void subtreeHasChanged() final;

    void setListAttributeTargetObserver(PassOwnPtrWillBeRawPtr<ListAttributeTargetObserver>);
    void resetListAttributeTargetObserver();
    void parseMaxLengthAttribute(const AtomicString&);
    void parseMinLengthAttribute(const AtomicString&);
    void updateValueIfNeeded();

    // Returns null if this isn't associated with any radio button group.
    RadioButtonGroupScope* radioButtonGroupScope() const;
    void addToRadioButtonGroup();
    void removeFromRadioButtonGroup();
    PassRefPtr<ComputedStyle> customStyleForLayoutObject() override;

    bool shouldDispatchFormControlChangeEvent(String&, String&) override;

    AtomicString m_name;
    String m_valueIfDirty;
    String m_suggestedValue;
    int m_size;
    int m_maxLength;
    int m_minLength;
    short m_maxResults;
    unsigned m_isChecked : 1;
    unsigned m_reflectsCheckedAttribute : 1;
    unsigned m_isIndeterminate : 1;
    unsigned m_isActivatedSubmit : 1;
    unsigned m_autocomplete : 2; // AutoCompleteSetting
    unsigned m_hasNonEmptyList : 1;
    unsigned m_stateRestored : 1;
    unsigned m_parsingInProgress : 1;
    unsigned m_valueAttributeWasUpdatedAfterParsing : 1;
    unsigned m_canReceiveDroppedFiles : 1;
    unsigned m_hasTouchEventHandler : 1;
    unsigned m_shouldRevealPassword : 1;
    unsigned m_needsToUpdateViewValue : 1;
    RefPtrWillBeMember<InputType> m_inputType;
    RefPtrWillBeMember<InputTypeView> m_inputTypeView;
    // The ImageLoader must be owned by this element because the loader code assumes
    // that it lives as long as its owning element lives. If we move the loader into
    // the ImageInput object we may delete the loader while this element lives on.
    OwnPtrWillBeMember<HTMLImageLoader> m_imageLoader;
    OwnPtrWillBeMember<ListAttributeTargetObserver> m_listAttributeTargetObserver;
};

} // namespace blink

#endif // HTMLInputElement_h
