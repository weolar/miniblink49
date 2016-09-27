/*
 * Copyright (C) 2008 Alex Mathews <possessedpenguinbob@gmail.com>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "config.h"
#include "platform/graphics/filters/FETile.h"

#include "SkTileImageFilter.h"

#include "platform/graphics/filters/Filter.h"
#include "platform/graphics/filters/SkiaImageFilterBuilder.h"
#include "platform/text/TextStream.h"

namespace blink {

FETile::FETile(Filter* filter)
    : FilterEffect(filter)
{
}

PassRefPtrWillBeRawPtr<FETile> FETile::create(Filter* filter)
{
    return adoptRefWillBeNoop(new FETile(filter));
}

FloatRect FETile::mapPaintRect(const FloatRect& rect, bool forward)
{
    return forward ? maxEffectRect() : inputEffect(0)->maxEffectRect();
}

static FloatRect getRect(FilterEffect* effect)
{
    FloatRect result = effect->filter()->filterRegion();
    FloatRect boundaries = effect->effectBoundaries();
    if (effect->hasX())
        result.setX(boundaries.x());
    if (effect->hasY())
        result.setY(boundaries.y());
    if (effect->hasWidth())
        result.setWidth(boundaries.width());
    if (effect->hasHeight())
        result.setHeight(boundaries.height());
    return result;
}

PassRefPtr<SkImageFilter> FETile::createImageFilter(SkiaImageFilterBuilder* builder)
{
    RefPtr<SkImageFilter> input(builder->build(inputEffect(0), operatingColorSpace()));
    FloatRect srcRect = inputEffect(0) ? getRect(inputEffect(0)) : filter()->filterRegion();
    FloatRect dstRect = getRect(this);
    return adoptRef(SkTileImageFilter::Create(srcRect, dstRect, input.get()));
}

TextStream& FETile::externalRepresentation(TextStream& ts, int indent) const
{
    writeIndent(ts, indent);
    ts << "[feTile";
    FilterEffect::externalRepresentation(ts);
    ts << "]\n";
    inputEffect(0)->externalRepresentation(ts, indent + 1);

    return ts;
}

} // namespace blink
