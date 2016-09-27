/*
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
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
#include "core/layout/svg/LayoutSVGResourceMasker.h"

#include "core/dom/ElementTraversal.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/paint/SVGPaintContext.h"
#include "core/svg/SVGElement.h"
#include "platform/graphics/paint/SkPictureBuilder.h"
#include "platform/transforms/AffineTransform.h"
#include "third_party/skia/include/core/SkPicture.h"

namespace blink {

LayoutSVGResourceMasker::LayoutSVGResourceMasker(SVGMaskElement* node)
    : LayoutSVGResourceContainer(node)
{
}

LayoutSVGResourceMasker::~LayoutSVGResourceMasker()
{
}

void LayoutSVGResourceMasker::removeAllClientsFromCache(bool markForInvalidation)
{
    m_maskContentPicture.clear();
    m_maskContentBoundaries = FloatRect();
    markAllClientsForInvalidation(markForInvalidation ? LayoutAndBoundariesInvalidation : ParentOnlyInvalidation);
}

void LayoutSVGResourceMasker::removeClientFromCache(LayoutObject* client, bool markForInvalidation)
{
    ASSERT(client);
    markClientForInvalidation(client, markForInvalidation ? BoundariesInvalidation : ParentOnlyInvalidation);
}

PassRefPtr<const SkPicture> LayoutSVGResourceMasker::createContentPicture(AffineTransform& contentTransformation, const FloatRect& targetBoundingBox,
    GraphicsContext* context)
{
    SVGUnitTypes::SVGUnitType contentUnits = toSVGMaskElement(element())->maskContentUnits()->currentValue()->enumValue();
    if (contentUnits == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        contentTransformation.translate(targetBoundingBox.x(), targetBoundingBox.y());
        contentTransformation.scaleNonUniform(targetBoundingBox.width(), targetBoundingBox.height());
    }

    if (m_maskContentPicture)
        return m_maskContentPicture;

    SubtreeContentTransformScope contentTransformScope(contentTransformation);

    // Using strokeBoundingBox (instead of paintInvalidationRectInLocalCoordinates) to avoid the intersection
    // with local clips/mask, which may yield incorrect results when mixing objectBoundingBox and
    // userSpaceOnUse units (http://crbug.com/294900).
    FloatRect bounds = strokeBoundingBox();

    SkPictureBuilder pictureBuilder(bounds, nullptr, context);

    ColorFilter maskContentFilter = style()->svgStyle().colorInterpolation() == CI_LINEARRGB
        ? ColorFilterSRGBToLinearRGB : ColorFilterNone;
    pictureBuilder.context().setColorFilter(maskContentFilter);

    for (SVGElement* childElement = Traversal<SVGElement>::firstChild(*element()); childElement; childElement = Traversal<SVGElement>::nextSibling(*childElement)) {
        LayoutObject* layoutObject = childElement->layoutObject();
        if (!layoutObject)
            continue;
        const ComputedStyle* style = layoutObject->style();
        if (!style || style->display() == NONE || style->visibility() != VISIBLE)
            continue;

        SVGPaintContext::paintSubtree(&pictureBuilder.context(), layoutObject);
    }

    m_maskContentPicture = pictureBuilder.endRecording();
    return m_maskContentPicture;
}

void LayoutSVGResourceMasker::calculateMaskContentPaintInvalidationRect()
{
    for (SVGElement* childElement = Traversal<SVGElement>::firstChild(*element()); childElement; childElement = Traversal<SVGElement>::nextSibling(*childElement)) {
        LayoutObject* layoutObject = childElement->layoutObject();
        if (!layoutObject)
            continue;
        const ComputedStyle* style = layoutObject->style();
        if (!style || style->display() == NONE || style->visibility() != VISIBLE)
            continue;
        m_maskContentBoundaries.unite(layoutObject->localToParentTransform().mapRect(layoutObject->paintInvalidationRectInLocalCoordinates()));
    }
}

FloatRect LayoutSVGResourceMasker::resourceBoundingBox(const LayoutObject* object)
{
    SVGMaskElement* maskElement = toSVGMaskElement(element());
    ASSERT(maskElement);

    FloatRect objectBoundingBox = object->objectBoundingBox();
    FloatRect maskBoundaries = SVGLengthContext::resolveRectangle<SVGMaskElement>(maskElement, maskElement->maskUnits()->currentValue()->enumValue(), objectBoundingBox);

    // Resource was not layouted yet. Give back clipping rect of the mask.
    if (selfNeedsLayout())
        return maskBoundaries;

    if (m_maskContentBoundaries.isEmpty())
        calculateMaskContentPaintInvalidationRect();

    FloatRect maskRect = m_maskContentBoundaries;
    if (maskElement->maskContentUnits()->currentValue()->value() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) {
        AffineTransform transform;
        transform.translate(objectBoundingBox.x(), objectBoundingBox.y());
        transform.scaleNonUniform(objectBoundingBox.width(), objectBoundingBox.height());
        maskRect = transform.mapRect(maskRect);
    }

    maskRect.intersect(maskBoundaries);
    return maskRect;
}

}
