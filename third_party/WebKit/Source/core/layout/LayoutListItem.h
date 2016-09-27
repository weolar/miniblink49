/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2009 Apple Inc. All rights reserved.
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

#ifndef LayoutListItem_h
#define LayoutListItem_h

#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class HTMLOListElement;
class LayoutListMarker;

class LayoutListItem final : public LayoutBlockFlow {
public:
    explicit LayoutListItem(Element*);

    int value() const
    {
        if (!m_isValueUpToDate) {
            updateValueNow();
        }
        return m_value;
    }
    void updateValue();

    bool hasExplicitValue() const { return m_hasExplicitValue; }
    int explicitValue() const { return m_explicitValue; }
    void setExplicitValue(int);
    void clearExplicitValue();

    void setNotInList(bool);
    bool notInList() const { return m_notInList; }

    const String& markerText() const;

    void updateListMarkerNumbers();

    static void updateItemValuesForOrderedList(const HTMLOListElement*);
    static unsigned itemCountForOrderedList(const HTMLOListElement*);

    bool isEmpty() const;

    LayoutListMarker* marker() const { return m_marker; }

    virtual const char* name() const override { return "LayoutListItem"; }

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectListItem || LayoutBlockFlow::isOfType(type); }

    virtual void willBeDestroyed() override;

    virtual void insertedIntoTree() override;
    virtual void willBeRemovedFromTree() override;

    virtual void paint(const PaintInfo&, const LayoutPoint&) override;

    virtual void subtreeDidChange() final;

    // Returns true if we re-attached and updated the location of the marker.
    bool updateMarkerLocation();

    void positionListMarker();

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override;

    virtual void addOverflowFromChildren() override;

    inline int calcValue() const;
    void updateValueNow() const;
    void explicitValueChanged();

    int m_explicitValue;
    LayoutListMarker* m_marker;
    mutable int m_value;

    bool m_hasExplicitValue : 1;
    mutable bool m_isValueUpToDate : 1;
    bool m_notInList : 1;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutListItem, isListItem());

} // namespace blink

#endif // LayoutListItem_h
