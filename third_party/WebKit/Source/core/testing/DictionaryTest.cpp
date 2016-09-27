// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "DictionaryTest.h"

#include "core/testing/InternalDictionary.h"
#include "core/testing/InternalDictionaryDerived.h"

namespace blink {

DictionaryTest::DictionaryTest()
    : m_requiredBooleanMember(false)
{
}

DictionaryTest::~DictionaryTest()
{
}

void DictionaryTest::set(const InternalDictionary& testingDictionary)
{
    reset();
    if (testingDictionary.hasLongMember())
        m_longMember = testingDictionary.longMember();
    if (testingDictionary.hasLongMemberWithClamp())
        m_longMemberWithClamp = testingDictionary.longMemberWithClamp();
    if (testingDictionary.hasLongMemberWithEnforceRange())
        m_longMemberWithEnforceRange = testingDictionary.longMemberWithEnforceRange();
    m_longMemberWithDefault = testingDictionary.longMemberWithDefault();
    if (testingDictionary.hasLongOrNullMember())
        m_longOrNullMember = testingDictionary.longOrNullMember();
    // |longOrNullMemberWithDefault| has a default value but can be null, so
    // we need to check availability.
    if (testingDictionary.hasLongOrNullMemberWithDefault())
        m_longOrNullMemberWithDefault = testingDictionary.longOrNullMemberWithDefault();
    if (testingDictionary.hasBooleanMember())
        m_booleanMember = testingDictionary.booleanMember();
    if (testingDictionary.hasDoubleMember())
        m_doubleMember = testingDictionary.doubleMember();
    if (testingDictionary.hasUnrestrictedDoubleMember())
        m_unrestrictedDoubleMember = testingDictionary.unrestrictedDoubleMember();
    m_stringMember = testingDictionary.stringMember();
    m_stringMemberWithDefault = testingDictionary.stringMemberWithDefault();
    m_byteStringMember = testingDictionary.byteStringMember();
    m_usvStringMember = testingDictionary.usvStringMember();
    if (testingDictionary.hasStringSequenceMember())
        m_stringSequenceMember = testingDictionary.stringSequenceMember();
    m_stringSequenceMemberWithDefault = testingDictionary.stringSequenceMemberWithDefault();
    if (testingDictionary.hasStringSequenceOrNullMember())
        m_stringSequenceOrNullMember = testingDictionary.stringSequenceOrNullMember();
    m_enumMember = testingDictionary.enumMember();
    m_enumMemberWithDefault = testingDictionary.enumMemberWithDefault();
    m_enumOrNullMember = testingDictionary.enumOrNullMember();
    if (testingDictionary.hasEnumArrayMember())
        m_enumArrayMember = testingDictionary.enumArrayMember();
    if (testingDictionary.hasElementMember())
        m_elementMember = testingDictionary.elementMember();
    if (testingDictionary.hasElementOrNullMember())
        m_elementOrNullMember = testingDictionary.elementOrNullMember();
    m_objectMember = testingDictionary.objectMember();
    m_objectOrNullMemberWithDefault = testingDictionary.objectOrNullMemberWithDefault();
    if (testingDictionary.hasDoubleOrStringMember())
        m_doubleOrStringMember = testingDictionary.doubleOrStringMember();
    if (testingDictionary.hasDoubleOrStringSequenceMember())
        m_doubleOrStringSequenceMember = testingDictionary.doubleOrStringSequenceMember();
    m_eventTargetOrNullMember = testingDictionary.eventTargetOrNullMember();
}

void DictionaryTest::get(InternalDictionary& result)
{
    if (m_longMember)
        result.setLongMember(m_longMember.get());
    if (m_longMemberWithClamp)
        result.setLongMemberWithClamp(m_longMemberWithClamp.get());
    if (m_longMemberWithEnforceRange)
        result.setLongMemberWithEnforceRange(m_longMemberWithEnforceRange.get());
    result.setLongMemberWithDefault(m_longMemberWithDefault);
    if (m_longOrNullMember)
        result.setLongOrNullMember(m_longOrNullMember.get());
    if (m_longOrNullMemberWithDefault)
        result.setLongOrNullMemberWithDefault(m_longOrNullMemberWithDefault.get());
    if (m_booleanMember)
        result.setBooleanMember(m_booleanMember.get());
    if (m_doubleMember)
        result.setDoubleMember(m_doubleMember.get());
    if (m_unrestrictedDoubleMember)
        result.setUnrestrictedDoubleMember(m_unrestrictedDoubleMember.get());
    result.setStringMember(m_stringMember);
    result.setStringMemberWithDefault(m_stringMemberWithDefault);
    result.setByteStringMember(m_byteStringMember);
    result.setUsvStringMember(m_usvStringMember);
    if (m_stringSequenceMember)
        result.setStringSequenceMember(m_stringSequenceMember.get());
    result.setStringSequenceMemberWithDefault(m_stringSequenceMemberWithDefault);
    if (m_stringSequenceOrNullMember)
        result.setStringSequenceOrNullMember(m_stringSequenceOrNullMember.get());
    result.setEnumMember(m_enumMember);
    result.setEnumMemberWithDefault(m_enumMemberWithDefault);
    result.setEnumOrNullMember(m_enumOrNullMember);
    if (m_enumArrayMember)
        result.setEnumArrayMember(m_enumArrayMember.get());
    if (m_elementMember)
        result.setElementMember(m_elementMember);
    if (m_elementOrNullMember)
        result.setElementOrNullMember(m_elementOrNullMember);
    result.setObjectMember(m_objectMember);
    result.setObjectOrNullMemberWithDefault(m_objectOrNullMemberWithDefault);
    if (!m_doubleOrStringMember.isNull())
        result.setDoubleOrStringMember(m_doubleOrStringMember);
    if (!m_doubleOrStringSequenceMember.isNull())
        result.setDoubleOrStringSequenceMember(m_doubleOrStringSequenceMember.get());
    result.setEventTargetOrNullMember(m_eventTargetOrNullMember);
}

void DictionaryTest::setDerived(const InternalDictionaryDerived& derived)
{
    ASSERT(derived.hasRequiredBooleanMember());
    set(derived);
    if (derived.hasDerivedStringMember())
        m_derivedStringMember = derived.derivedStringMember();
    m_derivedStringMemberWithDefault = derived.derivedStringMemberWithDefault();
    m_requiredBooleanMember = derived.requiredBooleanMember();
}

void DictionaryTest::getDerived(InternalDictionaryDerived& result)
{
    get(result);
    result.setDerivedStringMember(m_derivedStringMember);
    result.setDerivedStringMemberWithDefault(m_derivedStringMemberWithDefault);
    result.setRequiredBooleanMember(m_requiredBooleanMember);
}

void DictionaryTest::reset()
{
    m_longMember = nullptr;
    m_longMemberWithClamp = nullptr;
    m_longMemberWithEnforceRange = nullptr;
    m_longMemberWithDefault = -1; // This value should not be returned.
    m_longOrNullMember = nullptr;
    m_longOrNullMemberWithDefault = nullptr;
    m_booleanMember = nullptr;
    m_doubleMember = nullptr;
    m_unrestrictedDoubleMember = nullptr;
    m_stringMember = String();
    m_stringMemberWithDefault = String("Should not be returned");
    m_stringSequenceMember = nullptr;
    m_stringSequenceMemberWithDefault.fill("Should not be returned", 1);
    m_stringSequenceOrNullMember = nullptr;
    m_enumMember = String();
    m_enumMemberWithDefault = String();
    m_enumOrNullMember = String();
    m_enumArrayMember = nullptr;
    m_elementMember = nullptr;
    m_elementOrNullMember = nullptr;
    m_objectMember = ScriptValue();
    m_objectOrNullMemberWithDefault = ScriptValue();
    m_doubleOrStringMember = DoubleOrString();
    m_eventTargetOrNullMember = nullptr;
    m_derivedStringMember = String();
    m_derivedStringMemberWithDefault = String();
    m_requiredBooleanMember = false;
}

DEFINE_TRACE(DictionaryTest)
{
    visitor->trace(m_elementMember);
    visitor->trace(m_elementOrNullMember);
    visitor->trace(m_doubleOrStringSequenceMember);
    visitor->trace(m_eventTargetOrNullMember);
}

}
