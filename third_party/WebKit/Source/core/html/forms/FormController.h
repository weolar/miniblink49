/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2010, 2011, 2012 Google Inc. All rights reserved.
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

#ifndef FormController_h
#define FormController_h

#include "core/html/forms/RadioButtonGroupScope.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/ListHashSet.h"
#include "wtf/Vector.h"
#include "wtf/text/AtomicStringHash.h"

namespace blink {

class FormKeyGenerator;
class HTMLFormControlElementWithState;
class HTMLFormElement;
class SavedFormState;

class FormControlState {
public:
    FormControlState() : m_type(TypeSkip) { }
    explicit FormControlState(const String& value) : m_type(TypeRestore) { m_values.append(value); }
    static FormControlState deserialize(const Vector<String>& stateVector, size_t& index);
    FormControlState(const FormControlState& another) : m_type(another.m_type), m_values(another.m_values) { }
    FormControlState& operator=(const FormControlState&);

    bool isFailure() const { return m_type == TypeFailure; }
    size_t valueSize() const { return m_values.size(); }
    const String& operator[](size_t i) const { return m_values[i]; }
    void append(const String&);
    void serializeTo(Vector<String>& stateVector) const;

private:
    enum Type { TypeSkip, TypeRestore, TypeFailure };
    explicit FormControlState(Type type) : m_type(type) { }

    Type m_type;
    Vector<String> m_values;
};

inline FormControlState& FormControlState::operator=(const FormControlState& another)
{
    m_type = another.m_type;
    m_values = another.m_values;
    return *this;
}

inline void FormControlState::append(const String& value)
{
    m_type = TypeRestore;
    m_values.append(value);
}

using SavedFormStateMap = HashMap<AtomicString, OwnPtr<SavedFormState>>;

class DocumentState final : public RefCountedWillBeGarbageCollected<DocumentState> {
    DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(DocumentState);
public:
    static PassRefPtrWillBeRawPtr<DocumentState> create();
    DECLARE_TRACE();

    void addControl(HTMLFormControlElementWithState*);
    void removeControl(HTMLFormControlElementWithState*);
    Vector<String> toStateVector();

private:
    using FormElementListHashSet = WillBeHeapListHashSet<RefPtrWillBeMember<HTMLFormControlElementWithState>, 64>;
    FormElementListHashSet m_formControls;
};

class FormController final : public NoBaseWillBeGarbageCollectedFinalized<FormController> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(FormController);
public:
    static PassOwnPtrWillBeRawPtr<FormController> create()
    {
        return adoptPtrWillBeNoop(new FormController);
    }
    ~FormController();
    DECLARE_TRACE();

    RadioButtonGroupScope& radioButtonGroupScope() { return m_radioButtonGroupScope; }

    void registerStatefulFormControl(HTMLFormControlElementWithState&);
    void unregisterStatefulFormControl(HTMLFormControlElementWithState&);
    // This should be callled only by Document::formElementsState().
    DocumentState* formElementsState() const;
    // This should be callled only by Document::setStateForNewFormElements().
    void setStateForNewFormElements(const Vector<String>&);
    void willDeleteForm(HTMLFormElement*);
    void restoreControlStateFor(HTMLFormControlElementWithState&);
    void restoreControlStateIn(HTMLFormElement&);

    static Vector<String> getReferencedFilePaths(const Vector<String>& stateVector);

private:
    FormController();
    FormControlState takeStateForFormElement(const HTMLFormControlElementWithState&);
    static void formStatesFromStateVector(const Vector<String>&, SavedFormStateMap&);

    RadioButtonGroupScope m_radioButtonGroupScope;
    RefPtrWillBeMember<DocumentState> m_documentState;
    SavedFormStateMap m_savedFormStateMap;
    OwnPtrWillBeMember<FormKeyGenerator> m_formKeyGenerator;
};

} // namespace blink
#endif
