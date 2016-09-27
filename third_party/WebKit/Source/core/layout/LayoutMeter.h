/*
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef LayoutMeter_h
#define LayoutMeter_h

#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class HTMLMeterElement;

class LayoutMeter final : public LayoutBlockFlow {
public:
    explicit LayoutMeter(HTMLElement*);
    virtual ~LayoutMeter();

    HTMLMeterElement* meterElement() const;
    virtual void updateFromElement() override;

    virtual const char* name() const override { return "LayoutMeter"; }

private:
    virtual void updateLogicalWidth() override;
    virtual void computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues&) const override;

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectMeter || LayoutBlockFlow::isOfType(type); }
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutMeter, isMeter());

} // namespace blink

#endif // LayoutMeter_h
