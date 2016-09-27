/*
 * Copyright (C) 2007, 2008, 2009 Apple Inc. All rights reserved.
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
#include "core/html/forms/RadioButtonGroupScope.h"

#include "core/InputTypeNames.h"
#include "core/html/HTMLInputElement.h"
#include "wtf/HashSet.h"

namespace blink {

class RadioButtonGroup : public NoBaseWillBeGarbageCollected<RadioButtonGroup> {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(RadioButtonGroup);
public:
    static PassOwnPtrWillBeRawPtr<RadioButtonGroup> create();
    bool isEmpty() const { return m_members.isEmpty(); }
    bool isRequired() const { return m_requiredCount; }
    HTMLInputElement* checkedButton() const { return m_checkedButton; }
    void add(HTMLInputElement*);
    void updateCheckedState(HTMLInputElement*);
    void requiredAttributeChanged(HTMLInputElement*);
    void remove(HTMLInputElement*);
    bool contains(HTMLInputElement*) const;

    DECLARE_TRACE();

private:
    RadioButtonGroup();
    void setNeedsValidityCheckForAllButtons();
    bool isValid() const;
    void setCheckedButton(HTMLInputElement*);

    WillBeHeapHashSet<RawPtrWillBeMember<HTMLInputElement>> m_members;
    RawPtrWillBeMember<HTMLInputElement> m_checkedButton;
    size_t m_requiredCount;
};

RadioButtonGroup::RadioButtonGroup()
    : m_checkedButton(nullptr)
    , m_requiredCount(0)
{
}

PassOwnPtrWillBeRawPtr<RadioButtonGroup> RadioButtonGroup::create()
{
    return adoptPtrWillBeNoop(new RadioButtonGroup);
}

inline bool RadioButtonGroup::isValid() const
{
    return !isRequired() || m_checkedButton;
}

void RadioButtonGroup::setCheckedButton(HTMLInputElement* button)
{
    HTMLInputElement* oldCheckedButton = m_checkedButton;
    if (oldCheckedButton == button)
        return;
    m_checkedButton = button;
    if (oldCheckedButton)
        oldCheckedButton->setChecked(false);
}

void RadioButtonGroup::add(HTMLInputElement* button)
{
    ASSERT(button->type() == InputTypeNames::radio);
    if (!m_members.add(button).isNewEntry)
        return;
    bool groupWasValid = isValid();
    if (button->isRequired())
        ++m_requiredCount;
    if (button->checked())
        setCheckedButton(button);

    bool groupIsValid = isValid();
    if (groupWasValid != groupIsValid) {
        setNeedsValidityCheckForAllButtons();
    } else if (!groupIsValid) {
        // A radio button not in a group is always valid. We need to make it
        // invalid only if the group is invalid.
        button->setNeedsValidityCheck();
    }
}

void RadioButtonGroup::updateCheckedState(HTMLInputElement* button)
{
    ASSERT(button->type() == InputTypeNames::radio);
    ASSERT(m_members.contains(button));
    bool wasValid = isValid();
    if (button->checked()) {
        setCheckedButton(button);
    } else {
        if (m_checkedButton == button)
            m_checkedButton = nullptr;
    }
    if (wasValid != isValid())
        setNeedsValidityCheckForAllButtons();
    for (HTMLInputElement* const inputElement : m_members) {
        inputElement->pseudoStateChanged(CSSSelector::PseudoIndeterminate);
    }
}

void RadioButtonGroup::requiredAttributeChanged(HTMLInputElement* button)
{
    ASSERT(button->type() == InputTypeNames::radio);
    ASSERT(m_members.contains(button));
    bool wasValid = isValid();
    if (button->isRequired()) {
        ++m_requiredCount;
    } else {
        ASSERT(m_requiredCount);
        --m_requiredCount;
    }
    if (wasValid != isValid())
        setNeedsValidityCheckForAllButtons();
}

void RadioButtonGroup::remove(HTMLInputElement* button)
{
    ASSERT(button->type() == InputTypeNames::radio);
    WillBeHeapHashSet<RawPtrWillBeMember<HTMLInputElement>>::iterator it = m_members.find(button);
    if (it == m_members.end())
        return;
    bool wasValid = isValid();
    m_members.remove(it);
    if (button->isRequired()) {
        ASSERT(m_requiredCount);
        --m_requiredCount;
    }
    if (m_checkedButton == button)
        m_checkedButton = nullptr;

    if (m_members.isEmpty()) {
        ASSERT(!m_requiredCount);
        ASSERT(!m_checkedButton);
    } else if (wasValid != isValid()) {
        setNeedsValidityCheckForAllButtons();
    }
    if (!wasValid) {
        // A radio button not in a group is always valid. We need to make it
        // valid only if the group was invalid.
        button->setNeedsValidityCheck();
    }
}

void RadioButtonGroup::setNeedsValidityCheckForAllButtons()
{
    for (HTMLInputElement* const button : m_members) {
        ASSERT(button->type() == InputTypeNames::radio);
        button->setNeedsValidityCheck();
    }
}

bool RadioButtonGroup::contains(HTMLInputElement* button) const
{
    return m_members.contains(button);
}

DEFINE_TRACE(RadioButtonGroup)
{
#if ENABLE(OILPAN)
    visitor->trace(m_members);
    visitor->trace(m_checkedButton);
#endif
}

// ----------------------------------------------------------------

// Explicity define empty constructor and destructor in order to prevent the
// compiler from generating them as inlines. So we don't need to to define
// RadioButtonGroup in the header.
RadioButtonGroupScope::RadioButtonGroupScope()
{
}

RadioButtonGroupScope::~RadioButtonGroupScope()
{
}

void RadioButtonGroupScope::addButton(HTMLInputElement* element)
{
    ASSERT(element->type() == InputTypeNames::radio);
    if (element->name().isEmpty())
        return;

    if (!m_nameToGroupMap)
        m_nameToGroupMap = adoptPtrWillBeNoop(new NameToGroupMap);

    OwnPtrWillBeMember<RadioButtonGroup>& group = m_nameToGroupMap->add(element->name(), nullptr).storedValue->value;
    if (!group)
        group = RadioButtonGroup::create();
    group->add(element);
}

void RadioButtonGroupScope::updateCheckedState(HTMLInputElement* element)
{
    ASSERT(element->type() == InputTypeNames::radio);
    if (element->name().isEmpty())
        return;
    ASSERT(m_nameToGroupMap);
    if (!m_nameToGroupMap)
        return;
    RadioButtonGroup* group = m_nameToGroupMap->get(element->name());
    ASSERT(group);
    group->updateCheckedState(element);
}

void RadioButtonGroupScope::requiredAttributeChanged(HTMLInputElement* element)
{
    ASSERT(element->type() == InputTypeNames::radio);
    if (element->name().isEmpty())
        return;
    ASSERT(m_nameToGroupMap);
    if (!m_nameToGroupMap)
        return;
    RadioButtonGroup* group = m_nameToGroupMap->get(element->name());
    ASSERT(group);
    group->requiredAttributeChanged(element);
}

HTMLInputElement* RadioButtonGroupScope::checkedButtonForGroup(const AtomicString& name) const
{
    if (!m_nameToGroupMap)
        return nullptr;
    RadioButtonGroup* group = m_nameToGroupMap->get(name);
    return group ? group->checkedButton() : nullptr;
}

bool RadioButtonGroupScope::isInRequiredGroup(HTMLInputElement* element) const
{
    ASSERT(element->type() == InputTypeNames::radio);
    if (element->name().isEmpty())
        return false;
    if (!m_nameToGroupMap)
        return false;
    RadioButtonGroup* group = m_nameToGroupMap->get(element->name());
    return group && group->isRequired() && group->contains(element);
}

void RadioButtonGroupScope::removeButton(HTMLInputElement* element)
{
    ASSERT(element->type() == InputTypeNames::radio);
    if (element->name().isEmpty())
        return;
    if (!m_nameToGroupMap)
        return;

    RadioButtonGroup* group = m_nameToGroupMap->get(element->name());
    if (!group)
        return;
    group->remove(element);
    if (group->isEmpty()) {
        // We don't remove an empty RadioButtonGroup from m_nameToGroupMap for
        // better performance.
        ASSERT(!group->isRequired());
        ASSERT_WITH_SECURITY_IMPLICATION(!group->checkedButton());
    }
}

DEFINE_TRACE(RadioButtonGroupScope)
{
#if ENABLE(OILPAN)
    visitor->trace(m_nameToGroupMap);
#endif
}

} // namespace
