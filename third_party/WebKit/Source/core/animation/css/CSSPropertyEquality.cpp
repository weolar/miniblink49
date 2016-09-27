// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/css/CSSPropertyEquality.h"

#include "core/animation/css/CSSAnimations.h"
#include "core/style/DataEquivalency.h"
#include "core/style/ComputedStyle.h"
#include "core/style/ShadowList.h"

namespace blink {

namespace {

template <CSSPropertyID property>
bool fillLayersEqual(const FillLayer& aLayers, const FillLayer& bLayers)
{
    const FillLayer* aLayer = &aLayers;
    const FillLayer* bLayer = &bLayers;
    while (aLayer && bLayer) {
        switch (property) {
        case CSSPropertyBackgroundPositionX:
        case CSSPropertyWebkitMaskPositionX:
            if (aLayer->xPosition() != bLayer->xPosition())
                return false;
            break;
        case CSSPropertyBackgroundPositionY:
        case CSSPropertyWebkitMaskPositionY:
            if (aLayer->yPosition() != bLayer->yPosition())
                return false;
            break;
        case CSSPropertyBackgroundSize:
        case CSSPropertyWebkitMaskSize:
            if (!(aLayer->sizeLength() == bLayer->sizeLength()))
                return false;
            break;
        case CSSPropertyBackgroundImage:
            if (!dataEquivalent(aLayer->image(), bLayer->image()))
                return false;
            break;
        default:
            ASSERT_NOT_REACHED();
            return true;
        }

        aLayer = aLayer->next();
        bLayer = bLayer->next();
    }

    // FIXME: Shouldn't this be return !aLayer && !bLayer; ?
    return true;
}

}

bool CSSPropertyEquality::propertiesEqual(CSSPropertyID prop, const ComputedStyle& a, const ComputedStyle& b)
{
    switch (prop) {
    case CSSPropertyBackgroundColor:
        return a.backgroundColor() == b.backgroundColor()
            && a.visitedLinkBackgroundColor() == b.visitedLinkBackgroundColor();
    case CSSPropertyBackgroundImage:
        return fillLayersEqual<CSSPropertyBackgroundImage>(a.backgroundLayers(), b.backgroundLayers());
    case CSSPropertyBackgroundPositionX:
        return fillLayersEqual<CSSPropertyBackgroundPositionX>(a.backgroundLayers(), b.backgroundLayers());
    case CSSPropertyBackgroundPositionY:
        return fillLayersEqual<CSSPropertyBackgroundPositionY>(a.backgroundLayers(), b.backgroundLayers());
    case CSSPropertyBackgroundSize:
        return fillLayersEqual<CSSPropertyBackgroundSize>(a.backgroundLayers(), b.backgroundLayers());
    case CSSPropertyBaselineShift:
        return a.baselineShiftValue() == b.baselineShiftValue();
    case CSSPropertyBorderBottomColor:
        return a.borderBottomColor() == b.borderBottomColor()
            && a.visitedLinkBorderBottomColor() == b.visitedLinkBorderBottomColor();
    case CSSPropertyBorderBottomLeftRadius:
        return a.borderBottomLeftRadius() == b.borderBottomLeftRadius();
    case CSSPropertyBorderBottomRightRadius:
        return a.borderBottomRightRadius() == b.borderBottomRightRadius();
    case CSSPropertyBorderBottomWidth:
        return a.borderBottomWidth() == b.borderBottomWidth();
    case CSSPropertyBorderImageOutset:
        return a.borderImageOutset() == b.borderImageOutset();
    case CSSPropertyBorderImageSlice:
        return a.borderImageSlices() == b.borderImageSlices();
    case CSSPropertyBorderImageSource:
        return dataEquivalent(a.borderImageSource(), b.borderImageSource());
    case CSSPropertyBorderImageWidth:
        return a.borderImageWidth() == b.borderImageWidth();
    case CSSPropertyBorderLeftColor:
        return a.borderLeftColor() == b.borderLeftColor()
            && a.visitedLinkBorderLeftColor() == b.visitedLinkBorderLeftColor();
    case CSSPropertyBorderLeftWidth:
        return a.borderLeftWidth() == b.borderLeftWidth();
    case CSSPropertyBorderRightColor:
        return a.borderRightColor() == b.borderRightColor()
            && a.visitedLinkBorderRightColor() == b.visitedLinkBorderRightColor();
    case CSSPropertyBorderRightWidth:
        return a.borderRightWidth() == b.borderRightWidth();
    case CSSPropertyBorderTopColor:
        return a.borderTopColor() == b.borderTopColor()
            && a.visitedLinkBorderTopColor() == b.visitedLinkBorderTopColor();
    case CSSPropertyBorderTopLeftRadius:
        return a.borderTopLeftRadius() == b.borderTopLeftRadius();
    case CSSPropertyBorderTopRightRadius:
        return a.borderTopRightRadius() == b.borderTopRightRadius();
    case CSSPropertyBorderTopWidth:
        return a.borderTopWidth() == b.borderTopWidth();
    case CSSPropertyBottom:
        return a.bottom() == b.bottom();
    case CSSPropertyBoxShadow:
        return dataEquivalent(a.boxShadow(), b.boxShadow());
    case CSSPropertyClip:
        return a.clip() == b.clip();
    case CSSPropertyColor:
        return a.color() == b.color() && a.visitedLinkColor() == b.visitedLinkColor();
    case CSSPropertyFill: {
        const SVGComputedStyle& aSVG = a.svgStyle();
        const SVGComputedStyle& bSVG = b.svgStyle();
        return aSVG.fillPaintType() == bSVG.fillPaintType()
            && (aSVG.fillPaintType() != SVG_PAINTTYPE_RGBCOLOR || aSVG.fillPaintColor() == bSVG.fillPaintColor())
            && aSVG.visitedLinkFillPaintType() == bSVG.visitedLinkFillPaintType()
            && (aSVG.visitedLinkFillPaintType() != SVG_PAINTTYPE_RGBCOLOR || aSVG.visitedLinkFillPaintColor() == bSVG.visitedLinkFillPaintColor());
    }
    case CSSPropertyFillOpacity:
        return a.fillOpacity() == b.fillOpacity();
    case CSSPropertyFlexBasis:
        return a.flexBasis() == b.flexBasis();
    case CSSPropertyFlexGrow:
        return a.flexGrow() == b.flexGrow();
    case CSSPropertyFlexShrink:
        return a.flexShrink() == b.flexShrink();
    case CSSPropertyFloodColor:
        return a.floodColor() == b.floodColor();
    case CSSPropertyFloodOpacity:
        return a.floodOpacity() == b.floodOpacity();
    case CSSPropertyFontSize:
        // CSSPropertyFontSize: Must pass a specified size to setFontSize if Text Autosizing is enabled, but a computed size
        // if text zoom is enabled (if neither is enabled it's irrelevant as they're probably the same).
        // FIXME: Should we introduce an option to pass the computed font size here, allowing consumers to
        // enable text zoom rather than Text Autosizing? See http://crbug.com/227545.
        return a.specifiedFontSize() == b.specifiedFontSize();
    case CSSPropertyFontSizeAdjust:
        return a.fontSizeAdjust() == b.fontSizeAdjust();
    case CSSPropertyFontStretch:
        return a.fontStretch() == b.fontStretch();
    case CSSPropertyFontWeight:
        return a.fontWeight() == b.fontWeight();
    case CSSPropertyHeight:
        return a.height() == b.height();
    case CSSPropertyLeft:
        return a.left() == b.left();
    case CSSPropertyLetterSpacing:
        return a.letterSpacing() == b.letterSpacing();
    case CSSPropertyLightingColor:
        return a.lightingColor() == b.lightingColor();
    case CSSPropertyLineHeight:
        return a.specifiedLineHeight() == b.specifiedLineHeight();
    case CSSPropertyListStyleImage:
        return dataEquivalent(a.listStyleImage(), b.listStyleImage());
    case CSSPropertyMarginBottom:
        return a.marginBottom() == b.marginBottom();
    case CSSPropertyMarginLeft:
        return a.marginLeft() == b.marginLeft();
    case CSSPropertyMarginRight:
        return a.marginRight() == b.marginRight();
    case CSSPropertyMarginTop:
        return a.marginTop() == b.marginTop();
    case CSSPropertyMaxHeight:
        return a.maxHeight() == b.maxHeight();
    case CSSPropertyMaxWidth:
        return a.maxWidth() == b.maxWidth();
    case CSSPropertyMinHeight:
        return a.minHeight() == b.minHeight();
    case CSSPropertyMinWidth:
        return a.minWidth() == b.minWidth();
    case CSSPropertyMotionOffset:
        return a.motionOffset() == b.motionOffset();
    case CSSPropertyMotionRotation:
        return a.motionRotation() == b.motionRotation()
            && a.motionRotationType() == b.motionRotationType();
    case CSSPropertyObjectPosition:
        return a.objectPosition() == b.objectPosition();
    case CSSPropertyOpacity:
        return a.opacity() == b.opacity();
    case CSSPropertyOrphans:
        return a.orphans() == b.orphans();
    case CSSPropertyOutlineColor:
        return a.outlineColor() == b.outlineColor()
            && a.visitedLinkOutlineColor() == b.visitedLinkOutlineColor();
    case CSSPropertyOutlineOffset:
        return a.outlineOffset() == b.outlineOffset();
    case CSSPropertyOutlineWidth:
        return a.outlineWidth() == b.outlineWidth();
    case CSSPropertyPaddingBottom:
        return a.paddingBottom() == b.paddingBottom();
    case CSSPropertyPaddingLeft:
        return a.paddingLeft() == b.paddingLeft();
    case CSSPropertyPaddingRight:
        return a.paddingRight() == b.paddingRight();
    case CSSPropertyPaddingTop:
        return a.paddingTop() == b.paddingTop();
    case CSSPropertyRight:
        return a.right() == b.right();
    case CSSPropertyShapeImageThreshold:
        return a.shapeImageThreshold() == b.shapeImageThreshold();
    case CSSPropertyShapeMargin:
        return a.shapeMargin() == b.shapeMargin();
    case CSSPropertyShapeOutside:
        return dataEquivalent(a.shapeOutside(), b.shapeOutside());
    case CSSPropertyStopColor:
        return a.stopColor() == b.stopColor();
    case CSSPropertyStopOpacity:
        return a.stopOpacity() == b.stopOpacity();
    case CSSPropertyStroke: {
        const SVGComputedStyle& aSVG = a.svgStyle();
        const SVGComputedStyle& bSVG = b.svgStyle();
        return aSVG.strokePaintType() == bSVG.strokePaintType()
            && (aSVG.strokePaintType() != SVG_PAINTTYPE_RGBCOLOR || aSVG.strokePaintColor() == bSVG.strokePaintColor())
            && aSVG.visitedLinkStrokePaintType() == bSVG.visitedLinkStrokePaintType()
            && (aSVG.visitedLinkStrokePaintType() != SVG_PAINTTYPE_RGBCOLOR || aSVG.visitedLinkStrokePaintColor() == bSVG.visitedLinkStrokePaintColor());
    }
    case CSSPropertyStrokeDasharray:
        return a.strokeDashArray() == b.strokeDashArray();
    case CSSPropertyStrokeDashoffset:
        return a.strokeDashOffset() == b.strokeDashOffset();
    case CSSPropertyStrokeMiterlimit:
        return a.strokeMiterLimit() == b.strokeMiterLimit();
    case CSSPropertyStrokeOpacity:
        return a.strokeOpacity() == b.strokeOpacity();
    case CSSPropertyStrokeWidth:
        return a.strokeWidth() == b.strokeWidth();
    case CSSPropertyTextDecorationColor:
        return a.textDecorationColor() == b.textDecorationColor()
            && a.visitedLinkTextDecorationColor() == b.visitedLinkTextDecorationColor();
    case CSSPropertyTextIndent:
        return a.textIndent() == b.textIndent();
    case CSSPropertyTextShadow:
        return dataEquivalent(a.textShadow(), b.textShadow());
    case CSSPropertyTop:
        return a.top() == b.top();
    case CSSPropertyVerticalAlign:
        return a.verticalAlign() == b.verticalAlign()
            && (a.verticalAlign() != LENGTH || a.verticalAlignLength() == b.verticalAlignLength());
    case CSSPropertyVisibility:
        return a.visibility() == b.visibility();
    case CSSPropertyWebkitBorderHorizontalSpacing:
        return a.horizontalBorderSpacing() == b.horizontalBorderSpacing();
    case CSSPropertyWebkitBorderVerticalSpacing:
        return a.verticalBorderSpacing() == b.verticalBorderSpacing();
    case CSSPropertyWebkitClipPath:
        return dataEquivalent(a.clipPath(), b.clipPath());
    case CSSPropertyWebkitColumnCount:
        return a.columnCount() == b.columnCount();
    case CSSPropertyWebkitColumnGap:
        return a.columnGap() == b.columnGap();
    case CSSPropertyWebkitColumnRuleColor:
        return a.columnRuleColor() == b.columnRuleColor()
            && a.visitedLinkColumnRuleColor() == b.visitedLinkColumnRuleColor();
    case CSSPropertyWebkitColumnRuleWidth:
        return a.columnRuleWidth() == b.columnRuleWidth();
    case CSSPropertyWebkitColumnWidth:
        return a.columnWidth() == b.columnWidth();
    case CSSPropertyWebkitFilter:
        return a.filter() == b.filter();
    case CSSPropertyWebkitMaskBoxImageOutset:
        return a.maskBoxImageOutset() == b.maskBoxImageOutset();
    case CSSPropertyWebkitMaskBoxImageSlice:
        return a.maskBoxImageSlices() == b.maskBoxImageSlices();
    case CSSPropertyWebkitMaskBoxImageSource:
        return dataEquivalent(a.maskBoxImageSource(), b.maskBoxImageSource());
    case CSSPropertyWebkitMaskBoxImageWidth:
        return a.maskBoxImageWidth() == b.maskBoxImageWidth();
    case CSSPropertyWebkitMaskImage:
        return dataEquivalent(a.maskImage(), b.maskImage());
    case CSSPropertyWebkitMaskPositionX:
        return fillLayersEqual<CSSPropertyWebkitMaskPositionX>(a.maskLayers(), b.maskLayers());
    case CSSPropertyWebkitMaskPositionY:
        return fillLayersEqual<CSSPropertyWebkitMaskPositionY>(a.maskLayers(), b.maskLayers());
    case CSSPropertyWebkitMaskSize:
        return fillLayersEqual<CSSPropertyWebkitMaskSize>(a.maskLayers(), b.maskLayers());
    case CSSPropertyPerspective:
        return a.perspective() == b.perspective();
    case CSSPropertyPerspectiveOrigin:
        return a.perspectiveOriginX() == b.perspectiveOriginX() && a.perspectiveOriginY() == b.perspectiveOriginY();
    case CSSPropertyWebkitTextStrokeColor:
        return a.textStrokeColor() == b.textStrokeColor()
            && a.visitedLinkTextStrokeColor() == b.visitedLinkTextStrokeColor();
    case CSSPropertyTransform:
        return a.transform() == b.transform();
    case CSSPropertyTranslate:
        return dataEquivalent<TransformOperation>(a.translate(), b.translate());
    case CSSPropertyRotate:
        return dataEquivalent<TransformOperation>(a.rotate(), b.rotate());
    case CSSPropertyScale:
        return dataEquivalent<TransformOperation>(a.scale(), b.scale());
    case CSSPropertyTransformOrigin:
        return a.transformOriginX() == b.transformOriginX() && a.transformOriginY() == b.transformOriginY() && a.transformOriginZ() == b.transformOriginZ();
    case CSSPropertyWebkitPerspectiveOriginX:
        return a.perspectiveOriginX() == b.perspectiveOriginX();
    case CSSPropertyWebkitPerspectiveOriginY:
        return a.perspectiveOriginY() == b.perspectiveOriginY();
    case CSSPropertyWebkitTransformOriginX:
        return a.transformOriginX() == b.transformOriginX();
    case CSSPropertyWebkitTransformOriginY:
        return a.transformOriginY() == b.transformOriginY();
    case CSSPropertyWebkitTransformOriginZ:
        return a.transformOriginZ() == b.transformOriginZ();
    case CSSPropertyWidows:
        return a.widows() == b.widows();
    case CSSPropertyWidth:
        return a.width() == b.width();
    case CSSPropertyWordSpacing:
        return a.wordSpacing() == b.wordSpacing();
    case CSSPropertyCx:
        return a.svgStyle().cx() == b.svgStyle().cx();
    case CSSPropertyCy:
        return a.svgStyle().cy() == b.svgStyle().cy();
    case CSSPropertyX:
        return a.svgStyle().x() == b.svgStyle().x();
    case CSSPropertyY:
        return a.svgStyle().y() == b.svgStyle().y();
    case CSSPropertyR:
        return a.svgStyle().r() == b.svgStyle().r();
    case CSSPropertyRx:
        return a.svgStyle().rx() == b.svgStyle().rx();
    case CSSPropertyRy:
        return a.svgStyle().ry() == b.svgStyle().ry();
    case CSSPropertyZIndex:
        return a.hasAutoZIndex() == b.hasAutoZIndex() && (a.hasAutoZIndex() || a.zIndex() == b.zIndex());
    default:
        ASSERT_NOT_REACHED();
        return true;
    }
}

}
