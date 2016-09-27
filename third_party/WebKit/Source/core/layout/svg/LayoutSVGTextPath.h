/*
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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
 */

#ifndef LayoutSVGTextPath_h
#define LayoutSVGTextPath_h

#include "core/layout/svg/LayoutSVGInline.h"

namespace blink {

class LayoutSVGTextPath final : public LayoutSVGInline {
public:
    explicit LayoutSVGTextPath(Element*);

    Path layoutPath() const;
    float startOffset() const;

    virtual bool isChildAllowed(LayoutObject*, const ComputedStyle&) const override;

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectSVGTextPath || LayoutSVGInline::isOfType(type); }

    virtual const char* name() const override { return "LayoutSVGTextPath"; }

private:
    Path m_layoutPath;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutSVGTextPath, isSVGTextPath());

}

#endif // LayoutSVGTextPath_h
