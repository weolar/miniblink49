// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DictionaryTest_h
#define DictionaryTest_h

#include "bindings/core/v8/Nullable.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/Element.h"
#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class InternalDictionary;
class InternalDictionaryDerived;

class DictionaryTest : public GarbageCollectedFinalized<DictionaryTest>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DictionaryTest* create()
    {
        return new DictionaryTest();
    }
    virtual ~DictionaryTest();

    // Stores all members into corresponding fields
    void set(const InternalDictionary&);
    // Sets each member of the given TestDictionary from fields
    void get(InternalDictionary&);

    void setDerived(const InternalDictionaryDerived&);
    void getDerived(InternalDictionaryDerived&);

    DECLARE_TRACE();

private:
    DictionaryTest();

    void reset();

    // The reason to use Nullable<T> is convenience; we use Nullable<T> here to
    // record whether the member field is set or not. Some members are not
    // wrapped with Nullable because:
    //  - |longMemberWithDefault| has a non-null default value
    //  - String and PtrTypes can express whether they are null
    Nullable<int> m_longMember;
    Nullable<int> m_longMemberWithClamp;
    Nullable<int> m_longMemberWithEnforceRange;
    int m_longMemberWithDefault;
    Nullable<int> m_longOrNullMember;
    Nullable<int> m_longOrNullMemberWithDefault;
    Nullable<bool> m_booleanMember;
    Nullable<double> m_doubleMember;
    Nullable<double> m_unrestrictedDoubleMember;
    String m_stringMember;
    String m_stringMemberWithDefault;
    String m_byteStringMember;
    String m_usvStringMember;
    Nullable<Vector<String>> m_stringSequenceMember;
    Vector<String> m_stringSequenceMemberWithDefault;
    Nullable<Vector<String>> m_stringSequenceOrNullMember;
    String m_enumMember;
    String m_enumMemberWithDefault;
    String m_enumOrNullMember;
    Nullable<Vector<String>> m_enumArrayMember;
    RefPtrWillBeMember<Element> m_elementMember;
    RefPtrWillBeMember<Element> m_elementOrNullMember;
    ScriptValue m_objectMember;
    ScriptValue m_objectOrNullMemberWithDefault;
    DoubleOrString m_doubleOrStringMember;
    Nullable<HeapVector<DoubleOrString>> m_doubleOrStringSequenceMember;
    RefPtrWillBeMember<EventTarget> m_eventTargetOrNullMember;
    String m_derivedStringMember;
    String m_derivedStringMemberWithDefault;
    bool m_requiredBooleanMember;
};

} // namespace blink

#endif // DictionaryTest_h
