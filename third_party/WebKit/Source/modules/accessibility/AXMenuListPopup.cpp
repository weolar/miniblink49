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
#include "modules/accessibility/AXMenuListPopup.h"

#include "core/html/HTMLSelectElement.h"
#include "modules/accessibility/AXMenuListOption.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

using namespace HTMLNames;

AXMenuListPopup::AXMenuListPopup(AXObjectCacheImpl& axObjectCache)
    : AXMockObject(axObjectCache), m_activeIndex(-1)
{
}

bool AXMenuListPopup::isVisible() const
{
    return !isOffScreen();
}

bool AXMenuListPopup::isOffScreen() const
{
    if (!m_parent)
        return true;

    return m_parent->isCollapsed();
}

bool AXMenuListPopup::isEnabled() const
{
    if (!m_parent)
        return false;

    return m_parent->isEnabled();
}

bool AXMenuListPopup::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    return accessibilityIsIgnoredByDefault(ignoredReasons);
}

AXMenuListOption* AXMenuListPopup::menuListOptionAXObject(HTMLElement* element) const
{
    ASSERT(element);
    if (!isHTMLOptionElement(*element))
        return 0;

    AXObject* object = axObjectCache().getOrCreate(element);
    if (!object || !object->isMenuListOption())
        return 0;

    return toAXMenuListOption(object);
}

int AXMenuListPopup::getSelectedIndex() const
{
    if (!m_parent)
        return -1;

    Node* parentNode = m_parent->node();
    if (!isHTMLSelectElement(parentNode))
        return -1;

    HTMLSelectElement* htmlSelectElement = toHTMLSelectElement(parentNode);
    return htmlSelectElement->selectedIndex();
}

bool AXMenuListPopup::press() const
{
    if (!m_parent)
        return false;

    m_parent->press();
    return true;
}

void AXMenuListPopup::addChildren()
{
    if (!m_parent)
        return;

    Node* parentNode = m_parent->node();
    if (!isHTMLSelectElement(parentNode))
        return;

    HTMLSelectElement* htmlSelectElement = toHTMLSelectElement(parentNode);
    m_haveChildren = true;

    if (m_activeIndex == -1)
        m_activeIndex = getSelectedIndex();

    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = htmlSelectElement->listItems();
    unsigned length = listItems.size();
    for (unsigned i = 0; i < length; i++) {
        AXMenuListOption* option = menuListOptionAXObject(listItems[i]);
        if (option) {
            option->setParent(this);
            m_children.append(option);
        }
    }
}

void AXMenuListPopup::updateChildrenIfNecessary()
{
    if (m_haveChildren && m_parent && m_parent->needsToUpdateChildren())
        clearChildren();

    if (!m_haveChildren)
        addChildren();
}

void AXMenuListPopup::didUpdateActiveOption(int optionIndex)
{
    updateChildrenIfNecessary();

    AXObjectCacheImpl& cache = axObjectCache();
    if (m_activeIndex != optionIndex && m_activeIndex >= 0 && m_activeIndex < static_cast<int>(m_children.size())) {
        RefPtrWillBeRawPtr<AXObject> previousChild = m_children[m_activeIndex].get();
        cache.postNotification(previousChild.get(), AXObjectCacheImpl::AXMenuListItemUnselected);
    }

    if (optionIndex >= 0 && optionIndex < static_cast<int>(m_children.size())) {
        RefPtrWillBeRawPtr<AXObject> child = m_children[optionIndex].get();
        cache.postNotification(child.get(), AXObjectCacheImpl::AXFocusedUIElementChanged);
        cache.postNotification(child.get(), AXObjectCacheImpl::AXMenuListItemSelected);
    }

    m_activeIndex = optionIndex;
}

void AXMenuListPopup::didHide()
{
    AXObjectCacheImpl& cache = axObjectCache();
    cache.postNotification(this, AXObjectCacheImpl::AXHide);
    if (activeChild())
        cache.postNotification(activeChild(), AXObjectCacheImpl::AXMenuListItemUnselected);
}

void AXMenuListPopup::didShow()
{
    if (!m_haveChildren)
        addChildren();

    AXObjectCacheImpl& cache = axObjectCache();
    cache.postNotification(this, AXObjectCacheImpl::AXShow);
    int selectedIndex = getSelectedIndex();
    if (selectedIndex >= 0 && selectedIndex < static_cast<int>(m_children.size()))
        didUpdateActiveOption(selectedIndex);
    else
        cache.postNotification(m_parent, AXObjectCacheImpl::AXFocusedUIElementChanged);
}

AXObject* AXMenuListPopup::activeChild()
{
    if (m_activeIndex < 0 || m_activeIndex >= static_cast<int>(children().size()))
        return nullptr;

    return m_children[m_activeIndex].get();
}

} // namespace blink
