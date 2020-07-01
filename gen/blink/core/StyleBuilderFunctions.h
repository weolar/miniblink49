// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleBuilderFunctions_h
#define StyleBuilderFunctions_h

#include "core/css/resolver/StyleBuilderConverter.h"

namespace blink {

class CSSValue;
class StyleResolverState;

class StyleBuilderFunctions {
public:

    static void applyInitialCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertySize(StyleResolverState&);
    static void applyInheritCSSPropertySize(StyleResolverState&);
    static void applyValueCSSPropertySize(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyVectorEffect(StyleResolverState&);
    static void applyInheritCSSPropertyVectorEffect(StyleResolverState&);
    static void applyValueCSSPropertyVectorEffect(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFillRule(StyleResolverState&);
    static void applyInheritCSSPropertyFillRule(StyleResolverState&);
    static void applyValueCSSPropertyFillRule(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitLineBoxContain(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitLineBoxContain(StyleResolverState&);
    static void applyValueCSSPropertyWebkitLineBoxContain(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyCx(StyleResolverState&);
    static void applyInheritCSSPropertyCx(StyleResolverState&);
    static void applyValueCSSPropertyCx(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyCy(StyleResolverState&);
    static void applyInheritCSSPropertyCy(StyleResolverState&);
    static void applyValueCSSPropertyCy(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransitionDelay(StyleResolverState&);
    static void applyInheritCSSPropertyTransitionDelay(StyleResolverState&);
    static void applyValueCSSPropertyTransitionDelay(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyShapeOutside(StyleResolverState&);
    static void applyInheritCSSPropertyShapeOutside(StyleResolverState&);
    static void applyValueCSSPropertyShapeOutside(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextDecorationColor(StyleResolverState&);
    static void applyInheritCSSPropertyTextDecorationColor(StyleResolverState&);
    static void applyValueCSSPropertyTextDecorationColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitAppRegion(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitAppRegion(StyleResolverState&);
    static void applyValueCSSPropertyWebkitAppRegion(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationName(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationName(StyleResolverState&);
    static void applyValueCSSPropertyAnimationName(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitHyphenateCharacter(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitHyphenateCharacter(StyleResolverState&);
    static void applyValueCSSPropertyWebkitHyphenateCharacter(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxPack(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxPack(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxPack(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMarginBottomCollapse(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMarginBottomCollapse(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMarginBottomCollapse(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOutlineWidth(StyleResolverState&);
    static void applyInheritCSSPropertyOutlineWidth(StyleResolverState&);
    static void applyValueCSSPropertyOutlineWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeOpacity(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeOpacity(StyleResolverState&);
    static void applyValueCSSPropertyStrokeOpacity(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnWidth(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnWidth(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontFamily(StyleResolverState&);
    static void applyInheritCSSPropertyFontFamily(StyleResolverState&);
    static void applyValueCSSPropertyFontFamily(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxFlex(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxFlex(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxFlex(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationDuration(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationDuration(StyleResolverState&);
    static void applyValueCSSPropertyAnimationDuration(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationPlayState(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationPlayState(StyleResolverState&);
    static void applyValueCSSPropertyAnimationPlayState(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskComposite(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskComposite(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskComposite(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitAppearance(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitAppearance(StyleResolverState&);
    static void applyValueCSSPropertyWebkitAppearance(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderBottomStyle(StyleResolverState&);
    static void applyInheritCSSPropertyBorderBottomStyle(StyleResolverState&);
    static void applyValueCSSPropertyBorderBottomStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyHeight(StyleResolverState&);
    static void applyInheritCSSPropertyHeight(StyleResolverState&);
    static void applyValueCSSPropertyHeight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPaintOrder(StyleResolverState&);
    static void applyInheritCSSPropertyPaintOrder(StyleResolverState&);
    static void applyValueCSSPropertyPaintOrder(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextRendering(StyleResolverState&);
    static void applyInheritCSSPropertyTextRendering(StyleResolverState&);
    static void applyValueCSSPropertyTextRendering(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnBreakAfter(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnBreakAfter(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnBreakAfter(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderBottomLeftRadius(StyleResolverState&);
    static void applyInheritCSSPropertyBorderBottomLeftRadius(StyleResolverState&);
    static void applyValueCSSPropertyBorderBottomLeftRadius(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyColorInterpolation(StyleResolverState&);
    static void applyInheritCSSPropertyColorInterpolation(StyleResolverState&);
    static void applyValueCSSPropertyColorInterpolation(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskPositionY(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskPositionY(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskPositionY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskPositionX(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskPositionX(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskPositionX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextDecorationLine(StyleResolverState&);
    static void applyInheritCSSPropertyTextDecorationLine(StyleResolverState&);
    static void applyValueCSSPropertyTextDecorationLine(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyContent(StyleResolverState&);
    static void applyInheritCSSPropertyContent(StyleResolverState&);
    static void applyValueCSSPropertyContent(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnSpan(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnSpan(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnSpan(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyShapeMargin(StyleResolverState&);
    static void applyInheritCSSPropertyShapeMargin(StyleResolverState&);
    static void applyValueCSSPropertyShapeMargin(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOutlineColor(StyleResolverState&);
    static void applyInheritCSSPropertyOutlineColor(StyleResolverState&);
    static void applyValueCSSPropertyOutlineColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollSnapType(StyleResolverState&);
    static void applyInheritCSSPropertyScrollSnapType(StyleResolverState&);
    static void applyValueCSSPropertyScrollSnapType(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyImageRendering(StyleResolverState&);
    static void applyInheritCSSPropertyImageRendering(StyleResolverState&);
    static void applyValueCSSPropertyImageRendering(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitUserSelect(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitUserSelect(StyleResolverState&);
    static void applyValueCSSPropertyWebkitUserSelect(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextEmphasisStyle(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextEmphasisStyle(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextEmphasisStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPaddingBottom(StyleResolverState&);
    static void applyInheritCSSPropertyPaddingBottom(StyleResolverState&);
    static void applyValueCSSPropertyPaddingBottom(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyQuotes(StyleResolverState&);
    static void applyInheritCSSPropertyQuotes(StyleResolverState&);
    static void applyValueCSSPropertyQuotes(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridAutoRows(StyleResolverState&);
    static void applyInheritCSSPropertyGridAutoRows(StyleResolverState&);
    static void applyValueCSSPropertyGridAutoRows(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyClipPath(StyleResolverState&);
    static void applyInheritCSSPropertyClipPath(StyleResolverState&);
    static void applyValueCSSPropertyClipPath(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyObjectFit(StyleResolverState&);
    static void applyInheritCSSPropertyObjectFit(StyleResolverState&);
    static void applyValueCSSPropertyObjectFit(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStopOpacity(StyleResolverState&);
    static void applyInheritCSSPropertyStopOpacity(StyleResolverState&);
    static void applyValueCSSPropertyStopOpacity(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxReflect(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxReflect(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxReflect(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderTopRightRadius(StyleResolverState&);
    static void applyInheritCSSPropertyBorderTopRightRadius(StyleResolverState&);
    static void applyValueCSSPropertyBorderTopRightRadius(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxDirection(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxDirection(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxDirection(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitUserModify(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitUserModify(StyleResolverState&);
    static void applyValueCSSPropertyWebkitUserModify(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxDecorationBreak(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxDecorationBreak(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxDecorationBreak(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontKerning(StyleResolverState&);
    static void applyInheritCSSPropertyFontKerning(StyleResolverState&);
    static void applyValueCSSPropertyFontKerning(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontWeight(StyleResolverState&);
    static void applyInheritCSSPropertyFontWeight(StyleResolverState&);
    static void applyValueCSSPropertyFontWeight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyShapeImageThreshold(StyleResolverState&);
    static void applyInheritCSSPropertyShapeImageThreshold(StyleResolverState&);
    static void applyValueCSSPropertyShapeImageThreshold(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyRy(StyleResolverState&);
    static void applyInheritCSSPropertyRy(StyleResolverState&);
    static void applyValueCSSPropertyRy(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyRx(StyleResolverState&);
    static void applyInheritCSSPropertyRx(StyleResolverState&);
    static void applyValueCSSPropertyRx(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarginRight(StyleResolverState&);
    static void applyInheritCSSPropertyMarginRight(StyleResolverState&);
    static void applyValueCSSPropertyMarginRight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitFontSmoothing(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitFontSmoothing(StyleResolverState&);
    static void applyValueCSSPropertyWebkitFontSmoothing(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPageBreakBefore(StyleResolverState&);
    static void applyInheritCSSPropertyPageBreakBefore(StyleResolverState&);
    static void applyValueCSSPropertyPageBreakBefore(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationDelay(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationDelay(StyleResolverState&);
    static void applyValueCSSPropertyAnimationDelay(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnRuleColor(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnRuleColor(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnRuleColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitFontFeatureSettings(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitFontFeatureSettings(StyleResolverState&);
    static void applyValueCSSPropertyWebkitFontFeatureSettings(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextStrokeColor(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextStrokeColor(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextStrokeColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOpacity(StyleResolverState&);
    static void applyInheritCSSPropertyOpacity(StyleResolverState&);
    static void applyValueCSSPropertyOpacity(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyDominantBaseline(StyleResolverState&);
    static void applyInheritCSSPropertyDominantBaseline(StyleResolverState&);
    static void applyValueCSSPropertyDominantBaseline(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAlignSelf(StyleResolverState&);
    static void applyInheritCSSPropertyAlignSelf(StyleResolverState&);
    static void applyValueCSSPropertyAlignSelf(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyClear(StyleResolverState&);
    static void applyInheritCSSPropertyClear(StyleResolverState&);
    static void applyValueCSSPropertyClear(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnBreakInside(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnBreakInside(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnBreakInside(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskBoxImageSource(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskBoxImageSource(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskBoxImageSource(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransitionProperty(StyleResolverState&);
    static void applyInheritCSSPropertyTransitionProperty(StyleResolverState&);
    static void applyValueCSSPropertyTransitionProperty(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransformStyle(StyleResolverState&);
    static void applyInheritCSSPropertyTransformStyle(StyleResolverState&);
    static void applyValueCSSPropertyTransformStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMotionRotation(StyleResolverState&);
    static void applyInheritCSSPropertyMotionRotation(StyleResolverState&);
    static void applyValueCSSPropertyMotionRotation(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTapHighlightColor(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTapHighlightColor(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTapHighlightColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyCaptionSide(StyleResolverState&);
    static void applyInheritCSSPropertyCaptionSide(StyleResolverState&);
    static void applyValueCSSPropertyCaptionSide(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitPrintColorAdjust(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitPrintColorAdjust(StyleResolverState&);
    static void applyValueCSSPropertyWebkitPrintColorAdjust(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollBlocksOn(StyleResolverState&);
    static void applyInheritCSSPropertyScrollBlocksOn(StyleResolverState&);
    static void applyValueCSSPropertyScrollBlocksOn(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeDasharray(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeDasharray(StyleResolverState&);
    static void applyValueCSSPropertyStrokeDasharray(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFlexBasis(StyleResolverState&);
    static void applyInheritCSSPropertyFlexBasis(StyleResolverState&);
    static void applyValueCSSPropertyFlexBasis(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWidows(StyleResolverState&);
    static void applyInheritCSSPropertyWidows(StyleResolverState&);
    static void applyValueCSSPropertyWidows(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyLetterSpacing(StyleResolverState&);
    static void applyInheritCSSPropertyLetterSpacing(StyleResolverState&);
    static void applyValueCSSPropertyLetterSpacing(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitRubyPosition(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitRubyPosition(StyleResolverState&);
    static void applyValueCSSPropertyWebkitRubyPosition(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTransformOriginZ(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTransformOriginZ(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTransformOriginZ(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTransformOriginY(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTransformOriginY(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTransformOriginY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTransformOriginX(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTransformOriginX(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTransformOriginX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridTemplateRows(StyleResolverState&);
    static void applyInheritCSSPropertyGridTemplateRows(StyleResolverState&);
    static void applyValueCSSPropertyGridTemplateRows(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransform(StyleResolverState&);
    static void applyInheritCSSPropertyTransform(StyleResolverState&);
    static void applyValueCSSPropertyTransform(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskOrigin(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskOrigin(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskOrigin(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollBehavior(StyleResolverState&);
    static void applyInheritCSSPropertyScrollBehavior(StyleResolverState&);
    static void applyValueCSSPropertyScrollBehavior(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFill(StyleResolverState&);
    static void applyInheritCSSPropertyFill(StyleResolverState&);
    static void applyValueCSSPropertyFill(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridAutoFlow(StyleResolverState&);
    static void applyInheritCSSPropertyGridAutoFlow(StyleResolverState&);
    static void applyValueCSSPropertyGridAutoFlow(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMotionPath(StyleResolverState&);
    static void applyInheritCSSPropertyMotionPath(StyleResolverState&);
    static void applyValueCSSPropertyMotionPath(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStopColor(StyleResolverState&);
    static void applyInheritCSSPropertyStopColor(StyleResolverState&);
    static void applyValueCSSPropertyStopColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitLineClamp(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitLineClamp(StyleResolverState&);
    static void applyValueCSSPropertyWebkitLineClamp(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyJustifySelf(StyleResolverState&);
    static void applyInheritCSSPropertyJustifySelf(StyleResolverState&);
    static void applyValueCSSPropertyJustifySelf(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextJustify(StyleResolverState&);
    static void applyInheritCSSPropertyTextJustify(StyleResolverState&);
    static void applyValueCSSPropertyTextJustify(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyZoom(StyleResolverState&);
    static void applyInheritCSSPropertyZoom(StyleResolverState&);
    static void applyValueCSSPropertyZoom(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPerspectiveOrigin(StyleResolverState&);
    static void applyInheritCSSPropertyPerspectiveOrigin(StyleResolverState&);
    static void applyValueCSSPropertyPerspectiveOrigin(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextAnchor(StyleResolverState&);
    static void applyInheritCSSPropertyTextAnchor(StyleResolverState&);
    static void applyValueCSSPropertyTextAnchor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnCount(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnCount(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnCount(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontStyle(StyleResolverState&);
    static void applyInheritCSSPropertyFontStyle(StyleResolverState&);
    static void applyValueCSSPropertyFontStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderBottomRightRadius(StyleResolverState&);
    static void applyInheritCSSPropertyBorderBottomRightRadius(StyleResolverState&);
    static void applyValueCSSPropertyBorderBottomRightRadius(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderRightWidth(StyleResolverState&);
    static void applyInheritCSSPropertyBorderRightWidth(StyleResolverState&);
    static void applyValueCSSPropertyBorderRightWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderTopLeftRadius(StyleResolverState&);
    static void applyInheritCSSPropertyBorderTopLeftRadius(StyleResolverState&);
    static void applyValueCSSPropertyBorderTopLeftRadius(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontVariant(StyleResolverState&);
    static void applyInheritCSSPropertyFontVariant(StyleResolverState&);
    static void applyValueCSSPropertyFontVariant(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWritingMode(StyleResolverState&);
    static void applyInheritCSSPropertyWritingMode(StyleResolverState&);
    static void applyValueCSSPropertyWritingMode(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundAttachment(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundAttachment(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundAttachment(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextSecurity(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextSecurity(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextSecurity(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderLeftWidth(StyleResolverState&);
    static void applyInheritCSSPropertyBorderLeftWidth(StyleResolverState&);
    static void applyValueCSSPropertyBorderLeftWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitLineBreak(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitLineBreak(StyleResolverState&);
    static void applyValueCSSPropertyWebkitLineBreak(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundImage(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundImage(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundImage(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskClip(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskClip(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskClip(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyCounterReset(StyleResolverState&);
    static void applyInheritCSSPropertyCounterReset(StyleResolverState&);
    static void applyValueCSSPropertyCounterReset(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderImageSlice(StyleResolverState&);
    static void applyInheritCSSPropertyBorderImageSlice(StyleResolverState&);
    static void applyValueCSSPropertyBorderImageSlice(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyIsolation(StyleResolverState&);
    static void applyInheritCSSPropertyIsolation(StyleResolverState&);
    static void applyValueCSSPropertyIsolation(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGlyphOrientationHorizontal(StyleResolverState&);
    static void applyInheritCSSPropertyGlyphOrientationHorizontal(StyleResolverState&);
    static void applyValueCSSPropertyGlyphOrientationHorizontal(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFillOpacity(StyleResolverState&);
    static void applyInheritCSSPropertyFillOpacity(StyleResolverState&);
    static void applyValueCSSPropertyFillOpacity(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderTopWidth(StyleResolverState&);
    static void applyInheritCSSPropertyBorderTopWidth(StyleResolverState&);
    static void applyValueCSSPropertyBorderTopWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBottom(StyleResolverState&);
    static void applyInheritCSSPropertyBottom(StyleResolverState&);
    static void applyValueCSSPropertyBottom(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderCollapse(StyleResolverState&);
    static void applyInheritCSSPropertyBorderCollapse(StyleResolverState&);
    static void applyValueCSSPropertyBorderCollapse(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTop(StyleResolverState&);
    static void applyInheritCSSPropertyTop(StyleResolverState&);
    static void applyValueCSSPropertyTop(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollSnapPointsX(StyleResolverState&);
    static void applyInheritCSSPropertyScrollSnapPointsX(StyleResolverState&);
    static void applyValueCSSPropertyScrollSnapPointsX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollSnapPointsY(StyleResolverState&);
    static void applyInheritCSSPropertyScrollSnapPointsY(StyleResolverState&);
    static void applyValueCSSPropertyScrollSnapPointsY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnGap(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnGap(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnGap(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyUnicodeBidi(StyleResolverState&);
    static void applyInheritCSSPropertyUnicodeBidi(StyleResolverState&);
    static void applyValueCSSPropertyUnicodeBidi(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationIterationCount(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationIterationCount(StyleResolverState&);
    static void applyValueCSSPropertyAnimationIterationCount(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitRtlOrdering(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitRtlOrdering(StyleResolverState&);
    static void applyValueCSSPropertyWebkitRtlOrdering(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFloat(StyleResolverState&);
    static void applyInheritCSSPropertyFloat(StyleResolverState&);
    static void applyValueCSSPropertyFloat(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWordWrap(StyleResolverState&);
    static void applyInheritCSSPropertyWordWrap(StyleResolverState&);
    static void applyValueCSSPropertyWordWrap(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransformOrigin(StyleResolverState&);
    static void applyInheritCSSPropertyTransformOrigin(StyleResolverState&);
    static void applyValueCSSPropertyTransformOrigin(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarginTop(StyleResolverState&);
    static void applyInheritCSSPropertyMarginTop(StyleResolverState&);
    static void applyValueCSSPropertyMarginTop(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMaxWidth(StyleResolverState&);
    static void applyInheritCSSPropertyMaxWidth(StyleResolverState&);
    static void applyValueCSSPropertyMaxWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextIndent(StyleResolverState&);
    static void applyInheritCSSPropertyTextIndent(StyleResolverState&);
    static void applyValueCSSPropertyTextIndent(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextShadow(StyleResolverState&);
    static void applyInheritCSSPropertyTextShadow(StyleResolverState&);
    static void applyValueCSSPropertyTextShadow(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderImageOutset(StyleResolverState&);
    static void applyInheritCSSPropertyBorderImageOutset(StyleResolverState&);
    static void applyValueCSSPropertyBorderImageOutset(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPaddingRight(StyleResolverState&);
    static void applyInheritCSSPropertyPaddingRight(StyleResolverState&);
    static void applyValueCSSPropertyPaddingRight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderImageRepeat(StyleResolverState&);
    static void applyInheritCSSPropertyBorderImageRepeat(StyleResolverState&);
    static void applyValueCSSPropertyBorderImageRepeat(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyColumnFill(StyleResolverState&);
    static void applyInheritCSSPropertyColumnFill(StyleResolverState&);
    static void applyValueCSSPropertyColumnFill(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransitionTimingFunction(StyleResolverState&);
    static void applyInheritCSSPropertyTransitionTimingFunction(StyleResolverState&);
    static void applyValueCSSPropertyTransitionTimingFunction(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOverflowX(StyleResolverState&);
    static void applyInheritCSSPropertyOverflowX(StyleResolverState&);
    static void applyValueCSSPropertyOverflowX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOverflowY(StyleResolverState&);
    static void applyInheritCSSPropertyOverflowY(StyleResolverState&);
    static void applyValueCSSPropertyOverflowY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridTemplateColumns(StyleResolverState&);
    static void applyInheritCSSPropertyGridTemplateColumns(StyleResolverState&);
    static void applyValueCSSPropertyGridTemplateColumns(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPerspective(StyleResolverState&);
    static void applyInheritCSSPropertyPerspective(StyleResolverState&);
    static void applyValueCSSPropertyPerspective(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyLineHeight(StyleResolverState&);
    static void applyInheritCSSPropertyLineHeight(StyleResolverState&);
    static void applyValueCSSPropertyLineHeight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMaskSourceType(StyleResolverState&);
    static void applyInheritCSSPropertyMaskSourceType(StyleResolverState&);
    static void applyValueCSSPropertyMaskSourceType(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundRepeatY(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundRepeatY(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundRepeatY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOrder(StyleResolverState&);
    static void applyInheritCSSPropertyOrder(StyleResolverState&);
    static void applyValueCSSPropertyOrder(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxAlign(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxAlign(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxAlign(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScale(StyleResolverState&);
    static void applyInheritCSSPropertyScale(StyleResolverState&);
    static void applyValueCSSPropertyScale(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridAutoColumns(StyleResolverState&);
    static void applyInheritCSSPropertyGridAutoColumns(StyleResolverState&);
    static void applyValueCSSPropertyGridAutoColumns(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridRowStart(StyleResolverState&);
    static void applyInheritCSSPropertyGridRowStart(StyleResolverState&);
    static void applyValueCSSPropertyGridRowStart(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTransitionDuration(StyleResolverState&);
    static void applyInheritCSSPropertyTransitionDuration(StyleResolverState&);
    static void applyValueCSSPropertyTransitionDuration(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextStrokeWidth(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextStrokeWidth(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextStrokeWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeDashoffset(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeDashoffset(StyleResolverState&);
    static void applyValueCSSPropertyStrokeDashoffset(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPosition(StyleResolverState&);
    static void applyInheritCSSPropertyPosition(StyleResolverState&);
    static void applyValueCSSPropertyPosition(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGlyphOrientationVertical(StyleResolverState&);
    static void applyInheritCSSPropertyGlyphOrientationVertical(StyleResolverState&);
    static void applyValueCSSPropertyGlyphOrientationVertical(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextFillColor(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextFillColor(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextFillColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontStretch(StyleResolverState&);
    static void applyInheritCSSPropertyFontStretch(StyleResolverState&);
    static void applyValueCSSPropertyFontStretch(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMixBlendMode(StyleResolverState&);
    static void applyInheritCSSPropertyMixBlendMode(StyleResolverState&);
    static void applyValueCSSPropertyMixBlendMode(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBaselineShift(StyleResolverState&);
    static void applyInheritCSSPropertyBaselineShift(StyleResolverState&);
    static void applyValueCSSPropertyBaselineShift(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPaddingLeft(StyleResolverState&);
    static void applyInheritCSSPropertyPaddingLeft(StyleResolverState&);
    static void applyValueCSSPropertyPaddingLeft(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWhiteSpace(StyleResolverState&);
    static void applyInheritCSSPropertyWhiteSpace(StyleResolverState&);
    static void applyValueCSSPropertyWhiteSpace(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundClip(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundClip(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundClip(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOverflowWrap(StyleResolverState&);
    static void applyInheritCSSPropertyOverflowWrap(StyleResolverState&);
    static void applyValueCSSPropertyOverflowWrap(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyVerticalAlign(StyleResolverState&);
    static void applyInheritCSSPropertyVerticalAlign(StyleResolverState&);
    static void applyValueCSSPropertyVerticalAlign(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitLocale(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitLocale(StyleResolverState&);
    static void applyValueCSSPropertyWebkitLocale(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMarginAfterCollapse(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMarginAfterCollapse(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMarginAfterCollapse(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextEmphasisPosition(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextEmphasisPosition(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextEmphasisPosition(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWordSpacing(StyleResolverState&);
    static void applyInheritCSSPropertyWordSpacing(StyleResolverState&);
    static void applyValueCSSPropertyWordSpacing(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPageBreakAfter(StyleResolverState&);
    static void applyInheritCSSPropertyPageBreakAfter(StyleResolverState&);
    static void applyValueCSSPropertyPageBreakAfter(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOutlineStyle(StyleResolverState&);
    static void applyInheritCSSPropertyOutlineStyle(StyleResolverState&);
    static void applyValueCSSPropertyOutlineStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarkerEnd(StyleResolverState&);
    static void applyInheritCSSPropertyMarkerEnd(StyleResolverState&);
    static void applyValueCSSPropertyMarkerEnd(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderRightColor(StyleResolverState&);
    static void applyInheritCSSPropertyBorderRightColor(StyleResolverState&);
    static void applyValueCSSPropertyBorderRightColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxLines(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxLines(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxLines(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTableLayout(StyleResolverState&);
    static void applyInheritCSSPropertyTableLayout(StyleResolverState&);
    static void applyValueCSSPropertyTableLayout(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStroke(StyleResolverState&);
    static void applyInheritCSSPropertyStroke(StyleResolverState&);
    static void applyValueCSSPropertyStroke(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderRightStyle(StyleResolverState&);
    static void applyInheritCSSPropertyBorderRightStyle(StyleResolverState&);
    static void applyValueCSSPropertyBorderRightStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridRowEnd(StyleResolverState&);
    static void applyInheritCSSPropertyGridRowEnd(StyleResolverState&);
    static void applyValueCSSPropertyGridRowEnd(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextUnderlinePosition(StyleResolverState&);
    static void applyInheritCSSPropertyTextUnderlinePosition(StyleResolverState&);
    static void applyValueCSSPropertyTextUnderlinePosition(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackfaceVisibility(StyleResolverState&);
    static void applyInheritCSSPropertyBackfaceVisibility(StyleResolverState&);
    static void applyValueCSSPropertyBackfaceVisibility(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyLeft(StyleResolverState&);
    static void applyInheritCSSPropertyLeft(StyleResolverState&);
    static void applyValueCSSPropertyLeft(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWidth(StyleResolverState&);
    static void applyInheritCSSPropertyWidth(StyleResolverState&);
    static void applyValueCSSPropertyWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTouchAction(StyleResolverState&);
    static void applyInheritCSSPropertyTouchAction(StyleResolverState&);
    static void applyValueCSSPropertyTouchAction(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBackgroundComposite(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBackgroundComposite(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBackgroundComposite(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitClipPath(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitClipPath(StyleResolverState&);
    static void applyValueCSSPropertyWebkitClipPath(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeMiterlimit(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeMiterlimit(StyleResolverState&);
    static void applyValueCSSPropertyStrokeMiterlimit(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridColumnStart(StyleResolverState&);
    static void applyInheritCSSPropertyGridColumnStart(StyleResolverState&);
    static void applyValueCSSPropertyGridColumnStart(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarginBottom(StyleResolverState&);
    static void applyInheritCSSPropertyMarginBottom(StyleResolverState&);
    static void applyValueCSSPropertyMarginBottom(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyListStylePosition(StyleResolverState&);
    static void applyInheritCSSPropertyListStylePosition(StyleResolverState&);
    static void applyValueCSSPropertyListStylePosition(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitFilter(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitFilter(StyleResolverState&);
    static void applyValueCSSPropertyWebkitFilter(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOutlineOffset(StyleResolverState&);
    static void applyInheritCSSPropertyOutlineOffset(StyleResolverState&);
    static void applyValueCSSPropertyOutlineOffset(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyShapeRendering(StyleResolverState&);
    static void applyInheritCSSPropertyShapeRendering(StyleResolverState&);
    static void applyValueCSSPropertyShapeRendering(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskSize(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskSize(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskSize(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyJustifyItems(StyleResolverState&);
    static void applyInheritCSSPropertyJustifyItems(StyleResolverState&);
    static void applyValueCSSPropertyJustifyItems(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyRotate(StyleResolverState&);
    static void applyInheritCSSPropertyRotate(StyleResolverState&);
    static void applyValueCSSPropertyRotate(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyColorRendering(StyleResolverState&);
    static void applyInheritCSSPropertyColorRendering(StyleResolverState&);
    static void applyValueCSSPropertyColorRendering(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeLinejoin(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeLinejoin(StyleResolverState&);
    static void applyValueCSSPropertyStrokeLinejoin(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderImageWidth(StyleResolverState&);
    static void applyInheritCSSPropertyBorderImageWidth(StyleResolverState&);
    static void applyValueCSSPropertyBorderImageWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFlexGrow(StyleResolverState&);
    static void applyInheritCSSPropertyFlexGrow(StyleResolverState&);
    static void applyValueCSSPropertyFlexGrow(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMask(StyleResolverState&);
    static void applyInheritCSSPropertyMask(StyleResolverState&);
    static void applyValueCSSPropertyMask(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskImage(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskImage(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskImage(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationTimingFunction(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationTimingFunction(StyleResolverState&);
    static void applyValueCSSPropertyAnimationTimingFunction(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPointerEvents(StyleResolverState&);
    static void applyInheritCSSPropertyPointerEvents(StyleResolverState&);
    static void applyValueCSSPropertyPointerEvents(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxOrient(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxOrient(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxOrient(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeLinecap(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeLinecap(StyleResolverState&);
    static void applyValueCSSPropertyStrokeLinecap(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderLeftStyle(StyleResolverState&);
    static void applyInheritCSSPropertyBorderLeftStyle(StyleResolverState&);
    static void applyValueCSSPropertyBorderLeftStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnRuleWidth(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnRuleWidth(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnRuleWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitUserDrag(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitUserDrag(StyleResolverState&);
    static void applyValueCSSPropertyWebkitUserDrag(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridTemplateAreas(StyleResolverState&);
    static void applyInheritCSSPropertyGridTemplateAreas(StyleResolverState&);
    static void applyValueCSSPropertyGridTemplateAreas(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontVariantLigatures(StyleResolverState&);
    static void applyInheritCSSPropertyFontVariantLigatures(StyleResolverState&);
    static void applyValueCSSPropertyFontVariantLigatures(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarkerMid(StyleResolverState&);
    static void applyInheritCSSPropertyMarkerMid(StyleResolverState&);
    static void applyValueCSSPropertyMarkerMid(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextCombine(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextCombine(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextCombine(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyX(StyleResolverState&);
    static void applyInheritCSSPropertyX(StyleResolverState&);
    static void applyValueCSSPropertyX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyY(StyleResolverState&);
    static void applyInheritCSSPropertyY(StyleResolverState&);
    static void applyValueCSSPropertyY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFilter(StyleResolverState&);
    static void applyInheritCSSPropertyFilter(StyleResolverState&);
    static void applyValueCSSPropertyFilter(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyR(StyleResolverState&);
    static void applyInheritCSSPropertyR(StyleResolverState&);
    static void applyValueCSSPropertyR(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyResize(StyleResolverState&);
    static void applyInheritCSSPropertyResize(StyleResolverState&);
    static void applyValueCSSPropertyResize(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontSizeAdjust(StyleResolverState&);
    static void applyInheritCSSPropertyFontSizeAdjust(StyleResolverState&);
    static void applyValueCSSPropertyFontSizeAdjust(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFlexShrink(StyleResolverState&);
    static void applyInheritCSSPropertyFlexShrink(StyleResolverState&);
    static void applyValueCSSPropertyFlexShrink(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationDirection(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationDirection(StyleResolverState&);
    static void applyValueCSSPropertyAnimationDirection(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextOrientation(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextOrientation(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextOrientation(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyColorInterpolationFilters(StyleResolverState&);
    static void applyInheritCSSPropertyColorInterpolationFilters(StyleResolverState&);
    static void applyValueCSSPropertyColorInterpolationFilters(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnBreakBefore(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnBreakBefore(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnBreakBefore(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextTransform(StyleResolverState&);
    static void applyInheritCSSPropertyTextTransform(StyleResolverState&);
    static void applyValueCSSPropertyTextTransform(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyRight(StyleResolverState&);
    static void applyInheritCSSPropertyRight(StyleResolverState&);
    static void applyValueCSSPropertyRight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyGridColumnEnd(StyleResolverState&);
    static void applyInheritCSSPropertyGridColumnEnd(StyleResolverState&);
    static void applyValueCSSPropertyGridColumnEnd(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundSize(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundSize(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundSize(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollSnapCoordinate(StyleResolverState&);
    static void applyInheritCSSPropertyScrollSnapCoordinate(StyleResolverState&);
    static void applyValueCSSPropertyScrollSnapCoordinate(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAlignContent(StyleResolverState&);
    static void applyInheritCSSPropertyAlignContent(StyleResolverState&);
    static void applyValueCSSPropertyAlignContent(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyDirection(StyleResolverState&);
    static void applyInheritCSSPropertyDirection(StyleResolverState&);
    static void applyValueCSSPropertyDirection(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyClip(StyleResolverState&);
    static void applyInheritCSSPropertyClip(StyleResolverState&);
    static void applyValueCSSPropertyClip(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAnimationFillMode(StyleResolverState&);
    static void applyInheritCSSPropertyAnimationFillMode(StyleResolverState&);
    static void applyValueCSSPropertyAnimationFillMode(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyColor(StyleResolverState&);
    static void applyInheritCSSPropertyColor(StyleResolverState&);
    static void applyValueCSSPropertyColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBorderImage(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBorderImage(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBorderImage(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarginLeft(StyleResolverState&);
    static void applyInheritCSSPropertyMarginLeft(StyleResolverState&);
    static void applyValueCSSPropertyMarginLeft(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFlexWrap(StyleResolverState&);
    static void applyInheritCSSPropertyFlexWrap(StyleResolverState&);
    static void applyValueCSSPropertyFlexWrap(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMinHeight(StyleResolverState&);
    static void applyInheritCSSPropertyMinHeight(StyleResolverState&);
    static void applyValueCSSPropertyMinHeight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderLeftColor(StyleResolverState&);
    static void applyInheritCSSPropertyBorderLeftColor(StyleResolverState&);
    static void applyValueCSSPropertyBorderLeftColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFloodColor(StyleResolverState&);
    static void applyInheritCSSPropertyFloodColor(StyleResolverState&);
    static void applyValueCSSPropertyFloodColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMaxHeight(StyleResolverState&);
    static void applyInheritCSSPropertyMaxHeight(StyleResolverState&);
    static void applyValueCSSPropertyMaxHeight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWillChange(StyleResolverState&);
    static void applyInheritCSSPropertyWillChange(StyleResolverState&);
    static void applyValueCSSPropertyWillChange(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBufferedRendering(StyleResolverState&);
    static void applyInheritCSSPropertyBufferedRendering(StyleResolverState&);
    static void applyValueCSSPropertyBufferedRendering(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitWritingMode(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitWritingMode(StyleResolverState&);
    static void applyValueCSSPropertyWebkitWritingMode(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundPositionY(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundPositionY(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundPositionY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundPositionX(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundPositionX(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundPositionX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAlignmentBaseline(StyleResolverState&);
    static void applyInheritCSSPropertyAlignmentBaseline(StyleResolverState&);
    static void applyValueCSSPropertyAlignmentBaseline(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMinWidth(StyleResolverState&);
    static void applyInheritCSSPropertyMinWidth(StyleResolverState&);
    static void applyValueCSSPropertyMinWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMaskType(StyleResolverState&);
    static void applyInheritCSSPropertyMaskType(StyleResolverState&);
    static void applyValueCSSPropertyMaskType(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundOrigin(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundOrigin(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundOrigin(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitColumnRuleStyle(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitColumnRuleStyle(StyleResolverState&);
    static void applyValueCSSPropertyWebkitColumnRuleStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitBoxFlexGroup(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitBoxFlexGroup(StyleResolverState&);
    static void applyValueCSSPropertyWebkitBoxFlexGroup(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextAlignLast(StyleResolverState&);
    static void applyInheritCSSPropertyTextAlignLast(StyleResolverState&);
    static void applyValueCSSPropertyTextAlignLast(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMarginTopCollapse(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMarginTopCollapse(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMarginTopCollapse(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyCursor(StyleResolverState&);
    static void applyInheritCSSPropertyCursor(StyleResolverState&);
    static void applyValueCSSPropertyCursor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTabSize(StyleResolverState&);
    static void applyInheritCSSPropertyTabSize(StyleResolverState&);
    static void applyValueCSSPropertyTabSize(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyCounterIncrement(StyleResolverState&);
    static void applyInheritCSSPropertyCounterIncrement(StyleResolverState&);
    static void applyValueCSSPropertyCounterIncrement(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderBottomWidth(StyleResolverState&);
    static void applyInheritCSSPropertyBorderBottomWidth(StyleResolverState&);
    static void applyValueCSSPropertyBorderBottomWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitHighlight(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitHighlight(StyleResolverState&);
    static void applyValueCSSPropertyWebkitHighlight(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyEmptyCells(StyleResolverState&);
    static void applyInheritCSSPropertyEmptyCells(StyleResolverState&);
    static void applyValueCSSPropertyEmptyCells(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderImageSource(StyleResolverState&);
    static void applyInheritCSSPropertyBorderImageSource(StyleResolverState&);
    static void applyValueCSSPropertyBorderImageSource(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMotionOffset(StyleResolverState&);
    static void applyInheritCSSPropertyMotionOffset(StyleResolverState&);
    static void applyValueCSSPropertyMotionOffset(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextOverflow(StyleResolverState&);
    static void applyInheritCSSPropertyTextOverflow(StyleResolverState&);
    static void applyValueCSSPropertyTextOverflow(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBoxSizing(StyleResolverState&);
    static void applyInheritCSSPropertyBoxSizing(StyleResolverState&);
    static void applyValueCSSPropertyBoxSizing(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyStrokeWidth(StyleResolverState&);
    static void applyInheritCSSPropertyStrokeWidth(StyleResolverState&);
    static void applyValueCSSPropertyStrokeWidth(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundBlendMode(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundBlendMode(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundBlendMode(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyMarkerStart(StyleResolverState&);
    static void applyInheritCSSPropertyMarkerStart(StyleResolverState&);
    static void applyValueCSSPropertyMarkerStart(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextDecorationStyle(StyleResolverState&);
    static void applyInheritCSSPropertyTextDecorationStyle(StyleResolverState&);
    static void applyValueCSSPropertyTextDecorationStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundRepeatX(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundRepeatX(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundRepeatX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTranslate(StyleResolverState&);
    static void applyInheritCSSPropertyTranslate(StyleResolverState&);
    static void applyValueCSSPropertyTranslate(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderTopColor(StyleResolverState&);
    static void applyInheritCSSPropertyBorderTopColor(StyleResolverState&);
    static void applyValueCSSPropertyBorderTopColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBackgroundColor(StyleResolverState&);
    static void applyInheritCSSPropertyBackgroundColor(StyleResolverState&);
    static void applyValueCSSPropertyBackgroundColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPaddingTop(StyleResolverState&);
    static void applyInheritCSSPropertyPaddingTop(StyleResolverState&);
    static void applyValueCSSPropertyPaddingTop(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitTextEmphasisColor(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitTextEmphasisColor(StyleResolverState&);
    static void applyValueCSSPropertyWebkitTextEmphasisColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyDisplay(StyleResolverState&);
    static void applyInheritCSSPropertyDisplay(StyleResolverState&);
    static void applyValueCSSPropertyDisplay(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWordBreak(StyleResolverState&);
    static void applyInheritCSSPropertyWordBreak(StyleResolverState&);
    static void applyValueCSSPropertyWordBreak(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderTopStyle(StyleResolverState&);
    static void applyInheritCSSPropertyBorderTopStyle(StyleResolverState&);
    static void applyValueCSSPropertyBorderTopStyle(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyJustifyContent(StyleResolverState&);
    static void applyInheritCSSPropertyJustifyContent(StyleResolverState&);
    static void applyValueCSSPropertyJustifyContent(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBoxShadow(StyleResolverState&);
    static void applyInheritCSSPropertyBoxShadow(StyleResolverState&);
    static void applyValueCSSPropertyBoxShadow(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertySpeak(StyleResolverState&);
    static void applyInheritCSSPropertySpeak(StyleResolverState&);
    static void applyValueCSSPropertySpeak(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyZIndex(StyleResolverState&);
    static void applyInheritCSSPropertyZIndex(StyleResolverState&);
    static void applyValueCSSPropertyZIndex(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyListStyleImage(StyleResolverState&);
    static void applyInheritCSSPropertyListStyleImage(StyleResolverState&);
    static void applyValueCSSPropertyListStyleImage(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFloodOpacity(StyleResolverState&);
    static void applyInheritCSSPropertyFloodOpacity(StyleResolverState&);
    static void applyValueCSSPropertyFloodOpacity(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyClipRule(StyleResolverState&);
    static void applyInheritCSSPropertyClipRule(StyleResolverState&);
    static void applyValueCSSPropertyClipRule(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFlexDirection(StyleResolverState&);
    static void applyInheritCSSPropertyFlexDirection(StyleResolverState&);
    static void applyValueCSSPropertyFlexDirection(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyLightingColor(StyleResolverState&);
    static void applyInheritCSSPropertyLightingColor(StyleResolverState&);
    static void applyValueCSSPropertyLightingColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskRepeatY(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskRepeatY(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskRepeatY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitMaskRepeatX(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitMaskRepeatX(StyleResolverState&);
    static void applyValueCSSPropertyWebkitMaskRepeatX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyPageBreakInside(StyleResolverState&);
    static void applyInheritCSSPropertyPageBreakInside(StyleResolverState&);
    static void applyValueCSSPropertyPageBreakInside(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyListStyleType(StyleResolverState&);
    static void applyInheritCSSPropertyListStyleType(StyleResolverState&);
    static void applyValueCSSPropertyListStyleType(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyTextAlign(StyleResolverState&);
    static void applyInheritCSSPropertyTextAlign(StyleResolverState&);
    static void applyValueCSSPropertyTextAlign(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyAlignItems(StyleResolverState&);
    static void applyInheritCSSPropertyAlignItems(StyleResolverState&);
    static void applyValueCSSPropertyAlignItems(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyObjectPosition(StyleResolverState&);
    static void applyInheritCSSPropertyObjectPosition(StyleResolverState&);
    static void applyValueCSSPropertyObjectPosition(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyOrphans(StyleResolverState&);
    static void applyInheritCSSPropertyOrphans(StyleResolverState&);
    static void applyValueCSSPropertyOrphans(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyScrollSnapDestination(StyleResolverState&);
    static void applyInheritCSSPropertyScrollSnapDestination(StyleResolverState&);
    static void applyValueCSSPropertyScrollSnapDestination(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitPerspectiveOriginY(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitPerspectiveOriginY(StyleResolverState&);
    static void applyValueCSSPropertyWebkitPerspectiveOriginY(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyWebkitPerspectiveOriginX(StyleResolverState&);
    static void applyInheritCSSPropertyWebkitPerspectiveOriginX(StyleResolverState&);
    static void applyValueCSSPropertyWebkitPerspectiveOriginX(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyBorderBottomColor(StyleResolverState&);
    static void applyInheritCSSPropertyBorderBottomColor(StyleResolverState&);
    static void applyValueCSSPropertyBorderBottomColor(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyFontSize(StyleResolverState&);
    static void applyInheritCSSPropertyFontSize(StyleResolverState&);
    static void applyValueCSSPropertyFontSize(StyleResolverState&, CSSValue*);

    static void applyInitialCSSPropertyVisibility(StyleResolverState&);
    static void applyInheritCSSPropertyVisibility(StyleResolverState&);
    static void applyValueCSSPropertyVisibility(StyleResolverState&, CSSValue*);

	static void applyValueCSSPropertyVariable(StyleResolverState& state, CSSValue* value);
};

} // namespace blink

#endif
