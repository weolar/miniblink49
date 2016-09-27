/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc. All rights reserved.
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

#ifndef LayoutPart_h
#define LayoutPart_h

#include "core/CoreExport.h"
#include "core/layout/LayoutReplaced.h"
#include "platform/Widget.h"

namespace blink {

// LayoutObject for frames via LayoutFrame and LayoutIFrame, and plugins via LayoutEmbeddedObject.
class CORE_EXPORT LayoutPart : public LayoutReplaced {
public:
    explicit LayoutPart(Element*);
    virtual ~LayoutPart();

    bool requiresAcceleratedCompositing() const;

    virtual bool needsPreferredWidthsRecalculation() const override final;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction) override;

    void ref() { ++m_refCount; }
    void deref();

    Widget* widget() const;

    void updateOnWidgetChange();
    void updateWidgetPosition();
    void widgetPositionsUpdated();
    bool updateWidgetGeometry();

    virtual bool isLayoutPart() const override final { return true; }
    virtual void paintContents(const PaintInfo&, const LayoutPoint&);

protected:
    virtual DeprecatedPaintLayerType layerTypeRequired() const override;

    virtual void styleDidChange(StyleDifference, const ComputedStyle* oldStyle) override final;
    virtual void layout() override;
    virtual void paint(const PaintInfo&, const LayoutPoint&) override;
    virtual CursorDirective getCursor(const LayoutPoint&, Cursor&) const override final;

    // Overridden to invalidate the child frame if any.
    virtual void invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState&) override;

private:
    virtual CompositingReasons additionalCompositingReasons() const override;

    virtual void willBeDestroyed() override final;
    virtual void destroy() override final;

    bool setWidgetGeometry(const LayoutRect&);

    bool nodeAtPointOverWidget(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction);

    int m_refCount;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutPart, isLayoutPart());

} // namespace blink

#endif // LayoutPart_h
