/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2006 Apple Computer, Inc.
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

#ifndef PopupMenuClient_h
#define PopupMenuClient_h

#include "core/CoreExport.h"
#include "platform/LayoutUnit.h"
#include "platform/PlatformExport.h"
#include "platform/PopupMenuStyle.h"
#include "wtf/Forward.h"

namespace blink {

class Element;
class ComputedStyle;

class CORE_EXPORT PopupMenuClient {
public:
    virtual ~PopupMenuClient() { }
    virtual void valueChanged(unsigned listIndex, bool fireEvents = true) = 0;
    virtual void selectionChanged(unsigned listIndex, bool fireEvents = true) = 0;
    virtual void selectionCleared() = 0;

    virtual String itemText(unsigned listIndex) const = 0;
    virtual String itemToolTip(unsigned listIndex) const = 0;
    virtual String itemAccessibilityText(unsigned listIndex) const = 0;
    virtual bool itemIsEnabled(unsigned listIndex) const = 0;
    virtual PopupMenuStyle itemStyle(unsigned listIndex) const = 0;
    virtual PopupMenuStyle menuStyle() const = 0;
    virtual LayoutUnit clientPaddingLeft() const = 0;
    virtual LayoutUnit clientPaddingRight() const = 0;
    virtual int listSize() const = 0;
    virtual int selectedIndex() const = 0;
    virtual void popupDidHide() = 0;
    // A popup is canceled when the popup was hidden without selecting an item.
    virtual void popupDidCancel() = 0;
    virtual bool itemIsSeparator(unsigned listIndex) const = 0;
    virtual bool itemIsLabel(unsigned listIndex) const = 0;
    virtual bool itemIsSelected(unsigned listIndex) const = 0;
    // Provisional selection is a selection made using arrow keys or type ahead.
    virtual void provisionalSelectionChanged(unsigned) = 0;
    virtual IntRect elementRectRelativeToViewport() const = 0;
    virtual Element& ownerElement() const = 0;
    virtual const ComputedStyle* computedStyleForItem(Element&) const = 0;

    virtual void listBoxSelectItem(int /*listIndex*/, bool /*allowMultiplySelections*/, bool /*shift*/, bool /*fireOnChangeNow*/ = true) { ASSERT_NOT_REACHED(); }
    virtual bool multiple() const
    {
        ASSERT_NOT_REACHED();
        return false;
    }
};

}

#endif
