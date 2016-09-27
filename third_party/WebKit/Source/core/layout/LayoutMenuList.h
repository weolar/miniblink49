/*
 * This file is part of the select element layoutObject in WebCore.
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef LayoutMenuList_h
#define LayoutMenuList_h

#include "core/CoreExport.h"
#include "core/html/forms/PopupMenuClient.h"
#include "core/layout/LayoutFlexibleBox.h"
#include "platform/PopupMenu.h"
#include "platform/geometry/LayoutRect.h"

namespace blink {

class HTMLSelectElement;
class LayoutText;

class CORE_EXPORT LayoutMenuList final : public LayoutFlexibleBox, private PopupMenuClient {
public:
    explicit LayoutMenuList(Element*);
    virtual ~LayoutMenuList();

    bool popupIsVisible() const { return m_popupIsVisible; }
    void showPopup();
    void hidePopup();
    PopupMenu* popup() const { return m_popup.get(); }

    void setOptionsChanged(bool changed) { m_optionsChanged = changed; }

    void didSetSelectedIndex(int listIndex);

    String text() const;

    virtual PopupMenuStyle itemStyle(unsigned listIndex) const override;

    virtual const char* name() const override { return "LayoutMenuList"; }

protected:
    virtual void willBeDestroyed() override;

private:
    HTMLSelectElement* selectElement() const;

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectMenuList || LayoutFlexibleBox::isOfType(type); }
    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;

    virtual void addChild(LayoutObject* newChild, LayoutObject* beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject*) override;
    virtual bool createsAnonymousWrapper() const override { return true; }

    virtual void updateFromElement() override;

    virtual LayoutRect controlClipRect(const LayoutPoint&) const override;
    virtual bool hasControlClip() const override { return true; }

    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    // PopupMenuClient methods
    virtual void valueChanged(unsigned listIndex, bool fireOnChange = true) override;
    virtual void selectionChanged(unsigned, bool) override { }
    virtual void selectionCleared() override { }
    virtual String itemText(unsigned listIndex) const override;
    virtual String itemToolTip(unsigned listIndex) const override;
    virtual String itemAccessibilityText(unsigned listIndex) const override;
    virtual bool itemIsEnabled(unsigned listIndex) const override;
    virtual PopupMenuStyle menuStyle() const override;
    virtual LayoutUnit clientPaddingLeft() const override;
    virtual LayoutUnit clientPaddingRight() const override;
    virtual int listSize() const override;
    virtual int selectedIndex() const override;
    virtual void popupDidHide() override;
    virtual void popupDidCancel() override;
    virtual bool itemIsSeparator(unsigned listIndex) const override;
    virtual bool itemIsLabel(unsigned listIndex) const override;
    virtual bool itemIsSelected(unsigned listIndex) const override;
    virtual void provisionalSelectionChanged(unsigned) override;
    virtual void listBoxSelectItem(int listIndex, bool allowMultiplySelections, bool shift, bool fireOnChangeNow = true) override;
    virtual bool multiple() const override;
    virtual IntRect elementRectRelativeToViewport() const override;
    virtual Element& ownerElement() const override;
    virtual const ComputedStyle* computedStyleForItem(Element&) const override;

    virtual bool hasLineIfEmpty() const override { return true; }

    // Flexbox defines baselines differently than regular blocks.
    // For backwards compatibility, menulists need to do the regular block behavior.
    virtual int baselinePosition(FontBaseline baseline, bool firstLine, LineDirectionMode direction, LinePositionMode position) const override
    {
        return LayoutBlock::baselinePosition(baseline, firstLine, direction, position);
    }
    virtual int firstLineBoxBaseline() const override { return LayoutBlock::firstLineBoxBaseline(); }
    virtual int inlineBlockBaseline(LineDirectionMode direction) const override { return LayoutBlock::inlineBlockBaseline(direction); }

    void getItemBackgroundColor(unsigned listIndex, Color&, bool& itemHasCustomBackgroundColor) const;

    void createInnerBlock();
    void adjustInnerStyle();
    void setText(const String&);
    void setTextFromOption(int optionIndex);
    void updateOptionsWidth();
    void updateText();
    void setIndexToSelectOnCancel(int listIndex);

    void didUpdateActiveOption(int optionIndex);

    LayoutText* m_buttonText;
    LayoutBlock* m_innerBlock;

    bool m_optionsChanged : 1;
    bool m_isEmpty : 1;
    bool m_hasUpdatedActiveOption : 1;
    bool m_popupIsVisible : 1;
    int m_optionsWidth;

    int m_lastActiveIndex;

    RefPtr<ComputedStyle> m_optionStyle;

    RefPtrWillBePersistent<PopupMenu> m_popup;
    int m_indexToSelectOnCancel;

    // TODO(tkent): Use FRIEND_TEST macro provided by gtest_prod.h
    friend class LayoutMenuListTest_ElementRectRelativeToViewport_Test;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutMenuList, isMenuList());

}

#endif
