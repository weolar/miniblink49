/*
 * Copyright (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef LayoutCounter_h
#define LayoutCounter_h

#include "core/layout/LayoutText.h"
#include "core/style/CounterContent.h"

namespace blink {

class CounterNode;

class LayoutCounter final : public LayoutText {
public:
    LayoutCounter(Document*, const CounterContent&);
    virtual ~LayoutCounter();

    static void destroyCounterNodes(LayoutObject&);
    static void destroyCounterNode(LayoutObject&, const AtomicString& identifier);
    static void layoutObjectSubtreeAttached(LayoutObject*);
    static void layoutObjectSubtreeWillBeDetached(LayoutObject*);
    static void layoutObjectStyleChanged(LayoutObject&, const ComputedStyle* oldStyle, const ComputedStyle& newStyle);

    void updateCounter();

    virtual const char* name() const override { return "LayoutCounter"; }

protected:
    virtual void willBeDestroyed() override;

private:
    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectCounter || LayoutText::isOfType(type); }
    virtual PassRefPtr<StringImpl> originalText() const override;

    // Removes the reference to the CounterNode associated with this layoutObject.
    // This is used to cause a counter display update when the CounterNode tree changes.
    void invalidate();

    CounterContent m_counter;
    CounterNode* m_counterNode;
    LayoutCounter* m_nextForSameCounter;
    friend class CounterNode;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutCounter, isCounter());

} // namespace blink

#ifndef NDEBUG
// Outside the WebCore namespace for ease of invocation from gdb.
void showCounterLayoutTree(const blink::LayoutObject*, const char* counterName = nullptr);
#endif

#endif // LayoutCounter_h
