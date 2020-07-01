// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/CSSPropertyMetadata.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/BitArray.h"

namespace blink {

bool CSSPropertyMetadata::isInterpolableProperty(CSSPropertyID property)
{
    switch(property) {
    case CSSPropertyInvalid:
        ASSERT_NOT_REACHED();
        return false;
    case CSSPropertyWebkitMaskBoxImageWidth:
    case CSSPropertyCx:
    case CSSPropertyCy:
    case CSSPropertyShapeOutside:
    case CSSPropertyTextDecorationColor:
    case CSSPropertyOutlineWidth:
    case CSSPropertyStrokeOpacity:
    case CSSPropertyWebkitColumnWidth:
    case CSSPropertyHeight:
    case CSSPropertyBorderBottomLeftRadius:
    case CSSPropertyWebkitMaskPositionY:
    case CSSPropertyWebkitMaskPositionX:
    case CSSPropertyShapeMargin:
    case CSSPropertyOutlineColor:
    case CSSPropertyPaddingBottom:
    case CSSPropertyStopOpacity:
    case CSSPropertyBorderTopRightRadius:
    case CSSPropertyFontWeight:
    case CSSPropertyShapeImageThreshold:
    case CSSPropertyRy:
    case CSSPropertyRx:
    case CSSPropertyMarginRight:
    case CSSPropertyWebkitColumnRuleColor:
    case CSSPropertyWebkitTextStrokeColor:
    case CSSPropertyOpacity:
    case CSSPropertyWebkitMaskBoxImageOutset:
    case CSSPropertyWebkitMaskBoxImageSource:
    case CSSPropertyMotionRotation:
    case CSSPropertyWebkitBorderHorizontalSpacing:
    case CSSPropertyStrokeDasharray:
    case CSSPropertyFlexBasis:
    case CSSPropertyWidows:
    case CSSPropertyLetterSpacing:
    case CSSPropertyWebkitTransformOriginZ:
    case CSSPropertyWebkitTransformOriginY:
    case CSSPropertyWebkitTransformOriginX:
    case CSSPropertyTransform:
    case CSSPropertyFill:
    case CSSPropertyStopColor:
    case CSSPropertyPerspectiveOrigin:
    case CSSPropertyWebkitColumnCount:
    case CSSPropertyBorderBottomRightRadius:
    case CSSPropertyBorderRightWidth:
    case CSSPropertyBorderTopLeftRadius:
    case CSSPropertyBorderLeftWidth:
    case CSSPropertyBackgroundImage:
    case CSSPropertyBorderImageSlice:
    case CSSPropertyFillOpacity:
    case CSSPropertyBorderTopWidth:
    case CSSPropertyBottom:
    case CSSPropertyTop:
    case CSSPropertyWebkitColumnGap:
    case CSSPropertyTransformOrigin:
    case CSSPropertyMarginTop:
    case CSSPropertyMaxWidth:
    case CSSPropertyTextIndent:
    case CSSPropertyTextShadow:
    case CSSPropertyBorderImageOutset:
    case CSSPropertyPaddingRight:
    case CSSPropertyPerspective:
    case CSSPropertyLineHeight:
    case CSSPropertyScale:
    case CSSPropertyStrokeDashoffset:
    case CSSPropertyBaselineShift:
    case CSSPropertyPaddingLeft:
    case CSSPropertyVerticalAlign:
    case CSSPropertyWordSpacing:
    case CSSPropertyBorderRightColor:
    case CSSPropertyStroke:
    case CSSPropertyLeft:
    case CSSPropertyWidth:
    case CSSPropertyWebkitClipPath:
    case CSSPropertyStrokeMiterlimit:
    case CSSPropertyMarginBottom:
    case CSSPropertyWebkitFilter:
    case CSSPropertyOutlineOffset:
    case CSSPropertyWebkitMaskSize:
    case CSSPropertyRotate:
    case CSSPropertyBorderImageWidth:
    case CSSPropertyFlexGrow:
    case CSSPropertyWebkitMaskImage:
    case CSSPropertyWebkitColumnRuleWidth:
    case CSSPropertyX:
    case CSSPropertyY:
    case CSSPropertyR:
    case CSSPropertyFontSizeAdjust:
    case CSSPropertyFlexShrink:
    case CSSPropertyRight:
    case CSSPropertyBackgroundSize:
    case CSSPropertyClip:
    case CSSPropertyColor:
    case CSSPropertyWebkitBorderVerticalSpacing:
    case CSSPropertyMarginLeft:
    case CSSPropertyMinHeight:
    case CSSPropertyBorderLeftColor:
    case CSSPropertyFloodColor:
    case CSSPropertyMaxHeight:
    case CSSPropertyWebkitMaskBoxImageSlice:
    case CSSPropertyBackgroundPositionY:
    case CSSPropertyBackgroundPositionX:
    case CSSPropertyMinWidth:
    case CSSPropertyBorderBottomWidth:
    case CSSPropertyBorderImageSource:
    case CSSPropertyMotionOffset:
    case CSSPropertyStrokeWidth:
    case CSSPropertyTranslate:
    case CSSPropertyBorderTopColor:
    case CSSPropertyBackgroundColor:
    case CSSPropertyPaddingTop:
    case CSSPropertyBoxShadow:
    case CSSPropertyZIndex:
    case CSSPropertyListStyleImage:
    case CSSPropertyFloodOpacity:
    case CSSPropertyLightingColor:
    case CSSPropertyObjectPosition:
    case CSSPropertyOrphans:
    case CSSPropertyWebkitPerspectiveOriginY:
    case CSSPropertyWebkitPerspectiveOriginX:
    case CSSPropertyBorderBottomColor:
    case CSSPropertyFontSize:
    case CSSPropertyVisibility:
        return true;
    default:
        return false;
    }
}

bool CSSPropertyMetadata::isInheritedProperty(CSSPropertyID property)
{
    switch(property) {
    case CSSPropertyInvalid:
        ASSERT_NOT_REACHED();
        return false;
    case CSSPropertyFillRule:
    //case CSSPropertyWebkitLineBoxContain:
    case CSSPropertyWebkitHyphenateCharacter:
    case CSSPropertyStrokeOpacity:
    case CSSPropertyFontFamily:
    case CSSPropertyPaintOrder:
    case CSSPropertyTextRendering:
    case CSSPropertyColorInterpolation:
    case CSSPropertyImageRendering:
    case CSSPropertyWebkitUserSelect:
    case CSSPropertyWebkitTextEmphasisStyle:
    case CSSPropertyQuotes:
    case CSSPropertyWebkitBoxDirection:
    case CSSPropertyWebkitUserModify:
    case CSSPropertyFontKerning:
    case CSSPropertyFontWeight:
    case CSSPropertyWebkitFontSmoothing:
    case CSSPropertyWebkitFontFeatureSettings:
    case CSSPropertyWebkitTextStrokeColor:
    case CSSPropertyWebkitBorderHorizontalSpacing:
    case CSSPropertyWebkitTapHighlightColor:
    case CSSPropertyCaptionSide:
    case CSSPropertyWebkitPrintColorAdjust:
    case CSSPropertyStrokeDasharray:
    case CSSPropertyWidows:
    case CSSPropertyLetterSpacing:
    case CSSPropertyWebkitRubyPosition:
    case CSSPropertyFill:
    case CSSPropertyTextJustify:
    case CSSPropertyTextAnchor:
    case CSSPropertyFontStyle:
    case CSSPropertyFontVariant:
    case CSSPropertyWritingMode:
    case CSSPropertyWebkitTextSecurity:
    case CSSPropertyWebkitLineBreak:
    //case CSSPropertyGlyphOrientationHorizontal:
    case CSSPropertyFillOpacity:
    case CSSPropertyBorderCollapse:
    case CSSPropertyWebkitRtlOrdering:
    case CSSPropertyWordWrap:
    case CSSPropertyTextIndent:
    case CSSPropertyTextShadow:
    case CSSPropertyLineHeight:
    case CSSPropertyWebkitTextStrokeWidth:
    case CSSPropertyStrokeDashoffset:
    //case CSSPropertyGlyphOrientationVertical:
    case CSSPropertyWebkitTextDecorationsInEffect:
    case CSSPropertyWebkitTextFillColor:
    case CSSPropertyFontStretch:
    case CSSPropertyWhiteSpace:
    case CSSPropertyOverflowWrap:
    case CSSPropertyWebkitLocale:
    case CSSPropertyWebkitTextEmphasisPosition:
    case CSSPropertyWordSpacing:
    case CSSPropertyMarkerEnd:
    case CSSPropertyStroke:
    case CSSPropertyTextUnderlinePosition:
    case CSSPropertyStrokeMiterlimit:
    case CSSPropertyListStylePosition:
    case CSSPropertyShapeRendering:
    case CSSPropertyColorRendering:
    case CSSPropertyStrokeLinejoin:
    case CSSPropertyPointerEvents:
    case CSSPropertyStrokeLinecap:
    case CSSPropertyFontVariantLigatures:
    case CSSPropertyMarkerMid:
    case CSSPropertyWebkitTextCombine:
    case CSSPropertyFontSizeAdjust:
    case CSSPropertyWebkitTextOrientation:
    case CSSPropertyColorInterpolationFilters:
    case CSSPropertyTextTransform:
    case CSSPropertyDirection:
    case CSSPropertyColor:
    case CSSPropertyWebkitBorderVerticalSpacing:
    case CSSPropertyWebkitWritingMode:
    case CSSPropertyTextAlignLast:
    case CSSPropertyCursor:
    case CSSPropertyTabSize:
    case CSSPropertyWebkitHighlight:
    case CSSPropertyEmptyCells:
    case CSSPropertyStrokeWidth:
    case CSSPropertyMarkerStart:
    case CSSPropertyWebkitTextEmphasisColor:
    case CSSPropertyWordBreak:
    case CSSPropertySpeak:
    case CSSPropertyListStyleImage:
    case CSSPropertyClipRule:
    case CSSPropertyListStyleType:
    case CSSPropertyTextAlign:
    case CSSPropertyOrphans:
    case CSSPropertyFontSize:
    case CSSPropertyVisibility:
        return true;
    default:
        return false;
    }
}

bool CSSPropertyMetadata::isEnabledProperty(CSSPropertyID unresolvedProperty)
{
    CSSPropertyID property = resolveCSSPropertyID(unresolvedProperty);
    static BitArray<numCSSProperties>* enabledProperties = 0;
    if (!enabledProperties) {
        enabledProperties = new BitArray<numCSSProperties>(true); // All bits sets to 1.
        if (!RuntimeEnabledFeatures::css3TextDecorationsEnabled())
            enabledProperties->clear(CSSPropertyTextDecorationColor - firstCSSProperty);
//         if (!RuntimeEnabledFeatures::cssMotionPathEnabled())
//             enabledProperties->clear(CSSPropertyMotion - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridTemplate - firstCSSProperty);
        if (!RuntimeEnabledFeatures::css3TextDecorationsEnabled())
            enabledProperties->clear(CSSPropertyTextDecorationLine - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssScrollSnapPointsEnabled())
            enabledProperties->clear(CSSPropertyScrollSnapType - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridAutoRows - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridColumn - firstCSSProperty);
//         if (!RuntimeEnabledFeatures::cssMotionPathEnabled())
//             enabledProperties->clear(CSSPropertyMotionRotation - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGrid - firstCSSProperty);
//         if (!RuntimeEnabledFeatures::cssScrollBlocksOnEnabled())
//             enabledProperties->clear(CSSPropertyScrollBlocksOn - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridTemplateRows - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssomSmoothScrollEnabled())
            enabledProperties->clear(CSSPropertyScrollBehavior - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridAutoFlow - firstCSSProperty);
//         if (!RuntimeEnabledFeatures::cssMotionPathEnabled())
//             enabledProperties->clear(CSSPropertyMotionPath - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyJustifySelf - firstCSSProperty);
        if (!RuntimeEnabledFeatures::css3TextEnabled())
            enabledProperties->clear(CSSPropertyTextJustify - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssCompositingEnabled())
            enabledProperties->clear(CSSPropertyIsolation - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssScrollSnapPointsEnabled())
            enabledProperties->clear(CSSPropertyScrollSnapPointsX - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssScrollSnapPointsEnabled())
            enabledProperties->clear(CSSPropertyScrollSnapPointsY - firstCSSProperty);
        if (!RuntimeEnabledFeatures::columnFillEnabled())
            enabledProperties->clear(CSSPropertyColumnFill - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridTemplateColumns - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssMaskSourceTypeEnabled())
            enabledProperties->clear(CSSPropertyMaskSourceType - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled())
            enabledProperties->clear(CSSPropertyScale - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridAutoColumns - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridRowStart - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssCompositingEnabled())
            enabledProperties->clear(CSSPropertyMixBlendMode - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridRowEnd - firstCSSProperty);
        if (!RuntimeEnabledFeatures::css3TextDecorationsEnabled())
            enabledProperties->clear(CSSPropertyTextUnderlinePosition - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridRow - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridColumnStart - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyJustifyItems - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled())
            enabledProperties->clear(CSSPropertyRotate - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridTemplateAreas - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssFontSizeAdjustEnabled())
            enabledProperties->clear(CSSPropertyFontSizeAdjust - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridColumnEnd - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssScrollSnapPointsEnabled())
            enabledProperties->clear(CSSPropertyScrollSnapCoordinate - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssGridLayoutEnabled())
            enabledProperties->clear(CSSPropertyGridArea - firstCSSProperty);
        if (!RuntimeEnabledFeatures::css3TextEnabled())
            enabledProperties->clear(CSSPropertyTextAlignLast - firstCSSProperty);
//         if (!RuntimeEnabledFeatures::cssMotionPathEnabled())
//             enabledProperties->clear(CSSPropertyMotionOffset - firstCSSProperty);
        if (!RuntimeEnabledFeatures::css3TextDecorationsEnabled())
            enabledProperties->clear(CSSPropertyTextDecorationStyle - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled())
            enabledProperties->clear(CSSPropertyTranslate - firstCSSProperty);
        if (!RuntimeEnabledFeatures::cssScrollSnapPointsEnabled())
            enabledProperties->clear(CSSPropertyScrollSnapDestination - firstCSSProperty);
    }

	if (unresolvedProperty >= firstCSSProperty)
		return enabledProperties->get(property - firstCSSProperty);

	if (unresolvedProperty == CSSPropertyVariable)
		return true;

	return false;
}

void CSSPropertyMetadata::filterEnabledCSSPropertiesIntoVector(const CSSPropertyID* properties, size_t propertyCount, Vector<CSSPropertyID>& outVector)
{
    for (unsigned i = 0; i < propertyCount; i++) {
        CSSPropertyID property = properties[i];
        if (isEnabledProperty(property))
            outVector.append(property);
    }
}

} // namespace blink
