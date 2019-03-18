/*
 * Copyright (C) 2004 Zack Rusin <zack@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
 * Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
 * Copyright (C) 2011 Sencha, Inc. All rights reserved.
 * Copyright (C) 2015 Google Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "config.h"
#include "core/css/ComputedStyleCSSValueMapping.h"

#include "core/StylePropertyShorthand.h"
#include "core/css/BasicShapeFunctions.h"
#include "core/css/CSSBorderImage.h"
#include "core/css/CSSBorderImageSliceValue.h"
#include "core/css/CSSFontFeatureValue.h"
#include "core/css/CSSFunctionValue.h"
#include "core/css/CSSGridLineNamesValue.h"
#include "core/css/CSSGridTemplateAreasValue.h"
#include "core/css/CSSPathValue.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "core/css/CSSReflectValue.h"
#include "core/css/CSSShadowValue.h"
#include "core/css/CSSTimingFunctionValue.h"
#include "core/css/CSSValueList.h"
#include "core/css/CSSValuePool.h"
#include "core/css/Counter.h"
#include "core/css/Pair.h"
#include "core/css/Rect.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutGrid.h"
#include "core/layout/LayoutObject.h"
#include "core/style/ContentData.h"
#include "core/style/ComputedStyle.h"
#include "core/style/PathStyleMotionPath.h"
#include "core/style/QuotesData.h"
#include "core/style/ShadowList.h"
#include "platform/LengthFunctions.h"

namespace blink {

inline static bool isFlexOrGrid(Node* element)
{
    return element && element->ensureComputedStyle()
        && element->ensureComputedStyle()->isDisplayFlexibleOrGridBox();
}

inline static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> zoomAdjustedPixelValue(double value, const ComputedStyle& style)
{
    return cssValuePool().createValue(adjustFloatForAbsoluteZoom(value, style), CSSPrimitiveValue::CSS_PX);
}

inline static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> zoomAdjustedNumberValue(double value, const ComputedStyle& style)
{
    return cssValuePool().createValue(value / style.effectiveZoom(), CSSPrimitiveValue::CSS_NUMBER);
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> zoomAdjustedPixelValueForLength(const Length& length, const ComputedStyle& style)
{
    if (length.isFixed())
        return zoomAdjustedPixelValue(length.value(), style);
    return cssValuePool().createValue(length, style);
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> pixelValueForUnzoomedLength(const UnzoomedLength& unzoomedLength, const ComputedStyle& style)
{
    const Length& length = unzoomedLength.length();
    if (length.isFixed())
        return cssValuePool().createValue(length.value(), CSSPrimitiveValue::CSS_PX);
    return cssValuePool().createValue(length, style);
}

static PassRefPtrWillBeRawPtr<CSSValueList> createPositionListForLayer(CSSPropertyID propertyID, const FillLayer& layer, const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSValueList> positionList = CSSValueList::createSpaceSeparated();
    if (layer.isBackgroundXOriginSet()) {
        ASSERT_UNUSED(propertyID, propertyID == CSSPropertyBackgroundPosition || propertyID == CSSPropertyWebkitMaskPosition);
        positionList->append(cssValuePool().createValue(layer.backgroundXOrigin()));
    }
    positionList->append(zoomAdjustedPixelValueForLength(layer.xPosition(), style));
    if (layer.isBackgroundYOriginSet()) {
        ASSERT(propertyID == CSSPropertyBackgroundPosition || propertyID == CSSPropertyWebkitMaskPosition);
        positionList->append(cssValuePool().createValue(layer.backgroundYOrigin()));
    }
    positionList->append(zoomAdjustedPixelValueForLength(layer.yPosition(), style));
    return positionList.release();
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> ComputedStyleCSSValueMapping::currentColorOrValidColor(const ComputedStyle& style, const StyleColor& color)
{
    // This function does NOT look at visited information, so that computed style doesn't expose that.
    return cssValuePool().createColorValue(color.resolve(style.color()).rgb());
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForFillSize(const FillSize& fillSize, const ComputedStyle& style)
{
    if (fillSize.type == Contain)
        return cssValuePool().createIdentifierValue(CSSValueContain);

    if (fillSize.type == Cover)
        return cssValuePool().createIdentifierValue(CSSValueCover);

    if (fillSize.size.height().isAuto())
        return zoomAdjustedPixelValueForLength(fillSize.size.width(), style);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(zoomAdjustedPixelValueForLength(fillSize.size.width(), style));
    list->append(zoomAdjustedPixelValueForLength(fillSize.size.height(), style));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForFillRepeat(EFillRepeat xRepeat, EFillRepeat yRepeat)
{
    // For backwards compatibility, if both values are equal, just return one of them. And
    // if the two values are equivalent to repeat-x or repeat-y, just return the shorthand.
    if (xRepeat == yRepeat)
        return cssValuePool().createValue(xRepeat);
    if (xRepeat == RepeatFill && yRepeat == NoRepeatFill)
        return cssValuePool().createIdentifierValue(CSSValueRepeatX);
    if (xRepeat == NoRepeatFill && yRepeat == RepeatFill)
        return cssValuePool().createIdentifierValue(CSSValueRepeatY);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(cssValuePool().createValue(xRepeat));
    list->append(cssValuePool().createValue(yRepeat));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForFillSourceType(EMaskSourceType type)
{
    switch (type) {
    case MaskAlpha:
        return cssValuePool().createValue(CSSValueAlpha);
    case MaskLuminance:
        return cssValuePool().createValue(CSSValueLuminance);
    }

    ASSERT_NOT_REACHED();

    return nullptr;
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForPositionOffset(const ComputedStyle& style, CSSPropertyID propertyID, const LayoutObject* layoutObject)
{
    Length offset;
    switch (propertyID) {
    case CSSPropertyLeft:
        offset = style.left();
        break;
    case CSSPropertyRight:
        offset = style.right();
        break;
    case CSSPropertyTop:
        offset = style.top();
        break;
    case CSSPropertyBottom:
        offset = style.bottom();
        break;
    default:
        return nullptr;
    }

    if (offset.hasPercent() && layoutObject && layoutObject->isBox() && layoutObject->isPositioned()) {
        LayoutUnit containingBlockSize = (propertyID == CSSPropertyLeft || propertyID == CSSPropertyRight) ?
            toLayoutBox(layoutObject)->containingBlockLogicalWidthForContent() :
            toLayoutBox(layoutObject)->containingBlockLogicalHeightForGetComputedStyle();
        return zoomAdjustedPixelValue(valueForLength(offset, containingBlockSize), style);
    }
    if (offset.isAuto()) {
        // FIXME: It's not enough to simply return "auto" values for one offset if the other side is defined.
        // In other words if left is auto and right is not auto, then left's computed value is negative right().
        // So we should get the opposite length unit and see if it is auto.
        return cssValuePool().createIdentifierValue(CSSValueAuto);
    }

    return zoomAdjustedPixelValueForLength(offset, style);
}

static PassRefPtrWillBeRawPtr<CSSBorderImageSliceValue> valueForNinePieceImageSlice(const NinePieceImage& image)
{
    // Create the slices.
    RefPtrWillBeRawPtr<CSSPrimitiveValue> top = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> right = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> bottom = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> left = nullptr;

    // TODO(alancutter): Make this code aware of calc lengths.
    if (image.imageSlices().top().hasPercent())
        top = cssValuePool().createValue(image.imageSlices().top().value(), CSSPrimitiveValue::CSS_PERCENTAGE);
    else
        top = cssValuePool().createValue(image.imageSlices().top().value(), CSSPrimitiveValue::CSS_NUMBER);

    if (image.imageSlices().right() == image.imageSlices().top() && image.imageSlices().bottom() == image.imageSlices().top()
        && image.imageSlices().left() == image.imageSlices().top()) {
        right = top;
        bottom = top;
        left = top;
    } else {
        if (image.imageSlices().right().hasPercent())
            right = cssValuePool().createValue(image.imageSlices().right().value(), CSSPrimitiveValue::CSS_PERCENTAGE);
        else
            right = cssValuePool().createValue(image.imageSlices().right().value(), CSSPrimitiveValue::CSS_NUMBER);

        if (image.imageSlices().bottom() == image.imageSlices().top() && image.imageSlices().right() == image.imageSlices().left()) {
            bottom = top;
            left = right;
        } else {
            if (image.imageSlices().bottom().hasPercent())
                bottom = cssValuePool().createValue(image.imageSlices().bottom().value(), CSSPrimitiveValue::CSS_PERCENTAGE);
            else
                bottom = cssValuePool().createValue(image.imageSlices().bottom().value(), CSSPrimitiveValue::CSS_NUMBER);

            if (image.imageSlices().left() == image.imageSlices().right()) {
                left = right;
            } else {
                if (image.imageSlices().left().hasPercent())
                    left = cssValuePool().createValue(image.imageSlices().left().value(), CSSPrimitiveValue::CSS_PERCENTAGE);
                else
                    left = cssValuePool().createValue(image.imageSlices().left().value(), CSSPrimitiveValue::CSS_NUMBER);
            }
        }
    }

    RefPtrWillBeRawPtr<Quad> quad = Quad::create();
    quad->setTop(top);
    quad->setRight(right);
    quad->setBottom(bottom);
    quad->setLeft(left);

    return CSSBorderImageSliceValue::create(cssValuePool().createValue(quad.release()), image.fill());
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForNinePieceImageQuad(const BorderImageLengthBox& box, const ComputedStyle& style)
{
    // Create the slices.
    RefPtrWillBeRawPtr<CSSPrimitiveValue> top = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> right = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> bottom = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> left = nullptr;

    if (box.top().isNumber())
        top = cssValuePool().createValue(box.top().number(), CSSPrimitiveValue::CSS_NUMBER);
    else
        top = cssValuePool().createValue(box.top().length(), style);

    if (box.right() == box.top() && box.bottom() == box.top() && box.left() == box.top()) {
        right = top;
        bottom = top;
        left = top;
    } else {
        if (box.right().isNumber())
            right = cssValuePool().createValue(box.right().number(), CSSPrimitiveValue::CSS_NUMBER);
        else
            right = cssValuePool().createValue(box.right().length(), style);

        if (box.bottom() == box.top() && box.right() == box.left()) {
            bottom = top;
            left = right;
        } else {
            if (box.bottom().isNumber())
                bottom = cssValuePool().createValue(box.bottom().number(), CSSPrimitiveValue::CSS_NUMBER);
            else
                bottom = cssValuePool().createValue(box.bottom().length(), style);

            if (box.left() == box.right()) {
                left = right;
            } else {
                if (box.left().isNumber())
                    left = cssValuePool().createValue(box.left().number(), CSSPrimitiveValue::CSS_NUMBER);
                else
                    left = cssValuePool().createValue(box.left().length(), style);
            }
        }
    }

    RefPtrWillBeRawPtr<Quad> quad = Quad::create();
    quad->setTop(top);
    quad->setRight(right);
    quad->setBottom(bottom);
    quad->setLeft(left);

    return cssValuePool().createValue(quad.release());
}

static CSSValueID valueForRepeatRule(int rule)
{
    switch (rule) {
    case RepeatImageRule:
        return CSSValueRepeat;
    case RoundImageRule:
        return CSSValueRound;
    case SpaceImageRule:
        return CSSValueSpace;
    default:
        return CSSValueStretch;
    }
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForNinePieceImageRepeat(const NinePieceImage& image)
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> horizontalRepeat = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> verticalRepeat = nullptr;

    horizontalRepeat = cssValuePool().createIdentifierValue(valueForRepeatRule(image.horizontalRule()));
    if (image.horizontalRule() == image.verticalRule())
        verticalRepeat = horizontalRepeat;
    else
        verticalRepeat = cssValuePool().createIdentifierValue(valueForRepeatRule(image.verticalRule()));
    return cssValuePool().createValue(Pair::create(horizontalRepeat.release(), verticalRepeat.release(), Pair::DropIdenticalValues));
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForNinePieceImage(const NinePieceImage& image, const ComputedStyle& style)
{
    if (!image.hasImage())
        return cssValuePool().createIdentifierValue(CSSValueNone);

    // Image first.
    RefPtrWillBeRawPtr<CSSValue> imageValue = nullptr;
    if (image.image())
        imageValue = image.image()->cssValue();

    // Create the image slice.
    RefPtrWillBeRawPtr<CSSBorderImageSliceValue> imageSlices = valueForNinePieceImageSlice(image);

    // Create the border area slices.
    RefPtrWillBeRawPtr<CSSValue> borderSlices = valueForNinePieceImageQuad(image.borderSlices(), style);

    // Create the border outset.
    RefPtrWillBeRawPtr<CSSValue> outset = valueForNinePieceImageQuad(image.outset(), style);

    // Create the repeat rules.
    RefPtrWillBeRawPtr<CSSValue> repeat = valueForNinePieceImageRepeat(image);

    return createBorderImageValue(imageValue.release(), imageSlices.release(), borderSlices.release(), outset.release(), repeat.release());
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForReflection(const StyleReflection* reflection, const ComputedStyle& style)
{
    if (!reflection)
        return cssValuePool().createIdentifierValue(CSSValueNone);

    RefPtrWillBeRawPtr<CSSPrimitiveValue> offset = nullptr;
    // TODO(alancutter): Make this work correctly for calc lengths.
    if (reflection->offset().hasPercent())
        offset = cssValuePool().createValue(reflection->offset().percent(), CSSPrimitiveValue::CSS_PERCENTAGE);
    else
        offset = zoomAdjustedPixelValue(reflection->offset().value(), style);

    RefPtrWillBeRawPtr<CSSPrimitiveValue> direction = nullptr;
    switch (reflection->direction()) {
    case ReflectionBelow:
        direction = cssValuePool().createIdentifierValue(CSSValueBelow);
        break;
    case ReflectionAbove:
        direction = cssValuePool().createIdentifierValue(CSSValueAbove);
        break;
    case ReflectionLeft:
        direction = cssValuePool().createIdentifierValue(CSSValueLeft);
        break;
    case ReflectionRight:
        direction = cssValuePool().createIdentifierValue(CSSValueRight);
        break;
    }

    return CSSReflectValue::create(direction.release(), offset.release(), valueForNinePieceImage(reflection->mask(), style));
}

static ItemPosition resolveAlignmentAuto(ItemPosition position, Node* element)
{
    if (position != ItemPositionAuto)
        return position;

    return isFlexOrGrid(element) ? ItemPositionStretch : ItemPositionStart;
}

static PassRefPtrWillBeRawPtr<CSSValueList> valueForItemPositionWithOverflowAlignment(ItemPosition itemPosition, OverflowAlignment overflowAlignment, ItemPositionType positionType)
{
    RefPtrWillBeRawPtr<CSSValueList> result = CSSValueList::createSpaceSeparated();
    if (positionType == LegacyPosition)
        result->append(CSSPrimitiveValue::createIdentifier(CSSValueLegacy));
    result->append(CSSPrimitiveValue::create(itemPosition));
    if (itemPosition >= ItemPositionCenter && overflowAlignment != OverflowAlignmentDefault)
        result->append(CSSPrimitiveValue::create(overflowAlignment));
    ASSERT(result->length() <= 2);
    return result.release();
}

static PassRefPtrWillBeRawPtr<CSSValueList> valuesForGridShorthand(const StylePropertyShorthand& shorthand, const ComputedStyle& style, const LayoutObject* layoutObject, Node* styledNode, bool allowVisitedStyle)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSlashSeparated();
    for (size_t i = 0; i < shorthand.length(); ++i) {
        RefPtrWillBeRawPtr<CSSValue> value = ComputedStyleCSSValueMapping::get(shorthand.properties()[i], style, layoutObject, styledNode, allowVisitedStyle);
        ASSERT(value);
        list->append(value.release());
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValueList> valuesForShorthandProperty(const StylePropertyShorthand& shorthand, const ComputedStyle& style, const LayoutObject* layoutObject, Node* styledNode, bool allowVisitedStyle)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    for (size_t i = 0; i < shorthand.length(); ++i) {
        RefPtrWillBeRawPtr<CSSValue> value = ComputedStyleCSSValueMapping::get(shorthand.properties()[i], style, layoutObject, styledNode, allowVisitedStyle);
        ASSERT(value);
        list->append(value);
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValueList> valuesForBackgroundShorthand(const ComputedStyle& style, const LayoutObject* layoutObject, Node* styledNode, bool allowVisitedStyle)
{
    RefPtrWillBeRawPtr<CSSValueList> ret = CSSValueList::createCommaSeparated();
    const FillLayer* currLayer = &style.backgroundLayers();
    for (; currLayer; currLayer = currLayer->next()) {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSlashSeparated();
        RefPtrWillBeRawPtr<CSSValueList> beforeSlash = CSSValueList::createSpaceSeparated();
        if (!currLayer->next()) { // color only for final layer
            RefPtrWillBeRawPtr<CSSValue> value = ComputedStyleCSSValueMapping::get(CSSPropertyBackgroundColor, style, layoutObject, styledNode, allowVisitedStyle);
            ASSERT(value);
            beforeSlash->append(value);
        }
        beforeSlash->append(currLayer->image() ? currLayer->image()->cssValue() : cssValuePool().createIdentifierValue(CSSValueNone));
        beforeSlash->append(valueForFillRepeat(currLayer->repeatX(), currLayer->repeatY()));
        beforeSlash->append(cssValuePool().createValue(currLayer->attachment()));
        beforeSlash->append(createPositionListForLayer(CSSPropertyBackgroundPosition, *currLayer, style));
        list->append(beforeSlash);
        RefPtrWillBeRawPtr<CSSValueList> afterSlash = CSSValueList::createSpaceSeparated();
        afterSlash->append(valueForFillSize(currLayer->size(), style));
        afterSlash->append(cssValuePool().createValue(currLayer->origin()));
        afterSlash->append(cssValuePool().createValue(currLayer->clip()));
        list->append(afterSlash);
        ret->append(list);
    }
    return ret.release();
}

static ContentPosition resolveContentAlignmentAuto(ContentPosition position, ContentDistributionType distribution, Node* element)
{
    if (position != ContentPositionAuto || distribution != ContentDistributionDefault)
        return position;

    bool isFlex = element && element->ensureComputedStyle()
        && element->ensureComputedStyle()->isDisplayFlexibleBox();

    return isFlex ? ContentPositionFlexStart : ContentPositionStart;
}

static PassRefPtrWillBeRawPtr<CSSValueList> valueForContentPositionAndDistributionWithOverflowAlignment(ContentPosition position, OverflowAlignment overflowAlignment, ContentDistributionType distribution)
{
    RefPtrWillBeRawPtr<CSSValueList> result = CSSValueList::createSpaceSeparated();
    if (distribution != ContentDistributionDefault)
        result->append(CSSPrimitiveValue::create(distribution));
    if (distribution == ContentDistributionDefault || position != ContentPositionAuto)
        result->append(CSSPrimitiveValue::create(position));
    if ((position >= ContentPositionCenter || distribution != ContentDistributionDefault) && overflowAlignment != OverflowAlignmentDefault)
        result->append(CSSPrimitiveValue::create(overflowAlignment));
    ASSERT(result->length() > 0);
    ASSERT(result->length() <= 3);
    return result.release();
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForLineHeight(const ComputedStyle& style)
{
    Length length = style.lineHeight();
    if (length.isNegative())
        return cssValuePool().createIdentifierValue(CSSValueNormal);

    return zoomAdjustedPixelValue(floatValueForLength(length, style.fontDescription().specifiedSize()), style);
}

static CSSValueID identifierForFamily(const AtomicString& family)
{
    if (family == FontFamilyNames::webkit_cursive)
        return CSSValueCursive;
    if (family == FontFamilyNames::webkit_fantasy)
        return CSSValueFantasy;
    if (family == FontFamilyNames::webkit_monospace)
        return CSSValueMonospace;
    if (family == FontFamilyNames::webkit_pictograph)
        return CSSValueWebkitPictograph;
    if (family == FontFamilyNames::webkit_sans_serif)
        return CSSValueSansSerif;
    if (family == FontFamilyNames::webkit_serif)
        return CSSValueSerif;
    return CSSValueInvalid;
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForFamily(const AtomicString& family)
{
    if (CSSValueID familyIdentifier = identifierForFamily(family))
        return cssValuePool().createIdentifierValue(familyIdentifier);
    return cssValuePool().createValue(family.string(), CSSPrimitiveValue::CSS_CUSTOM_IDENT);
}

static PassRefPtrWillBeRawPtr<CSSValueList> valueForFontFamily(const ComputedStyle& style)
{
    const FontFamily& firstFamily = style.fontDescription().family();
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    for (const FontFamily* family = &firstFamily; family; family = family->next())
        list->append(valueForFamily(family->family()));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForFontSize(const ComputedStyle& style)
{
    return zoomAdjustedPixelValue(style.fontDescription().computedSize(), style);
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForFontStretch(const ComputedStyle& style)
{
    return cssValuePool().createValue(style.fontDescription().stretch());
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForFontStyle(const ComputedStyle& style)
{
    return cssValuePool().createValue(style.fontDescription().style());
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForFontVariant(const ComputedStyle& style)
{
    return cssValuePool().createValue(style.fontDescription().variant());
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueForFontWeight(const ComputedStyle& style)
{
    return cssValuePool().createValue(style.fontDescription().weight());
}

static PassRefPtrWillBeRawPtr<CSSValue> specifiedValueForGridTrackBreadth(const GridLength& trackBreadth, const ComputedStyle& style)
{
    if (!trackBreadth.isLength())
        return cssValuePool().createValue(trackBreadth.flex(), CSSPrimitiveValue::CSS_FR);

    const Length& trackBreadthLength = trackBreadth.length();
    if (trackBreadthLength.isAuto())
        return cssValuePool().createIdentifierValue(CSSValueAuto);
    return zoomAdjustedPixelValueForLength(trackBreadthLength, style);
}

static PassRefPtrWillBeRawPtr<CSSValue> specifiedValueForGridTrackSize(const GridTrackSize& trackSize, const ComputedStyle& style)
{
    switch (trackSize.type()) {
    case LengthTrackSizing:
        return specifiedValueForGridTrackBreadth(trackSize.length(), style);
    case MinMaxTrackSizing:
        RefPtrWillBeRawPtr<CSSFunctionValue> minMaxTrackBreadths = CSSFunctionValue::create(CSSValueMinmax);
        minMaxTrackBreadths->append(specifiedValueForGridTrackBreadth(trackSize.minTrackBreadth(), style));
        minMaxTrackBreadths->append(specifiedValueForGridTrackBreadth(trackSize.maxTrackBreadth(), style));
        return minMaxTrackBreadths.release();
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

static void addValuesForNamedGridLinesAtIndex(const OrderedNamedGridLines& orderedNamedGridLines, size_t i, CSSValueList& list)
{
    const Vector<String>& namedGridLines = orderedNamedGridLines.get(i);
    if (namedGridLines.isEmpty())
        return;

    RefPtrWillBeRawPtr<CSSGridLineNamesValue> lineNames = CSSGridLineNamesValue::create();
    for (size_t j = 0; j < namedGridLines.size(); ++j)
        lineNames->append(cssValuePool().createValue(namedGridLines[j], CSSPrimitiveValue::CSS_CUSTOM_IDENT));
    list.append(lineNames.release());
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForGridTrackList(GridTrackSizingDirection direction, const LayoutObject* layoutObject, const ComputedStyle& style)
{
    const Vector<GridTrackSize>& trackSizes = direction == ForColumns ? style.gridTemplateColumns() : style.gridTemplateRows();
    const OrderedNamedGridLines& orderedNamedGridLines = direction == ForColumns ? style.orderedNamedGridColumnLines() : style.orderedNamedGridRowLines();
    bool isLayoutGrid = layoutObject && layoutObject->isLayoutGrid();

    // Handle the 'none' case.
    bool trackListIsEmpty = trackSizes.isEmpty();
    if (isLayoutGrid && trackListIsEmpty) {
        // For grids we should consider every listed track, whether implicitly or explicitly created. If we don't have
        // any explicit track and there are no children then there are no implicit tracks. We cannot simply check the
        // number of rows/columns in our internal grid representation because it's always at least 1x1 (see r143331).
        trackListIsEmpty = !toLayoutBlock(layoutObject)->firstChild();
    }

    if (trackListIsEmpty) {
        ASSERT(orderedNamedGridLines.isEmpty());
        return cssValuePool().createIdentifierValue(CSSValueNone);
    }

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    size_t insertionIndex;
    if (isLayoutGrid) {
        const Vector<LayoutUnit>& trackPositions = direction == ForColumns ? toLayoutGrid(layoutObject)->columnPositions() : toLayoutGrid(layoutObject)->rowPositions();
        // There are at least #tracks + 1 grid lines (trackPositions). Apart from that, the grid container can generate implicit grid tracks,
        // so we'll have more trackPositions than trackSizes as the latter only contain the explicit grid.
        ASSERT(trackPositions.size() - 1 >= trackSizes.size());

        for (size_t i = 0; i < trackPositions.size() - 1; ++i) {
            addValuesForNamedGridLinesAtIndex(orderedNamedGridLines, i, *list);
            list->append(zoomAdjustedPixelValue(trackPositions[i + 1] - trackPositions[i], style));
        }
        insertionIndex = trackPositions.size() - 1;
    } else {
        for (size_t i = 0; i < trackSizes.size(); ++i) {
            addValuesForNamedGridLinesAtIndex(orderedNamedGridLines, i, *list);
            list->append(specifiedValueForGridTrackSize(trackSizes[i], style));
        }
        insertionIndex = trackSizes.size();
    }
    // Those are the trailing <string>* allowed in the syntax.
    addValuesForNamedGridLinesAtIndex(orderedNamedGridLines, insertionIndex, *list);
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForGridPosition(const GridPosition& position)
{
    if (position.isAuto())
        return cssValuePool().createIdentifierValue(CSSValueAuto);

    if (position.isNamedGridArea())
        return cssValuePool().createValue(position.namedGridLine(), CSSPrimitiveValue::CSS_CUSTOM_IDENT);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    if (position.isSpan()) {
        list->append(cssValuePool().createIdentifierValue(CSSValueSpan));
        list->append(cssValuePool().createValue(position.spanPosition(), CSSPrimitiveValue::CSS_NUMBER));
    } else {
        list->append(cssValuePool().createValue(position.integerPosition(), CSSPrimitiveValue::CSS_NUMBER));
    }

    if (!position.namedGridLine().isNull())
        list->append(cssValuePool().createValue(position.namedGridLine(), CSSPrimitiveValue::CSS_CUSTOM_IDENT));
    return list;
}

static LayoutRect sizingBox(const LayoutObject* layoutObject)
{
    if (!layoutObject->isBox())
        return LayoutRect();

    const LayoutBox* box = toLayoutBox(layoutObject);
    return box->style()->boxSizing() == BORDER_BOX ? box->borderBoxRect() : box->computedCSSContentBoxRect();
}

static PassRefPtrWillBeRawPtr<CSSValue> scrollBlocksOnFlagsToCSSValue(WebScrollBlocksOn scrollBlocksOn)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();

    if (scrollBlocksOn == WebScrollBlocksOnNone)
        return cssValuePool().createIdentifierValue(CSSValueNone);

    if (scrollBlocksOn & WebScrollBlocksOnStartTouch)
        list->append(cssValuePool().createIdentifierValue(CSSValueStartTouch));
    if (scrollBlocksOn & WebScrollBlocksOnWheelEvent)
        list->append(cssValuePool().createIdentifierValue(CSSValueWheelEvent));
    if (scrollBlocksOn & WebScrollBlocksOnScrollEvent)
        list->append(cssValuePool().createIdentifierValue(CSSValueScrollEvent));
    ASSERT(list->length());
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> renderTextDecorationFlagsToCSSValue(int textDecoration)
{
    // Blink value is ignored.
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    if (textDecoration & TextDecorationUnderline)
        list->append(cssValuePool().createIdentifierValue(CSSValueUnderline));
    if (textDecoration & TextDecorationOverline)
        list->append(cssValuePool().createIdentifierValue(CSSValueOverline));
    if (textDecoration & TextDecorationLineThrough)
        list->append(cssValuePool().createIdentifierValue(CSSValueLineThrough));

    if (!list->length())
        return cssValuePool().createIdentifierValue(CSSValueNone);
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForTextDecorationStyle(TextDecorationStyle textDecorationStyle)
{
    switch (textDecorationStyle) {
    case TextDecorationStyleSolid:
        return cssValuePool().createIdentifierValue(CSSValueSolid);
    case TextDecorationStyleDouble:
        return cssValuePool().createIdentifierValue(CSSValueDouble);
    case TextDecorationStyleDotted:
        return cssValuePool().createIdentifierValue(CSSValueDotted);
    case TextDecorationStyleDashed:
        return cssValuePool().createIdentifierValue(CSSValueDashed);
    case TextDecorationStyleWavy:
        return cssValuePool().createIdentifierValue(CSSValueWavy);
    }

    ASSERT_NOT_REACHED();
    return cssValuePool().createExplicitInitialValue();
}

static PassRefPtrWillBeRawPtr<CSSValue> touchActionFlagsToCSSValue(TouchAction touchAction)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    if (touchAction == TouchActionAuto)
        list->append(cssValuePool().createIdentifierValue(CSSValueAuto));
    if (touchAction & TouchActionNone) {
        ASSERT(touchAction == TouchActionNone);
        list->append(cssValuePool().createIdentifierValue(CSSValueNone));
    }
    if (touchAction == (TouchActionPanX | TouchActionPanY | TouchActionPinchZoom)) {
        list->append(cssValuePool().createIdentifierValue(CSSValueManipulation));
    } else {
        if ((touchAction & TouchActionPanX) == TouchActionPanX)
            list->append(cssValuePool().createIdentifierValue(CSSValuePanX));
        else if (touchAction & TouchActionPanLeft)
            list->append(cssValuePool().createIdentifierValue(CSSValuePanLeft));
        else if (touchAction & TouchActionPanRight)
            list->append(cssValuePool().createIdentifierValue(CSSValuePanRight));

        if ((touchAction & TouchActionPanY) == TouchActionPanY)
            list->append(cssValuePool().createIdentifierValue(CSSValuePanY));
        else if (touchAction & TouchActionPanUp)
            list->append(cssValuePool().createIdentifierValue(CSSValuePanUp));
        else if (touchAction & TouchActionPanDown)
            list->append(cssValuePool().createIdentifierValue(CSSValuePanDown));
    }
    ASSERT(list->length());
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForWillChange(const Vector<CSSPropertyID>& willChangeProperties, bool willChangeContents, bool willChangeScrollPosition)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    if (willChangeContents)
        list->append(cssValuePool().createIdentifierValue(CSSValueContents));
    if (willChangeScrollPosition)
        list->append(cssValuePool().createIdentifierValue(CSSValueScrollPosition));
    for (size_t i = 0; i < willChangeProperties.size(); ++i)
        list->append(cssValuePool().createIdentifierValue(willChangeProperties[i]));
    if (!list->length())
        list->append(cssValuePool().createIdentifierValue(CSSValueAuto));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationDelay(const CSSTimingData* timingData)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    if (timingData) {
        for (size_t i = 0; i < timingData->delayList().size(); ++i)
            list->append(cssValuePool().createValue(timingData->delayList()[i], CSSPrimitiveValue::CSS_S));
    } else {
        list->append(cssValuePool().createValue(CSSTimingData::initialDelay(), CSSPrimitiveValue::CSS_S));
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationDirection(Timing::PlaybackDirection direction)
{
    switch (direction) {
    case Timing::PlaybackDirectionNormal:
        return cssValuePool().createIdentifierValue(CSSValueNormal);
    case Timing::PlaybackDirectionAlternate:
        return cssValuePool().createIdentifierValue(CSSValueAlternate);
    case Timing::PlaybackDirectionReverse:
        return cssValuePool().createIdentifierValue(CSSValueReverse);
    case Timing::PlaybackDirectionAlternateReverse:
        return cssValuePool().createIdentifierValue(CSSValueAlternateReverse);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationDuration(const CSSTimingData* timingData)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    if (timingData) {
        for (size_t i = 0; i < timingData->durationList().size(); ++i)
            list->append(cssValuePool().createValue(timingData->durationList()[i], CSSPrimitiveValue::CSS_S));
    } else {
        list->append(cssValuePool().createValue(CSSTimingData::initialDuration(), CSSPrimitiveValue::CSS_S));
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationFillMode(Timing::FillMode fillMode)
{
    switch (fillMode) {
    case Timing::FillModeNone:
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case Timing::FillModeForwards:
        return cssValuePool().createIdentifierValue(CSSValueForwards);
    case Timing::FillModeBackwards:
        return cssValuePool().createIdentifierValue(CSSValueBackwards);
    case Timing::FillModeBoth:
        return cssValuePool().createIdentifierValue(CSSValueBoth);
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationIterationCount(double iterationCount)
{
    if (iterationCount == std::numeric_limits<double>::infinity())
        return cssValuePool().createIdentifierValue(CSSValueInfinite);
    return cssValuePool().createValue(iterationCount, CSSPrimitiveValue::CSS_NUMBER);
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationPlayState(EAnimPlayState playState)
{
    if (playState == AnimPlayStatePlaying)
        return cssValuePool().createIdentifierValue(CSSValueRunning);
    ASSERT(playState == AnimPlayStatePaused);
    return cssValuePool().createIdentifierValue(CSSValuePaused);
}

static PassRefPtrWillBeRawPtr<CSSValue> createTimingFunctionValue(const TimingFunction* timingFunction)
{
    switch (timingFunction->type()) {
    case TimingFunction::CubicBezierFunction:
        {
            const CubicBezierTimingFunction* bezierTimingFunction = toCubicBezierTimingFunction(timingFunction);
            if (bezierTimingFunction->subType() != CubicBezierTimingFunction::Custom) {
                CSSValueID valueId = CSSValueInvalid;
                switch (bezierTimingFunction->subType()) {
                case CubicBezierTimingFunction::Ease:
                    valueId = CSSValueEase;
                    break;
                case CubicBezierTimingFunction::EaseIn:
                    valueId = CSSValueEaseIn;
                    break;
                case CubicBezierTimingFunction::EaseOut:
                    valueId = CSSValueEaseOut;
                    break;
                case CubicBezierTimingFunction::EaseInOut:
                    valueId = CSSValueEaseInOut;
                    break;
                default:
                    ASSERT_NOT_REACHED();
                    return nullptr;
                }
                return cssValuePool().createIdentifierValue(valueId);
            }
            return CSSCubicBezierTimingFunctionValue::create(bezierTimingFunction->x1(), bezierTimingFunction->y1(), bezierTimingFunction->x2(), bezierTimingFunction->y2());
        }

    case TimingFunction::StepsFunction:
        {
            const StepsTimingFunction* stepsTimingFunction = toStepsTimingFunction(timingFunction);
            StepsTimingFunction::StepAtPosition position = stepsTimingFunction->stepAtPosition();
            int steps = stepsTimingFunction->numberOfSteps();
            ASSERT(position == StepsTimingFunction::Start || position == StepsTimingFunction::End);

            if (steps > 1)
                return CSSStepsTimingFunctionValue::create(steps, position);
            CSSValueID valueId = position == StepsTimingFunction::Start ? CSSValueStepStart : CSSValueStepEnd;
            return cssValuePool().createIdentifierValue(valueId);
        }

    default:
        return cssValuePool().createIdentifierValue(CSSValueLinear);
    }
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForAnimationTimingFunction(const CSSTimingData* timingData)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    if (timingData) {
        for (size_t i = 0; i < timingData->timingFunctionList().size(); ++i)
            list->append(createTimingFunctionValue(timingData->timingFunctionList()[i].get()));
    } else {
        list->append(createTimingFunctionValue(CSSTimingData::initialTimingFunction().get()));
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValueList> valuesForBorderRadiusCorner(LengthSize radius, const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    if (radius.width().type() == Percent)
        list->append(cssValuePool().createValue(radius.width().percent(), CSSPrimitiveValue::CSS_PERCENTAGE));
    else
        list->append(zoomAdjustedPixelValueForLength(radius.width(), style));
    if (radius.height().type() == Percent)
        list->append(cssValuePool().createValue(radius.height().percent(), CSSPrimitiveValue::CSS_PERCENTAGE));
    else
        list->append(zoomAdjustedPixelValueForLength(radius.height(), style));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForBorderRadiusCorner(LengthSize radius, const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSValueList> list = valuesForBorderRadiusCorner(radius, style);
    if (list->item(0)->equals(*list->item(1)))
        return list->item(0);
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSFunctionValue> valueForMatrixTransform(const TransformationMatrix& transform, const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSFunctionValue> transformValue = nullptr;
    if (transform.isAffine()) {
        transformValue = CSSFunctionValue::create(CSSValueMatrix);

        transformValue->append(cssValuePool().createValue(transform.a(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.b(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.c(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.d(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(zoomAdjustedNumberValue(transform.e(), style));
        transformValue->append(zoomAdjustedNumberValue(transform.f(), style));
    } else {
        transformValue = CSSFunctionValue::create(CSSValueMatrix3d);

        transformValue->append(cssValuePool().createValue(transform.m11(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m12(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m13(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m14(), CSSPrimitiveValue::CSS_NUMBER));

        transformValue->append(cssValuePool().createValue(transform.m21(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m22(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m23(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m24(), CSSPrimitiveValue::CSS_NUMBER));

        transformValue->append(cssValuePool().createValue(transform.m31(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m32(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m33(), CSSPrimitiveValue::CSS_NUMBER));
        transformValue->append(cssValuePool().createValue(transform.m34(), CSSPrimitiveValue::CSS_NUMBER));

        transformValue->append(zoomAdjustedNumberValue(transform.m41(), style));
        transformValue->append(zoomAdjustedNumberValue(transform.m42(), style));
        transformValue->append(zoomAdjustedNumberValue(transform.m43(), style));
        transformValue->append(cssValuePool().createValue(transform.m44(), CSSPrimitiveValue::CSS_NUMBER));
    }

    return transformValue.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> computedTransform(const LayoutObject* layoutObject, const ComputedStyle& style)
{
    if (!layoutObject || !layoutObject->hasTransformRelatedProperty() || !style.hasTransform())
        return cssValuePool().createIdentifierValue(CSSValueNone);

    IntRect box;
    if (layoutObject->isBox())
        box = pixelSnappedIntRect(toLayoutBox(layoutObject)->borderBoxRect());

    TransformationMatrix transform;
    style.applyTransform(transform, LayoutSize(box.size()), ComputedStyle::ExcludeTransformOrigin, ComputedStyle::ExcludeMotionPath, ComputedStyle::ExcludeIndependentTransformProperties);

    // FIXME: Need to print out individual functions (https://bugs.webkit.org/show_bug.cgi?id=23924)
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(valueForMatrixTransform(transform, style));

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> createTransitionPropertyValue(const CSSTransitionData::TransitionProperty& property)
{
    if (property.propertyType == CSSTransitionData::TransitionNone)
        return cssValuePool().createIdentifierValue(CSSValueNone);
    if (property.propertyType == CSSTransitionData::TransitionAll)
        return cssValuePool().createIdentifierValue(CSSValueAll);
    if (property.propertyType == CSSTransitionData::TransitionUnknown)
        return cssValuePool().createValue(property.propertyString, CSSPrimitiveValue::CSS_CUSTOM_IDENT);
    ASSERT(property.propertyType == CSSTransitionData::TransitionSingleProperty);
    return cssValuePool().createValue(getPropertyNameString(property.unresolvedProperty), CSSPrimitiveValue::CSS_CUSTOM_IDENT);
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForTransitionProperty(const CSSTransitionData* transitionData)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    if (transitionData) {
        for (size_t i = 0; i < transitionData->propertyList().size(); ++i)
            list->append(createTransitionPropertyValue(transitionData->propertyList()[i]));
    } else {
        list->append(cssValuePool().createIdentifierValue(CSSValueAll));
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> createLineBoxContainValue(unsigned lineBoxContain)
{
    if (!lineBoxContain)
        return cssValuePool().createIdentifierValue(CSSValueNone);
    return CSSLineBoxContainValue::create(lineBoxContain);
}

CSSValueID valueForQuoteType(const QuoteType quoteType)
{
    switch (quoteType) {
    case NO_OPEN_QUOTE:
        return CSSValueNoOpenQuote;
    case NO_CLOSE_QUOTE:
        return CSSValueNoCloseQuote;
    case CLOSE_QUOTE:
        return CSSValueCloseQuote;
    case OPEN_QUOTE:
        return CSSValueOpenQuote;
    }
    ASSERT_NOT_REACHED();
    return CSSValueInvalid;
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForContentData(const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    for (const ContentData* contentData = style.contentData(); contentData; contentData = contentData->next()) {
        if (contentData->isCounter()) {
            const CounterContent* counter = toCounterContentData(contentData)->counter();
            ASSERT(counter);
            RefPtrWillBeRawPtr<CSSPrimitiveValue> identifier = cssValuePool().createValue(counter->identifier(), CSSPrimitiveValue::CSS_CUSTOM_IDENT);
            RefPtrWillBeRawPtr<CSSPrimitiveValue> separator = cssValuePool().createValue(counter->separator(), CSSPrimitiveValue::CSS_CUSTOM_IDENT);
            CSSValueID listStyleIdent = CSSValueNone;
            if (counter->listStyle() != NoneListStyle)
                listStyleIdent = static_cast<CSSValueID>(CSSValueDisc + counter->listStyle());
            RefPtrWillBeRawPtr<CSSPrimitiveValue> listStyle = cssValuePool().createIdentifierValue(listStyleIdent);
            list->append(cssValuePool().createValue(Counter::create(identifier.release(), listStyle.release(), separator.release())));
        } else if (contentData->isImage()) {
            const StyleImage* image = toImageContentData(contentData)->image();
            ASSERT(image);
            list->append(image->cssValue());
        } else if (contentData->isText()) {
            list->append(cssValuePool().createValue(toTextContentData(contentData)->text(), CSSPrimitiveValue::CSS_STRING));
        } else if (contentData->isQuote()) {
            const QuoteType quoteType = toQuoteContentData(contentData)->quote();
            list->append(cssValuePool().createIdentifierValue(valueForQuoteType(quoteType)));
        } else {
            ASSERT_NOT_REACHED();
        }
    }
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForCounterDirectives(const ComputedStyle& style, CSSPropertyID propertyID)
{
    const CounterDirectiveMap* map = style.counterDirectives();
    if (!map)
        return cssValuePool().createIdentifierValue(CSSValueNone);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    for (const auto& item : *map) {
        bool isValidCounterValue = propertyID == CSSPropertyCounterIncrement ? item.value.isIncrement() : item.value.isReset();
        if (!isValidCounterValue)
            continue;

        list->append(cssValuePool().createValue(item.key, CSSPrimitiveValue::CSS_CUSTOM_IDENT));
        short number = propertyID == CSSPropertyCounterIncrement ? item.value.incrementValue() : item.value.resetValue();
        list->append(cssValuePool().createValue((double)number, CSSPrimitiveValue::CSS_NUMBER));
    }

    if (!list->length())
        return cssValuePool().createIdentifierValue(CSSValueNone);

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForShape(const ComputedStyle& style, ShapeValue* shapeValue)
{
    if (!shapeValue)
        return cssValuePool().createIdentifierValue(CSSValueNone);
    if (shapeValue->type() == ShapeValue::Box)
        return cssValuePool().createValue(shapeValue->cssBox());
    if (shapeValue->type() == ShapeValue::Image) {
        if (shapeValue->image())
            return shapeValue->image()->cssValue();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    }

    ASSERT(shapeValue->type() == ShapeValue::Shape);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(valueForBasicShape(style, shapeValue->shape()));
    if (shapeValue->cssBox() != BoxMissing)
        list->append(cssValuePool().createValue(shapeValue->cssBox()));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValueList> valuesForSidesShorthand(const StylePropertyShorthand& shorthand, const ComputedStyle& style, const LayoutObject* layoutObject, Node* styledNode, bool allowVisitedStyle)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    // Assume the properties are in the usual order top, right, bottom, left.
    RefPtrWillBeRawPtr<CSSValue> topValue = ComputedStyleCSSValueMapping::get(shorthand.properties()[0], style, layoutObject, styledNode, allowVisitedStyle);
    RefPtrWillBeRawPtr<CSSValue> rightValue = ComputedStyleCSSValueMapping::get(shorthand.properties()[1], style, layoutObject, styledNode, allowVisitedStyle);
    RefPtrWillBeRawPtr<CSSValue> bottomValue = ComputedStyleCSSValueMapping::get(shorthand.properties()[2], style, layoutObject, styledNode, allowVisitedStyle);
    RefPtrWillBeRawPtr<CSSValue> leftValue = ComputedStyleCSSValueMapping::get(shorthand.properties()[3], style, layoutObject, styledNode, allowVisitedStyle);

    // All 4 properties must be specified.
    if (!topValue || !rightValue || !bottomValue || !leftValue)
        return nullptr;

    bool showLeft = !compareCSSValuePtr(rightValue, leftValue);
    bool showBottom = !compareCSSValuePtr(topValue, bottomValue) || showLeft;
    bool showRight = !compareCSSValuePtr(topValue, rightValue) || showBottom;

    list->append(topValue.release());
    if (showRight)
        list->append(rightValue.release());
    if (showBottom)
        list->append(bottomValue.release());
    if (showLeft)
        list->append(leftValue.release());

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValueList> valueForBorderRadiusShorthand(const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSlashSeparated();

    bool showHorizontalBottomLeft = style.borderTopRightRadius().width() != style.borderBottomLeftRadius().width();
    bool showHorizontalBottomRight = showHorizontalBottomLeft || (style.borderBottomRightRadius().width() != style.borderTopLeftRadius().width());
    bool showHorizontalTopRight = showHorizontalBottomRight || (style.borderTopRightRadius().width() != style.borderTopLeftRadius().width());

    bool showVerticalBottomLeft = style.borderTopRightRadius().height() != style.borderBottomLeftRadius().height();
    bool showVerticalBottomRight = showVerticalBottomLeft || (style.borderBottomRightRadius().height() != style.borderTopLeftRadius().height());
    bool showVerticalTopRight = showVerticalBottomRight || (style.borderTopRightRadius().height() != style.borderTopLeftRadius().height());

    RefPtrWillBeRawPtr<CSSValueList> topLeftRadius = valuesForBorderRadiusCorner(style.borderTopLeftRadius(), style);
    RefPtrWillBeRawPtr<CSSValueList> topRightRadius = valuesForBorderRadiusCorner(style.borderTopRightRadius(), style);
    RefPtrWillBeRawPtr<CSSValueList> bottomRightRadius = valuesForBorderRadiusCorner(style.borderBottomRightRadius(), style);
    RefPtrWillBeRawPtr<CSSValueList> bottomLeftRadius = valuesForBorderRadiusCorner(style.borderBottomLeftRadius(), style);

    RefPtrWillBeRawPtr<CSSValueList> horizontalRadii = CSSValueList::createSpaceSeparated();
    horizontalRadii->append(topLeftRadius->item(0));
    if (showHorizontalTopRight)
        horizontalRadii->append(topRightRadius->item(0));
    if (showHorizontalBottomRight)
        horizontalRadii->append(bottomRightRadius->item(0));
    if (showHorizontalBottomLeft)
        horizontalRadii->append(bottomLeftRadius->item(0));

    list->append(horizontalRadii.release());

    RefPtrWillBeRawPtr<CSSValueList> verticalRadii = CSSValueList::createSpaceSeparated();
    verticalRadii->append(topLeftRadius->item(1));
    if (showVerticalTopRight)
        verticalRadii->append(topRightRadius->item(1));
    if (showVerticalBottomRight)
        verticalRadii->append(bottomRightRadius->item(1));
    if (showVerticalBottomLeft)
        verticalRadii->append(bottomLeftRadius->item(1));

    if (!verticalRadii->equals(*toCSSValueList(list->item(0))))
        list->append(verticalRadii.release());

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> glyphOrientationToCSSPrimitiveValue(EGlyphOrientation orientation)
{
    switch (orientation) {
    case GO_0DEG:
        return CSSPrimitiveValue::create(0.0f, CSSPrimitiveValue::CSS_DEG);
    case GO_90DEG:
        return CSSPrimitiveValue::create(90.0f, CSSPrimitiveValue::CSS_DEG);
    case GO_180DEG:
        return CSSPrimitiveValue::create(180.0f, CSSPrimitiveValue::CSS_DEG);
    case GO_270DEG:
        return CSSPrimitiveValue::create(270.0f, CSSPrimitiveValue::CSS_DEG);
    default:
        return nullptr;
    }
}

static PassRefPtrWillBeRawPtr<CSSValue> strokeDashArrayToCSSValueList(const SVGDashArray& dashes, const ComputedStyle& style)
{
    if (dashes.isEmpty())
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    for (const Length& dashLength : dashes.vector())
        list->append(zoomAdjustedPixelValueForLength(dashLength, style));

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> paintOrderToCSSValueList(EPaintOrder paintorder)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    do {
        EPaintOrderType paintOrderType = (EPaintOrderType)(paintorder & ((1 << kPaintOrderBitwidth) - 1));
        switch (paintOrderType) {
        case PT_FILL:
        case PT_STROKE:
        case PT_MARKERS:
            list->append(CSSPrimitiveValue::create(paintOrderType));
            break;
        case PT_NONE:
        default:
            ASSERT_NOT_REACHED();
            break;
        }
    } while (paintorder >>= kPaintOrderBitwidth);

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> adjustSVGPaintForCurrentColor(SVGPaintType paintType, const String& url, const Color& color, const Color& currentColor)
{
    if (paintType >= SVG_PAINTTYPE_URI_NONE) {
        RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createSpaceSeparated();
        values->append(CSSPrimitiveValue::create(url, CSSPrimitiveValue::CSS_URI));
        if (paintType == SVG_PAINTTYPE_URI_NONE)
            values->append(CSSPrimitiveValue::create(CSSValueNone));
        else if (paintType == SVG_PAINTTYPE_URI_CURRENTCOLOR)
            values->append(CSSPrimitiveValue::createColor(currentColor.rgb()));
        else if (paintType == SVG_PAINTTYPE_URI_RGBCOLOR)
            values->append(CSSPrimitiveValue::createColor(color.rgb()));
        return values.release();
    }
    if (paintType == SVG_PAINTTYPE_NONE)
        return CSSPrimitiveValue::create(CSSValueNone);
    if (paintType == SVG_PAINTTYPE_CURRENTCOLOR)
        return CSSPrimitiveValue::createColor(currentColor.rgb());

    return CSSPrimitiveValue::createColor(color.rgb());
}

static inline String serializeAsFragmentIdentifier(const AtomicString& resource)
{
    return "#" + resource;
}

PassRefPtrWillBeRawPtr<CSSValue> ComputedStyleCSSValueMapping::valueForShadowData(const ShadowData& shadow, const ComputedStyle& style, bool useSpread)
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> x = zoomAdjustedPixelValue(shadow.x(), style);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> y = zoomAdjustedPixelValue(shadow.y(), style);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> blur = zoomAdjustedPixelValue(shadow.blur(), style);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> spread = useSpread ? zoomAdjustedPixelValue(shadow.spread(), style) : PassRefPtrWillBeRawPtr<CSSPrimitiveValue>(nullptr);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> shadowStyle = shadow.style() == Normal ? PassRefPtrWillBeRawPtr<CSSPrimitiveValue>(nullptr) : cssValuePool().createIdentifierValue(CSSValueInset);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> color = currentColorOrValidColor(style, shadow.color());
    return CSSShadowValue::create(x.release(), y.release(), blur.release(), spread.release(), shadowStyle.release(), color.release());
}

PassRefPtrWillBeRawPtr<CSSValue> ComputedStyleCSSValueMapping::valueForShadowList(const ShadowList* shadowList, const ComputedStyle& style, bool useSpread)
{
    if (!shadowList)
        return cssValuePool().createIdentifierValue(CSSValueNone);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    size_t shadowCount = shadowList->shadows().size();
    for (size_t i = 0; i < shadowCount; ++i)
        list->append(valueForShadowData(shadowList->shadows()[i], style, useSpread));
    return list.release();
}

PassRefPtrWillBeRawPtr<CSSValue> ComputedStyleCSSValueMapping::valueForFilter(const ComputedStyle& style)
{
    if (style.filter().operations().isEmpty())
        return cssValuePool().createIdentifierValue(CSSValueNone);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();

    RefPtrWillBeRawPtr<CSSFunctionValue> filterValue = nullptr;

    for (const auto& operation : style.filter().operations()) {
        FilterOperation* filterOperation = operation.get();
        switch (filterOperation->type()) {
        case FilterOperation::REFERENCE:
            filterValue = CSSFunctionValue::create(CSSValueUrl);
            filterValue->append(cssValuePool().createValue(toReferenceFilterOperation(filterOperation)->url(), CSSPrimitiveValue::CSS_CUSTOM_IDENT));
            break;
        case FilterOperation::GRAYSCALE:
            filterValue = CSSFunctionValue::create(CSSValueGrayscale);
            filterValue->append(cssValuePool().createValue(toBasicColorMatrixFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::SEPIA:
            filterValue = CSSFunctionValue::create(CSSValueSepia);
            filterValue->append(cssValuePool().createValue(toBasicColorMatrixFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::SATURATE:
            filterValue = CSSFunctionValue::create(CSSValueSaturate);
            filterValue->append(cssValuePool().createValue(toBasicColorMatrixFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::HUE_ROTATE:
            filterValue = CSSFunctionValue::create(CSSValueHueRotate);
            filterValue->append(cssValuePool().createValue(toBasicColorMatrixFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_DEG));
            break;
        case FilterOperation::INVERT:
            filterValue = CSSFunctionValue::create(CSSValueInvert);
            filterValue->append(cssValuePool().createValue(toBasicComponentTransferFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::OPACITY:
            filterValue = CSSFunctionValue::create(CSSValueOpacity);
            filterValue->append(cssValuePool().createValue(toBasicComponentTransferFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::BRIGHTNESS:
            filterValue = CSSFunctionValue::create(CSSValueBrightness);
            filterValue->append(cssValuePool().createValue(toBasicComponentTransferFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::CONTRAST:
            filterValue = CSSFunctionValue::create(CSSValueContrast);
            filterValue->append(cssValuePool().createValue(toBasicComponentTransferFilterOperation(filterOperation)->amount(), CSSPrimitiveValue::CSS_NUMBER));
            break;
        case FilterOperation::BLUR:
            filterValue = CSSFunctionValue::create(CSSValueBlur);
            filterValue->append(zoomAdjustedPixelValue(toBlurFilterOperation(filterOperation)->stdDeviation().value(), style));
            break;
        case FilterOperation::DROP_SHADOW: {
            DropShadowFilterOperation* dropShadowOperation = toDropShadowFilterOperation(filterOperation);
            filterValue = CSSFunctionValue::create(CSSValueDropShadow);
            // We want our computed style to look like that of a text shadow (has neither spread nor inset style).
            ShadowData shadow(dropShadowOperation->location(), dropShadowOperation->stdDeviation(), 0, Normal, StyleColor(dropShadowOperation->color()));
            filterValue->append(valueForShadowData(shadow, style, false));
            break;
        }
        default:
            ASSERT_NOT_REACHED();
            break;
        }
        list->append(filterValue.release());
    }

    return list.release();
}

PassRefPtrWillBeRawPtr<CSSValue> ComputedStyleCSSValueMapping::valueForFont(const ComputedStyle& style)
{
    // Add a slash between size and line-height.
    RefPtrWillBeRawPtr<CSSValueList> sizeAndLineHeight = CSSValueList::createSlashSeparated();
    sizeAndLineHeight->append(valueForFontSize(style));
    sizeAndLineHeight->append(valueForLineHeight(style));

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(valueForFontStyle(style));
    list->append(valueForFontVariant(style));
    list->append(valueForFontWeight(style));
    list->append(valueForFontStretch(style));
    list->append(sizeAndLineHeight.release());
    list->append(valueForFontFamily(style));

    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForScrollSnapDestination(const LengthPoint& destination, const ComputedStyle& style)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(zoomAdjustedPixelValueForLength(destination.x(), style));
    list->append(zoomAdjustedPixelValueForLength(destination.y(), style));
    return list.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForScrollSnapPoints(const ScrollSnapPoints& points, const ComputedStyle& style)
{
    if (points.hasRepeat) {
        RefPtrWillBeRawPtr<CSSFunctionValue> repeat = CSSFunctionValue::create(CSSValueRepeat);
        repeat->append(zoomAdjustedPixelValueForLength(points.repeatOffset, style));
        return repeat.release();
    }

    return cssValuePool().createIdentifierValue(CSSValueNone);
}

static PassRefPtrWillBeRawPtr<CSSValue> valueForScrollSnapCoordinate(const Vector<LengthPoint>& coordinates, const ComputedStyle& style)
{
    if (coordinates.isEmpty())
        return cssValuePool().createIdentifierValue(CSSValueNone);

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();

    for (auto& coordinate : coordinates) {
        auto pair = CSSValueList::createSpaceSeparated();
        pair->append(zoomAdjustedPixelValueForLength(coordinate.x(), style));
        pair->append(zoomAdjustedPixelValueForLength(coordinate.y(), style));
        list->append(pair);
    }

    return list.release();
}

PassRefPtrWillBeRawPtr<CSSValue> ComputedStyleCSSValueMapping::get(CSSPropertyID propertyID, const ComputedStyle& style, const LayoutObject* layoutObject, Node* styledNode, bool allowVisitedStyle)
{
    const SVGComputedStyle& svgStyle = style.svgStyle();
    propertyID = CSSProperty::resolveDirectionAwareProperty(propertyID, style.direction(), style.writingMode());
    switch (propertyID) {
    case CSSPropertyInvalid:
        return nullptr;

    case CSSPropertyBackgroundColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyBackgroundColor).rgb()) : currentColorOrValidColor(style, style.backgroundColor());
    case CSSPropertyBackgroundImage:
    case CSSPropertyWebkitMaskImage: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskImage ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next()) {
            if (currLayer->image())
                list->append(currLayer->image()->cssValue());
            else
                list->append(cssValuePool().createIdentifierValue(CSSValueNone));
        }
        return list.release();
    }
    case CSSPropertyBackgroundSize:
    case CSSPropertyWebkitMaskSize: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskSize ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next())
            list->append(valueForFillSize(currLayer->size(), style));
        return list.release();
    }
    case CSSPropertyBackgroundRepeat:
    case CSSPropertyWebkitMaskRepeat: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskRepeat ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next())
            list->append(valueForFillRepeat(currLayer->repeatX(), currLayer->repeatY()));
        return list.release();
    }
    case CSSPropertyMaskSourceType: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        for (const FillLayer* currLayer = &style.maskLayers(); currLayer; currLayer = currLayer->next())
            list->append(valueForFillSourceType(currLayer->maskSourceType()));
        return list.release();
    }
    case CSSPropertyWebkitBackgroundComposite:
    case CSSPropertyWebkitMaskComposite: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskComposite ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next())
            list->append(cssValuePool().createValue(currLayer->composite()));
        return list.release();
    }
    case CSSPropertyBackgroundAttachment: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        for (const FillLayer* currLayer = &style.backgroundLayers(); currLayer; currLayer = currLayer->next())
            list->append(cssValuePool().createValue(currLayer->attachment()));
        return list.release();
    }
    case CSSPropertyBackgroundClip:
    case CSSPropertyBackgroundOrigin:
    case CSSPropertyWebkitBackgroundClip:
    case CSSPropertyWebkitBackgroundOrigin:
    case CSSPropertyWebkitMaskClip:
    case CSSPropertyWebkitMaskOrigin: {
        bool isClip = propertyID == CSSPropertyBackgroundClip || propertyID == CSSPropertyWebkitBackgroundClip || propertyID == CSSPropertyWebkitMaskClip;
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = (propertyID == CSSPropertyWebkitMaskClip || propertyID == CSSPropertyWebkitMaskOrigin) ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next()) {
            EFillBox box = isClip ? currLayer->clip() : currLayer->origin();
            list->append(cssValuePool().createValue(box));
        }
        return list.release();
    }
    case CSSPropertyBackgroundPosition:
    case CSSPropertyWebkitMaskPosition: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskPosition ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next())
            list->append(createPositionListForLayer(propertyID, *currLayer, style));
        return list.release();
    }
    case CSSPropertyBackgroundPositionX:
    case CSSPropertyWebkitMaskPositionX: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskPositionX ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next())
            list->append(zoomAdjustedPixelValueForLength(currLayer->xPosition(), style));
        return list.release();
    }
    case CSSPropertyBackgroundPositionY:
    case CSSPropertyWebkitMaskPositionY: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const FillLayer* currLayer = propertyID == CSSPropertyWebkitMaskPositionY ? &style.maskLayers() : &style.backgroundLayers();
        for (; currLayer; currLayer = currLayer->next())
            list->append(zoomAdjustedPixelValueForLength(currLayer->yPosition(), style));
        return list.release();
    }
    case CSSPropertyBorderCollapse:
        if (style.borderCollapse())
            return cssValuePool().createIdentifierValue(CSSValueCollapse);
        return cssValuePool().createIdentifierValue(CSSValueSeparate);
    case CSSPropertyBorderSpacing: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        list->append(zoomAdjustedPixelValue(style.horizontalBorderSpacing(), style));
        list->append(zoomAdjustedPixelValue(style.verticalBorderSpacing(), style));
        return list.release();
    }
    case CSSPropertyWebkitBorderHorizontalSpacing:
        return zoomAdjustedPixelValue(style.horizontalBorderSpacing(), style);
    case CSSPropertyWebkitBorderVerticalSpacing:
        return zoomAdjustedPixelValue(style.verticalBorderSpacing(), style);
    case CSSPropertyBorderImageSource:
        if (style.borderImageSource())
            return style.borderImageSource()->cssValue();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case CSSPropertyBorderTopColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyBorderTopColor).rgb()) : currentColorOrValidColor(style, style.borderTopColor());
    case CSSPropertyBorderRightColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyBorderRightColor).rgb()) : currentColorOrValidColor(style, style.borderRightColor());
    case CSSPropertyBorderBottomColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyBorderBottomColor).rgb()) : currentColorOrValidColor(style, style.borderBottomColor());
    case CSSPropertyBorderLeftColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyBorderLeftColor).rgb()) : currentColorOrValidColor(style, style.borderLeftColor());
    case CSSPropertyBorderTopStyle:
        return cssValuePool().createValue(style.borderTopStyle());
    case CSSPropertyBorderRightStyle:
        return cssValuePool().createValue(style.borderRightStyle());
    case CSSPropertyBorderBottomStyle:
        return cssValuePool().createValue(style.borderBottomStyle());
    case CSSPropertyBorderLeftStyle:
        return cssValuePool().createValue(style.borderLeftStyle());
    case CSSPropertyBorderTopWidth:
        return zoomAdjustedPixelValue(style.borderTopWidth(), style);
    case CSSPropertyBorderRightWidth:
        return zoomAdjustedPixelValue(style.borderRightWidth(), style);
    case CSSPropertyBorderBottomWidth:
        return zoomAdjustedPixelValue(style.borderBottomWidth(), style);
    case CSSPropertyBorderLeftWidth:
        return zoomAdjustedPixelValue(style.borderLeftWidth(), style);
    case CSSPropertyBottom:
        return valueForPositionOffset(style, CSSPropertyBottom, layoutObject);
    case CSSPropertyWebkitBoxAlign:
        return cssValuePool().createValue(style.boxAlign());
    case CSSPropertyWebkitBoxDecorationBreak:
        if (style.boxDecorationBreak() == DSLICE)
            return cssValuePool().createIdentifierValue(CSSValueSlice);
    return cssValuePool().createIdentifierValue(CSSValueClone);
    case CSSPropertyWebkitBoxDirection:
        return cssValuePool().createValue(style.boxDirection());
    case CSSPropertyWebkitBoxFlex:
        return cssValuePool().createValue(style.boxFlex(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyWebkitBoxFlexGroup:
        return cssValuePool().createValue(style.boxFlexGroup(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyWebkitBoxLines:
        return cssValuePool().createValue(style.boxLines());
    case CSSPropertyWebkitBoxOrdinalGroup:
        return cssValuePool().createValue(style.boxOrdinalGroup(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyWebkitBoxOrient:
        return cssValuePool().createValue(style.boxOrient());
    case CSSPropertyWebkitBoxPack:
        return cssValuePool().createValue(style.boxPack());
    case CSSPropertyWebkitBoxReflect:
        return valueForReflection(style.boxReflect(), style);
    case CSSPropertyBoxShadow:
        return valueForShadowList(style.boxShadow(), style, true);
    case CSSPropertyCaptionSide:
        return cssValuePool().createValue(style.captionSide());
    case CSSPropertyClear:
        return cssValuePool().createValue(style.clear());
    case CSSPropertyColor:
        return cssValuePool().createColorValue(allowVisitedStyle ? style.visitedDependentColor(CSSPropertyColor).rgb() : style.color().rgb());
    case CSSPropertyWebkitPrintColorAdjust:
        return cssValuePool().createValue(style.printColorAdjust());
    case CSSPropertyWebkitColumnCount:
        if (style.hasAutoColumnCount())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return cssValuePool().createValue(style.columnCount(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyColumnFill:
        ASSERT(RuntimeEnabledFeatures::columnFillEnabled());
        return cssValuePool().createValue(style.columnFill());
    case CSSPropertyWebkitColumnGap:
        if (style.hasNormalColumnGap())
            return cssValuePool().createIdentifierValue(CSSValueNormal);
        return zoomAdjustedPixelValue(style.columnGap(), style);
    case CSSPropertyWebkitColumnRuleColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyOutlineColor).rgb()) : currentColorOrValidColor(style, style.columnRuleColor());
    case CSSPropertyWebkitColumnRuleStyle:
        return cssValuePool().createValue(style.columnRuleStyle());
    case CSSPropertyWebkitColumnRuleWidth:
        return zoomAdjustedPixelValue(style.columnRuleWidth(), style);
    case CSSPropertyWebkitColumnSpan:
        return cssValuePool().createIdentifierValue(style.columnSpan() ? CSSValueAll : CSSValueNone);
    case CSSPropertyWebkitColumnBreakAfter:
        return cssValuePool().createValue(style.columnBreakAfter());
    case CSSPropertyWebkitColumnBreakBefore:
        return cssValuePool().createValue(style.columnBreakBefore());
    case CSSPropertyWebkitColumnBreakInside:
        return cssValuePool().createValue(style.columnBreakInside());
    case CSSPropertyWebkitColumnWidth:
        if (style.hasAutoColumnWidth())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return zoomAdjustedPixelValue(style.columnWidth(), style);
    case CSSPropertyTabSize:
        return cssValuePool().createValue(
            style.tabSize().getPixelSize(1.0), style.tabSize().isSpaces() ? CSSPrimitiveValue::CSS_NUMBER : CSSPrimitiveValue::CSS_PX);
    case CSSPropertyCursor: {
        RefPtrWillBeRawPtr<CSSValueList> list = nullptr;
        CursorList* cursors = style.cursors();
        if (cursors && cursors->size() > 0) {
            list = CSSValueList::createCommaSeparated();
            for (unsigned i = 0; i < cursors->size(); ++i) {
                if (StyleImage* image = cursors->at(i).image())
                    list->append(image->cssValue());
            }
        }
        RefPtrWillBeRawPtr<CSSValue> value = cssValuePool().createValue(style.cursor());
        if (list) {
            list->append(value.release());
            return list.release();
        }
        return value.release();
    }
    case CSSPropertyDirection:
        return cssValuePool().createValue(style.direction());
    case CSSPropertyDisplay:
        return cssValuePool().createValue(style.display());
    case CSSPropertyEmptyCells:
        return cssValuePool().createValue(style.emptyCells());
    case CSSPropertyAlignContent:
        return valueForContentPositionAndDistributionWithOverflowAlignment(resolveContentAlignmentAuto(style.alignContentPosition(), style.alignContentDistribution(), styledNode), style.alignContentOverflowAlignment(), style.alignContentDistribution());
    case CSSPropertyAlignItems:
        return valueForItemPositionWithOverflowAlignment(resolveAlignmentAuto(style.alignItemsPosition(), styledNode), style.alignItemsOverflowAlignment(), NonLegacyPosition);
    case CSSPropertyAlignSelf:
        return valueForItemPositionWithOverflowAlignment(resolveAlignmentAuto(style.alignSelfPosition(), styledNode->parentNode()), style.alignSelfOverflowAlignment(), NonLegacyPosition);
    case CSSPropertyFlex:
        return valuesForShorthandProperty(flexShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyFlexBasis:
        return zoomAdjustedPixelValueForLength(style.flexBasis(), style);
    case CSSPropertyFlexDirection:
        return cssValuePool().createValue(style.flexDirection());
    case CSSPropertyFlexFlow:
        return valuesForShorthandProperty(flexFlowShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyFlexGrow:
        return cssValuePool().createValue(style.flexGrow());
    case CSSPropertyFlexShrink:
        return cssValuePool().createValue(style.flexShrink());
    case CSSPropertyFlexWrap:
        return cssValuePool().createValue(style.flexWrap());
    case CSSPropertyJustifyContent:
        return valueForContentPositionAndDistributionWithOverflowAlignment(resolveContentAlignmentAuto(style.justifyContentPosition(), style.justifyContentDistribution(), styledNode), style.justifyContentOverflowAlignment(), style.justifyContentDistribution());
    case CSSPropertyOrder:
        return cssValuePool().createValue(style.order(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyFloat:
        if (style.display() != NONE && style.hasOutOfFlowPosition())
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return cssValuePool().createValue(style.floating());
    case CSSPropertyFont:
        return valueForFont(style);
    case CSSPropertyFontFamily:
        return valueForFontFamily(style);
    case CSSPropertyFontSize:
        return valueForFontSize(style);
    case CSSPropertyFontSizeAdjust:
        if (style.hasFontSizeAdjust())
            return cssValuePool().createValue(style.fontSizeAdjust(), CSSPrimitiveValue::CSS_NUMBER);
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case CSSPropertyFontStretch:
        return valueForFontStretch(style);
    case CSSPropertyFontStyle:
        return valueForFontStyle(style);
    case CSSPropertyFontVariant:
        return valueForFontVariant(style);
    case CSSPropertyFontWeight:
        return valueForFontWeight(style);
    case CSSPropertyWebkitFontFeatureSettings: {
        const FontFeatureSettings* featureSettings = style.fontDescription().featureSettings();
        if (!featureSettings || !featureSettings->size())
            return cssValuePool().createIdentifierValue(CSSValueNormal);
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        for (unsigned i = 0; i < featureSettings->size(); ++i) {
            const FontFeature& feature = featureSettings->at(i);
            RefPtrWillBeRawPtr<CSSFontFeatureValue> featureValue = CSSFontFeatureValue::create(feature.tag(), feature.value());
            list->append(featureValue.release());
        }
        return list.release();
    }
    case CSSPropertyGridAutoFlow: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        switch (style.gridAutoFlow()) {
        case AutoFlowRow:
        case AutoFlowRowDense:
            list->append(cssValuePool().createIdentifierValue(CSSValueRow));
            break;
        case AutoFlowColumn:
        case AutoFlowColumnDense:
            list->append(cssValuePool().createIdentifierValue(CSSValueColumn));
            break;
        default:
            ASSERT_NOT_REACHED();
        }

        switch (style.gridAutoFlow()) {
        case AutoFlowRowDense:
        case AutoFlowColumnDense:
            list->append(cssValuePool().createIdentifierValue(CSSValueDense));
            break;
        default:
            // Do nothing.
            break;
        }

        return list.release();
    }
    // Specs mention that getComputedStyle() should return the used value of the property instead of the computed
    // one for grid-definition-{rows|columns} but not for the grid-auto-{rows|columns} as things like
    // grid-auto-columns: 2fr; cannot be resolved to a value in pixels as the '2fr' means very different things
    // depending on the size of the explicit grid or the number of implicit tracks added to the grid. See
    // http://lists.w3.org/Archives/Public/www-style/2013Nov/0014.html
    case CSSPropertyGridAutoColumns:
        return specifiedValueForGridTrackSize(style.gridAutoColumns(), style);
    case CSSPropertyGridAutoRows:
        return specifiedValueForGridTrackSize(style.gridAutoRows(), style);

    case CSSPropertyGridTemplateColumns:
        return valueForGridTrackList(ForColumns, layoutObject, style);
    case CSSPropertyGridTemplateRows:
        return valueForGridTrackList(ForRows, layoutObject, style);

    case CSSPropertyGridColumnStart:
        return valueForGridPosition(style.gridColumnStart());
    case CSSPropertyGridColumnEnd:
        return valueForGridPosition(style.gridColumnEnd());
    case CSSPropertyGridRowStart:
        return valueForGridPosition(style.gridRowStart());
    case CSSPropertyGridRowEnd:
        return valueForGridPosition(style.gridRowEnd());
    case CSSPropertyGridColumn:
        return valuesForGridShorthand(gridColumnShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyGridRow:
        return valuesForGridShorthand(gridRowShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyGridArea:
        return valuesForGridShorthand(gridAreaShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyGridTemplate:
        return valuesForGridShorthand(gridTemplateShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyGrid:
        return valuesForGridShorthand(gridShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyGridTemplateAreas:
        if (!style.namedGridAreaRowCount()) {
            ASSERT(!style.namedGridAreaColumnCount());
            return cssValuePool().createIdentifierValue(CSSValueNone);
        }

        return CSSGridTemplateAreasValue::create(style.namedGridArea(), style.namedGridAreaRowCount(), style.namedGridAreaColumnCount());

    case CSSPropertyHeight:
        if (layoutObject) {
            // According to http://www.w3.org/TR/CSS2/visudet.html#the-height-property,
            // the "height" property does not apply for non-replaced inline elements.
            if (!layoutObject->isReplaced() && layoutObject->isInline())
                return cssValuePool().createIdentifierValue(CSSValueAuto);
            return zoomAdjustedPixelValue(sizingBox(layoutObject).height(), style);
        }
        return zoomAdjustedPixelValueForLength(style.height(), style);
    case CSSPropertyWebkitHighlight:
        if (style.highlight() == nullAtom)
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return cssValuePool().createValue(style.highlight(), CSSPrimitiveValue::CSS_STRING);
    case CSSPropertyWebkitHyphenateCharacter:
        if (style.hyphenationString().isNull())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return cssValuePool().createValue(style.hyphenationString(), CSSPrimitiveValue::CSS_STRING);
    case CSSPropertyImageRendering:
        return CSSPrimitiveValue::create(style.imageRendering());
    case CSSPropertyIsolation:
        return cssValuePool().createValue(style.isolation());
    case CSSPropertyJustifyItems:
        return valueForItemPositionWithOverflowAlignment(resolveAlignmentAuto(style.justifyItemsPosition(), styledNode), style.justifyItemsOverflowAlignment(), style.justifyItemsPositionType());
    case CSSPropertyJustifySelf:
        return valueForItemPositionWithOverflowAlignment(resolveAlignmentAuto(style.justifySelfPosition(), styledNode->parentNode()), style.justifySelfOverflowAlignment(), NonLegacyPosition);
    case CSSPropertyLeft:
        return valueForPositionOffset(style, CSSPropertyLeft, layoutObject);
    case CSSPropertyLetterSpacing:
        if (!style.letterSpacing())
            return cssValuePool().createIdentifierValue(CSSValueNormal);
        return zoomAdjustedPixelValue(style.letterSpacing(), style);
    case CSSPropertyWebkitLineClamp:
        if (style.lineClamp().isNone())
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return cssValuePool().createValue(style.lineClamp().value(), style.lineClamp().isPercentage() ? CSSPrimitiveValue::CSS_PERCENTAGE : CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyLineHeight:
        return valueForLineHeight(style);
    case CSSPropertyListStyleImage:
        if (style.listStyleImage())
            return style.listStyleImage()->cssValue();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case CSSPropertyListStylePosition:
        return cssValuePool().createValue(style.listStylePosition());
    case CSSPropertyListStyleType:
        return cssValuePool().createValue(style.listStyleType());
    case CSSPropertyWebkitLocale:
        if (style.locale().isNull())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return cssValuePool().createValue(style.locale(), CSSPrimitiveValue::CSS_STRING);
    case CSSPropertyMarginTop: {
        Length marginTop = style.marginTop();
        if (marginTop.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(marginTop, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->marginTop(), style);
    }
    case CSSPropertyMarginRight: {
        Length marginRight = style.marginRight();
        if (marginRight.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(marginRight, style);
        float value;
        if (marginRight.hasPercent()) {
            // LayoutBox gives a marginRight() that is the distance between the right-edge of the child box
            // and the right-edge of the containing box, when display == BLOCK. Let's calculate the absolute
            // value of the specified margin-right % instead of relying on LayoutBox's marginRight() value.
            value = minimumValueForLength(marginRight, toLayoutBox(layoutObject)->containingBlockLogicalWidthForContent()).toFloat();
        } else {
            value = toLayoutBox(layoutObject)->marginRight().toFloat();
        }
        return zoomAdjustedPixelValue(value, style);
    }
    case CSSPropertyMarginBottom: {
        Length marginBottom = style.marginBottom();
        if (marginBottom.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(marginBottom, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->marginBottom(), style);
    }
    case CSSPropertyMarginLeft: {
        Length marginLeft = style.marginLeft();
        if (marginLeft.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(marginLeft, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->marginLeft(), style);
    }
    case CSSPropertyWebkitUserModify:
        return cssValuePool().createValue(style.userModify());
    case CSSPropertyMaxHeight: {
        const Length& maxHeight = style.maxHeight();
        if (maxHeight.isMaxSizeNone())
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return zoomAdjustedPixelValueForLength(maxHeight, style);
    }
    case CSSPropertyMaxWidth: {
        const Length& maxWidth = style.maxWidth();
        if (maxWidth.isMaxSizeNone())
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return zoomAdjustedPixelValueForLength(maxWidth, style);
    }
    case CSSPropertyMinHeight:
        if (style.minHeight().isAuto()) {
            if (isFlexOrGrid(styledNode->parentNode()))
                return cssValuePool().createIdentifierValue(CSSValueAuto);
            return zoomAdjustedPixelValue(0, style);
        }
        return zoomAdjustedPixelValueForLength(style.minHeight(), style);
    case CSSPropertyMinWidth:
        if (style.minWidth().isAuto()) {
            if (isFlexOrGrid(styledNode->parentNode()))
                return cssValuePool().createIdentifierValue(CSSValueAuto);
            return zoomAdjustedPixelValue(0, style);
        }
        return zoomAdjustedPixelValueForLength(style.minWidth(), style);
    case CSSPropertyObjectFit:
        return cssValuePool().createValue(style.objectFit());
    case CSSPropertyObjectPosition:
        return cssValuePool().createValue(
            Pair::create(
                zoomAdjustedPixelValueForLength(style.objectPosition().x(), style),
                zoomAdjustedPixelValueForLength(style.objectPosition().y(), style),
                Pair::KeepIdenticalValues));
    case CSSPropertyOpacity:
        return cssValuePool().createValue(style.opacity(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyOrphans:
        if (style.hasAutoOrphans())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return cssValuePool().createValue(style.orphans(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyOutlineColor:
        return allowVisitedStyle ? cssValuePool().createColorValue(style.visitedDependentColor(CSSPropertyOutlineColor).rgb()) : currentColorOrValidColor(style, style.outlineColor());
    case CSSPropertyOutlineOffset:
        return zoomAdjustedPixelValue(style.outlineOffset(), style);
    case CSSPropertyOutlineStyle:
        if (style.outlineStyleIsAuto())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return cssValuePool().createValue(style.outlineStyle());
    case CSSPropertyOutlineWidth:
        return zoomAdjustedPixelValue(style.outlineWidth(), style);
    case CSSPropertyOverflow:
        return cssValuePool().createValue(max(style.overflowX(), style.overflowY()));
    case CSSPropertyOverflowWrap:
        return cssValuePool().createValue(style.overflowWrap());
    case CSSPropertyOverflowX:
        return cssValuePool().createValue(style.overflowX());
    case CSSPropertyOverflowY:
        return cssValuePool().createValue(style.overflowY());
    case CSSPropertyPaddingTop: {
        Length paddingTop = style.paddingTop();
        if (paddingTop.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(paddingTop, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->computedCSSPaddingTop(), style);
    }
    case CSSPropertyPaddingRight: {
        Length paddingRight = style.paddingRight();
        if (paddingRight.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(paddingRight, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->computedCSSPaddingRight(), style);
    }
    case CSSPropertyPaddingBottom: {
        Length paddingBottom = style.paddingBottom();
        if (paddingBottom.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(paddingBottom, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->computedCSSPaddingBottom(), style);
    }
    case CSSPropertyPaddingLeft: {
        Length paddingLeft = style.paddingLeft();
        if (paddingLeft.isFixed() || !layoutObject || !layoutObject->isBox())
            return zoomAdjustedPixelValueForLength(paddingLeft, style);
        return zoomAdjustedPixelValue(toLayoutBox(layoutObject)->computedCSSPaddingLeft(), style);
    }
    case CSSPropertyPageBreakAfter:
        return cssValuePool().createValue(style.pageBreakAfter());
    case CSSPropertyPageBreakBefore:
        return cssValuePool().createValue(style.pageBreakBefore());
    case CSSPropertyPageBreakInside: {
        EPageBreak pageBreak = style.pageBreakInside();
        ASSERT(pageBreak != PBALWAYS);
        if (pageBreak == PBALWAYS)
            return nullptr;
        return cssValuePool().createValue(style.pageBreakInside());
    }
    case CSSPropertyPosition:
        return cssValuePool().createValue(style.position());
    case CSSPropertyQuotes:
        if (!style.quotes()) {
            // TODO(ramya.v): We should return the quote values that we're actually using.
            return nullptr;
        }
        if (style.quotes()->size()) {
            RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
            for (int i = 0; i < style.quotes()->size(); i++) {
                list->append(cssValuePool().createValue(style.quotes()->getOpenQuote(i), CSSPrimitiveValue::CSS_STRING));
                list->append(cssValuePool().createValue(style.quotes()->getCloseQuote(i), CSSPrimitiveValue::CSS_STRING));
            }
            return list.release();
        }
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case CSSPropertyRight:
        return valueForPositionOffset(style, CSSPropertyRight, layoutObject);
    case CSSPropertyWebkitRubyPosition:
        return cssValuePool().createValue(style.rubyPosition());
    case CSSPropertyScrollBehavior:
        return cssValuePool().createValue(style.scrollBehavior());
//     case CSSPropertyScrollBlocksOn:
//         return scrollBlocksOnFlagsToCSSValue(style.scrollBlocksOn());
    case CSSPropertyTableLayout:
        return cssValuePool().createValue(style.tableLayout());
    case CSSPropertyTextAlign:
        return cssValuePool().createValue(style.textAlign());
    case CSSPropertyTextAlignLast:
        return cssValuePool().createValue(style.textAlignLast());
    case CSSPropertyTextDecoration:
        if (RuntimeEnabledFeatures::css3TextDecorationsEnabled())
            return valuesForShorthandProperty(textDecorationShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
        // Fall through.
    case CSSPropertyTextDecorationLine:
        return renderTextDecorationFlagsToCSSValue(style.textDecoration());
    case CSSPropertyTextDecorationStyle:
        return valueForTextDecorationStyle(style.textDecorationStyle());
    case CSSPropertyTextDecorationColor:
        return currentColorOrValidColor(style, style.textDecorationColor());
    case CSSPropertyTextJustify:
        return cssValuePool().createValue(style.textJustify());
    case CSSPropertyTextUnderlinePosition:
        return cssValuePool().createValue(style.textUnderlinePosition());
    case CSSPropertyWebkitTextDecorationsInEffect:
        return renderTextDecorationFlagsToCSSValue(style.textDecorationsInEffect());
    case CSSPropertyWebkitTextFillColor:
        return currentColorOrValidColor(style, style.textFillColor());
    case CSSPropertyWebkitTextEmphasisColor:
        return currentColorOrValidColor(style, style.textEmphasisColor());
    case CSSPropertyWebkitTextEmphasisPosition:
        return cssValuePool().createValue(style.textEmphasisPosition());
    case CSSPropertyWebkitTextEmphasisStyle:
        switch (style.textEmphasisMark()) {
        case TextEmphasisMarkNone:
            return cssValuePool().createIdentifierValue(CSSValueNone);
        case TextEmphasisMarkCustom:
            return cssValuePool().createValue(style.textEmphasisCustomMark(), CSSPrimitiveValue::CSS_STRING);
        case TextEmphasisMarkAuto:
            ASSERT_NOT_REACHED();
            // Fall through
        case TextEmphasisMarkDot:
        case TextEmphasisMarkCircle:
        case TextEmphasisMarkDoubleCircle:
        case TextEmphasisMarkTriangle:
        case TextEmphasisMarkSesame: {
            RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
            list->append(cssValuePool().createValue(style.textEmphasisFill()));
            list->append(cssValuePool().createValue(style.textEmphasisMark()));
            return list.release();
        }
        }
    case CSSPropertyTextIndent: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        list->append(zoomAdjustedPixelValueForLength(style.textIndent(), style));
        if (RuntimeEnabledFeatures::css3TextEnabled() && (style.textIndentLine() == TextIndentEachLine || style.textIndentType() == TextIndentHanging)) {
            if (style.textIndentLine() == TextIndentEachLine)
                list->append(cssValuePool().createIdentifierValue(CSSValueEachLine));
            if (style.textIndentType() == TextIndentHanging)
                list->append(cssValuePool().createIdentifierValue(CSSValueHanging));
        }
        return list.release();
    }
    case CSSPropertyTextShadow:
        return valueForShadowList(style.textShadow(), style, false);
    case CSSPropertyTextRendering:
        return cssValuePool().createValue(style.fontDescription().textRendering());
    case CSSPropertyTextOverflow:
        if (style.textOverflow())
            return cssValuePool().createIdentifierValue(CSSValueEllipsis);
        return cssValuePool().createIdentifierValue(CSSValueClip);
    case CSSPropertyWebkitTextSecurity:
        return cssValuePool().createValue(style.textSecurity());
    case CSSPropertyWebkitTextStrokeColor:
        return currentColorOrValidColor(style, style.textStrokeColor());
    case CSSPropertyWebkitTextStrokeWidth:
        return zoomAdjustedPixelValue(style.textStrokeWidth(), style);
    case CSSPropertyTextTransform:
        return cssValuePool().createValue(style.textTransform());
    case CSSPropertyTop:
        return valueForPositionOffset(style, CSSPropertyTop, layoutObject);
    case CSSPropertyTouchAction:
        return touchActionFlagsToCSSValue(style.touchAction());
    case CSSPropertyUnicodeBidi:
        return cssValuePool().createValue(style.unicodeBidi());
    case CSSPropertyVerticalAlign:
        switch (style.verticalAlign()) {
        case BASELINE:
            return cssValuePool().createIdentifierValue(CSSValueBaseline);
        case MIDDLE:
            return cssValuePool().createIdentifierValue(CSSValueMiddle);
        case SUB:
            return cssValuePool().createIdentifierValue(CSSValueSub);
        case SUPER:
            return cssValuePool().createIdentifierValue(CSSValueSuper);
        case TEXT_TOP:
            return cssValuePool().createIdentifierValue(CSSValueTextTop);
        case TEXT_BOTTOM:
            return cssValuePool().createIdentifierValue(CSSValueTextBottom);
        case TOP:
            return cssValuePool().createIdentifierValue(CSSValueTop);
        case BOTTOM:
            return cssValuePool().createIdentifierValue(CSSValueBottom);
        case BASELINE_MIDDLE:
            return cssValuePool().createIdentifierValue(CSSValueWebkitBaselineMiddle);
        case LENGTH:
            return zoomAdjustedPixelValueForLength(style.verticalAlignLength(), style);
        }
        ASSERT_NOT_REACHED();
        return nullptr;
    case CSSPropertyVisibility:
        return cssValuePool().createValue(style.visibility());
    case CSSPropertyWhiteSpace:
        return cssValuePool().createValue(style.whiteSpace());
    case CSSPropertyWidows:
        return cssValuePool().createValue(style.widows(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyWidth:
        if (layoutObject) {
            // According to http://www.w3.org/TR/CSS2/visudet.html#the-width-property,
            // the "width" property does not apply for non-replaced inline elements.
            if (!layoutObject->isReplaced() && layoutObject->isInline())
                return cssValuePool().createIdentifierValue(CSSValueAuto);
            return zoomAdjustedPixelValue(sizingBox(layoutObject).width(), style);
        }
        return zoomAdjustedPixelValueForLength(style.width(), style);
    case CSSPropertyWillChange:
        return valueForWillChange(style.willChangeProperties(), style.willChangeContents(), style.willChangeScrollPosition());
    case CSSPropertyWordBreak:
        return cssValuePool().createValue(style.wordBreak());
    case CSSPropertyWordSpacing:
        return zoomAdjustedPixelValue(style.wordSpacing(), style);
    case CSSPropertyWordWrap:
        return cssValuePool().createValue(style.overflowWrap());
    case CSSPropertyWebkitLineBreak:
        return cssValuePool().createValue(style.lineBreak());
    case CSSPropertyResize:
        return cssValuePool().createValue(style.resize());
    case CSSPropertyFontKerning:
        return cssValuePool().createValue(style.fontDescription().kerning());
    case CSSPropertyWebkitFontSmoothing:
        return cssValuePool().createValue(style.fontDescription().fontSmoothing());
    case CSSPropertyFontVariantLigatures: {
        FontDescription::LigaturesState commonLigaturesState = style.fontDescription().commonLigaturesState();
        FontDescription::LigaturesState discretionaryLigaturesState = style.fontDescription().discretionaryLigaturesState();
        FontDescription::LigaturesState historicalLigaturesState = style.fontDescription().historicalLigaturesState();
        FontDescription::LigaturesState contextualLigaturesState = style.fontDescription().contextualLigaturesState();
        if (commonLigaturesState == FontDescription::NormalLigaturesState && discretionaryLigaturesState == FontDescription::NormalLigaturesState
            && historicalLigaturesState == FontDescription::NormalLigaturesState && contextualLigaturesState == FontDescription::NormalLigaturesState)
            return cssValuePool().createIdentifierValue(CSSValueNormal);

        RefPtrWillBeRawPtr<CSSValueList> valueList = CSSValueList::createSpaceSeparated();
        if (commonLigaturesState != FontDescription::NormalLigaturesState)
            valueList->append(cssValuePool().createIdentifierValue(commonLigaturesState == FontDescription::DisabledLigaturesState ? CSSValueNoCommonLigatures : CSSValueCommonLigatures));
        if (discretionaryLigaturesState != FontDescription::NormalLigaturesState)
            valueList->append(cssValuePool().createIdentifierValue(discretionaryLigaturesState == FontDescription::DisabledLigaturesState ? CSSValueNoDiscretionaryLigatures : CSSValueDiscretionaryLigatures));
        if (historicalLigaturesState != FontDescription::NormalLigaturesState)
            valueList->append(cssValuePool().createIdentifierValue(historicalLigaturesState == FontDescription::DisabledLigaturesState ? CSSValueNoHistoricalLigatures : CSSValueHistoricalLigatures));
        if (contextualLigaturesState != FontDescription::NormalLigaturesState)
            valueList->append(cssValuePool().createIdentifierValue(contextualLigaturesState == FontDescription::DisabledLigaturesState ? CSSValueNoContextual : CSSValueContextual));
        return valueList;
    }
    case CSSPropertyZIndex:
        if (style.hasAutoZIndex())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        return cssValuePool().createValue(style.zIndex(), CSSPrimitiveValue::CSS_INTEGER);
    case CSSPropertyZoom:
        return cssValuePool().createValue(style.zoom(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyBoxSizing:
        if (style.boxSizing() == CONTENT_BOX)
            return cssValuePool().createIdentifierValue(CSSValueContentBox);
        return cssValuePool().createIdentifierValue(CSSValueBorderBox);
    case CSSPropertyWebkitAppRegion:
        return cssValuePool().createIdentifierValue(style.getDraggableRegionMode() == DraggableRegionDrag ? CSSValueDrag : CSSValueNoDrag);
    case CSSPropertyAnimationDelay:
        return valueForAnimationDelay(style.animations());
    case CSSPropertyAnimationDirection: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const CSSAnimationData* animationData = style.animations();
        if (animationData) {
            for (size_t i = 0; i < animationData->directionList().size(); ++i)
                list->append(valueForAnimationDirection(animationData->directionList()[i]));
        } else {
            list->append(cssValuePool().createIdentifierValue(CSSValueNormal));
        }
        return list.release();
    }
    case CSSPropertyAnimationDuration:
        return valueForAnimationDuration(style.animations());
    case CSSPropertyAnimationFillMode: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const CSSAnimationData* animationData = style.animations();
        if (animationData) {
            for (size_t i = 0; i < animationData->fillModeList().size(); ++i)
                list->append(valueForAnimationFillMode(animationData->fillModeList()[i]));
        } else {
            list->append(cssValuePool().createIdentifierValue(CSSValueNone));
        }
        return list.release();
    }
    case CSSPropertyAnimationIterationCount: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const CSSAnimationData* animationData = style.animations();
        if (animationData) {
            for (size_t i = 0; i < animationData->iterationCountList().size(); ++i)
                list->append(valueForAnimationIterationCount(animationData->iterationCountList()[i]));
        } else {
            list->append(cssValuePool().createValue(CSSAnimationData::initialIterationCount(), CSSPrimitiveValue::CSS_NUMBER));
        }
        return list.release();
    }
    case CSSPropertyAnimationName: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const CSSAnimationData* animationData = style.animations();
        if (animationData) {
            for (size_t i = 0; i < animationData->nameList().size(); ++i)
                list->append(cssValuePool().createValue(animationData->nameList()[i], CSSPrimitiveValue::CSS_CUSTOM_IDENT));
        } else {
            list->append(cssValuePool().createIdentifierValue(CSSValueNone));
        }
        return list.release();
    }
    case CSSPropertyAnimationPlayState: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        const CSSAnimationData* animationData = style.animations();
        if (animationData) {
            for (size_t i = 0; i < animationData->playStateList().size(); ++i)
                list->append(valueForAnimationPlayState(animationData->playStateList()[i]));
        } else {
            list->append(cssValuePool().createIdentifierValue(CSSValueRunning));
        }
        return list.release();
    }
    case CSSPropertyAnimationTimingFunction:
        return valueForAnimationTimingFunction(style.animations());
    case CSSPropertyAnimation: {
        const CSSAnimationData* animationData = style.animations();
        if (animationData) {
            RefPtrWillBeRawPtr<CSSValueList> animationsList = CSSValueList::createCommaSeparated();
            for (size_t i = 0; i < animationData->nameList().size(); ++i) {
                RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
                list->append(cssValuePool().createValue(animationData->nameList()[i], CSSPrimitiveValue::CSS_CUSTOM_IDENT));
                list->append(cssValuePool().createValue(CSSTimingData::getRepeated(animationData->durationList(), i), CSSPrimitiveValue::CSS_S));
                list->append(createTimingFunctionValue(CSSTimingData::getRepeated(animationData->timingFunctionList(), i).get()));
                list->append(cssValuePool().createValue(CSSTimingData::getRepeated(animationData->delayList(), i), CSSPrimitiveValue::CSS_S));
                list->append(valueForAnimationIterationCount(CSSTimingData::getRepeated(animationData->iterationCountList(), i)));
                list->append(valueForAnimationDirection(CSSTimingData::getRepeated(animationData->directionList(), i)));
                list->append(valueForAnimationFillMode(CSSTimingData::getRepeated(animationData->fillModeList(), i)));
                list->append(valueForAnimationPlayState(CSSTimingData::getRepeated(animationData->playStateList(), i)));
                animationsList->append(list);
            }
            return animationsList.release();
        }

        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        // animation-name default value.
        list->append(cssValuePool().createIdentifierValue(CSSValueNone));
        list->append(cssValuePool().createValue(CSSAnimationData::initialDuration(), CSSPrimitiveValue::CSS_S));
        list->append(createTimingFunctionValue(CSSAnimationData::initialTimingFunction().get()));
        list->append(cssValuePool().createValue(CSSAnimationData::initialDelay(), CSSPrimitiveValue::CSS_S));
        list->append(cssValuePool().createValue(CSSAnimationData::initialIterationCount(), CSSPrimitiveValue::CSS_NUMBER));
        list->append(valueForAnimationDirection(CSSAnimationData::initialDirection()));
        list->append(valueForAnimationFillMode(CSSAnimationData::initialFillMode()));
        // Initial animation-play-state.
        list->append(cssValuePool().createIdentifierValue(CSSValueRunning));
        return list.release();
    }
    case CSSPropertyWebkitAppearance:
        return cssValuePool().createValue(style.appearance());
    case CSSPropertyBackfaceVisibility:
        return cssValuePool().createIdentifierValue((style.backfaceVisibility() == BackfaceVisibilityHidden) ? CSSValueHidden : CSSValueVisible);
    case CSSPropertyWebkitBorderImage:
        return valueForNinePieceImage(style.borderImage(), style);
    case CSSPropertyBorderImageOutset:
        return valueForNinePieceImageQuad(style.borderImage().outset(), style);
    case CSSPropertyBorderImageRepeat:
        return valueForNinePieceImageRepeat(style.borderImage());
    case CSSPropertyBorderImageSlice:
        return valueForNinePieceImageSlice(style.borderImage());
    case CSSPropertyBorderImageWidth:
        return valueForNinePieceImageQuad(style.borderImage().borderSlices(), style);
    case CSSPropertyWebkitMaskBoxImage:
        return valueForNinePieceImage(style.maskBoxImage(), style);
    case CSSPropertyWebkitMaskBoxImageOutset:
        return valueForNinePieceImageQuad(style.maskBoxImage().outset(), style);
    case CSSPropertyWebkitMaskBoxImageRepeat:
        return valueForNinePieceImageRepeat(style.maskBoxImage());
    case CSSPropertyWebkitMaskBoxImageSlice:
        return valueForNinePieceImageSlice(style.maskBoxImage());
    case CSSPropertyWebkitMaskBoxImageWidth:
        return valueForNinePieceImageQuad(style.maskBoxImage().borderSlices(), style);
    case CSSPropertyWebkitMaskBoxImageSource:
        if (style.maskBoxImageSource())
            return style.maskBoxImageSource()->cssValue();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case CSSPropertyWebkitFontSizeDelta:
        // Not a real style property -- used by the editing engine -- so has no computed value.
        return nullptr;
    case CSSPropertyWebkitMarginBottomCollapse:
    case CSSPropertyWebkitMarginAfterCollapse:
        return cssValuePool().createValue(style.marginAfterCollapse());
    case CSSPropertyWebkitMarginTopCollapse:
    case CSSPropertyWebkitMarginBeforeCollapse:
        return cssValuePool().createValue(style.marginBeforeCollapse());
    case CSSPropertyPerspective:
        if (!style.hasPerspective())
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return zoomAdjustedPixelValue(style.perspective(), style);
    case CSSPropertyPerspectiveOrigin: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        if (layoutObject) {
            LayoutRect box;
            if (layoutObject->isBox())
                box = toLayoutBox(layoutObject)->borderBoxRect();

            list->append(zoomAdjustedPixelValue(minimumValueForLength(style.perspectiveOriginX(), box.width()), style));
            list->append(zoomAdjustedPixelValue(minimumValueForLength(style.perspectiveOriginY(), box.height()), style));
        } else {
            list->append(zoomAdjustedPixelValueForLength(style.perspectiveOriginX(), style));
            list->append(zoomAdjustedPixelValueForLength(style.perspectiveOriginY(), style));
        }
        return list.release();
    }
    case CSSPropertyWebkitRtlOrdering:
        return cssValuePool().createIdentifierValue(style.rtlOrdering() ? CSSValueVisual : CSSValueLogical);
    case CSSPropertyWebkitTapHighlightColor:
        return currentColorOrValidColor(style, style.tapHighlightColor());
    case CSSPropertyWebkitUserDrag:
        return cssValuePool().createValue(style.userDrag());
    case CSSPropertyWebkitUserSelect:
        return cssValuePool().createValue(style.userSelect());
    case CSSPropertyBorderBottomLeftRadius:
        return valueForBorderRadiusCorner(style.borderBottomLeftRadius(), style);
    case CSSPropertyBorderBottomRightRadius:
        return valueForBorderRadiusCorner(style.borderBottomRightRadius(), style);
    case CSSPropertyBorderTopLeftRadius:
        return valueForBorderRadiusCorner(style.borderTopLeftRadius(), style);
    case CSSPropertyBorderTopRightRadius:
        return valueForBorderRadiusCorner(style.borderTopRightRadius(), style);
    case CSSPropertyClip: {
        if (style.hasAutoClip())
            return cssValuePool().createIdentifierValue(CSSValueAuto);
        RefPtrWillBeRawPtr<Rect> rect = Rect::create();
        rect->setTop(zoomAdjustedPixelValue(style.clip().top().value(), style));
        rect->setRight(zoomAdjustedPixelValue(style.clip().right().value(), style));
        rect->setBottom(zoomAdjustedPixelValue(style.clip().bottom().value(), style));
        rect->setLeft(zoomAdjustedPixelValue(style.clip().left().value(), style));
        return cssValuePool().createValue(rect.release());
    }
    case CSSPropertySpeak:
        return cssValuePool().createValue(style.speak());
    case CSSPropertyTransform:
        return computedTransform(layoutObject, style);
    case CSSPropertyTransformOrigin: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        if (layoutObject) {
            LayoutRect box;
            if (layoutObject->isBox())
                box = toLayoutBox(layoutObject)->borderBoxRect();

            list->append(zoomAdjustedPixelValue(minimumValueForLength(style.transformOriginX(), box.width()), style));
            list->append(zoomAdjustedPixelValue(minimumValueForLength(style.transformOriginY(), box.height()), style));
            if (style.transformOriginZ() != 0)
                list->append(zoomAdjustedPixelValue(style.transformOriginZ(), style));
        } else {
            list->append(zoomAdjustedPixelValueForLength(style.transformOriginX(), style));
            list->append(zoomAdjustedPixelValueForLength(style.transformOriginY(), style));
            if (style.transformOriginZ() != 0)
                list->append(zoomAdjustedPixelValue(style.transformOriginZ(), style));
        }
        return list.release();
    }
    case CSSPropertyTransformStyle:
        return cssValuePool().createIdentifierValue((style.transformStyle3D() == TransformStyle3DPreserve3D) ? CSSValuePreserve3d : CSSValueFlat);
    case CSSPropertyTransitionDelay:
        return valueForAnimationDelay(style.transitions());
    case CSSPropertyTransitionDuration:
        return valueForAnimationDuration(style.transitions());
    case CSSPropertyTransitionProperty:
        return valueForTransitionProperty(style.transitions());
    case CSSPropertyTransitionTimingFunction:
        return valueForAnimationTimingFunction(style.transitions());
    case CSSPropertyTransition: {
        const CSSTransitionData* transitionData = style.transitions();
        if (transitionData) {
            RefPtrWillBeRawPtr<CSSValueList> transitionsList = CSSValueList::createCommaSeparated();
            for (size_t i = 0; i < transitionData->propertyList().size(); ++i) {
                RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
                list->append(createTransitionPropertyValue(transitionData->propertyList()[i]));
                list->append(cssValuePool().createValue(CSSTimingData::getRepeated(transitionData->durationList(), i), CSSPrimitiveValue::CSS_S));
                list->append(createTimingFunctionValue(CSSTimingData::getRepeated(transitionData->timingFunctionList(), i).get()));
                list->append(cssValuePool().createValue(CSSTimingData::getRepeated(transitionData->delayList(), i), CSSPrimitiveValue::CSS_S));
                transitionsList->append(list);
            }
            return transitionsList.release();
        }

        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        // transition-property default value.
        list->append(cssValuePool().createIdentifierValue(CSSValueAll));
        list->append(cssValuePool().createValue(CSSTransitionData::initialDuration(), CSSPrimitiveValue::CSS_S));
        list->append(createTimingFunctionValue(CSSTransitionData::initialTimingFunction().get()));
        list->append(cssValuePool().createValue(CSSTransitionData::initialDelay(), CSSPrimitiveValue::CSS_S));
        return list.release();
    }
    case CSSPropertyPointerEvents:
        return cssValuePool().createValue(style.pointerEvents());
    case CSSPropertyWebkitWritingMode:
        return cssValuePool().createValue(style.writingMode());
    case CSSPropertyWebkitTextCombine:
        return cssValuePool().createValue(style.textCombine());
    case CSSPropertyWebkitTextOrientation:
        return CSSPrimitiveValue::create(style.textOrientation());
//     case CSSPropertyWebkitLineBoxContain:
//         return createLineBoxContainValue(style.lineBoxContain());
    case CSSPropertyContent:
        return valueForContentData(style);
    case CSSPropertyCounterIncrement:
        return valueForCounterDirectives(style, propertyID);
    case CSSPropertyCounterReset:
        return valueForCounterDirectives(style, propertyID);
    case CSSPropertyWebkitClipPath:
        if (ClipPathOperation* operation = style.clipPath()) {
            if (operation->type() == ClipPathOperation::SHAPE)
                return valueForBasicShape(style, toShapeClipPathOperation(operation)->basicShape());
            if (operation->type() == ClipPathOperation::REFERENCE)
                return CSSPrimitiveValue::create(toReferenceClipPathOperation(operation)->url(), CSSPrimitiveValue::CSS_URI);
        }
        return cssValuePool().createIdentifierValue(CSSValueNone);
    case CSSPropertyShapeMargin:
        return cssValuePool().createValue(style.shapeMargin(), style);
    case CSSPropertyShapeImageThreshold:
        return cssValuePool().createValue(style.shapeImageThreshold(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyShapeOutside:
        return valueForShape(style, style.shapeOutside());
    case CSSPropertyWebkitFilter:
        return valueForFilter(style);
    case CSSPropertyMixBlendMode:
        return cssValuePool().createValue(style.blendMode());

    case CSSPropertyBackgroundBlendMode: {
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
        for (const FillLayer* currLayer = &style.backgroundLayers(); currLayer; currLayer = currLayer->next())
            list->append(cssValuePool().createValue(currLayer->blendMode()));
        return list.release();
    }
    case CSSPropertyBackground:
        return valuesForBackgroundShorthand(style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorder: {
        RefPtrWillBeRawPtr<CSSValue> value = get(CSSPropertyBorderTop, style, layoutObject, styledNode, allowVisitedStyle);
        const CSSPropertyID properties[] = {
            CSSPropertyBorderRight,
            CSSPropertyBorderBottom,
            CSSPropertyBorderLeft
        };
        for (size_t i = 0; i < WTF_ARRAY_LENGTH(properties); ++i) {
            if (!compareCSSValuePtr<CSSValue>(value, get(properties[i], style, layoutObject, styledNode, allowVisitedStyle)))
                return nullptr;
        }
        return value.release();
    }
    case CSSPropertyBorderBottom:
        return valuesForShorthandProperty(borderBottomShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorderColor:
        return valuesForSidesShorthand(borderColorShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorderLeft:
        return valuesForShorthandProperty(borderLeftShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorderImage:
        return valueForNinePieceImage(style.borderImage(), style);
    case CSSPropertyBorderRadius:
        return valueForBorderRadiusShorthand(style);
    case CSSPropertyBorderRight:
        return valuesForShorthandProperty(borderRightShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorderStyle:
        return valuesForSidesShorthand(borderStyleShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorderTop:
        return valuesForShorthandProperty(borderTopShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyBorderWidth:
        return valuesForSidesShorthand(borderWidthShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyWebkitColumnRule:
        return valuesForShorthandProperty(webkitColumnRuleShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyWebkitColumns:
        return valuesForShorthandProperty(webkitColumnsShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyListStyle:
        return valuesForShorthandProperty(listStyleShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyMargin:
        return valuesForSidesShorthand(marginShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyOutline:
        return valuesForShorthandProperty(outlineShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    case CSSPropertyPadding:
        return valuesForSidesShorthand(paddingShorthand(), style, layoutObject, styledNode, allowVisitedStyle);
    // Individual properties not part of the spec.
    case CSSPropertyBackgroundRepeatX:
    case CSSPropertyBackgroundRepeatY:
        return nullptr;

    case CSSPropertyMotion:
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        return valuesForShorthandProperty(motionShorthand(), style, layoutObject, styledNode, allowVisitedStyle);

    case CSSPropertyMotionPath: {
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        const StyleMotionPath* styleMotionPath = style.motionPath();
        if (!styleMotionPath)
            return cssValuePool().createIdentifierValue(CSSValueNone);

        ASSERT(styleMotionPath->isPathStyleMotionPath());
        return CSSPathValue::create(toPathStyleMotionPath(styleMotionPath)->pathString());
    }

    case CSSPropertyMotionOffset:
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        return zoomAdjustedPixelValueForLength(style.motionOffset(), style);

    case CSSPropertyMotionRotation: {
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        if (style.motionRotationType() == MotionRotationAuto)
            list->append(cssValuePool().createIdentifierValue(CSSValueAuto));
        list->append(cssValuePool().createValue(style.motionRotation(), CSSPrimitiveValue::CSS_DEG));
        return list.release();
    }

    // Unimplemented CSS 3 properties (including CSS3 shorthand properties).
    case CSSPropertyWebkitTextEmphasis:
        return nullptr;

    // Directional properties are resolved by resolveDirectionAwareProperty() before the switch.
    case CSSPropertyWebkitBorderEnd:
    case CSSPropertyWebkitBorderEndColor:
    case CSSPropertyWebkitBorderEndStyle:
    case CSSPropertyWebkitBorderEndWidth:
    case CSSPropertyWebkitBorderStart:
    case CSSPropertyWebkitBorderStartColor:
    case CSSPropertyWebkitBorderStartStyle:
    case CSSPropertyWebkitBorderStartWidth:
    case CSSPropertyWebkitBorderAfter:
    case CSSPropertyWebkitBorderAfterColor:
    case CSSPropertyWebkitBorderAfterStyle:
    case CSSPropertyWebkitBorderAfterWidth:
    case CSSPropertyWebkitBorderBefore:
    case CSSPropertyWebkitBorderBeforeColor:
    case CSSPropertyWebkitBorderBeforeStyle:
    case CSSPropertyWebkitBorderBeforeWidth:
    case CSSPropertyWebkitMarginEnd:
    case CSSPropertyWebkitMarginStart:
    case CSSPropertyWebkitMarginAfter:
    case CSSPropertyWebkitMarginBefore:
    case CSSPropertyWebkitPaddingEnd:
    case CSSPropertyWebkitPaddingStart:
    case CSSPropertyWebkitPaddingAfter:
    case CSSPropertyWebkitPaddingBefore:
    case CSSPropertyWebkitLogicalWidth:
    case CSSPropertyWebkitLogicalHeight:
    case CSSPropertyWebkitMinLogicalWidth:
    case CSSPropertyWebkitMinLogicalHeight:
    case CSSPropertyWebkitMaxLogicalWidth:
    case CSSPropertyWebkitMaxLogicalHeight:
        ASSERT_NOT_REACHED();
        return nullptr;

    // Unimplemented @font-face properties.
    case CSSPropertySrc:
    case CSSPropertyUnicodeRange:
        return nullptr;

    // Other unimplemented properties.
    case CSSPropertyPage: // for @page
    case CSSPropertySize: // for @page
        return nullptr;

    // Unimplemented -webkit- properties.
    case CSSPropertyWebkitMarginCollapse:
    case CSSPropertyWebkitMask:
    case CSSPropertyWebkitMaskRepeatX:
    case CSSPropertyWebkitMaskRepeatY:
    case CSSPropertyWebkitPerspectiveOriginX:
    case CSSPropertyWebkitPerspectiveOriginY:
    case CSSPropertyWebkitTextStroke:
    case CSSPropertyWebkitTransformOriginX:
    case CSSPropertyWebkitTransformOriginY:
    case CSSPropertyWebkitTransformOriginZ:
        return nullptr;

    // @viewport rule properties.
    case CSSPropertyMaxZoom:
    case CSSPropertyMinZoom:
    case CSSPropertyOrientation:
    case CSSPropertyUserZoom:
        return nullptr;

    // SVG properties.
    case CSSPropertyClipRule:
        return CSSPrimitiveValue::create(svgStyle.clipRule());
    case CSSPropertyFloodOpacity:
        return CSSPrimitiveValue::create(svgStyle.floodOpacity(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyStopOpacity:
        return CSSPrimitiveValue::create(svgStyle.stopOpacity(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyColorInterpolation:
        return CSSPrimitiveValue::create(svgStyle.colorInterpolation());
    case CSSPropertyColorInterpolationFilters:
        return CSSPrimitiveValue::create(svgStyle.colorInterpolationFilters());
    case CSSPropertyFillOpacity:
        return CSSPrimitiveValue::create(svgStyle.fillOpacity(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyFillRule:
        return CSSPrimitiveValue::create(svgStyle.fillRule());
    case CSSPropertyColorRendering:
        return CSSPrimitiveValue::create(svgStyle.colorRendering());
    case CSSPropertyShapeRendering:
        return CSSPrimitiveValue::create(svgStyle.shapeRendering());
    case CSSPropertyStrokeLinecap:
        return CSSPrimitiveValue::create(svgStyle.capStyle());
    case CSSPropertyStrokeLinejoin:
        return CSSPrimitiveValue::create(svgStyle.joinStyle());
    case CSSPropertyStrokeMiterlimit:
        return CSSPrimitiveValue::create(svgStyle.strokeMiterLimit(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyStrokeOpacity:
        return CSSPrimitiveValue::create(svgStyle.strokeOpacity(), CSSPrimitiveValue::CSS_NUMBER);
    case CSSPropertyAlignmentBaseline:
        return CSSPrimitiveValue::create(svgStyle.alignmentBaseline());
    case CSSPropertyDominantBaseline:
        return CSSPrimitiveValue::create(svgStyle.dominantBaseline());
    case CSSPropertyTextAnchor:
        return CSSPrimitiveValue::create(svgStyle.textAnchor());
    case CSSPropertyWritingMode:
        return CSSPrimitiveValue::create(svgStyle.writingMode());
    case CSSPropertyClipPath:
        if (!svgStyle.clipperResource().isEmpty())
            return CSSPrimitiveValue::create(serializeAsFragmentIdentifier(svgStyle.clipperResource()), CSSPrimitiveValue::CSS_URI);
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);
    case CSSPropertyMask:
        if (!svgStyle.maskerResource().isEmpty())
            return CSSPrimitiveValue::create(serializeAsFragmentIdentifier(svgStyle.maskerResource()), CSSPrimitiveValue::CSS_URI);
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);
    case CSSPropertyFilter:
        if (!svgStyle.filterResource().isEmpty())
            return CSSPrimitiveValue::create(serializeAsFragmentIdentifier(svgStyle.filterResource()), CSSPrimitiveValue::CSS_URI);
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);
    case CSSPropertyFloodColor:
        return currentColorOrValidColor(style, svgStyle.floodColor());
    case CSSPropertyLightingColor:
        return currentColorOrValidColor(style, svgStyle.lightingColor());
    case CSSPropertyStopColor:
        return currentColorOrValidColor(style, svgStyle.stopColor());
    case CSSPropertyFill:
        return adjustSVGPaintForCurrentColor(svgStyle.fillPaintType(), svgStyle.fillPaintUri(), svgStyle.fillPaintColor(), style.color());
    case CSSPropertyMarkerEnd:
        if (!svgStyle.markerEndResource().isEmpty())
            return CSSPrimitiveValue::create(serializeAsFragmentIdentifier(svgStyle.markerEndResource()), CSSPrimitiveValue::CSS_URI);
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);
    case CSSPropertyMarkerMid:
        if (!svgStyle.markerMidResource().isEmpty())
            return CSSPrimitiveValue::create(serializeAsFragmentIdentifier(svgStyle.markerMidResource()), CSSPrimitiveValue::CSS_URI);
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);
    case CSSPropertyMarkerStart:
        if (!svgStyle.markerStartResource().isEmpty())
            return CSSPrimitiveValue::create(serializeAsFragmentIdentifier(svgStyle.markerStartResource()), CSSPrimitiveValue::CSS_URI);
        return CSSPrimitiveValue::createIdentifier(CSSValueNone);
    case CSSPropertyStroke:
        return adjustSVGPaintForCurrentColor(svgStyle.strokePaintType(), svgStyle.strokePaintUri(), svgStyle.strokePaintColor(), style.color());
    case CSSPropertyStrokeDasharray:
        return strokeDashArrayToCSSValueList(*svgStyle.strokeDashArray(), style);
    case CSSPropertyStrokeDashoffset:
        return zoomAdjustedPixelValueForLength(svgStyle.strokeDashOffset(), style);
    case CSSPropertyStrokeWidth:
        return pixelValueForUnzoomedLength(svgStyle.strokeWidth(), style);
    case CSSPropertyBaselineShift: {
        switch (svgStyle.baselineShift()) {
        case BS_SUPER:
            return CSSPrimitiveValue::createIdentifier(CSSValueSuper);
        case BS_SUB:
            return CSSPrimitiveValue::createIdentifier(CSSValueSub);
        case BS_LENGTH:
            return zoomAdjustedPixelValueForLength(svgStyle.baselineShiftValue(), style);
        }
        ASSERT_NOT_REACHED();
        return nullptr;
    }
    case CSSPropertyBufferedRendering:
        return CSSPrimitiveValue::create(svgStyle.bufferedRendering());
//     case CSSPropertyGlyphOrientationHorizontal:
//         return glyphOrientationToCSSPrimitiveValue(svgStyle.glyphOrientationHorizontal());
//     case CSSPropertyGlyphOrientationVertical: {
//         if (RefPtrWillBeRawPtr<CSSPrimitiveValue> value = glyphOrientationToCSSPrimitiveValue(svgStyle.glyphOrientationVertical()))
//             return value.release();
//         if (svgStyle.glyphOrientationVertical() == GO_AUTO)
//             return CSSPrimitiveValue::createIdentifier(CSSValueAuto);
//         return nullptr;
//     }
    case CSSPropertyPaintOrder:
        return paintOrderToCSSValueList(svgStyle.paintOrder());
    case CSSPropertyVectorEffect:
        return CSSPrimitiveValue::create(svgStyle.vectorEffect());
    case CSSPropertyMaskType:
        return CSSPrimitiveValue::create(svgStyle.maskType());
    case CSSPropertyMarker:
//     case CSSPropertyEnableBackground:
//         // the above properties are not yet implemented in the engine
//         return nullptr;
    case CSSPropertyCx:
        return zoomAdjustedPixelValueForLength(svgStyle.cx(), style);
    case CSSPropertyCy:
        return zoomAdjustedPixelValueForLength(svgStyle.cy(), style);
    case CSSPropertyX:
        return zoomAdjustedPixelValueForLength(svgStyle.x(), style);
    case CSSPropertyY:
        return zoomAdjustedPixelValueForLength(svgStyle.y(), style);
    case CSSPropertyR:
        return zoomAdjustedPixelValueForLength(svgStyle.r(), style);
    case CSSPropertyRx:
        return zoomAdjustedPixelValueForLength(svgStyle.rx(), style);
    case CSSPropertyRy:
        return zoomAdjustedPixelValueForLength(svgStyle.ry(), style);
    case CSSPropertyScrollSnapType:
        return cssValuePool().createValue(style.scrollSnapType());
    case CSSPropertyScrollSnapPointsX:
        return valueForScrollSnapPoints(style.scrollSnapPointsX(), style);
    case CSSPropertyScrollSnapPointsY:
        return valueForScrollSnapPoints(style.scrollSnapPointsY(), style);
    case CSSPropertyScrollSnapCoordinate:
        return valueForScrollSnapCoordinate(style.scrollSnapCoordinate(), style);
    case CSSPropertyScrollSnapDestination:
        return valueForScrollSnapDestination(style.scrollSnapDestination(), style);
    case CSSPropertyTranslate: {
        if (!style.translate())
            return cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PX);

        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        if (layoutObject && layoutObject->isBox()) {
            LayoutRect box = toLayoutBox(layoutObject)->borderBoxRect();
            list->append(zoomAdjustedPixelValue(floatValueForLength(style.translate()->x(), box.width().toFloat()), style));

            if (!style.translate()->y().isZero() || style.translate()->z() != 0)
                list->append(zoomAdjustedPixelValue(floatValueForLength(style.translate()->y(), box.height().toFloat()), style));

        } else {
            // No box to resolve the percentage values
            list->append(zoomAdjustedPixelValueForLength(style.translate()->x(), style));

            if (!style.translate()->y().isZero() || style.translate()->z() != 0)
                list->append(zoomAdjustedPixelValueForLength(style.translate()->y(), style));
        }

        if (style.translate()->z() != 0)
            list->append(zoomAdjustedPixelValue(style.translate()->z(), style));

        return list.release();
    }
    case CSSPropertyRotate: {
        if (!style.rotate())
            return cssValuePool().createValue(0, CSSPrimitiveValue::CSS_DEG);

        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        list->append(cssValuePool().createValue(style.rotate()->angle(), CSSPrimitiveValue::CSS_DEG));
        if (style.rotate()->x() != 0 || style.rotate()->y() != 0 || style.rotate()->z() != 1) {
            list->append(cssValuePool().createValue(style.rotate()->x(), CSSPrimitiveValue::CSS_NUMBER));
            list->append(cssValuePool().createValue(style.rotate()->y(), CSSPrimitiveValue::CSS_NUMBER));
            list->append(cssValuePool().createValue(style.rotate()->z(), CSSPrimitiveValue::CSS_NUMBER));
        }
        return list.release();
    }
    case CSSPropertyScale: {
        if (!style.scale())
            return cssValuePool().createValue(1, CSSPrimitiveValue::CSS_NUMBER);
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        list->append(cssValuePool().createValue(style.scale()->x(), CSSPrimitiveValue::CSS_NUMBER));
        if (style.scale()->y() == 1 && style.scale()->z() == 1)
            return list.release();
        list->append(cssValuePool().createValue(style.scale()->y(), CSSPrimitiveValue::CSS_NUMBER));
        if (style.scale()->z() != 1)
            list->append(cssValuePool().createValue(style.scale()->z(), CSSPrimitiveValue::CSS_NUMBER));
        return list.release();
    }
	case CSSPropertyVariable:
		// TODO(leviw): We should have a way to retrive variables here.
		ASSERT_NOT_REACHED();
		return nullptr;

    case CSSPropertyAll:
        return nullptr;
    default:
        break;
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

}
