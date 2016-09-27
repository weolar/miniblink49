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

#ifndef RadioButtonGroupScope_h
#define RadioButtonGroupScope_h

#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/StringHash.h"

namespace blink {

class HTMLInputElement;
class RadioButtonGroup;

class RadioButtonGroupScope {
    DISALLOW_ALLOCATION();
public:
    RadioButtonGroupScope();
    ~RadioButtonGroupScope();
    DECLARE_TRACE();
    void addButton(HTMLInputElement*);
    void updateCheckedState(HTMLInputElement*);
    void requiredAttributeChanged(HTMLInputElement*);
    void removeButton(HTMLInputElement*);
    HTMLInputElement* checkedButtonForGroup(const AtomicString& groupName) const;
    bool isInRequiredGroup(HTMLInputElement*) const;

private:
    using NameToGroupMap = WillBeHeapHashMap<AtomicString, OwnPtrWillBeMember<RadioButtonGroup>, CaseFoldingHash>;
    OwnPtrWillBeMember<NameToGroupMap> m_nameToGroupMap;
};

} // namespace blink

#endif // RadioButtonGroupScope_h
