/*
 * Copyright (C) 2005 Apple Computer
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

#ifndef LayoutButton_h
#define LayoutButton_h

#include "core/html/HTMLInputElement.h"
#include "core/layout/LayoutFlexibleBox.h"

namespace blink {

// LayoutButtons are just like normal flexboxes except that they will generate an anonymous block child.
// For inputs, they will also generate an anonymous LayoutText and keep its style and content up
// to date as the button changes.
class LayoutButton final : public LayoutFlexibleBox {
public:
    explicit LayoutButton(Element*);
    virtual ~LayoutButton();

    virtual const char* name() const override { return "LayoutButton"; }
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectLayoutButton || LayoutFlexibleBox::isOfType(type); }

    virtual bool canBeSelectionLeaf() const override { return node() && node()->hasEditableStyle(); }
    virtual bool canCollapseAnonymousBlockChild() const override { return true; }

    virtual void addChild(LayoutObject* newChild, LayoutObject *beforeChild = nullptr) override;
    virtual void removeChild(LayoutObject*) override;
    virtual void removeLeftoverAnonymousBlock(LayoutBlock*) override { }
    virtual bool createsAnonymousWrapper() const override { return true; }

    virtual bool hasControlClip() const override;
    virtual LayoutRect controlClipRect(const LayoutPoint&) const override;

    virtual int baselinePosition(FontBaseline, bool firstLine, LineDirectionMode, LinePositionMode) const override;

private:
    virtual void updateAnonymousChildStyle(const LayoutObject& child, ComputedStyle& childStyle) const override;

    virtual bool hasLineIfEmpty() const override { return isHTMLInputElement(node()); }

    LayoutBlock* m_inner;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutButton, isLayoutButton());

} // namespace blink

#endif // LayoutButton_h
