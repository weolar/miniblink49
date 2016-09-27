/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef HTMLSelectElement_h
#define HTMLSelectElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLFormControlElementWithState.h"
#include "core/html/HTMLOptionsCollection.h"
#include "core/html/forms/TypeAhead.h"
#include "wtf/Vector.h"

namespace blink {

class AutoscrollController;
class ExceptionState;
class HTMLOptionElement;
class HTMLOptionElementOrHTMLOptGroupElement;
class HTMLElementOrLong;

class CORE_EXPORT HTMLSelectElement final : public HTMLFormControlElementWithState, public TypeAheadDataSource {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLSelectElement> create(Document&);
    static PassRefPtrWillBeRawPtr<HTMLSelectElement> create(Document&, HTMLFormElement*);

    int selectedIndex() const;
    void setSelectedIndex(int);
    int suggestedIndex() const;
    void setSuggestedIndex(int);

    void optionSelectedByUser(int index, bool dispatchChangeEvent, bool allowMultipleSelection = false);

    // For ValidityState
    String validationMessage() const override;
    bool valueMissing() const override;

    void resetImpl() override;

    unsigned length() const;

    unsigned size() const { return m_size; }
    bool multiple() const { return m_multiple; }

    bool usesMenuList() const;

    void add(const HTMLOptionElementOrHTMLOptGroupElement&, const HTMLElementOrLong&, ExceptionState&);

    using Node::remove;
    void remove(int index);

    String value() const;
    void setValue(const String&, bool sendEvents = false);
    String suggestedValue() const;
    void setSuggestedValue(const String&);

    PassRefPtrWillBeRawPtr<HTMLOptionsCollection> options();
    PassRefPtrWillBeRawPtr<HTMLCollection> selectedOptions();

    void optionElementChildrenChanged();

    void setRecalcListItems();
    void invalidateSelectedItems();
    void updateListItemSelectedStates();

    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems() const;

    void accessKeyAction(bool sendMouseEvents) override;
    void accessKeySetSelectedIndex(int);

    void setMultiple(bool);

    void setSize(unsigned);

    void setOption(unsigned index, HTMLOptionElement*, ExceptionState&);
    void setLength(unsigned, ExceptionState&);

    Element* namedItem(const AtomicString& name);
    HTMLOptionElement* item(unsigned index);

    void scrollToSelection();
    void scrollToIndex(int listIndex);

    void listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow = true);

    bool canSelectAll() const;
    void selectAll();
    int listToOptionIndex(int listIndex) const;
    void listBoxOnChange();
    int optionToListIndex(int optionIndex) const;
    int activeSelectionEndListIndex() const;
    void setActiveSelectionAnchorIndex(int);
    void setActiveSelectionEndIndex(int);

    // For use in the implementation of HTMLOptionElement.
    void optionSelectionStateChanged(HTMLOptionElement*, bool optionIsSelected);
    void optionInserted(const HTMLOptionElement&, bool optionIsSelected);
    void optionRemoved(const HTMLOptionElement&);
    bool anonymousIndexedSetter(unsigned, PassRefPtrWillBeRawPtr<HTMLOptionElement>, ExceptionState&);

    void updateListOnLayoutObject();

    HTMLOptionElement* spatialNavigationFocusedOption();
    void handleMouseRelease();

    int listIndexForOption(const HTMLOptionElement&);

    DECLARE_VIRTUAL_TRACE();

protected:
    HTMLSelectElement(Document&, HTMLFormElement*);

private:
    void willRecalcStyle(StyleRecalcChange) override;

    const AtomicString& formControlType() const override;

    bool shouldShowFocusRingOnMouseFocus() const override;

    void dispatchFocusEvent(Element* oldFocusedElement, WebFocusType) override;
    void dispatchBlurEvent(Element* newFocusedElement, WebFocusType) override;

    bool canStartSelection() const override { return false; }

    bool isEnumeratable() const override { return true; }
    bool isInteractiveContent() const override;
    bool supportsAutofocus() const override;
    bool supportLabels() const override { return true; }

    FormControlState saveFormControlState() const override;
    void restoreFormControlState(const FormControlState&) override;

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    bool isPresentationAttribute(const QualifiedName&) const override;

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    bool appendFormData(FormDataList&, bool) override;
    void didAddUserAgentShadowRoot(ShadowRoot&) override;

    void defaultEventHandler(Event*) override;

    void dispatchInputAndChangeEventForMenuList(bool requiresUserGesture = true);

    void recalcListItems(bool updateSelectedStates = true) const;

    void typeAheadFind(KeyboardEvent*);
    void saveLastSelection();

    InsertionNotificationRequest insertedInto(ContainerNode*) override;

    bool isOptionalFormControl() const override { return !isRequiredFormControl(); }
    bool isRequiredFormControl() const override;

    bool hasPlaceholderLabelOption() const;

    enum SelectOptionFlag {
        DeselectOtherOptions = 1 << 0,
        DispatchInputAndChangeEvent = 1 << 1,
        UserDriven = 1 << 2,
    };
    typedef unsigned SelectOptionFlags;
    void selectOption(int optionIndex, SelectOptionFlags = 0);
    void deselectItemsWithoutValidation(HTMLElement* elementToExclude = 0);
    void parseMultipleAttribute(const AtomicString&);
    int lastSelectedListIndex() const;
    void updateSelectedState(int listIndex, bool multi, bool shift);
    void menuListDefaultEventHandler(Event*);
    void handlePopupOpenKeyboardEvent(Event*);
    bool shouldOpenPopupForKeyDownEvent(KeyboardEvent*);
    bool shouldOpenPopupForKeyPressEvent(KeyboardEvent*);
    void listBoxDefaultEventHandler(Event*);
    void setOptionsChangedOnLayoutObject();
    size_t searchOptionsForValue(const String&, size_t listIndexStart, size_t listIndexEnd) const;
    void updateListBoxSelection(bool deselectOtherOptions, bool scroll = true);

    enum SkipDirection {
        SkipBackwards = -1,
        SkipForwards = 1
    };
    int nextValidIndex(int listIndex, SkipDirection, int skip) const;
    int nextSelectableListIndex(int startIndex) const;
    int previousSelectableListIndex(int startIndex) const;
    int firstSelectableListIndex() const;
    int lastSelectableListIndex() const;
    int nextSelectableListIndexPageAway(int startIndex, SkipDirection) const;
    int listIndexForEventTargetOption(const Event&);
    AutoscrollController* autoscrollController() const;

    void childrenChanged(const ChildrenChange&) override;
    bool areAuthorShadowsAllowed() const override { return false; }
    void finishParsingChildren() override;

    // TypeAheadDataSource functions.
    int indexOfSelectedOption() const override;
    int optionCount() const override;
    String optionAtIndex(int index) const override;

    // m_listItems contains HTMLOptionElement, HTMLOptGroupElement, and HTMLHRElement objects.
    mutable WillBeHeapVector<RawPtrWillBeMember<HTMLElement>> m_listItems;
    Vector<bool> m_lastOnChangeSelection;
    Vector<bool> m_cachedStateForActiveSelection;
    TypeAhead m_typeAhead;
    unsigned m_size;
    int m_lastOnChangeIndex;
    int m_activeSelectionAnchorIndex;
    int m_activeSelectionEndIndex;
    bool m_isProcessingUserDrivenChange;
    bool m_multiple;
    bool m_activeSelectionState;
    mutable bool m_shouldRecalcListItems;
    int m_suggestedIndex;
    bool m_isAutofilledByPreview;
};

} // namespace blink

#endif // HTMLSelectElement_h
