/*
 * Copyright (C) 2010 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/accessibility/AXMenuList.h"

#include "core/layout/LayoutMenuList.h"
#include "modules/accessibility/AXMenuListPopup.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

AXMenuList::AXMenuList(LayoutMenuList* layoutObject, AXObjectCacheImpl& axObjectCache)
    : AXLayoutObject(layoutObject, axObjectCache)
{
}

PassRefPtrWillBeRawPtr<AXMenuList> AXMenuList::create(LayoutMenuList* layoutObject, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXMenuList(layoutObject, axObjectCache));
}

AccessibilityRole AXMenuList::determineAccessibilityRole()
{
    if ((m_ariaRole = determineAriaRoleAttribute()) != UnknownRole)
        return m_ariaRole;

    return PopUpButtonRole;
}

bool AXMenuList::press() const
{
    if (!m_layoutObject)
        return false;

    LayoutMenuList* menuList = toLayoutMenuList(m_layoutObject);
    if (menuList->popupIsVisible())
        menuList->hidePopup();
    else
        menuList->showPopup();
    return true;
}

void AXMenuList::clearChildren()
{
    if (m_children.isEmpty())
        return;

    // There's no reason to clear our AXMenuListPopup child. If we get a
    // call to clearChildren, it's because the options might have changed,
    // so call it on our popup.
    ASSERT(m_children.size() == 1);
    m_children[0]->clearChildren();
    m_childrenDirty = false;
}

void AXMenuList::addChildren()
{
    m_haveChildren = true;

    AXObjectCacheImpl& cache = axObjectCache();

    AXObject* list = cache.getOrCreate(MenuListPopupRole);
    if (!list)
        return;

    toAXMockObject(list)->setParent(this);
    if (list->accessibilityIsIgnored()) {
        cache.remove(list->axObjectID());
        return;
    }

    m_children.append(list);

    list->addChildren();
}

bool AXMenuList::isCollapsed() const
{
    // Collapsed is the "default" state, so if the LayoutObject doesn't exist
    // this makes slightly more sense than returning false.
    if (!m_layoutObject)
        return true;

    return !toLayoutMenuList(m_layoutObject)->popupIsVisible();
}

AccessibilityExpanded AXMenuList::isExpanded() const
{
    if (isCollapsed())
        return ExpandedCollapsed;

    return ExpandedExpanded;
}

bool AXMenuList::canSetFocusAttribute() const
{
    if (!node())
        return false;

    return !toElement(node())->isDisabledFormControl();
}

void AXMenuList::didUpdateActiveOption(int optionIndex)
{
    const auto& childObjects = children();
    if (!childObjects.isEmpty()) {
        ASSERT(childObjects.size() == 1);
        ASSERT(childObjects[0]->isMenuListPopup());

        if (childObjects[0]->isMenuListPopup()) {
            if (AXMenuListPopup* popup = toAXMenuListPopup(childObjects[0].get()))
                popup->didUpdateActiveOption(optionIndex);
        }
    }

    axObjectCache().postNotification(this, AXObjectCacheImpl::AXMenuListValueChanged);
}

void AXMenuList::didShowPopup()
{
    if (children().size() != 1)
        return;

    AXMenuListPopup* popup = toAXMenuListPopup(children()[0].get());
    popup->didShow();
}

void AXMenuList::didHidePopup()
{
    if (children().size() != 1)
        return;

    AXMenuListPopup* popup = toAXMenuListPopup(children()[0].get());
    popup->didHide();

    if (node() && node()->focused())
        axObjectCache().postNotification(this, AXObjectCacheImpl::AXFocusedUIElementChanged);
}

} // namespace blink
