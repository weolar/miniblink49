/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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

#ifndef SVGDefsElement_h
#define SVGDefsElement_h

#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGGraphicsElement.h"

namespace blink {

class SVGDefsElement final : public SVGGraphicsElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGDefsElement);

    bool supportsFocus() const override { return false; }

private:
    explicit SVGDefsElement(Document&);

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
};

} // namespace blink

#endif // SVGDefsElement_h
