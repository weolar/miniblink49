/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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
#include "StylePropertyShorthand.h"
#include "platform/RuntimeEnabledFeatures.h"

#include "wtf/HashMap.h"
#include "wtf/StdLibExtras.h"

namespace blink {

const StylePropertyShorthand& backgroundRepeatShorthand()
{
    static const CSSPropertyID backgroundRepeatProperties[] = {
        CSSPropertyBackgroundRepeatX,
        CSSPropertyBackgroundRepeatY,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, backgroundRepeatLonghands, (CSSPropertyBackgroundRepeat, backgroundRepeatProperties, WTF_ARRAY_LENGTH(backgroundRepeatProperties)));
    return backgroundRepeatLonghands;
}

const StylePropertyShorthand& flexShorthand()
{
    static const CSSPropertyID flexProperties[] = {
        CSSPropertyFlexGrow,
        CSSPropertyFlexShrink,
        CSSPropertyFlexBasis,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, flexLonghands, (CSSPropertyFlex, flexProperties, WTF_ARRAY_LENGTH(flexProperties)));
    return flexLonghands;
}

const StylePropertyShorthand& markerShorthand()
{
    static const CSSPropertyID markerProperties[] = {
        CSSPropertyMarkerStart,
        CSSPropertyMarkerMid,
        CSSPropertyMarkerEnd,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, markerLonghands, (CSSPropertyMarker, markerProperties, WTF_ARRAY_LENGTH(markerProperties)));
    return markerLonghands;
}

const StylePropertyShorthand& flexFlowShorthand()
{
    static const CSSPropertyID flexFlowProperties[] = {
        CSSPropertyFlexDirection,
        CSSPropertyFlexWrap,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, flexFlowLonghands, (CSSPropertyFlexFlow, flexFlowProperties, WTF_ARRAY_LENGTH(flexFlowProperties)));
    return flexFlowLonghands;
}

const StylePropertyShorthand& outlineShorthand()
{
    static const CSSPropertyID outlineProperties[] = {
        CSSPropertyOutlineColor,
        CSSPropertyOutlineStyle,
        CSSPropertyOutlineWidth,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, outlineLonghands, (CSSPropertyOutline, outlineProperties, WTF_ARRAY_LENGTH(outlineProperties)));
    return outlineLonghands;
}

const StylePropertyShorthand& webkitColumnsShorthand()
{
    static const CSSPropertyID webkitColumnsProperties[] = {
        CSSPropertyWebkitColumnWidth,
        CSSPropertyWebkitColumnCount,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitColumnsLonghands, (CSSPropertyWebkitColumns, webkitColumnsProperties, WTF_ARRAY_LENGTH(webkitColumnsProperties)));
    return webkitColumnsLonghands;
}

const StylePropertyShorthand& overflowShorthand()
{
    static const CSSPropertyID overflowProperties[] = {
        CSSPropertyOverflowX,
        CSSPropertyOverflowY,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, overflowLonghands, (CSSPropertyOverflow, overflowProperties, WTF_ARRAY_LENGTH(overflowProperties)));
    return overflowLonghands;
}

const StylePropertyShorthand& webkitTextStrokeShorthand()
{
    static const CSSPropertyID webkitTextStrokeProperties[] = {
        CSSPropertyWebkitTextStrokeWidth,
        CSSPropertyWebkitTextStrokeColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitTextStrokeLonghands, (CSSPropertyWebkitTextStroke, webkitTextStrokeProperties, WTF_ARRAY_LENGTH(webkitTextStrokeProperties)));
    return webkitTextStrokeLonghands;
}

const StylePropertyShorthand& listStyleShorthand()
{
    static const CSSPropertyID listStyleProperties[] = {
        CSSPropertyListStyleType,
        CSSPropertyListStylePosition,
        CSSPropertyListStyleImage,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, listStyleLonghands, (CSSPropertyListStyle, listStyleProperties, WTF_ARRAY_LENGTH(listStyleProperties)));
    return listStyleLonghands;
}

const StylePropertyShorthand& borderStyleShorthand()
{
    static const CSSPropertyID borderStyleProperties[] = {
        CSSPropertyBorderTopStyle,
        CSSPropertyBorderRightStyle,
        CSSPropertyBorderBottomStyle,
        CSSPropertyBorderLeftStyle,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderStyleLonghands, (CSSPropertyBorderStyle, borderStyleProperties, WTF_ARRAY_LENGTH(borderStyleProperties)));
    return borderStyleLonghands;
}

const StylePropertyShorthand& webkitMarginCollapseShorthand()
{
    static const CSSPropertyID webkitMarginCollapseProperties[] = {
        CSSPropertyWebkitMarginBeforeCollapse,
        CSSPropertyWebkitMarginAfterCollapse,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitMarginCollapseLonghands, (CSSPropertyWebkitMarginCollapse, webkitMarginCollapseProperties, WTF_ARRAY_LENGTH(webkitMarginCollapseProperties)));
    return webkitMarginCollapseLonghands;
}

const StylePropertyShorthand& marginShorthand()
{
    static const CSSPropertyID marginProperties[] = {
        CSSPropertyMarginTop,
        CSSPropertyMarginRight,
        CSSPropertyMarginBottom,
        CSSPropertyMarginLeft,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, marginLonghands, (CSSPropertyMargin, marginProperties, WTF_ARRAY_LENGTH(marginProperties)));
    return marginLonghands;
}

const StylePropertyShorthand& borderSpacingShorthand()
{
    static const CSSPropertyID borderSpacingProperties[] = {
        CSSPropertyWebkitBorderHorizontalSpacing,
        CSSPropertyWebkitBorderVerticalSpacing,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderSpacingLonghands, (CSSPropertyBorderSpacing, borderSpacingProperties, WTF_ARRAY_LENGTH(borderSpacingProperties)));
    return borderSpacingLonghands;
}

const StylePropertyShorthand& textDecorationShorthand()
{
    static const CSSPropertyID textDecorationProperties[] = {
        CSSPropertyTextDecorationLine,
        CSSPropertyTextDecorationStyle,
        CSSPropertyTextDecorationColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, textDecorationLonghands, (CSSPropertyTextDecoration, textDecorationProperties, WTF_ARRAY_LENGTH(textDecorationProperties)));
    return textDecorationLonghands;
}

const StylePropertyShorthand& borderRightShorthand()
{
    static const CSSPropertyID borderRightProperties[] = {
        CSSPropertyBorderRightWidth,
        CSSPropertyBorderRightStyle,
        CSSPropertyBorderRightColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderRightLonghands, (CSSPropertyBorderRight, borderRightProperties, WTF_ARRAY_LENGTH(borderRightProperties)));
    return borderRightLonghands;
}

const StylePropertyShorthand& backgroundShorthand()
{
    static const CSSPropertyID backgroundProperties[] = {
        CSSPropertyBackgroundImage,
        CSSPropertyBackgroundPositionX,
        CSSPropertyBackgroundPositionY,
        CSSPropertyBackgroundSize,
        CSSPropertyBackgroundRepeatX,
        CSSPropertyBackgroundRepeatY,
        CSSPropertyBackgroundAttachment,
        CSSPropertyBackgroundOrigin,
        CSSPropertyBackgroundClip,
        CSSPropertyBackgroundColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, backgroundLonghands, (CSSPropertyBackground, backgroundProperties, WTF_ARRAY_LENGTH(backgroundProperties)));
    return backgroundLonghands;
}

const StylePropertyShorthand& webkitMaskRepeatShorthand()
{
    static const CSSPropertyID webkitMaskRepeatProperties[] = {
        CSSPropertyWebkitMaskRepeatX,
        CSSPropertyWebkitMaskRepeatY,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitMaskRepeatLonghands, (CSSPropertyWebkitMaskRepeat, webkitMaskRepeatProperties, WTF_ARRAY_LENGTH(webkitMaskRepeatProperties)));
    return webkitMaskRepeatLonghands;
}

const StylePropertyShorthand& gridShorthand()
{
    static const CSSPropertyID gridProperties[] = {
        CSSPropertyGridTemplateColumns,
        CSSPropertyGridTemplateRows,
        CSSPropertyGridTemplateAreas,
        CSSPropertyGridAutoFlow,
        CSSPropertyGridAutoColumns,
        CSSPropertyGridAutoRows,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, gridLonghands, (CSSPropertyGrid, gridProperties, WTF_ARRAY_LENGTH(gridProperties)));
    return gridLonghands;
}

const StylePropertyShorthand& webkitBorderEndShorthand()
{
    static const CSSPropertyID webkitBorderEndProperties[] = {
        CSSPropertyWebkitBorderEndWidth,
        CSSPropertyWebkitBorderEndStyle,
        CSSPropertyWebkitBorderEndColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitBorderEndLonghands, (CSSPropertyWebkitBorderEnd, webkitBorderEndProperties, WTF_ARRAY_LENGTH(webkitBorderEndProperties)));
    return webkitBorderEndLonghands;
}

const StylePropertyShorthand& borderTopShorthand()
{
    static const CSSPropertyID borderTopProperties[] = {
        CSSPropertyBorderTopWidth,
        CSSPropertyBorderTopStyle,
        CSSPropertyBorderTopColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderTopLonghands, (CSSPropertyBorderTop, borderTopProperties, WTF_ARRAY_LENGTH(borderTopProperties)));
    return borderTopLonghands;
}

const StylePropertyShorthand& webkitBorderAfterShorthand()
{
    static const CSSPropertyID webkitBorderAfterProperties[] = {
        CSSPropertyWebkitBorderAfterWidth,
        CSSPropertyWebkitBorderAfterStyle,
        CSSPropertyWebkitBorderAfterColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitBorderAfterLonghands, (CSSPropertyWebkitBorderAfter, webkitBorderAfterProperties, WTF_ARRAY_LENGTH(webkitBorderAfterProperties)));
    return webkitBorderAfterLonghands;
}

const StylePropertyShorthand& fontShorthand()
{
    static const CSSPropertyID fontProperties[] = {
        CSSPropertyFontStyle,
        CSSPropertyFontVariant,
        CSSPropertyFontWeight,
        CSSPropertyFontStretch,
        CSSPropertyFontSize,
        CSSPropertyLineHeight,
        CSSPropertyFontFamily,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, fontLonghands, (CSSPropertyFont, fontProperties, WTF_ARRAY_LENGTH(fontProperties)));
    return fontLonghands;
}

const StylePropertyShorthand& transitionShorthand()
{
    static const CSSPropertyID transitionProperties[] = {
        CSSPropertyTransitionProperty,
        CSSPropertyTransitionDuration,
        CSSPropertyTransitionTimingFunction,
        CSSPropertyTransitionDelay,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, transitionLonghands, (CSSPropertyTransition, transitionProperties, WTF_ARRAY_LENGTH(transitionProperties)));
    return transitionLonghands;
}

const StylePropertyShorthand& animationShorthand()
{
    static const CSSPropertyID animationProperties[] = {
        CSSPropertyAnimationName,
        CSSPropertyAnimationDuration,
        CSSPropertyAnimationTimingFunction,
        CSSPropertyAnimationDelay,
        CSSPropertyAnimationIterationCount,
        CSSPropertyAnimationDirection,
        CSSPropertyAnimationFillMode,
        CSSPropertyAnimationPlayState,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, animationLonghands, (CSSPropertyAnimation, animationProperties, WTF_ARRAY_LENGTH(animationProperties)));
    return animationLonghands;
}

const StylePropertyShorthand& webkitBorderBeforeShorthand()
{
    static const CSSPropertyID webkitBorderBeforeProperties[] = {
        CSSPropertyWebkitBorderBeforeWidth,
        CSSPropertyWebkitBorderBeforeStyle,
        CSSPropertyWebkitBorderBeforeColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitBorderBeforeLonghands, (CSSPropertyWebkitBorderBefore, webkitBorderBeforeProperties, WTF_ARRAY_LENGTH(webkitBorderBeforeProperties)));
    return webkitBorderBeforeLonghands;
}

const StylePropertyShorthand& paddingShorthand()
{
    static const CSSPropertyID paddingProperties[] = {
        CSSPropertyPaddingTop,
        CSSPropertyPaddingRight,
        CSSPropertyPaddingBottom,
        CSSPropertyPaddingLeft,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, paddingLonghands, (CSSPropertyPadding, paddingProperties, WTF_ARRAY_LENGTH(paddingProperties)));
    return paddingLonghands;
}

const StylePropertyShorthand& webkitMaskPositionShorthand()
{
    static const CSSPropertyID webkitMaskPositionProperties[] = {
        CSSPropertyWebkitMaskPositionX,
        CSSPropertyWebkitMaskPositionY,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitMaskPositionLonghands, (CSSPropertyWebkitMaskPosition, webkitMaskPositionProperties, WTF_ARRAY_LENGTH(webkitMaskPositionProperties)));
    return webkitMaskPositionLonghands;
}

const StylePropertyShorthand& borderLeftShorthand()
{
    static const CSSPropertyID borderLeftProperties[] = {
        CSSPropertyBorderLeftWidth,
        CSSPropertyBorderLeftStyle,
        CSSPropertyBorderLeftColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderLeftLonghands, (CSSPropertyBorderLeft, borderLeftProperties, WTF_ARRAY_LENGTH(borderLeftProperties)));
    return borderLeftLonghands;
}

const StylePropertyShorthand& webkitMaskShorthand()
{
    static const CSSPropertyID webkitMaskProperties[] = {
        CSSPropertyWebkitMaskImage,
        CSSPropertyWebkitMaskPositionX,
        CSSPropertyWebkitMaskPositionY,
        CSSPropertyWebkitMaskSize,
        CSSPropertyWebkitMaskRepeatX,
        CSSPropertyWebkitMaskRepeatY,
        CSSPropertyWebkitMaskOrigin,
        CSSPropertyWebkitMaskClip,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitMaskLonghands, (CSSPropertyWebkitMask, webkitMaskProperties, WTF_ARRAY_LENGTH(webkitMaskProperties)));
    return webkitMaskLonghands;
}

const StylePropertyShorthand& gridColumnShorthand()
{
    static const CSSPropertyID gridColumnProperties[] = {
        CSSPropertyGridColumnStart,
        CSSPropertyGridColumnEnd,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, gridColumnLonghands, (CSSPropertyGridColumn, gridColumnProperties, WTF_ARRAY_LENGTH(gridColumnProperties)));
    return gridColumnLonghands;
}

const StylePropertyShorthand& webkitBorderStartShorthand()
{
    static const CSSPropertyID webkitBorderStartProperties[] = {
        CSSPropertyWebkitBorderStartWidth,
        CSSPropertyWebkitBorderStartStyle,
        CSSPropertyWebkitBorderStartColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitBorderStartLonghands, (CSSPropertyWebkitBorderStart, webkitBorderStartProperties, WTF_ARRAY_LENGTH(webkitBorderStartProperties)));
    return webkitBorderStartLonghands;
}

const StylePropertyShorthand& gridAreaShorthand()
{
    static const CSSPropertyID gridAreaProperties[] = {
        CSSPropertyGridRowStart,
        CSSPropertyGridColumnStart,
        CSSPropertyGridRowEnd,
        CSSPropertyGridColumnEnd,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, gridAreaLonghands, (CSSPropertyGridArea, gridAreaProperties, WTF_ARRAY_LENGTH(gridAreaProperties)));
    return gridAreaLonghands;
}

const StylePropertyShorthand& borderColorShorthand()
{
    static const CSSPropertyID borderColorProperties[] = {
        CSSPropertyBorderTopColor,
        CSSPropertyBorderRightColor,
        CSSPropertyBorderBottomColor,
        CSSPropertyBorderLeftColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderColorLonghands, (CSSPropertyBorderColor, borderColorProperties, WTF_ARRAY_LENGTH(borderColorProperties)));
    return borderColorLonghands;
}

const StylePropertyShorthand& webkitColumnRuleShorthand()
{
    static const CSSPropertyID webkitColumnRuleProperties[] = {
        CSSPropertyWebkitColumnRuleWidth,
        CSSPropertyWebkitColumnRuleStyle,
        CSSPropertyWebkitColumnRuleColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitColumnRuleLonghands, (CSSPropertyWebkitColumnRule, webkitColumnRuleProperties, WTF_ARRAY_LENGTH(webkitColumnRuleProperties)));
    return webkitColumnRuleLonghands;
}

const StylePropertyShorthand& borderWidthShorthand()
{
    static const CSSPropertyID borderWidthProperties[] = {
        CSSPropertyBorderTopWidth,
        CSSPropertyBorderRightWidth,
        CSSPropertyBorderBottomWidth,
        CSSPropertyBorderLeftWidth,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderWidthLonghands, (CSSPropertyBorderWidth, borderWidthProperties, WTF_ARRAY_LENGTH(borderWidthProperties)));
    return borderWidthLonghands;
}

const StylePropertyShorthand& gridTemplateShorthand()
{
    static const CSSPropertyID gridTemplateProperties[] = {
        CSSPropertyGridTemplateColumns,
        CSSPropertyGridTemplateRows,
        CSSPropertyGridTemplateAreas,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, gridTemplateLonghands, (CSSPropertyGridTemplate, gridTemplateProperties, WTF_ARRAY_LENGTH(gridTemplateProperties)));
    return gridTemplateLonghands;
}

const StylePropertyShorthand& motionShorthand()
{
    static const CSSPropertyID motionProperties[] = {
        CSSPropertyMotionPath,
        CSSPropertyMotionOffset,
        CSSPropertyMotionRotation,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, motionLonghands, (CSSPropertyMotion, motionProperties, WTF_ARRAY_LENGTH(motionProperties)));
    return motionLonghands;
}

const StylePropertyShorthand& backgroundPositionShorthand()
{
    static const CSSPropertyID backgroundPositionProperties[] = {
        CSSPropertyBackgroundPositionX,
        CSSPropertyBackgroundPositionY,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, backgroundPositionLonghands, (CSSPropertyBackgroundPosition, backgroundPositionProperties, WTF_ARRAY_LENGTH(backgroundPositionProperties)));
    return backgroundPositionLonghands;
}

const StylePropertyShorthand& borderShorthand()
{
    static const CSSPropertyID borderProperties[] = {
        CSSPropertyBorderTopColor,
        CSSPropertyBorderTopStyle,
        CSSPropertyBorderTopWidth,
        CSSPropertyBorderRightColor,
        CSSPropertyBorderRightStyle,
        CSSPropertyBorderRightWidth,
        CSSPropertyBorderBottomColor,
        CSSPropertyBorderBottomStyle,
        CSSPropertyBorderBottomWidth,
        CSSPropertyBorderLeftColor,
        CSSPropertyBorderLeftStyle,
        CSSPropertyBorderLeftWidth,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderLonghands, (CSSPropertyBorder, borderProperties, WTF_ARRAY_LENGTH(borderProperties)));
    return borderLonghands;
}

const StylePropertyShorthand& borderBottomShorthand()
{
    static const CSSPropertyID borderBottomProperties[] = {
        CSSPropertyBorderBottomWidth,
        CSSPropertyBorderBottomStyle,
        CSSPropertyBorderBottomColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderBottomLonghands, (CSSPropertyBorderBottom, borderBottomProperties, WTF_ARRAY_LENGTH(borderBottomProperties)));
    return borderBottomLonghands;
}

const StylePropertyShorthand& gridRowShorthand()
{
    static const CSSPropertyID gridRowProperties[] = {
        CSSPropertyGridRowStart,
        CSSPropertyGridRowEnd,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, gridRowLonghands, (CSSPropertyGridRow, gridRowProperties, WTF_ARRAY_LENGTH(gridRowProperties)));
    return gridRowLonghands;
}

const StylePropertyShorthand& borderRadiusShorthand()
{
    static const CSSPropertyID borderRadiusProperties[] = {
        CSSPropertyBorderTopLeftRadius,
        CSSPropertyBorderTopRightRadius,
        CSSPropertyBorderBottomRightRadius,
        CSSPropertyBorderBottomLeftRadius,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderRadiusLonghands, (CSSPropertyBorderRadius, borderRadiusProperties, WTF_ARRAY_LENGTH(borderRadiusProperties)));
    return borderRadiusLonghands;
}

const StylePropertyShorthand& webkitTextEmphasisShorthand()
{
    static const CSSPropertyID webkitTextEmphasisProperties[] = {
        CSSPropertyWebkitTextEmphasisStyle,
        CSSPropertyWebkitTextEmphasisColor,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitTextEmphasisLonghands, (CSSPropertyWebkitTextEmphasis, webkitTextEmphasisProperties, WTF_ARRAY_LENGTH(webkitTextEmphasisProperties)));
    return webkitTextEmphasisLonghands;
}

const StylePropertyShorthand& webkitMaskBoxImageShorthand()
{
    static const CSSPropertyID webkitMaskBoxImageProperties[] = {
        CSSPropertyWebkitMaskBoxImageSource,
        CSSPropertyWebkitMaskBoxImageSlice,
        CSSPropertyWebkitMaskBoxImageWidth,
        CSSPropertyWebkitMaskBoxImageOutset,
        CSSPropertyWebkitMaskBoxImageRepeat,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, webkitMaskBoxImageLonghands, (CSSPropertyWebkitMaskBoxImage, webkitMaskBoxImageProperties, WTF_ARRAY_LENGTH(webkitMaskBoxImageProperties)));
    return webkitMaskBoxImageLonghands;
}

const StylePropertyShorthand& borderImageShorthand()
{
    static const CSSPropertyID borderImageProperties[] = {
        CSSPropertyBorderImageSource,
        CSSPropertyBorderImageSlice,
        CSSPropertyBorderImageWidth,
        CSSPropertyBorderImageOutset,
        CSSPropertyBorderImageRepeat,
    };
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, borderImageLonghands, (CSSPropertyBorderImage, borderImageProperties, WTF_ARRAY_LENGTH(borderImageProperties)));
    return borderImageLonghands;
}

// Returns an empty list if the property is not a shorthand
const StylePropertyShorthand& shorthandForProperty(CSSPropertyID propertyID)
{
    DEFINE_STATIC_LOCAL(StylePropertyShorthand, emptyShorthand, ());
    if (propertyID == CSSPropertyTextDecoration && !RuntimeEnabledFeatures::css3TextDecorationsEnabled())
        return emptyShorthand;
    switch (propertyID) {
        case CSSPropertyBackgroundRepeat:
            return backgroundRepeatShorthand();
        case CSSPropertyFlex:
            return flexShorthand();
        case CSSPropertyMarker:
            return markerShorthand();
        case CSSPropertyFlexFlow:
            return flexFlowShorthand();
        case CSSPropertyOutline:
            return outlineShorthand();
        case CSSPropertyWebkitColumns:
            return webkitColumnsShorthand();
        case CSSPropertyOverflow:
            return overflowShorthand();
        case CSSPropertyWebkitTextStroke:
            return webkitTextStrokeShorthand();
        case CSSPropertyListStyle:
            return listStyleShorthand();
        case CSSPropertyBorderStyle:
            return borderStyleShorthand();
        case CSSPropertyWebkitMarginCollapse:
            return webkitMarginCollapseShorthand();
        case CSSPropertyMargin:
            return marginShorthand();
        case CSSPropertyBorderSpacing:
            return borderSpacingShorthand();
        case CSSPropertyTextDecoration:
            return textDecorationShorthand();
        case CSSPropertyBorderRight:
            return borderRightShorthand();
        case CSSPropertyBackground:
            return backgroundShorthand();
        case CSSPropertyWebkitMaskRepeat:
            return webkitMaskRepeatShorthand();
        case CSSPropertyGrid:
            return gridShorthand();
        case CSSPropertyWebkitBorderEnd:
            return webkitBorderEndShorthand();
        case CSSPropertyBorderTop:
            return borderTopShorthand();
        case CSSPropertyWebkitBorderAfter:
            return webkitBorderAfterShorthand();
        case CSSPropertyFont:
            return fontShorthand();
        case CSSPropertyTransition:
            return transitionShorthand();
        case CSSPropertyAnimation:
            return animationShorthand();
        case CSSPropertyWebkitBorderBefore:
            return webkitBorderBeforeShorthand();
        case CSSPropertyPadding:
            return paddingShorthand();
        case CSSPropertyWebkitMaskPosition:
            return webkitMaskPositionShorthand();
        case CSSPropertyBorderLeft:
            return borderLeftShorthand();
        case CSSPropertyWebkitMask:
            return webkitMaskShorthand();
        case CSSPropertyGridColumn:
            return gridColumnShorthand();
        case CSSPropertyWebkitBorderStart:
            return webkitBorderStartShorthand();
        case CSSPropertyGridArea:
            return gridAreaShorthand();
        case CSSPropertyBorderColor:
            return borderColorShorthand();
        case CSSPropertyWebkitColumnRule:
            return webkitColumnRuleShorthand();
        case CSSPropertyBorderWidth:
            return borderWidthShorthand();
        case CSSPropertyGridTemplate:
            return gridTemplateShorthand();
        case CSSPropertyMotion:
            return motionShorthand();
        case CSSPropertyBackgroundPosition:
            return backgroundPositionShorthand();
        case CSSPropertyBorder:
            return borderShorthand();
        case CSSPropertyBorderBottom:
            return borderBottomShorthand();
        case CSSPropertyGridRow:
            return gridRowShorthand();
        case CSSPropertyBorderRadius:
            return borderRadiusShorthand();
        case CSSPropertyWebkitTextEmphasis:
            return webkitTextEmphasisShorthand();
        case CSSPropertyWebkitMaskBoxImage:
            return webkitMaskBoxImageShorthand();
        case CSSPropertyBorderImage:
            return borderImageShorthand();
    default: {
        return emptyShorthand;
    }
    }
}

void getMatchingShorthandsForLonghand(CSSPropertyID propertyID, Vector<StylePropertyShorthand, 4>* result)
{
    ASSERT(!result->size());
    switch (propertyID) {
    case CSSPropertyWebkitMaskBoxImageWidth: {
        result->uncheckedAppend(webkitMaskBoxImageShorthand());
        break;
    }
    case CSSPropertyWebkitMaskBoxImageRepeat: {
        result->uncheckedAppend(webkitMaskBoxImageShorthand());
        break;
    }
    case CSSPropertyWebkitBorderEndColor: {
        result->uncheckedAppend(webkitBorderEndShorthand());
        break;
    }
    case CSSPropertyWebkitColumnRuleColor: {
        result->uncheckedAppend(webkitColumnRuleShorthand());
        break;
    }
    case CSSPropertyFlexWrap: {
        result->uncheckedAppend(flexFlowShorthand());
        break;
    }
    case CSSPropertyWebkitBorderHorizontalSpacing: {
        result->uncheckedAppend(borderSpacingShorthand());
        break;
    }
    case CSSPropertyWebkitBorderEndWidth: {
        result->uncheckedAppend(webkitBorderEndShorthand());
        break;
    }
    case CSSPropertyFlexBasis: {
        result->uncheckedAppend(flexShorthand());
        break;
    }
    case CSSPropertyBorderLeftColor: {
        result->uncheckedAppend(borderLeftShorthand());
        result->uncheckedAppend(borderColorShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyWebkitBorderBeforeStyle: {
        result->uncheckedAppend(webkitBorderBeforeShorthand());
        break;
    }
    case CSSPropertyFontStretch: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyWebkitBorderStartColor: {
        result->uncheckedAppend(webkitBorderStartShorthand());
        break;
    }
    case CSSPropertyFlexShrink: {
        result->uncheckedAppend(flexShorthand());
        break;
    }
    case CSSPropertyTransitionDelay: {
        result->uncheckedAppend(transitionShorthand());
        break;
    }
    case CSSPropertyGridTemplateRows: {
        result->uncheckedAppend(gridShorthand());
        result->uncheckedAppend(gridTemplateShorthand());
        break;
    }
    case CSSPropertyBackgroundClip: {
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyWebkitBorderEndStyle: {
        result->uncheckedAppend(webkitBorderEndShorthand());
        break;
    }
    case CSSPropertyTextDecorationColor: {
        result->uncheckedAppend(textDecorationShorthand());
        break;
    }
    case CSSPropertyMotionRotation: {
        result->uncheckedAppend(motionShorthand());
        break;
    }
    case CSSPropertyAnimationName: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyBackgroundPositionY: {
        result->uncheckedAppend(backgroundShorthand());
        result->uncheckedAppend(backgroundPositionShorthand());
        break;
    }
    case CSSPropertyBackgroundPositionX: {
        result->uncheckedAppend(backgroundShorthand());
        result->uncheckedAppend(backgroundPositionShorthand());
        break;
    }
    case CSSPropertyWebkitMarginAfterCollapse: {
        result->uncheckedAppend(webkitMarginCollapseShorthand());
        break;
    }
    case CSSPropertyOutlineWidth: {
        result->uncheckedAppend(outlineShorthand());
        break;
    }
    case CSSPropertyWebkitMaskOrigin: {
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyOutlineStyle: {
        result->uncheckedAppend(outlineShorthand());
        break;
    }
    case CSSPropertyGridAutoFlow: {
        result->uncheckedAppend(gridShorthand());
        break;
    }
    case CSSPropertyMarkerEnd: {
        result->uncheckedAppend(markerShorthand());
        break;
    }
    case CSSPropertyBackgroundOrigin: {
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyMotionPath: {
        result->uncheckedAppend(motionShorthand());
        break;
    }
    case CSSPropertyWebkitColumnRuleStyle: {
        result->uncheckedAppend(webkitColumnRuleShorthand());
        break;
    }
    case CSSPropertyWebkitColumnWidth: {
        result->uncheckedAppend(webkitColumnsShorthand());
        break;
    }
    case CSSPropertyBorderRightColor: {
        result->uncheckedAppend(borderRightShorthand());
        result->uncheckedAppend(borderColorShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyAnimationDuration: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyAnimationPlayState: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyBorderBottomStyle: {
        result->uncheckedAppend(borderStyleShorthand());
        result->uncheckedAppend(borderShorthand());
        result->uncheckedAppend(borderBottomShorthand());
        break;
    }
    case CSSPropertyBorderBottomWidth: {
        result->uncheckedAppend(borderWidthShorthand());
        result->uncheckedAppend(borderShorthand());
        result->uncheckedAppend(borderBottomShorthand());
        break;
    }
    case CSSPropertyBorderTopRightRadius: {
        result->uncheckedAppend(borderRadiusShorthand());
        break;
    }
    case CSSPropertyWebkitColumnCount: {
        result->uncheckedAppend(webkitColumnsShorthand());
        break;
    }
    case CSSPropertyFontStyle: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyBorderTopLeftRadius: {
        result->uncheckedAppend(borderRadiusShorthand());
        break;
    }
    case CSSPropertyBorderBottomRightRadius: {
        result->uncheckedAppend(borderRadiusShorthand());
        break;
    }
    case CSSPropertyBorderRightWidth: {
        result->uncheckedAppend(borderRightShorthand());
        result->uncheckedAppend(borderWidthShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyWebkitBorderAfterStyle: {
        result->uncheckedAppend(webkitBorderAfterShorthand());
        break;
    }
    case CSSPropertyBorderImageSource: {
        result->uncheckedAppend(borderImageShorthand());
        break;
    }
    case CSSPropertyBorderRightStyle: {
        result->uncheckedAppend(borderStyleShorthand());
        result->uncheckedAppend(borderRightShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyMotionOffset: {
        result->uncheckedAppend(motionShorthand());
        break;
    }
    case CSSPropertyFontVariant: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyBorderBottomLeftRadius: {
        result->uncheckedAppend(borderRadiusShorthand());
        break;
    }
    case CSSPropertyGridRowEnd: {
        result->uncheckedAppend(gridAreaShorthand());
        result->uncheckedAppend(gridRowShorthand());
        break;
    }
    case CSSPropertyBackgroundAttachment: {
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyWebkitMaskBoxImageOutset: {
        result->uncheckedAppend(webkitMaskBoxImageShorthand());
        break;
    }
    case CSSPropertyBorderLeftWidth: {
        result->uncheckedAppend(borderLeftShorthand());
        result->uncheckedAppend(borderWidthShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyPaddingBottom: {
        result->uncheckedAppend(paddingShorthand());
        break;
    }
    case CSSPropertyWebkitMaskPositionX: {
        result->uncheckedAppend(webkitMaskPositionShorthand());
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyMarkerStart: {
        result->uncheckedAppend(markerShorthand());
        break;
    }
    case CSSPropertyBackgroundImage: {
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyWebkitBorderAfterColor: {
        result->uncheckedAppend(webkitBorderAfterShorthand());
        break;
    }
    case CSSPropertyTextDecorationLine: {
        result->uncheckedAppend(textDecorationShorthand());
        break;
    }
    case CSSPropertyWebkitMaskClip: {
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyTextDecorationStyle: {
        result->uncheckedAppend(textDecorationShorthand());
        break;
    }
    case CSSPropertyBorderImageSlice: {
        result->uncheckedAppend(borderImageShorthand());
        break;
    }
    case CSSPropertyWebkitBorderAfterWidth: {
        result->uncheckedAppend(webkitBorderAfterShorthand());
        break;
    }
    case CSSPropertyBorderTopColor: {
        result->uncheckedAppend(borderTopShorthand());
        result->uncheckedAppend(borderColorShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyBackgroundColor: {
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyOutlineColor: {
        result->uncheckedAppend(outlineShorthand());
        break;
    }
    case CSSPropertyPaddingTop: {
        result->uncheckedAppend(paddingShorthand());
        break;
    }
    case CSSPropertyBorderTopWidth: {
        result->uncheckedAppend(borderTopShorthand());
        result->uncheckedAppend(borderWidthShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyGridColumnStart: {
        result->uncheckedAppend(gridColumnShorthand());
        result->uncheckedAppend(gridAreaShorthand());
        break;
    }
    case CSSPropertyMarginBottom: {
        result->uncheckedAppend(marginShorthand());
        break;
    }
    case CSSPropertyListStylePosition: {
        result->uncheckedAppend(listStyleShorthand());
        break;
    }
    case CSSPropertyAnimationIterationCount: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyWebkitTextEmphasisColor: {
        result->uncheckedAppend(webkitTextEmphasisShorthand());
        break;
    }
    case CSSPropertyWebkitTextEmphasisStyle: {
        result->uncheckedAppend(webkitTextEmphasisShorthand());
        break;
    }
    case CSSPropertyWebkitMaskPositionY: {
        result->uncheckedAppend(webkitMaskPositionShorthand());
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyWebkitMaskSize: {
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyGridAutoRows: {
        result->uncheckedAppend(gridShorthand());
        break;
    }
    case CSSPropertyMarginTop: {
        result->uncheckedAppend(marginShorthand());
        break;
    }
    case CSSPropertyBorderTopStyle: {
        result->uncheckedAppend(borderStyleShorthand());
        result->uncheckedAppend(borderTopShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyWebkitBorderBeforeWidth: {
        result->uncheckedAppend(webkitBorderBeforeShorthand());
        break;
    }
    case CSSPropertyBorderImageOutset: {
        result->uncheckedAppend(borderImageShorthand());
        break;
    }
    case CSSPropertyFontFamily: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyBorderImageWidth: {
        result->uncheckedAppend(borderImageShorthand());
        break;
    }
    case CSSPropertyWebkitMaskBoxImageSource: {
        result->uncheckedAppend(webkitMaskBoxImageShorthand());
        break;
    }
    case CSSPropertyFlexGrow: {
        result->uncheckedAppend(flexShorthand());
        break;
    }
    case CSSPropertyWebkitBorderBeforeColor: {
        result->uncheckedAppend(webkitBorderBeforeShorthand());
        break;
    }
    case CSSPropertyListStyleImage: {
        result->uncheckedAppend(listStyleShorthand());
        break;
    }
    case CSSPropertyWebkitMarginBeforeCollapse: {
        result->uncheckedAppend(webkitMarginCollapseShorthand());
        break;
    }
    case CSSPropertyWebkitMaskImage: {
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyAnimationTimingFunction: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyWebkitColumnRuleWidth: {
        result->uncheckedAppend(webkitColumnRuleShorthand());
        break;
    }
    case CSSPropertyFlexDirection: {
        result->uncheckedAppend(flexFlowShorthand());
        break;
    }
    case CSSPropertyWebkitMaskRepeatY: {
        result->uncheckedAppend(webkitMaskRepeatShorthand());
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyWebkitMaskRepeatX: {
        result->uncheckedAppend(webkitMaskRepeatShorthand());
        result->uncheckedAppend(webkitMaskShorthand());
        break;
    }
    case CSSPropertyGridTemplateAreas: {
        result->uncheckedAppend(gridShorthand());
        result->uncheckedAppend(gridTemplateShorthand());
        break;
    }
    case CSSPropertyBorderImageRepeat: {
        result->uncheckedAppend(borderImageShorthand());
        break;
    }
    case CSSPropertyWebkitBorderStartStyle: {
        result->uncheckedAppend(webkitBorderStartShorthand());
        break;
    }
    case CSSPropertyListStyleType: {
        result->uncheckedAppend(listStyleShorthand());
        break;
    }
    case CSSPropertyOverflowX: {
        result->uncheckedAppend(overflowShorthand());
        break;
    }
    case CSSPropertyFontWeight: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyTransitionTimingFunction: {
        result->uncheckedAppend(transitionShorthand());
        break;
    }
    case CSSPropertyBorderLeftStyle: {
        result->uncheckedAppend(borderStyleShorthand());
        result->uncheckedAppend(borderLeftShorthand());
        result->uncheckedAppend(borderShorthand());
        break;
    }
    case CSSPropertyOverflowY: {
        result->uncheckedAppend(overflowShorthand());
        break;
    }
    case CSSPropertyMarkerMid: {
        result->uncheckedAppend(markerShorthand());
        break;
    }
    case CSSPropertyMarginRight: {
        result->uncheckedAppend(marginShorthand());
        break;
    }
    case CSSPropertyWebkitBorderStartWidth: {
        result->uncheckedAppend(webkitBorderStartShorthand());
        break;
    }
    case CSSPropertyAnimationDelay: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyAnimationFillMode: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyGridTemplateColumns: {
        result->uncheckedAppend(gridShorthand());
        result->uncheckedAppend(gridTemplateShorthand());
        break;
    }
    case CSSPropertyAnimationDirection: {
        result->uncheckedAppend(animationShorthand());
        break;
    }
    case CSSPropertyWebkitMaskBoxImageSlice: {
        result->uncheckedAppend(webkitMaskBoxImageShorthand());
        break;
    }
    case CSSPropertyWebkitTextStrokeColor: {
        result->uncheckedAppend(webkitTextStrokeShorthand());
        break;
    }
    case CSSPropertyLineHeight: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyPaddingLeft: {
        result->uncheckedAppend(paddingShorthand());
        break;
    }
    case CSSPropertyBackgroundRepeatY: {
        result->uncheckedAppend(backgroundRepeatShorthand());
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyBackgroundRepeatX: {
        result->uncheckedAppend(backgroundRepeatShorthand());
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyBorderBottomColor: {
        result->uncheckedAppend(borderColorShorthand());
        result->uncheckedAppend(borderShorthand());
        result->uncheckedAppend(borderBottomShorthand());
        break;
    }
    case CSSPropertyFontSize: {
        result->uncheckedAppend(fontShorthand());
        break;
    }
    case CSSPropertyGridColumnEnd: {
        result->uncheckedAppend(gridColumnShorthand());
        result->uncheckedAppend(gridAreaShorthand());
        break;
    }
    case CSSPropertyBackgroundSize: {
        result->uncheckedAppend(backgroundShorthand());
        break;
    }
    case CSSPropertyPaddingRight: {
        result->uncheckedAppend(paddingShorthand());
        break;
    }
    case CSSPropertyWebkitBorderVerticalSpacing: {
        result->uncheckedAppend(borderSpacingShorthand());
        break;
    }
    case CSSPropertyGridAutoColumns: {
        result->uncheckedAppend(gridShorthand());
        break;
    }
    case CSSPropertyGridRowStart: {
        result->uncheckedAppend(gridAreaShorthand());
        result->uncheckedAppend(gridRowShorthand());
        break;
    }
    case CSSPropertyTransitionDuration: {
        result->uncheckedAppend(transitionShorthand());
        break;
    }
    case CSSPropertyTransitionProperty: {
        result->uncheckedAppend(transitionShorthand());
        break;
    }
    case CSSPropertyMarginLeft: {
        result->uncheckedAppend(marginShorthand());
        break;
    }
    case CSSPropertyWebkitTextStrokeWidth: {
        result->uncheckedAppend(webkitTextStrokeShorthand());
        break;
    }
    default:
        break;
    }
}

} // namespace blink
