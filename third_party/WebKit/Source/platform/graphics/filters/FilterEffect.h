/*
 * Copyright (C) 2008 Alex Mathews <possessedpenguinbob@gmail.com>
 * Copyright (C) 2009 Dirk Schulze <krit@webkit.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#ifndef FilterEffect_h
#define FilterEffect_h

#include "platform/PlatformExport.h"
#include "platform/geometry/FloatRect.h"
#include "platform/geometry/IntRect.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/ColorSpace.h"
#include "platform/heap/Handle.h"
#include "third_party/skia/include/core/SkImageFilter.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class Filter;
class FilterEffect;
class TextStream;

class SkiaImageFilterBuilder;

typedef WillBeHeapVector<RefPtrWillBeMember<FilterEffect>> FilterEffectVector;

enum FilterEffectType {
    FilterEffectTypeUnknown,
    FilterEffectTypeImage,
    FilterEffectTypeTile,
    FilterEffectTypeSourceInput
};

enum DetermineSubregionFlag {
    DetermineSubregionNone = 0,
    MapRectForward = 1,
    ClipToFilterRegion = 1 << 1
};

typedef int DetermineSubregionFlags;

class PLATFORM_EXPORT FilterEffect : public RefCountedWillBeGarbageCollectedFinalized<FilterEffect> {
public:
    virtual ~FilterEffect();
    DECLARE_VIRTUAL_TRACE();

    void clearResult();

    FilterEffectVector& inputEffects() { return m_inputEffects; }
    FilterEffect* inputEffect(unsigned) const;
    unsigned numberOfEffectInputs() const { return m_inputEffects.size(); }

    inline bool hasImageFilter() const
    {
        return m_imageFilters[0] || m_imageFilters[1] || m_imageFilters[2] || m_imageFilters[3];
    }

    IntRect absolutePaintRect() const { return m_absolutePaintRect; }

    FloatRect maxEffectRect() const { return m_maxEffectRect; }
    void setMaxEffectRect(const FloatRect& maxEffectRect) { m_maxEffectRect = maxEffectRect; }

    virtual PassRefPtr<SkImageFilter> createImageFilter(SkiaImageFilterBuilder*);
    virtual PassRefPtr<SkImageFilter> createImageFilterWithoutValidation(SkiaImageFilterBuilder*);

    // Mapping a rect forwards determines which which destination pixels a
    // given source rect would affect. Mapping a rect backwards determines
    // which pixels from the source rect would be required to fill a given
    // destination rect. Note that these are not necessarily the inverse of
    // each other. For example, for FEGaussianBlur, they are the same
    // transformation.
    virtual FloatRect mapRect(const FloatRect& rect, bool forward = true) { return rect; }
    // A version of the above that is used for calculating paint rects. We can't
    // use mapRect above for that, because that is also used for calculating effect
    // regions for CSS filters and has undesirable effects for tile and
    // displacement map.
    virtual FloatRect mapPaintRect(const FloatRect& rect, bool forward)
    {
        return mapRect(rect, forward);
    }
    FloatRect mapRectRecursive(const FloatRect&);

    virtual FilterEffectType filterEffectType() const { return FilterEffectTypeUnknown; }

    virtual TextStream& externalRepresentation(TextStream&, int indention = 0) const;

    // The following functions are SVG specific and will move to LayoutSVGResourceFilterPrimitive.
    // See bug https://bugs.webkit.org/show_bug.cgi?id=45614.
    bool hasX() const { return m_hasX; }
    void setHasX(bool value) { m_hasX = value; }

    bool hasY() const { return m_hasY; }
    void setHasY(bool value) { m_hasY = value; }

    bool hasWidth() const { return m_hasWidth; }
    void setHasWidth(bool value) { m_hasWidth = value; }

    bool hasHeight() const { return m_hasHeight; }
    void setHasHeight(bool value) { m_hasHeight = value; }

    FloatRect filterPrimitiveSubregion() const { return m_filterPrimitiveSubregion; }
    void setFilterPrimitiveSubregion(const FloatRect& filterPrimitiveSubregion) { m_filterPrimitiveSubregion = filterPrimitiveSubregion; }

    FloatRect effectBoundaries() const { return m_effectBoundaries; }
    void setEffectBoundaries(const FloatRect& effectBoundaries) { m_effectBoundaries = effectBoundaries; }

    Filter* filter() { return m_filter; }
    const Filter* filter() const { return m_filter; }

    bool clipsToBounds() const { return m_clipsToBounds; }
    void setClipsToBounds(bool value) { m_clipsToBounds = value; }

    ColorSpace operatingColorSpace() const { return m_operatingColorSpace; }
    virtual void setOperatingColorSpace(ColorSpace colorSpace) { m_operatingColorSpace = colorSpace; }

    FloatRect determineFilterPrimitiveSubregion(DetermineSubregionFlags = DetermineSubregionNone);

    virtual FloatRect determineAbsolutePaintRect(const FloatRect& requestedAbsoluteRect);
    virtual bool affectsTransparentPixels() { return false; }

    // Return false if the filter will only operate correctly on valid RGBA values, with
    // alpha in [0,255] and each color component in [0, alpha].
    virtual bool mayProduceInvalidPreMultipliedPixels() { return false; }

    SkImageFilter* getImageFilter(ColorSpace, bool requiresPMColorValidation) const;
    void setImageFilter(ColorSpace, bool requiresPMColorValidation, PassRefPtr<SkImageFilter>);

protected:
    FilterEffect(Filter*);

    Color adaptColorToOperatingColorSpace(const Color& deviceColor);

    SkImageFilter::CropRect getCropRect(const FloatSize& cropOffset) const;

    void addAbsolutePaintRect(const FloatRect& absolutePaintRect);

private:
    FilterEffectVector m_inputEffects;

    IntRect m_absolutePaintRect;

    // The maximum size of a filter primitive. In SVG this is the primitive subregion in absolute coordinate space.
    // The absolute paint rect should never be bigger than m_maxEffectRect.
    FloatRect m_maxEffectRect;
    RawPtrWillBeMember<Filter> m_filter;

    // The following member variables are SVG specific and will move to LayoutSVGResourceFilterPrimitive.
    // See bug https://bugs.webkit.org/show_bug.cgi?id=45614.

    // The subregion of a filter primitive according to the SVG Filter specification in local coordinates.
    // This is SVG specific and needs to move to LayoutSVGResourceFilterPrimitive.
    FloatRect m_filterPrimitiveSubregion;

    // x, y, width and height of the actual SVGFE*Element. Is needed to determine the subregion of the
    // filter primitive on a later step.
    FloatRect m_effectBoundaries;
    bool m_hasX;
    bool m_hasY;
    bool m_hasWidth;
    bool m_hasHeight;

    // Should the effect clip to its primitive region, or expand to use the combined region of its inputs.
    bool m_clipsToBounds;

    ColorSpace m_operatingColorSpace;

    RefPtr<SkImageFilter> m_imageFilters[4];
};

} // namespace blink

#endif // FilterEffect_h
