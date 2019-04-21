// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/resolver/StyleBuilder.h"

#include "StyleBuilderFunctions.h"
#include "core/css/CSSProperty.h"
#include "core/css/resolver/StyleResolverState.h"

// FIXME: currently we're just generating a switch statement, but we should
//   test other variations for performance once we have more properties here.

namespace blink {

void StyleBuilder::applyProperty(CSSPropertyID property, StyleResolverState& state, CSSValue* value, bool isInitial, bool isInherit) {
    switch(property) {
    case CSSPropertyWebkitMaskBoxImageWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageWidth(state, value);
        return;

    case CSSPropertySize:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertySize(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertySize(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertySize(state, value);
        return;

    case CSSPropertyVectorEffect:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyVectorEffect(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyVectorEffect(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyVectorEffect(state, value);
        return;

    case CSSPropertyFillRule:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFillRule(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFillRule(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFillRule(state, value);
        return;

//     case CSSPropertyWebkitLineBoxContain:
//         if (isInitial)
//             StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineBoxContain(state);
//         else if (isInherit)
//             StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineBoxContain(state);
//         else
//             StyleBuilderFunctions::applyValueCSSPropertyWebkitLineBoxContain(state, value);
//         return;

    case CSSPropertyTextDecoration:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationLine(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationLine(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextDecorationLine(state, value);
        return;

    case CSSPropertyCx:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyCx(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyCx(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyCx(state, value);
        return;

    case CSSPropertyCy:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyCy(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyCy(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyCy(state, value);
        return;

    case CSSPropertyTransitionDelay:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransitionDelay(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransitionDelay(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransitionDelay(state, value);
        return;

    case CSSPropertyShapeOutside:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyShapeOutside(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyShapeOutside(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyShapeOutside(state, value);
        return;

    case CSSPropertyTextDecorationColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextDecorationColor(state, value);
        return;

    case CSSPropertyWebkitAppRegion:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitAppRegion(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitAppRegion(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitAppRegion(state, value);
        return;

    case CSSPropertyAnimationName:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationName(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationName(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationName(state, value);
        return;

    case CSSPropertyWebkitHyphenateCharacter:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitHyphenateCharacter(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitHyphenateCharacter(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitHyphenateCharacter(state, value);
        return;

    case CSSPropertyWebkitBoxPack:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxPack(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxPack(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxPack(state, value);
        return;

    case CSSPropertyWebkitMarginBottomCollapse:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginBottomCollapse(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginBottomCollapse(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginBottomCollapse(state, value);
        return;

    case CSSPropertyOutlineWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOutlineWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOutlineWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOutlineWidth(state, value);
        return;

    case CSSPropertyStrokeOpacity:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeOpacity(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeOpacity(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeOpacity(state, value);
        return;

    case CSSPropertyWebkitColumnWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnWidth(state, value);
        return;

    case CSSPropertyFontFamily:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontFamily(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontFamily(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontFamily(state, value);
        return;

    case CSSPropertyWebkitBoxFlex:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxFlex(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxFlex(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxFlex(state, value);
        return;

    case CSSPropertyAnimationDuration:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationDuration(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationDuration(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationDuration(state, value);
        return;

    case CSSPropertyAnimationPlayState:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationPlayState(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationPlayState(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationPlayState(state, value);
        return;

    case CSSPropertyWebkitMaskComposite:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskComposite(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskComposite(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskComposite(state, value);
        return;

    case CSSPropertyWebkitAppearance:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitAppearance(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitAppearance(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitAppearance(state, value);
        return;

    case CSSPropertyBorderBottomStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderBottomStyle(state, value);
        return;

    case CSSPropertyHeight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyHeight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyHeight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyHeight(state, value);
        return;

    case CSSPropertyPaintOrder:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPaintOrder(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPaintOrder(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPaintOrder(state, value);
        return;

    case CSSPropertyTextRendering:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextRendering(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextRendering(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextRendering(state, value);
        return;

    case CSSPropertyWebkitColumnBreakAfter:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakAfter(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakAfter(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakAfter(state, value);
        return;

    case CSSPropertyBorderBottomLeftRadius:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomLeftRadius(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomLeftRadius(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderBottomLeftRadius(state, value);
        return;

    case CSSPropertyColorInterpolation:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyColorInterpolation(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyColorInterpolation(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyColorInterpolation(state, value);
        return;

    case CSSPropertyWebkitMaskPositionY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskPositionY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskPositionY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskPositionY(state, value);
        return;

    case CSSPropertyWebkitMaskPositionX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskPositionX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskPositionX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskPositionX(state, value);
        return;

    case CSSPropertyTextDecorationLine:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationLine(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationLine(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextDecorationLine(state, value);
        return;

    case CSSPropertyContent:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyContent(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyContent(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyContent(state, value);
        return;

    case CSSPropertyWebkitColumnSpan:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnSpan(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnSpan(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnSpan(state, value);
        return;

    case CSSPropertyWebkitBackgroundClip:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundClip(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundClip(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundClip(state, value);
        return;

    case CSSPropertyShapeMargin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyShapeMargin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyShapeMargin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyShapeMargin(state, value);
        return;

    case CSSPropertyOutlineColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOutlineColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOutlineColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOutlineColor(state, value);
        return;

    case CSSPropertyScrollSnapType:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapType(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapType(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScrollSnapType(state, value);
        return;

    case CSSPropertyImageRendering:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyImageRendering(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyImageRendering(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyImageRendering(state, value);
        return;

    case CSSPropertyWebkitUserSelect:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserSelect(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserSelect(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitUserSelect(state, value);
        return;

    case CSSPropertyWebkitTextEmphasisStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisStyle(state, value);
        return;

    case CSSPropertyPaddingBottom:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPaddingBottom(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPaddingBottom(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPaddingBottom(state, value);
        return;

    case CSSPropertyQuotes:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyQuotes(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyQuotes(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyQuotes(state, value);
        return;

    case CSSPropertyGridAutoRows:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridAutoRows(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridAutoRows(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridAutoRows(state, value);
        return;

    case CSSPropertyClipPath:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyClipPath(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyClipPath(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyClipPath(state, value);
        return;

    case CSSPropertyObjectFit:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyObjectFit(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyObjectFit(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyObjectFit(state, value);
        return;

    case CSSPropertyStopOpacity:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStopOpacity(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStopOpacity(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStopOpacity(state, value);
        return;

    case CSSPropertyWebkitBoxReflect:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxReflect(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxReflect(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxReflect(state, value);
        return;

    case CSSPropertyBorderTopRightRadius:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderTopRightRadius(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderTopRightRadius(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderTopRightRadius(state, value);
        return;

    case CSSPropertyWebkitBoxDirection:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxDirection(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxDirection(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxDirection(state, value);
        return;

    case CSSPropertyWebkitUserModify:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserModify(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserModify(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitUserModify(state, value);
        return;

    case CSSPropertyWebkitBoxDecorationBreak:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxDecorationBreak(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxDecorationBreak(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxDecorationBreak(state, value);
        return;

    case CSSPropertyFontKerning:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontKerning(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontKerning(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontKerning(state, value);
        return;

    case CSSPropertyFontWeight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontWeight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontWeight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontWeight(state, value);
        return;

    case CSSPropertyShapeImageThreshold:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyShapeImageThreshold(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyShapeImageThreshold(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyShapeImageThreshold(state, value);
        return;

    case CSSPropertyRy:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyRy(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyRy(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyRy(state, value);
        return;

    case CSSPropertyRx:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyRx(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyRx(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyRx(state, value);
        return;

    case CSSPropertyMarginRight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarginRight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarginRight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarginRight(state, value);
        return;

    case CSSPropertyWebkitFontSmoothing:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitFontSmoothing(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitFontSmoothing(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitFontSmoothing(state, value);
        return;

    case CSSPropertyPageBreakBefore:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPageBreakBefore(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPageBreakBefore(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPageBreakBefore(state, value);
        return;

    case CSSPropertyAnimationDelay:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationDelay(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationDelay(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationDelay(state, value);
        return;

    case CSSPropertyWebkitColumnRuleColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleColor(state, value);
        return;

    case CSSPropertyWebkitFontFeatureSettings:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitFontFeatureSettings(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitFontFeatureSettings(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitFontFeatureSettings(state, value);
        return;

    case CSSPropertyWebkitTextStrokeColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextStrokeColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextStrokeColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextStrokeColor(state, value);
        return;

    case CSSPropertyOpacity:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOpacity(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOpacity(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOpacity(state, value);
        return;

    case CSSPropertyDominantBaseline:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyDominantBaseline(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyDominantBaseline(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyDominantBaseline(state, value);
        return;

    case CSSPropertyWebkitMaskBoxImageOutset:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageOutset(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageOutset(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageOutset(state, value);
        return;

    case CSSPropertyAlignSelf:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAlignSelf(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAlignSelf(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAlignSelf(state, value);
        return;

    case CSSPropertyClear:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyClear(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyClear(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyClear(state, value);
        return;

    case CSSPropertyWebkitColumnBreakInside:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakInside(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakInside(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakInside(state, value);
        return;

    case CSSPropertyWebkitMaskBoxImageSource:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageSource(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageSource(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageSource(state, value);
        return;

    case CSSPropertyTransitionProperty:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransitionProperty(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransitionProperty(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransitionProperty(state, value);
        return;

    case CSSPropertyTransformStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransformStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransformStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransformStyle(state, value);
        return;

    case CSSPropertyMotionRotation:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMotionRotation(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMotionRotation(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMotionRotation(state, value);
        return;

    case CSSPropertyWebkitBorderHorizontalSpacing:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderHorizontalSpacing(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderHorizontalSpacing(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderHorizontalSpacing(state, value);
        return;

    case CSSPropertyWebkitTapHighlightColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTapHighlightColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTapHighlightColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTapHighlightColor(state, value);
        return;

    case CSSPropertyCaptionSide:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyCaptionSide(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyCaptionSide(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyCaptionSide(state, value);
        return;

    case CSSPropertyWebkitPrintColorAdjust:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitPrintColorAdjust(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitPrintColorAdjust(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitPrintColorAdjust(state, value);
        return;

//     case CSSPropertyScrollBlocksOn:
//         if (isInitial)
//             StyleBuilderFunctions::applyInitialCSSPropertyScrollBlocksOn(state);
//         else if (isInherit)
//             StyleBuilderFunctions::applyInheritCSSPropertyScrollBlocksOn(state);
//         else
//             StyleBuilderFunctions::applyValueCSSPropertyScrollBlocksOn(state, value);
//         return;

    case CSSPropertyStrokeDasharray:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeDasharray(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeDasharray(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeDasharray(state, value);
        return;

    case CSSPropertyFlexBasis:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFlexBasis(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFlexBasis(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFlexBasis(state, value);
        return;

    case CSSPropertyWidows:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWidows(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWidows(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWidows(state, value);
        return;

    case CSSPropertyLetterSpacing:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyLetterSpacing(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyLetterSpacing(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyLetterSpacing(state, value);
        return;

    case CSSPropertyWebkitRubyPosition:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitRubyPosition(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitRubyPosition(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitRubyPosition(state, value);
        return;

    case CSSPropertyWebkitTransformOriginZ:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginZ(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginZ(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginZ(state, value);
        return;

    case CSSPropertyWebkitTransformOriginY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginY(state, value);
        return;

    case CSSPropertyWebkitTransformOriginX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginX(state, value);
        return;

    case CSSPropertyGridTemplateRows:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateRows(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateRows(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridTemplateRows(state, value);
        return;

    case CSSPropertyTransform:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransform(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransform(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransform(state, value);
        return;

    case CSSPropertyWebkitMaskOrigin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskOrigin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskOrigin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskOrigin(state, value);
        return;

    case CSSPropertyScrollBehavior:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScrollBehavior(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScrollBehavior(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScrollBehavior(state, value);
        return;

    case CSSPropertyFill:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFill(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFill(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFill(state, value);
        return;

    case CSSPropertyGridAutoFlow:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridAutoFlow(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridAutoFlow(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridAutoFlow(state, value);
        return;

    case CSSPropertyMotionPath:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMotionPath(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMotionPath(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMotionPath(state, value);
        return;

    case CSSPropertyStopColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStopColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStopColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStopColor(state, value);
        return;

    case CSSPropertyWebkitLineClamp:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineClamp(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineClamp(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitLineClamp(state, value);
        return;

    case CSSPropertyJustifySelf:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyJustifySelf(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyJustifySelf(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyJustifySelf(state, value);
        return;

    case CSSPropertyTextJustify:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextJustify(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextJustify(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextJustify(state, value);
        return;

    case CSSPropertyZoom:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyZoom(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyZoom(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyZoom(state, value);
        return;

    case CSSPropertyWebkitMaskBoxImageRepeat:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageRepeat(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageRepeat(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageRepeat(state, value);
        return;

    case CSSPropertyPerspectiveOrigin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPerspectiveOrigin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPerspectiveOrigin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPerspectiveOrigin(state, value);
        return;

    case CSSPropertyTextAnchor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextAnchor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextAnchor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextAnchor(state, value);
        return;

    case CSSPropertyWebkitColumnCount:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnCount(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnCount(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnCount(state, value);
        return;

    case CSSPropertyFontStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontStyle(state, value);
        return;

    case CSSPropertyBorderBottomRightRadius:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomRightRadius(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomRightRadius(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderBottomRightRadius(state, value);
        return;

    case CSSPropertyBorderRightWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderRightWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderRightWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderRightWidth(state, value);
        return;

    case CSSPropertyBorderTopLeftRadius:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderTopLeftRadius(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderTopLeftRadius(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderTopLeftRadius(state, value);
        return;

    case CSSPropertyFontVariant:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontVariant(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontVariant(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontVariant(state, value);
        return;

    case CSSPropertyWritingMode:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWritingMode(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWritingMode(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWritingMode(state, value);
        return;

    case CSSPropertyBackgroundAttachment:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundAttachment(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundAttachment(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundAttachment(state, value);
        return;

    case CSSPropertyWebkitTextSecurity:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextSecurity(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextSecurity(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextSecurity(state, value);
        return;

    case CSSPropertyBorderLeftWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderLeftWidth(state, value);
        return;

    case CSSPropertyWebkitLineBreak:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineBreak(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineBreak(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitLineBreak(state, value);
        return;

    case CSSPropertyBackgroundImage:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundImage(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundImage(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundImage(state, value);
        return;

    case CSSPropertyWebkitMaskClip:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskClip(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskClip(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskClip(state, value);
        return;

    case CSSPropertyCounterReset:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyCounterReset(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyCounterReset(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyCounterReset(state, value);
        return;

    case CSSPropertyBorderImageSlice:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderImageSlice(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderImageSlice(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderImageSlice(state, value);
        return;

    case CSSPropertyIsolation:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyIsolation(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyIsolation(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyIsolation(state, value);
        return;

//     case CSSPropertyGlyphOrientationHorizontal:
//         if (isInitial)
//             StyleBuilderFunctions::applyInitialCSSPropertyGlyphOrientationHorizontal(state);
//         else if (isInherit)
//             StyleBuilderFunctions::applyInheritCSSPropertyGlyphOrientationHorizontal(state);
//         else
//             StyleBuilderFunctions::applyValueCSSPropertyGlyphOrientationHorizontal(state, value);
//         return;

    case CSSPropertyFillOpacity:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFillOpacity(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFillOpacity(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFillOpacity(state, value);
        return;

    case CSSPropertyBorderTopWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderTopWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderTopWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderTopWidth(state, value);
        return;

    case CSSPropertyBottom:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBottom(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBottom(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBottom(state, value);
        return;

    case CSSPropertyBorderCollapse:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderCollapse(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderCollapse(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderCollapse(state, value);
        return;

    case CSSPropertyTop:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTop(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTop(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTop(state, value);
        return;

    case CSSPropertyScrollSnapPointsX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapPointsX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapPointsX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScrollSnapPointsX(state, value);
        return;

    case CSSPropertyScrollSnapPointsY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapPointsY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapPointsY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScrollSnapPointsY(state, value);
        return;

    case CSSPropertyWebkitColumnGap:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnGap(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnGap(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnGap(state, value);
        return;

    case CSSPropertyUnicodeBidi:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyUnicodeBidi(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyUnicodeBidi(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyUnicodeBidi(state, value);
        return;

    case CSSPropertyAnimationIterationCount:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationIterationCount(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationIterationCount(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationIterationCount(state, value);
        return;

    case CSSPropertyWebkitRtlOrdering:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitRtlOrdering(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitRtlOrdering(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitRtlOrdering(state, value);
        return;

    case CSSPropertyFloat:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFloat(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFloat(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFloat(state, value);
        return;

    case CSSPropertyWordWrap:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWordWrap(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWordWrap(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWordWrap(state, value);
        return;

    case CSSPropertyTransformOrigin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransformOrigin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransformOrigin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransformOrigin(state, value);
        return;

    case CSSPropertyMarginTop:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarginTop(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarginTop(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarginTop(state, value);
        return;

    case CSSPropertyMaxWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMaxWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMaxWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMaxWidth(state, value);
        return;

    case CSSPropertyTextIndent:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextIndent(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextIndent(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextIndent(state, value);
        return;

    case CSSPropertyTextShadow:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextShadow(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextShadow(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextShadow(state, value);
        return;

    case CSSPropertyBorderImageOutset:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderImageOutset(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderImageOutset(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderImageOutset(state, value);
        return;

    case CSSPropertyPaddingRight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPaddingRight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPaddingRight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPaddingRight(state, value);
        return;

    case CSSPropertyWebkitBoxOrdinalGroup:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxOrdinalGroup(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxOrdinalGroup(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxOrdinalGroup(state, value);
        return;

    case CSSPropertyBorderImageRepeat:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderImageRepeat(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderImageRepeat(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderImageRepeat(state, value);
        return;

    case CSSPropertyColumnFill:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyColumnFill(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyColumnFill(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyColumnFill(state, value);
        return;

    case CSSPropertyTransitionTimingFunction:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransitionTimingFunction(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransitionTimingFunction(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransitionTimingFunction(state, value);
        return;

    case CSSPropertyOverflowX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOverflowX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOverflowX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOverflowX(state, value);
        return;

    case CSSPropertyOverflowY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOverflowY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOverflowY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOverflowY(state, value);
        return;

    case CSSPropertyGridTemplateColumns:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateColumns(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateColumns(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridTemplateColumns(state, value);
        return;

    case CSSPropertyPerspective:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPerspective(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPerspective(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPerspective(state, value);
        return;

    case CSSPropertyLineHeight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyLineHeight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyLineHeight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyLineHeight(state, value);
        return;

    case CSSPropertyMaskSourceType:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMaskSourceType(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMaskSourceType(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMaskSourceType(state, value);
        return;

    case CSSPropertyBackgroundRepeatY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundRepeatY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundRepeatY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundRepeatY(state, value);
        return;

    case CSSPropertyOrder:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOrder(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOrder(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOrder(state, value);
        return;

    case CSSPropertyWebkitBoxAlign:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxAlign(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxAlign(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxAlign(state, value);
        return;

    case CSSPropertyScale:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScale(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScale(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScale(state, value);
        return;

    case CSSPropertyGridAutoColumns:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridAutoColumns(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridAutoColumns(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridAutoColumns(state, value);
        return;

    case CSSPropertyGridRowStart:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridRowStart(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridRowStart(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridRowStart(state, value);
        return;

    case CSSPropertyTransitionDuration:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTransitionDuration(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTransitionDuration(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTransitionDuration(state, value);
        return;

    case CSSPropertyWebkitTextStrokeWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextStrokeWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextStrokeWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextStrokeWidth(state, value);
        return;

    case CSSPropertyStrokeDashoffset:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeDashoffset(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeDashoffset(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeDashoffset(state, value);
        return;

    case CSSPropertyPosition:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPosition(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPosition(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPosition(state, value);
        return;

//     case CSSPropertyGlyphOrientationVertical:
//         if (isInitial)
//             StyleBuilderFunctions::applyInitialCSSPropertyGlyphOrientationVertical(state);
//         else if (isInherit)
//             StyleBuilderFunctions::applyInheritCSSPropertyGlyphOrientationVertical(state);
//         else
//             StyleBuilderFunctions::applyValueCSSPropertyGlyphOrientationVertical(state, value);
//         return;

    case CSSPropertyWebkitTextFillColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextFillColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextFillColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextFillColor(state, value);
        return;

    case CSSPropertyFontStretch:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontStretch(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontStretch(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontStretch(state, value);
        return;

    case CSSPropertyMixBlendMode:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMixBlendMode(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMixBlendMode(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMixBlendMode(state, value);
        return;

    case CSSPropertyBaselineShift:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBaselineShift(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBaselineShift(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBaselineShift(state, value);
        return;

    case CSSPropertyWebkitBackgroundOrigin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundOrigin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundOrigin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundOrigin(state, value);
        return;

    case CSSPropertyPaddingLeft:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPaddingLeft(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPaddingLeft(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPaddingLeft(state, value);
        return;

    case CSSPropertyWhiteSpace:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWhiteSpace(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWhiteSpace(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWhiteSpace(state, value);
        return;

    case CSSPropertyBackgroundClip:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundClip(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundClip(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundClip(state, value);
        return;

    case CSSPropertyOverflowWrap:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOverflowWrap(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOverflowWrap(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOverflowWrap(state, value);
        return;

    case CSSPropertyVerticalAlign:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyVerticalAlign(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyVerticalAlign(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyVerticalAlign(state, value);
        return;

    case CSSPropertyWebkitLocale:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitLocale(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitLocale(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitLocale(state, value);
        return;

    case CSSPropertyWebkitMarginAfterCollapse:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginAfterCollapse(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginAfterCollapse(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginAfterCollapse(state, value);
        return;

    case CSSPropertyWebkitTextEmphasisPosition:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisPosition(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisPosition(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisPosition(state, value);
        return;

    case CSSPropertyWordSpacing:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWordSpacing(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWordSpacing(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWordSpacing(state, value);
        return;

    case CSSPropertyPageBreakAfter:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPageBreakAfter(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPageBreakAfter(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPageBreakAfter(state, value);
        return;

    case CSSPropertyOutlineStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOutlineStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOutlineStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOutlineStyle(state, value);
        return;

    case CSSPropertyMarkerEnd:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarkerEnd(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarkerEnd(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarkerEnd(state, value);
        return;

    case CSSPropertyBorderRightColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderRightColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderRightColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderRightColor(state, value);
        return;

    case CSSPropertyWebkitBoxLines:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxLines(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxLines(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxLines(state, value);
        return;

    case CSSPropertyTableLayout:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTableLayout(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTableLayout(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTableLayout(state, value);
        return;

    case CSSPropertyStroke:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStroke(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStroke(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStroke(state, value);
        return;

    case CSSPropertyBorderRightStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderRightStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderRightStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderRightStyle(state, value);
        return;

    case CSSPropertyGridRowEnd:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridRowEnd(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridRowEnd(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridRowEnd(state, value);
        return;

    case CSSPropertyTextUnderlinePosition:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextUnderlinePosition(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextUnderlinePosition(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextUnderlinePosition(state, value);
        return;

    case CSSPropertyBackfaceVisibility:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackfaceVisibility(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackfaceVisibility(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackfaceVisibility(state, value);
        return;

    case CSSPropertyLeft:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyLeft(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyLeft(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyLeft(state, value);
        return;

    case CSSPropertyWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWidth(state, value);
        return;

    case CSSPropertyTouchAction:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTouchAction(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTouchAction(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTouchAction(state, value);
        return;

//     case CSSPropertyWebkitBackgroundComposite:
//         if (isInitial)
//             StyleBuilderFunctions::applyInitialCSSPropertyWebkitBackgroundComposite(state);
//         else if (isInherit)
//             StyleBuilderFunctions::applyInheritCSSPropertyWebkitBackgroundComposite(state);
//         else
//             StyleBuilderFunctions::applyValueCSSPropertyWebkitBackgroundComposite(state, value);
//         return;

    case CSSPropertyWebkitClipPath:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitClipPath(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitClipPath(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitClipPath(state, value);
        return;

    case CSSPropertyStrokeMiterlimit:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeMiterlimit(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeMiterlimit(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeMiterlimit(state, value);
        return;

    case CSSPropertyGridColumnStart:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridColumnStart(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridColumnStart(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridColumnStart(state, value);
        return;

    case CSSPropertyMarginBottom:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarginBottom(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarginBottom(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarginBottom(state, value);
        return;

    case CSSPropertyListStylePosition:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyListStylePosition(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyListStylePosition(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyListStylePosition(state, value);
        return;

    case CSSPropertyWebkitFilter:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitFilter(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitFilter(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitFilter(state, value);
        return;

    case CSSPropertyOutlineOffset:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOutlineOffset(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOutlineOffset(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOutlineOffset(state, value);
        return;

    case CSSPropertyShapeRendering:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyShapeRendering(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyShapeRendering(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyShapeRendering(state, value);
        return;

    case CSSPropertyWebkitMaskSize:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskSize(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskSize(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskSize(state, value);
        return;

    case CSSPropertyJustifyItems:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyJustifyItems(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyJustifyItems(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyJustifyItems(state, value);
        return;

    case CSSPropertyRotate:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyRotate(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyRotate(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyRotate(state, value);
        return;

    case CSSPropertyColorRendering:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyColorRendering(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyColorRendering(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyColorRendering(state, value);
        return;

    case CSSPropertyStrokeLinejoin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeLinejoin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeLinejoin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeLinejoin(state, value);
        return;

    case CSSPropertyBorderImageWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderImageWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderImageWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderImageWidth(state, value);
        return;

    case CSSPropertyFlexGrow:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFlexGrow(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFlexGrow(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFlexGrow(state, value);
        return;

    case CSSPropertyMask:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMask(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMask(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMask(state, value);
        return;

    case CSSPropertyWebkitMarginBeforeCollapse:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginBeforeCollapse(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginBeforeCollapse(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginBeforeCollapse(state, value);
        return;

    case CSSPropertyWebkitMaskImage:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskImage(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskImage(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskImage(state, value);
        return;

    case CSSPropertyAnimationTimingFunction:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationTimingFunction(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationTimingFunction(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationTimingFunction(state, value);
        return;

    case CSSPropertyPointerEvents:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPointerEvents(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPointerEvents(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPointerEvents(state, value);
        return;

    case CSSPropertyWebkitBoxOrient:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxOrient(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxOrient(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxOrient(state, value);
        return;

    case CSSPropertyStrokeLinecap:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeLinecap(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeLinecap(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeLinecap(state, value);
        return;

    case CSSPropertyBorderLeftStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderLeftStyle(state, value);
        return;

    case CSSPropertyWebkitColumnRuleWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleWidth(state, value);
        return;

    case CSSPropertyWebkitUserDrag:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserDrag(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserDrag(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitUserDrag(state, value);
        return;

    case CSSPropertyGridTemplateAreas:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateAreas(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateAreas(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridTemplateAreas(state, value);
        return;

    case CSSPropertyFontVariantLigatures:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontVariantLigatures(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontVariantLigatures(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontVariantLigatures(state, value);
        return;

    case CSSPropertyMarkerMid:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarkerMid(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarkerMid(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarkerMid(state, value);
        return;

    case CSSPropertyWebkitTextCombine:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextCombine(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextCombine(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextCombine(state, value);
        return;

    case CSSPropertyX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyX(state, value);
        return;

    case CSSPropertyY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyY(state, value);
        return;

//     case CSSPropertyFilter:
//         if (isInitial)
//             StyleBuilderFunctions::applyInitialCSSPropertyFilter(state);
//         else if (isInherit)
//             StyleBuilderFunctions::applyInheritCSSPropertyFilter(state);
//         else
//             StyleBuilderFunctions::applyValueCSSPropertyFilter(state, value);
//         return;

    case CSSPropertyR:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyR(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyR(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyR(state, value);
        return;

    case CSSPropertyResize:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyResize(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyResize(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyResize(state, value);
        return;

    case CSSPropertyFontSizeAdjust:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontSizeAdjust(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontSizeAdjust(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontSizeAdjust(state, value);
        return;

    case CSSPropertyFlexShrink:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFlexShrink(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFlexShrink(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFlexShrink(state, value);
        return;

    case CSSPropertyAnimationDirection:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationDirection(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationDirection(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationDirection(state, value);
        return;

    case CSSPropertyWebkitTextOrientation:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextOrientation(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextOrientation(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextOrientation(state, value);
        return;

    case CSSPropertyColorInterpolationFilters:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyColorInterpolationFilters(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyColorInterpolationFilters(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyColorInterpolationFilters(state, value);
        return;

    case CSSPropertyWebkitColumnBreakBefore:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakBefore(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakBefore(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakBefore(state, value);
        return;

    case CSSPropertyTextTransform:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextTransform(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextTransform(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextTransform(state, value);
        return;

    case CSSPropertyRight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyRight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyRight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyRight(state, value);
        return;

    case CSSPropertyGridColumnEnd:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyGridColumnEnd(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyGridColumnEnd(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyGridColumnEnd(state, value);
        return;

    case CSSPropertyBackgroundSize:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundSize(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundSize(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundSize(state, value);
        return;

    case CSSPropertyScrollSnapCoordinate:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapCoordinate(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapCoordinate(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScrollSnapCoordinate(state, value);
        return;

    case CSSPropertyAlignContent:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAlignContent(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAlignContent(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAlignContent(state, value);
        return;

    case CSSPropertyDirection:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyDirection(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyDirection(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyDirection(state, value);
        return;

    case CSSPropertyClip:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyClip(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyClip(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyClip(state, value);
        return;

    case CSSPropertyAnimationFillMode:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAnimationFillMode(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAnimationFillMode(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAnimationFillMode(state, value);
        return;

    case CSSPropertyColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyColor(state, value);
        return;

    case CSSPropertyWebkitBorderImage:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderImage(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderImage(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderImage(state, value);
        return;

    case CSSPropertyWebkitBorderVerticalSpacing:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderVerticalSpacing(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderVerticalSpacing(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderVerticalSpacing(state, value);
        return;

    case CSSPropertyMarginLeft:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarginLeft(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarginLeft(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarginLeft(state, value);
        return;

    case CSSPropertyFlexWrap:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFlexWrap(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFlexWrap(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFlexWrap(state, value);
        return;

    case CSSPropertyMinHeight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMinHeight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMinHeight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMinHeight(state, value);
        return;

    case CSSPropertyBorderLeftColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderLeftColor(state, value);
        return;

    case CSSPropertyFloodColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFloodColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFloodColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFloodColor(state, value);
        return;

    case CSSPropertyMaxHeight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMaxHeight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMaxHeight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMaxHeight(state, value);
        return;

    case CSSPropertyWillChange:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWillChange(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWillChange(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWillChange(state, value);
        return;

    case CSSPropertyBufferedRendering:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBufferedRendering(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBufferedRendering(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBufferedRendering(state, value);
        return;

    case CSSPropertyWebkitMaskBoxImageSlice:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageSlice(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageSlice(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageSlice(state, value);
        return;

    case CSSPropertyWebkitWritingMode:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitWritingMode(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitWritingMode(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitWritingMode(state, value);
        return;

    case CSSPropertyBackgroundPositionY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundPositionY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundPositionY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundPositionY(state, value);
        return;

    case CSSPropertyBackgroundPositionX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundPositionX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundPositionX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundPositionX(state, value);
        return;

    case CSSPropertyAlignmentBaseline:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAlignmentBaseline(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAlignmentBaseline(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAlignmentBaseline(state, value);
        return;

    case CSSPropertyMinWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMinWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMinWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMinWidth(state, value);
        return;

    case CSSPropertyMaskType:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMaskType(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMaskType(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMaskType(state, value);
        return;

    case CSSPropertyBackgroundOrigin:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundOrigin(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundOrigin(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundOrigin(state, value);
        return;

    case CSSPropertyWebkitColumnRuleStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleStyle(state, value);
        return;

    case CSSPropertyWebkitBoxFlexGroup:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxFlexGroup(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxFlexGroup(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxFlexGroup(state, value);
        return;

    case CSSPropertyTextAlignLast:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextAlignLast(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextAlignLast(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextAlignLast(state, value);
        return;

    case CSSPropertyWebkitMarginTopCollapse:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginTopCollapse(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginTopCollapse(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginTopCollapse(state, value);
        return;

    case CSSPropertyCursor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyCursor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyCursor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyCursor(state, value);
        return;

    case CSSPropertyTabSize:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTabSize(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTabSize(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTabSize(state, value);
        return;

    case CSSPropertyCounterIncrement:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyCounterIncrement(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyCounterIncrement(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyCounterIncrement(state, value);
        return;

    case CSSPropertyBorderBottomWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderBottomWidth(state, value);
        return;

    case CSSPropertyWebkitHighlight:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitHighlight(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitHighlight(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitHighlight(state, value);
        return;

    case CSSPropertyEmptyCells:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyEmptyCells(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyEmptyCells(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyEmptyCells(state, value);
        return;

    case CSSPropertyBorderImageSource:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderImageSource(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderImageSource(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderImageSource(state, value);
        return;

    case CSSPropertyMotionOffset:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMotionOffset(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMotionOffset(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMotionOffset(state, value);
        return;

    case CSSPropertyTextOverflow:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextOverflow(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextOverflow(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextOverflow(state, value);
        return;

    case CSSPropertyBoxSizing:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBoxSizing(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBoxSizing(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBoxSizing(state, value);
        return;

    case CSSPropertyStrokeWidth:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyStrokeWidth(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyStrokeWidth(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyStrokeWidth(state, value);
        return;

    case CSSPropertyBackgroundBlendMode:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundBlendMode(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundBlendMode(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundBlendMode(state, value);
        return;

    case CSSPropertyMarkerStart:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyMarkerStart(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyMarkerStart(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyMarkerStart(state, value);
        return;

    case CSSPropertyTextDecorationStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextDecorationStyle(state, value);
        return;

    case CSSPropertyBackgroundRepeatX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundRepeatX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundRepeatX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundRepeatX(state, value);
        return;

    case CSSPropertyTranslate:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTranslate(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTranslate(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTranslate(state, value);
        return;

    case CSSPropertyBorderTopColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderTopColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderTopColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderTopColor(state, value);
        return;

    case CSSPropertyBackgroundColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBackgroundColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBackgroundColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBackgroundColor(state, value);
        return;

    case CSSPropertyPaddingTop:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPaddingTop(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPaddingTop(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPaddingTop(state, value);
        return;

    case CSSPropertyWebkitTextEmphasisColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisColor(state, value);
        return;

    case CSSPropertyDisplay:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyDisplay(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyDisplay(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyDisplay(state, value);
        return;

    case CSSPropertyWordBreak:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWordBreak(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWordBreak(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWordBreak(state, value);
        return;

    case CSSPropertyBorderTopStyle:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderTopStyle(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderTopStyle(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderTopStyle(state, value);
        return;

    case CSSPropertyJustifyContent:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyJustifyContent(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyJustifyContent(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyJustifyContent(state, value);
        return;

    case CSSPropertyBoxShadow:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBoxShadow(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBoxShadow(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBoxShadow(state, value);
        return;

    case CSSPropertySpeak:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertySpeak(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertySpeak(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertySpeak(state, value);
        return;

    case CSSPropertyZIndex:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyZIndex(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyZIndex(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyZIndex(state, value);
        return;

    case CSSPropertyListStyleImage:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyListStyleImage(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyListStyleImage(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyListStyleImage(state, value);
        return;

    case CSSPropertyFloodOpacity:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFloodOpacity(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFloodOpacity(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFloodOpacity(state, value);
        return;

    case CSSPropertyClipRule:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyClipRule(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyClipRule(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyClipRule(state, value);
        return;

    case CSSPropertyFlexDirection:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFlexDirection(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFlexDirection(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFlexDirection(state, value);
        return;

    case CSSPropertyLightingColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyLightingColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyLightingColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyLightingColor(state, value);
        return;

    case CSSPropertyWebkitMaskRepeatY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskRepeatY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskRepeatY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskRepeatY(state, value);
        return;

    case CSSPropertyWebkitMaskRepeatX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskRepeatX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskRepeatX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskRepeatX(state, value);
        return;

    case CSSPropertyPageBreakInside:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyPageBreakInside(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyPageBreakInside(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyPageBreakInside(state, value);
        return;

    case CSSPropertyListStyleType:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyListStyleType(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyListStyleType(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyListStyleType(state, value);
        return;

    case CSSPropertyTextAlign:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyTextAlign(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyTextAlign(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyTextAlign(state, value);
        return;

    case CSSPropertyAlignItems:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyAlignItems(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyAlignItems(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyAlignItems(state, value);
        return;

    case CSSPropertyObjectPosition:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyObjectPosition(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyObjectPosition(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyObjectPosition(state, value);
        return;

    case CSSPropertyOrphans:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyOrphans(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyOrphans(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyOrphans(state, value);
        return;

    case CSSPropertyScrollSnapDestination:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapDestination(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapDestination(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyScrollSnapDestination(state, value);
        return;

    case CSSPropertyWebkitPerspectiveOriginY:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitPerspectiveOriginY(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitPerspectiveOriginY(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitPerspectiveOriginY(state, value);
        return;

    case CSSPropertyWebkitPerspectiveOriginX:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyWebkitPerspectiveOriginX(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyWebkitPerspectiveOriginX(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyWebkitPerspectiveOriginX(state, value);
        return;

    case CSSPropertyBorderBottomColor:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomColor(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomColor(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyBorderBottomColor(state, value);
        return;

    case CSSPropertyFontSize:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyFontSize(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyFontSize(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyFontSize(state, value);
        return;

    case CSSPropertyVisibility:
        if (isInitial)
            StyleBuilderFunctions::applyInitialCSSPropertyVisibility(state);
        else if (isInherit)
            StyleBuilderFunctions::applyInheritCSSPropertyVisibility(state);
        else
            StyleBuilderFunctions::applyValueCSSPropertyVisibility(state, value);
        return;

	case CSSPropertyVariable:
		ASSERT(!isInitial && !isInherit);
		StyleBuilderFunctions::applyValueCSSPropertyVariable(state, value);
		return;

    case CSSPropertyWebkitBorderEndColor:
    case CSSPropertyWebkitBorderBeforeStyle:
    case CSSPropertyWebkitBorderEndStyle:
    case CSSPropertyWebkitPaddingStart:
    case CSSPropertyWebkitPaddingEnd:
    case CSSPropertyWebkitBorderStartWidth:
    case CSSPropertyWebkitMaxLogicalWidth:
    case CSSPropertyWebkitLogicalHeight:
    case CSSPropertyWebkitMinLogicalWidth:
    case CSSPropertyWebkitBorderBeforeWidth:
    case CSSPropertyWebkitPaddingBefore:
    case CSSPropertyWebkitBorderBeforeColor:
    case CSSPropertyWebkitMarginEnd:
    case CSSPropertyWebkitBorderAfterWidth:
    case CSSPropertyWebkitMinLogicalHeight:
    case CSSPropertyWebkitBorderEndWidth:
    case CSSPropertyWebkitLogicalWidth:
    case CSSPropertyWebkitBorderAfterColor:
    case CSSPropertyWebkitMaxLogicalHeight:
    case CSSPropertyWebkitBorderStartColor:
    case CSSPropertyWebkitBorderAfterStyle:
    case CSSPropertyWebkitPaddingAfter:
    case CSSPropertyWebkitBorderStartStyle:
    case CSSPropertyWebkitMarginBefore:
    case CSSPropertyWebkitMarginStart:
    case CSSPropertyWebkitMarginAfter:
    {
        CSSPropertyID resolvedProperty = CSSProperty::resolveDirectionAwareProperty(property, state.style()->direction(), state.style()->writingMode());
        ASSERT(resolvedProperty != property);
        applyProperty(resolvedProperty, state, value);
        return;
    }
    case CSSPropertyMinZoom:
    case CSSPropertyPage:
    case CSSPropertyAll:
    case CSSPropertyOrientation:
    case CSSPropertyUnicodeRange:
    case CSSPropertyWebkitTextDecorationsInEffect:
    case CSSPropertyMaxZoom:
    case CSSPropertyUserZoom:
    case CSSPropertySrc:
    //case CSSPropertyEnableBackground:
    case CSSPropertyWebkitFontSizeDelta:
    case CSSPropertyFilter:
        return;
    default:
        ASSERT_NOT_REACHED();
    }
}

} // namespace blink
