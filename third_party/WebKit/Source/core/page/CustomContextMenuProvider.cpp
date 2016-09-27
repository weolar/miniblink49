// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/page/CustomContextMenuProvider.h"

#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/events/EventDispatcher.h"
#include "core/events/MouseEvent.h"
#include "core/html/HTMLMenuElement.h"
#include "core/html/HTMLMenuItemElement.h"
#include "core/page/ContextMenuController.h"
#include "core/page/Page.h"
#include "platform/ContextMenu.h"

namespace blink {

using namespace HTMLNames;

CustomContextMenuProvider::CustomContextMenuProvider(HTMLMenuElement& menu, HTMLElement& subject)
    : m_menu(menu)
    , m_subjectElement(subject)
{
}

CustomContextMenuProvider::~CustomContextMenuProvider()
{
}

DEFINE_TRACE(CustomContextMenuProvider)
{
    visitor->trace(m_menu);
    visitor->trace(m_subjectElement);
    visitor->trace(m_menuItems);
    ContextMenuProvider::trace(visitor);
}

void CustomContextMenuProvider::populateContextMenu(ContextMenu* menu)
{
    populateContextMenuItems(*m_menu, *menu);
}

void CustomContextMenuProvider::contextMenuItemSelected(const ContextMenuItem* item)
{
    if (HTMLElement* element = menuItemAt(item->action())) {
        RefPtrWillBeRawPtr<SimulatedMouseEvent> click = SimulatedMouseEvent::create(EventTypeNames::click, m_menu->document().domWindow(), Event::create());
        click->setRelatedTarget(m_subjectElement.get());
        element->dispatchEvent(click.release());
    }
}

void CustomContextMenuProvider::contextMenuCleared()
{
    m_menuItems.clear();
    m_subjectElement = nullptr;
}

void CustomContextMenuProvider::appendSeparator(ContextMenu& contextMenu)
{
    // Avoid separators at the start of any menu and submenu.
    if (!contextMenu.items().size())
        return;

    // Collapse all sequences of two or more adjacent separators in the menu or
    // any submenus to a single separator.
    ContextMenuItem lastItem = contextMenu.items().last();
    if (lastItem.type() == SeparatorType)
        return;

    contextMenu.appendItem(ContextMenuItem(SeparatorType, ContextMenuItemCustomTagNoAction, String(), String()));
}

void CustomContextMenuProvider::appendMenuItem(HTMLMenuItemElement* menuItem, ContextMenu& contextMenu)
{
    // Avoid menuitems with no label.
    String labelString = menuItem->fastGetAttribute(labelAttr);
    if (labelString.isEmpty())
        return;

    m_menuItems.append(menuItem);

    bool enabled = !menuItem->fastHasAttribute(disabledAttr);
    String icon = menuItem->fastGetAttribute(iconAttr);
    if (!icon.isEmpty()) {
        // To obtain the absolute URL of the icon when the attribute's value is not the empty string,
        // the attribute's value must be resolved relative to the element.
        KURL iconURL = KURL(menuItem->baseURI(), icon);
        icon = iconURL.string();
    }
    ContextMenuAction action = static_cast<ContextMenuAction>(ContextMenuItemBaseCustomTag + m_menuItems.size() - 1);
    if (equalIgnoringCase(menuItem->fastGetAttribute(typeAttr), "checkbox") || equalIgnoringCase(menuItem->fastGetAttribute(typeAttr), "radio"))
        contextMenu.appendItem(ContextMenuItem(CheckableActionType, action, labelString, icon, enabled, menuItem->fastHasAttribute(checkedAttr)));
    else
        contextMenu.appendItem(ContextMenuItem(ActionType, action, labelString, icon, enabled, false));
}

void CustomContextMenuProvider::populateContextMenuItems(const HTMLMenuElement& menu, ContextMenu& contextMenu)
{
    HTMLElement* nextElement = Traversal<HTMLElement>::firstWithin(menu);
    while (nextElement) {
        if (isHTMLHRElement(*nextElement)) {
            appendSeparator(contextMenu);
            nextElement = Traversal<HTMLElement>::next(*nextElement, &menu);
        } else if (isHTMLMenuElement(*nextElement)) {
            ContextMenu subMenu;
            String labelString = nextElement->fastGetAttribute(labelAttr);
            if (labelString.isNull()) {
                appendSeparator(contextMenu);
                populateContextMenuItems(*toHTMLMenuElement(nextElement), contextMenu);
                appendSeparator(contextMenu);
            } else if (!labelString.isEmpty()) {
                populateContextMenuItems(*toHTMLMenuElement(nextElement), subMenu);
                contextMenu.appendItem(ContextMenuItem(SubmenuType, ContextMenuItemCustomTagNoAction, labelString, String(), &subMenu));
            }
            nextElement = Traversal<HTMLElement>::nextSibling(*nextElement);
        } else if (isHTMLMenuItemElement(*nextElement)) {
            appendMenuItem(toHTMLMenuItemElement(nextElement), contextMenu);
            if (ContextMenuItemBaseCustomTag + m_menuItems.size() >= ContextMenuItemLastCustomTag)
                break;
            nextElement = Traversal<HTMLElement>::next(*nextElement, &menu);
        } else {
            nextElement = Traversal<HTMLElement>::next(*nextElement, &menu);
        }
    }

    // Remove separators at the end of the menu and any submenus.
    while (contextMenu.items().size() && contextMenu.items().last().type() == SeparatorType)
        contextMenu.removeLastItem();
}

HTMLElement* CustomContextMenuProvider::menuItemAt(unsigned menuId)
{
    int itemIndex = menuId - ContextMenuItemBaseCustomTag;
    if (itemIndex < 0 || static_cast<unsigned long>(itemIndex) >= m_menuItems.size())
        return nullptr;
    return m_menuItems[itemIndex].get();
}

} // namespace blink
