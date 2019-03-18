/*
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 *           (C) 2000 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Graham Dennis (graham.dennis@gmail.com)
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
 *
 */

#ifndef ComputedStyle_h
#define ComputedStyle_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/animation/css/CSSAnimationData.h"
#include "core/animation/css/CSSTransitionData.h"
#include "core/css/CSSLineBoxContainValue.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/style/BorderValue.h"
#include "core/style/CounterDirectives.h"
#include "core/style/DataRef.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/style/LineClampValue.h"
#include "core/style/NinePieceImage.h"
#include "core/style/OutlineValue.h"
#include "core/style/SVGComputedStyle.h"
#include "core/style/ShapeValue.h"
#include "core/style/StyleBackgroundData.h"
#include "core/style/StyleBoxData.h"
#include "core/style/StyleContentAlignmentData.h"
#include "core/style/StyleDeprecatedFlexibleBoxData.h"
#include "core/style/StyleDifference.h"
#include "core/style/StyleFilterData.h"
#include "core/style/StyleFlexibleBoxData.h"
#include "core/style/StyleGridData.h"
#include "core/style/StyleGridItemData.h"
#include "core/style/StyleInheritedData.h"
#include "core/style/StyleMotionPath.h"
#include "core/style/StyleMultiColData.h"
#include "core/style/StyleRareInheritedData.h"
#include "core/style/StyleRareNonInheritedData.h"
#include "core/style/StyleReflection.h"
#include "core/style/StyleScrollSnapData.h"
#include "core/style/StyleSelfAlignmentData.h"
#include "core/style/StyleSurroundData.h"
#include "core/style/StyleTransformData.h"
#include "core/style/StyleVisualData.h"
#include "core/style/StyleWillChangeData.h"
#include "core/style/TransformOrigin.h"
#include "platform/Length.h"
#include "platform/LengthBox.h"
#include "platform/LengthPoint.h"
#include "platform/LengthSize.h"
#include "platform/ThemeTypes.h"
#include "platform/fonts/FontBaseline.h"
#include "platform/fonts/FontDescription.h"
#include "platform/geometry/FloatRoundedRect.h"
#include "platform/geometry/LayoutRectOutsets.h"
#include "platform/graphics/Color.h"
#include "platform/graphics/GraphicsTypes.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/text/TextDirection.h"
#include "platform/text/TextRun.h"
#include "platform/text/UnicodeBidi.h"
#include "platform/transforms/TransformOperations.h"
#include "public/platform/WebScrollBlocksOn.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Vector.h"

template<typename T, typename U> inline bool compareEqual(const T& t, const U& u) { return t == static_cast<T>(u); }

#define SET_VAR(group, variable, value) \
    if (!compareEqual(group->variable, value)) \
        group.access()->variable = value

#define SET_VAR_WITH_SETTER(group, getter, setter, value)    \
    if (!compareEqual(group->getter(), value)) \
        group.access()->setter(value)

#define SET_BORDERVALUE_COLOR(group, variable, value) \
    if (!compareEqual(group->variable.color(), value)) \
        group.access()->variable.setColor(value)

namespace blink {

using std::max;

class FilterOperations;

class AppliedTextDecoration;
class BorderData;
struct BorderEdge;
class CounterContent;
class Font;
class FontMetrics;
class RotateTransformOperation;
class ScaleTransformOperation;
class ShadowList;
class StyleImage;
class StyleInheritedData;
class StyleResolver;
class TransformationMatrix;
class TranslateTransformOperation;

class ContentData;

typedef Vector<RefPtr<ComputedStyle>, 4> PseudoStyleCache;

class CORE_EXPORT ComputedStyle: public RefCounted<ComputedStyle> {
    friend class AnimatedStyleBuilder; // Used by Web Animations CSS. Sets the color styles
    friend class CSSAnimatableValueFactory; // Used by Web Animations CSS. Gets visited and unvisited colors separately.
    friend class CSSPropertyEquality; // Used by CSS animations. We can't allow them to animate based off visited colors.
    friend class ApplyStyleCommand; // Editing has to only reveal unvisited info.
    friend class EditingStyle; // Editing has to only reveal unvisited info.
    friend class ComputedStyleCSSValueMapping; // Needs to be able to see visited and unvisited colors for devtools.
    friend class StyleBuilderFunctions; // Sets color styles
    friend class CachedUAStyle; // Saves Border/Background information for later comparison.

    // FIXME: When we stop resolving currentColor at style time, these can be removed.
    friend class CSSToStyleMap;
    friend class FilterOperationResolver;
    friend class StyleBuilderConverter;
    friend class StyleResolverState;
    friend class StyleResolver;
protected:

    // non-inherited attributes
    DataRef<StyleBoxData> m_box;
    DataRef<StyleVisualData> visual;
    DataRef<StyleBackgroundData> m_background;
    DataRef<StyleSurroundData> surround;
    DataRef<StyleRareNonInheritedData> rareNonInheritedData;

    // inherited attributes
    DataRef<StyleRareInheritedData> rareInheritedData;
    DataRef<StyleInheritedData> inherited;

    // list of associated pseudo styles
    OwnPtr<PseudoStyleCache> m_cachedPseudoStyles;

    DataRef<SVGComputedStyle> m_svgStyle;

// !START SYNC!: Keep this in sync with the copy constructor in ComputedStyle.cpp and implicitlyInherited() in StyleResolver.cpp

    // inherit
    struct InheritedFlags {
        bool operator==(const InheritedFlags& other) const
        {
            return (_empty_cells == other._empty_cells)
                && (_caption_side == other._caption_side)
                && (_list_style_type == other._list_style_type)
                && (_list_style_position == other._list_style_position)
                && (_visibility == other._visibility)
                && (_text_align == other._text_align)
                && (_text_transform == other._text_transform)
                && (m_textUnderline == other.m_textUnderline)
                && (_cursor_style == other._cursor_style)
                && (_direction == other._direction)
                && (_white_space == other._white_space)
                && (_border_collapse == other._border_collapse)
                && (_box_direction == other._box_direction)
                && (m_rtlOrdering == other.m_rtlOrdering)
                && (m_printColorAdjust == other.m_printColorAdjust)
                && (_pointerEvents == other._pointerEvents)
                && (_insideLink == other._insideLink)
                && (m_writingMode == other.m_writingMode);
        }

        bool operator!=(const InheritedFlags& other) const { return !(*this == other); }

        unsigned _empty_cells : 1; // EEmptyCell
        unsigned _caption_side : 2; // ECaptionSide
        unsigned _list_style_type : 7; // EListStyleType
        unsigned _list_style_position : 1; // EListStylePosition
        unsigned _visibility : 2; // EVisibility
        unsigned _text_align : 4; // ETextAlign
        unsigned _text_transform : 2; // ETextTransform
        unsigned m_textUnderline : 1;
        unsigned _cursor_style : 6; // ECursor
        unsigned _direction : 1; // TextDirection
        unsigned _white_space : 3; // EWhiteSpace
        unsigned _border_collapse : 1; // EBorderCollapse
        unsigned _box_direction : 1; // EBoxDirection (CSS3 box_direction property, flexible box layout module)
        // 32 bits

        // non CSS2 inherited
        unsigned m_rtlOrdering : 1; // Order
        unsigned m_printColorAdjust : PrintColorAdjustBits;
        unsigned _pointerEvents : 4; // EPointerEvents
        unsigned _insideLink : 2; // EInsideLink

        // CSS Text Layout Module Level 3: Vertical writing support
        unsigned m_writingMode : 2; // WritingMode
        // 42 bits
    } inherited_flags;

// don't inherit
    struct NonInheritedFlags {
        // Compare computed styles, differences in other flags should not cause an inequality.
        bool operator==(const NonInheritedFlags& other) const
        {
            return effectiveDisplay == other.effectiveDisplay
                && originalDisplay == other.originalDisplay
                && overflowX == other.overflowX
                && overflowY == other.overflowY
                && verticalAlign == other.verticalAlign
                && clear == other.clear
                && position == other.position
                && floating == other.floating
                && tableLayout == other.tableLayout
                && unicodeBidi == other.unicodeBidi
                // hasViewportUnits
                && pageBreakBefore == other.pageBreakBefore
                && pageBreakAfter == other.pageBreakAfter
                && pageBreakInside == other.pageBreakInside;
                // styleType
                // pseudoBits
                // explicitInheritance
                // unique
                // emptyState
                // affectedByFocus
                // affectedByHover
                // affectedByActive
                // affectedByDrag
                // isLink
        }

        bool operator!=(const NonInheritedFlags& other) const { return !(*this == other); }

        unsigned effectiveDisplay : 5; // EDisplay
        unsigned originalDisplay : 5; // EDisplay
        unsigned overflowX : 3; // EOverflow
        unsigned overflowY : 3; // EOverflow
        unsigned verticalAlign : 4; // EVerticalAlign
        unsigned clear : 2; // EClear
        unsigned position : 3; // EPosition
        unsigned floating : 2; // EFloat
        unsigned tableLayout : 1; // ETableLayout
        unsigned unicodeBidi : 3; // EUnicodeBidi

        // This is set if we used viewport units when resolving a length.
        // It is mutable so we can pass around const ComputedStyles to resolve lengths.
        mutable unsigned hasViewportUnits : 1;

        // 32 bits

        unsigned pageBreakBefore : 2; // EPageBreak
        unsigned pageBreakAfter : 2; // EPageBreak
        unsigned pageBreakInside : 2; // EPageBreak

        unsigned styleType : 6; // PseudoId
        unsigned pseudoBits : 8;
        unsigned explicitInheritance : 1; // Explicitly inherits a non-inherited property
        unsigned unique : 1; // Style can not be shared.

        unsigned emptyState : 1;

        unsigned affectedByFocus : 1;
        unsigned affectedByHover : 1;
        unsigned affectedByActive : 1;
        unsigned affectedByDrag : 1;

        unsigned isLink : 1;
        // If you add more style bits here, you will also need to update ComputedStyle::copyNonInheritedFromCached()
        // 62 bits
    } noninherited_flags;

// !END SYNC!

protected:
    void setBitDefaults()
    {
        inherited_flags._empty_cells = initialEmptyCells();
        inherited_flags._caption_side = initialCaptionSide();
        inherited_flags._list_style_type = initialListStyleType();
        inherited_flags._list_style_position = initialListStylePosition();
        inherited_flags._visibility = initialVisibility();
        inherited_flags._text_align = initialTextAlign();
        inherited_flags._text_transform = initialTextTransform();
        inherited_flags.m_textUnderline = false;
        inherited_flags._cursor_style = initialCursor();
        inherited_flags._direction = initialDirection();
        inherited_flags._white_space = initialWhiteSpace();
        inherited_flags._border_collapse = initialBorderCollapse();
        inherited_flags.m_rtlOrdering = initialRTLOrdering();
        inherited_flags._box_direction = initialBoxDirection();
        inherited_flags.m_printColorAdjust = initialPrintColorAdjust();
        inherited_flags._pointerEvents = initialPointerEvents();
        inherited_flags._insideLink = NotInsideLink;
        inherited_flags.m_writingMode = initialWritingMode();

        noninherited_flags.effectiveDisplay = noninherited_flags.originalDisplay = initialDisplay();
        noninherited_flags.overflowX = initialOverflowX();
        noninherited_flags.overflowY = initialOverflowY();
        noninherited_flags.verticalAlign = initialVerticalAlign();
        noninherited_flags.clear = initialClear();
        noninherited_flags.position = initialPosition();
        noninherited_flags.floating = initialFloating();
        noninherited_flags.tableLayout = initialTableLayout();
        noninherited_flags.unicodeBidi = initialUnicodeBidi();
        noninherited_flags.pageBreakBefore = initialPageBreak();
        noninherited_flags.pageBreakAfter = initialPageBreak();
        noninherited_flags.pageBreakInside = initialPageBreak();
        noninherited_flags.styleType = NOPSEUDO;
        noninherited_flags.pseudoBits = 0;
        noninherited_flags.explicitInheritance = false;
        noninherited_flags.unique = false;
        noninherited_flags.emptyState = false;
        noninherited_flags.hasViewportUnits = false;
        noninherited_flags.affectedByFocus = false;
        noninherited_flags.affectedByHover = false;
        noninherited_flags.affectedByActive = false;
        noninherited_flags.affectedByDrag = false;
        noninherited_flags.isLink = false;
    }

private:
    ALWAYS_INLINE ComputedStyle();

    enum InitialStyleTag {
        InitialStyle
    };
    ALWAYS_INLINE explicit ComputedStyle(InitialStyleTag);
    ALWAYS_INLINE ComputedStyle(const ComputedStyle&);

    static PassRefPtr<ComputedStyle> createInitialStyle();
    static inline ComputedStyle* initialStyle()
    {
        DEFINE_STATIC_REF(ComputedStyle, s_initialStyle, (ComputedStyle::createInitialStyle()));
        return s_initialStyle;
    }

public:
    static PassRefPtr<ComputedStyle> create();
    static PassRefPtr<ComputedStyle> createAnonymousStyleWithDisplay(const ComputedStyle& parentStyle, EDisplay);
    static PassRefPtr<ComputedStyle> clone(const ComputedStyle&);

    // Computes how the style change should be propagated down the tree.
    static StyleRecalcChange stylePropagationDiff(const ComputedStyle* oldStyle, const ComputedStyle* newStyle);

    static ItemPosition resolveAlignment(const ComputedStyle& parentStyle, const ComputedStyle& childStyle, ItemPosition resolvedAutoPositionForLayoutObject);
    static ItemPosition resolveJustification(const ComputedStyle& parentStyle, const ComputedStyle& childStyle, ItemPosition resolvedAutoPositionForLayoutObject);

    StyleDifference visualInvalidationDiff(const ComputedStyle&) const;

    enum IsAtShadowBoundary {
        AtShadowBoundary,
        NotAtShadowBoundary,
    };

    void inheritFrom(const ComputedStyle& inheritParent, IsAtShadowBoundary = NotAtShadowBoundary);
    void copyNonInheritedFromCached(const ComputedStyle&);

    PseudoId styleType() const { return static_cast<PseudoId>(noninherited_flags.styleType); }
    void setStyleType(PseudoId styleType) { noninherited_flags.styleType = styleType; }

    ComputedStyle* getCachedPseudoStyle(PseudoId) const;
    ComputedStyle* addCachedPseudoStyle(PassRefPtr<ComputedStyle>);
    void removeCachedPseudoStyle(PseudoId);

    const PseudoStyleCache* cachedPseudoStyles() const { return m_cachedPseudoStyles.get(); }

    void setHasViewportUnits(bool hasViewportUnits = true) const { noninherited_flags.hasViewportUnits = hasViewportUnits; }
    bool hasViewportUnits() const { return noninherited_flags.hasViewportUnits; }

    bool affectedByFocus() const { return noninherited_flags.affectedByFocus; }
    bool affectedByHover() const { return noninherited_flags.affectedByHover; }
    bool affectedByActive() const { return noninherited_flags.affectedByActive; }
    bool affectedByDrag() const { return noninherited_flags.affectedByDrag; }

    void setAffectedByFocus() { noninherited_flags.affectedByFocus = true; }
    void setAffectedByHover() { noninherited_flags.affectedByHover = true; }
    void setAffectedByActive() { noninherited_flags.affectedByActive = true; }
    void setAffectedByDrag() { noninherited_flags.affectedByDrag = true; }

    bool operator==(const ComputedStyle& other) const;
    bool operator!=(const ComputedStyle& other) const { return !(*this == other); }
    bool isFloating() const { return noninherited_flags.floating != NoFloat; }
    bool hasMargin() const { return surround->margin.nonZero(); }
    bool hasBorderFill() const { return surround->border.hasBorderFill(); }
    bool hasBorder() const { return surround->border.hasBorder(); }
    bool hasBorderDecoration() const { return hasBorder() || hasBorderFill(); }
    bool hasPadding() const { return surround->padding.nonZero(); }
    bool hasMarginBeforeQuirk() const { return marginBefore().quirk(); }
    bool hasMarginAfterQuirk() const { return marginAfter().quirk(); }

    bool hasBackgroundImage() const { return m_background->background().hasImage(); }
    bool hasFixedBackgroundImage() const { return m_background->background().hasFixedImage(); }

    bool hasEntirelyFixedBackground() const;

    bool hasAppearance() const { return appearance() != NoControlPart; }

    bool hasBackground() const
    {
        Color color = visitedDependentColor(CSSPropertyBackgroundColor);
        if (color.alpha())
            return true;
        return hasBackgroundImage();
    }

    LayoutRectOutsets imageOutsets(const NinePieceImage&) const;
    bool hasBorderImageOutsets() const
    {
        return borderImage().hasImage() && borderImage().outset().nonZero();
    }
    LayoutRectOutsets borderImageOutsets() const
    {
        return imageOutsets(borderImage());
    }

    bool hasFilterOutsets() const { return hasFilter() && filter().hasOutsets(); }
    FilterOutsets filterOutsets() const { return hasFilter() ? filter().outsets() : FilterOutsets(); }

    Order rtlOrdering() const { return static_cast<Order>(inherited_flags.m_rtlOrdering); }
    void setRTLOrdering(Order o) { inherited_flags.m_rtlOrdering = o; }

    bool isStyleAvailable() const;

    bool hasAnyPublicPseudoStyles() const;
    bool hasPseudoStyle(PseudoId) const;
    void setHasPseudoStyle(PseudoId);
    bool hasUniquePseudoStyle() const;
    bool hasPseudoElementStyle() const;

    // attribute getter methods

    EDisplay display() const { return static_cast<EDisplay>(noninherited_flags.effectiveDisplay); }
    EDisplay originalDisplay() const { return static_cast<EDisplay>(noninherited_flags.originalDisplay); }

    const Length& left() const { return surround->offset.left(); }
    const Length& right() const { return surround->offset.right(); }
    const Length& top() const { return surround->offset.top(); }
    const Length& bottom() const { return surround->offset.bottom(); }

    // Accessors for positioned object edges that take into account writing mode.
    const Length& logicalLeft() const { return surround->offset.logicalLeft(writingMode()); }
    const Length& logicalRight() const { return surround->offset.logicalRight(writingMode()); }
    const Length& logicalTop() const { return surround->offset.before(writingMode()); }
    const Length& logicalBottom() const { return surround->offset.after(writingMode()); }

    // Whether or not a positioned element requires normal flow x/y to be computed
    // to determine its position.
    bool hasAutoLeftAndRight() const { return left().isAuto() && right().isAuto(); }
    bool hasAutoTopAndBottom() const { return top().isAuto() && bottom().isAuto(); }
    bool hasStaticInlinePosition(bool horizontal) const { return horizontal ? hasAutoLeftAndRight() : hasAutoTopAndBottom(); }
    bool hasStaticBlockPosition(bool horizontal) const { return horizontal ? hasAutoTopAndBottom() : hasAutoLeftAndRight(); }

    EPosition position() const { return static_cast<EPosition>(noninherited_flags.position); }
    bool hasOutOfFlowPosition() const { return position() == AbsolutePosition || position() == FixedPosition; }
    bool hasInFlowPosition() const { return position() == RelativePosition; }
    bool hasViewportConstrainedPosition() const { return position() == FixedPosition; }
    EFloat floating() const { return static_cast<EFloat>(noninherited_flags.floating); }

    const Length& width() const { return m_box->width(); }
    const Length& height() const { return m_box->height(); }
    const Length& minWidth() const { return m_box->minWidth(); }
    const Length& maxWidth() const { return m_box->maxWidth(); }
    const Length& minHeight() const { return m_box->minHeight(); }
    const Length& maxHeight() const { return m_box->maxHeight(); }

    const Length& logicalWidth() const { return isHorizontalWritingMode() ? width() : height(); }
    const Length& logicalHeight() const { return isHorizontalWritingMode() ? height() : width(); }
    const Length& logicalMinWidth() const { return isHorizontalWritingMode() ? minWidth() : minHeight(); }
    const Length& logicalMaxWidth() const { return isHorizontalWritingMode() ? maxWidth() : maxHeight(); }
    const Length& logicalMinHeight() const { return isHorizontalWritingMode() ? minHeight() : minWidth(); }
    const Length& logicalMaxHeight() const { return isHorizontalWritingMode() ? maxHeight() : maxWidth(); }

    const BorderData& border() const { return surround->border; }
    const BorderValue& borderLeft() const { return surround->border.left(); }
    const BorderValue& borderRight() const { return surround->border.right(); }
    const BorderValue& borderTop() const { return surround->border.top(); }
    const BorderValue& borderBottom() const { return surround->border.bottom(); }

    const BorderValue& borderBefore() const;
    const BorderValue& borderAfter() const;
    const BorderValue& borderStart() const;
    const BorderValue& borderEnd() const;

    const NinePieceImage& borderImage() const { return surround->border.image(); }
    StyleImage* borderImageSource() const { return surround->border.image().image(); }
    const LengthBox& borderImageSlices() const { return surround->border.image().imageSlices(); }
    bool borderImageSlicesFill() const { return surround->border.image().fill(); }
    const BorderImageLengthBox& borderImageWidth() const { return surround->border.image().borderSlices(); }
    const BorderImageLengthBox& borderImageOutset() const { return surround->border.image().outset(); }

    const LengthSize& borderTopLeftRadius() const { return surround->border.topLeft(); }
    const LengthSize& borderTopRightRadius() const { return surround->border.topRight(); }
    const LengthSize& borderBottomLeftRadius() const { return surround->border.bottomLeft(); }
    const LengthSize& borderBottomRightRadius() const { return surround->border.bottomRight(); }
    bool hasBorderRadius() const { return surround->border.hasBorderRadius(); }

    int borderLeftWidth() const { return surround->border.borderLeftWidth(); }
    EBorderStyle borderLeftStyle() const { return surround->border.left().style(); }
    int borderRightWidth() const { return surround->border.borderRightWidth(); }
    EBorderStyle borderRightStyle() const { return surround->border.right().style(); }
    int borderTopWidth() const { return surround->border.borderTopWidth(); }
    EBorderStyle borderTopStyle() const { return surround->border.top().style(); }
    int borderBottomWidth() const { return surround->border.borderBottomWidth(); }
    EBorderStyle borderBottomStyle() const { return surround->border.bottom().style(); }

    int borderBeforeWidth() const;
    int borderAfterWidth() const;
    int borderStartWidth() const;
    int borderEndWidth() const;

    int outlineSize() const { return max(0, outlineWidth() + outlineOffset()); }
    int outlineWidth() const
    {
        if (m_background->outline().style() == BNONE)
            return 0;
        return m_background->outline().width();
    }
    bool hasOutline() const { return outlineWidth() > 0 && outlineStyle() > BHIDDEN; }
    EBorderStyle outlineStyle() const { return m_background->outline().style(); }
    OutlineIsAuto outlineStyleIsAuto() const { return static_cast<OutlineIsAuto>(m_background->outline().isAuto()); }
    int outlineOutset() const;

    EOverflow overflowX() const { return static_cast<EOverflow>(noninherited_flags.overflowX); }
    EOverflow overflowY() const { return static_cast<EOverflow>(noninherited_flags.overflowY); }
    // It's sufficient to just check one direction, since it's illegal to have visible on only one overflow value.
    bool isOverflowVisible() const { ASSERT(overflowX() != OVISIBLE || overflowX() == overflowY()); return overflowX() == OVISIBLE; }
    bool isOverflowPaged() const { return overflowY() == OPAGEDX || overflowY() == OPAGEDY; }

    EVisibility visibility() const { return static_cast<EVisibility>(inherited_flags._visibility); }
    EVerticalAlign verticalAlign() const { return static_cast<EVerticalAlign>(noninherited_flags.verticalAlign); }
    const Length& verticalAlignLength() const { return m_box->verticalAlign(); }

    const Length& clipLeft() const { return visual->clip.left(); }
    const Length& clipRight() const { return visual->clip.right(); }
    const Length& clipTop() const { return visual->clip.top(); }
    const Length& clipBottom() const { return visual->clip.bottom(); }
    const LengthBox& clip() const { return visual->clip; }
    bool hasAutoClip() const { return visual->hasAutoClip; }

    EUnicodeBidi unicodeBidi() const { return static_cast<EUnicodeBidi>(noninherited_flags.unicodeBidi); }

    EClear clear() const { return static_cast<EClear>(noninherited_flags.clear); }
    ETableLayout tableLayout() const { return static_cast<ETableLayout>(noninherited_flags.tableLayout); }
    bool isFixedTableLayout() const { return tableLayout() == TFIXED && !logicalWidth().isAuto(); }

    const Font& font() const;
    const FontMetrics& fontMetrics() const;
    const FontDescription& fontDescription() const;
    float specifiedFontSize() const;
    float computedFontSize() const;
    int fontSize() const;
    float fontSizeAdjust() const;
    bool hasFontSizeAdjust() const;
    FontWeight fontWeight() const;
    FontStretch fontStretch() const;

    float textAutosizingMultiplier() const { return inherited->textAutosizingMultiplier; }

    const Length& textIndent() const { return rareInheritedData->indent; }
    TextIndentLine textIndentLine() const { return static_cast<TextIndentLine>(rareInheritedData->m_textIndentLine); }
    TextIndentType textIndentType() const { return static_cast<TextIndentType>(rareInheritedData->m_textIndentType); }
    ETextAlign textAlign() const { return static_cast<ETextAlign>(inherited_flags._text_align); }
    TextAlignLast textAlignLast() const { return static_cast<TextAlignLast>(rareInheritedData->m_textAlignLast); }
    TextJustify textJustify() const { return static_cast<TextJustify>(rareInheritedData->m_textJustify); }
    ETextTransform textTransform() const { return static_cast<ETextTransform>(inherited_flags._text_transform); }
    TextDecoration textDecorationsInEffect() const;
    const Vector<AppliedTextDecoration>& appliedTextDecorations() const;
    TextDecoration textDecoration() const { return static_cast<TextDecoration>(visual->textDecoration); }
    TextUnderlinePosition textUnderlinePosition() const { return static_cast<TextUnderlinePosition>(rareInheritedData->m_textUnderlinePosition); }
    TextDecorationStyle textDecorationStyle() const { return static_cast<TextDecorationStyle>(rareNonInheritedData->m_textDecorationStyle); }
    float wordSpacing() const;
    float letterSpacing() const;
	StyleVariableData* variables() const;
	
	void setVariable(const AtomicString&, PassRefPtr<CSSVariableData>);
	void removeVariable(const AtomicString&);

    float zoom() const { return visual->m_zoom; }
    float effectiveZoom() const { return rareInheritedData->m_effectiveZoom; }

    TextDirection direction() const { return static_cast<TextDirection>(inherited_flags._direction); }
    bool isLeftToRightDirection() const { return direction() == LTR; }
    bool selfOrAncestorHasDirAutoAttribute() const { return rareInheritedData->m_selfOrAncestorHasDirAutoAttribute; }

    const Length& specifiedLineHeight() const;
    Length lineHeight() const;
    int computedLineHeight() const;

    EWhiteSpace whiteSpace() const { return static_cast<EWhiteSpace>(inherited_flags._white_space); }
    static bool autoWrap(EWhiteSpace ws)
    {
        // Nowrap and pre don't automatically wrap.
        return ws != NOWRAP && ws != PRE;
    }

    bool autoWrap() const
    {
        return autoWrap(whiteSpace());
    }

    static bool preserveNewline(EWhiteSpace ws)
    {
        // Normal and nowrap do not preserve newlines.
        return ws != NORMAL && ws != NOWRAP;
    }

    bool preserveNewline() const
    {
        return preserveNewline(whiteSpace());
    }

    static bool collapseWhiteSpace(EWhiteSpace ws)
    {
        // Pre and prewrap do not collapse whitespace.
        return ws != PRE && ws != PRE_WRAP;
    }

    bool collapseWhiteSpace() const
    {
        return collapseWhiteSpace(whiteSpace());
    }

    bool isCollapsibleWhiteSpace(UChar c) const
    {
        switch (c) {
        case ' ':
        case '\t':
            return collapseWhiteSpace();
        case '\n':
            return !preserveNewline();
        }
        return false;
    }

    bool breakOnlyAfterWhiteSpace() const
    {
        return whiteSpace() == PRE_WRAP || lineBreak() == LineBreakAfterWhiteSpace;
    }

    bool breakWords() const
    {
        return wordBreak() == BreakWordBreak || overflowWrap() == BreakOverflowWrap;
    }

    EFillBox backgroundClip() const { return static_cast<EFillBox>(m_background->background().clip()); }
    FillLayer& accessBackgroundLayers() { return m_background.access()->m_background; }
    const FillLayer& backgroundLayers() const { return m_background->background(); }

    StyleImage* maskImage() const { return rareNonInheritedData->m_mask.image(); }
    FillLayer& accessMaskLayers() { return rareNonInheritedData.access()->m_mask; }
    const FillLayer& maskLayers() const { return rareNonInheritedData->m_mask; }

    const NinePieceImage& maskBoxImage() const { return rareNonInheritedData->m_maskBoxImage; }
    StyleImage* maskBoxImageSource() const { return rareNonInheritedData->m_maskBoxImage.image(); }
    const LengthBox& maskBoxImageSlices() const { return rareNonInheritedData->m_maskBoxImage.imageSlices(); }
    bool maskBoxImageSlicesFill() const { return rareNonInheritedData->m_maskBoxImage.fill(); }
    const BorderImageLengthBox& maskBoxImageWidth() const { return rareNonInheritedData->m_maskBoxImage.borderSlices(); }
    const BorderImageLengthBox& maskBoxImageOutset() const { return rareNonInheritedData->m_maskBoxImage.outset(); }

    EBorderCollapse borderCollapse() const { return static_cast<EBorderCollapse>(inherited_flags._border_collapse); }
    short horizontalBorderSpacing() const;
    short verticalBorderSpacing() const;
    EEmptyCell emptyCells() const { return static_cast<EEmptyCell>(inherited_flags._empty_cells); }
    ECaptionSide captionSide() const { return static_cast<ECaptionSide>(inherited_flags._caption_side); }

    EListStyleType listStyleType() const { return static_cast<EListStyleType>(inherited_flags._list_style_type); }
    StyleImage* listStyleImage() const;
    EListStylePosition listStylePosition() const { return static_cast<EListStylePosition>(inherited_flags._list_style_position); }

    const Length& marginTop() const { return surround->margin.top(); }
    const Length& marginBottom() const { return surround->margin.bottom(); }
    const Length& marginLeft() const { return surround->margin.left(); }
    const Length& marginRight() const { return surround->margin.right(); }
    const Length& marginBefore() const { return surround->margin.before(writingMode()); }
    const Length& marginAfter() const { return surround->margin.after(writingMode()); }
    const Length& marginStart() const { return surround->margin.start(writingMode(), direction()); }
    const Length& marginEnd() const { return surround->margin.end(writingMode(), direction()); }
    const Length& marginStartUsing(const ComputedStyle* otherStyle) const { return surround->margin.start(otherStyle->writingMode(), otherStyle->direction()); }
    const Length& marginEndUsing(const ComputedStyle* otherStyle) const { return surround->margin.end(otherStyle->writingMode(), otherStyle->direction()); }
    const Length& marginBeforeUsing(const ComputedStyle* otherStyle) const { return surround->margin.before(otherStyle->writingMode()); }
    const Length& marginAfterUsing(const ComputedStyle* otherStyle) const { return surround->margin.after(otherStyle->writingMode()); }

    const LengthBox& paddingBox() const { return surround->padding; }
    const Length& paddingTop() const { return surround->padding.top(); }
    const Length& paddingBottom() const { return surround->padding.bottom(); }
    const Length& paddingLeft() const { return surround->padding.left(); }
    const Length& paddingRight() const { return surround->padding.right(); }
    const Length& paddingBefore() const { return surround->padding.before(writingMode()); }
    const Length& paddingAfter() const { return surround->padding.after(writingMode()); }
    const Length& paddingStart() const { return surround->padding.start(writingMode(), direction()); }
    const Length& paddingEnd() const { return surround->padding.end(writingMode(), direction()); }

    ECursor cursor() const { return static_cast<ECursor>(inherited_flags._cursor_style); }
    CursorList* cursors() const { return rareInheritedData->cursorData.get(); }

    EInsideLink insideLink() const { return static_cast<EInsideLink>(inherited_flags._insideLink); }
    bool isLink() const { return noninherited_flags.isLink; }

    short widows() const { return rareInheritedData->widows; }
    short orphans() const { return rareInheritedData->orphans; }
    bool hasAutoWidows() const { return rareInheritedData->widows == 1; }
    bool hasAutoOrphans() const { return rareInheritedData->m_hasAutoOrphans; }
    EPageBreak pageBreakInside() const { return static_cast<EPageBreak>(noninherited_flags.pageBreakInside); }
    EPageBreak pageBreakBefore() const { return static_cast<EPageBreak>(noninherited_flags.pageBreakBefore); }
    EPageBreak pageBreakAfter() const { return static_cast<EPageBreak>(noninherited_flags.pageBreakAfter); }

    // CSS3 Getter Methods

    int outlineOffset() const
    {
        if (m_background->outline().style() == BNONE)
            return 0;
        return m_background->outline().offset();
    }

    ShadowList* textShadow() const { return rareInheritedData->textShadow.get(); }

    float textStrokeWidth() const { return rareInheritedData->textStrokeWidth; }
    float opacity() const { return rareNonInheritedData->opacity; }
    bool hasOpacity() const { return opacity() < 1.0f; }
    ControlPart appearance() const { return static_cast<ControlPart>(rareNonInheritedData->m_appearance); }
    EBoxAlignment boxAlign() const { return static_cast<EBoxAlignment>(rareNonInheritedData->m_deprecatedFlexibleBox->align); }
    EBoxDirection boxDirection() const { return static_cast<EBoxDirection>(inherited_flags._box_direction); }
    float boxFlex() const { return rareNonInheritedData->m_deprecatedFlexibleBox->flex; }
    unsigned boxFlexGroup() const { return rareNonInheritedData->m_deprecatedFlexibleBox->flexGroup; }
    EBoxLines boxLines() const { return static_cast<EBoxLines>(rareNonInheritedData->m_deprecatedFlexibleBox->lines); }
    unsigned boxOrdinalGroup() const { return rareNonInheritedData->m_deprecatedFlexibleBox->ordinalGroup; }
    EBoxOrient boxOrient() const { return static_cast<EBoxOrient>(rareNonInheritedData->m_deprecatedFlexibleBox->orient); }
    EBoxPack boxPack() const { return static_cast<EBoxPack>(rareNonInheritedData->m_deprecatedFlexibleBox->pack); }

    int order() const { return rareNonInheritedData->m_order; }
    const Vector<String>& callbackSelectors() const { return rareNonInheritedData->m_callbackSelectors; }
    float flexGrow() const { return rareNonInheritedData->m_flexibleBox->m_flexGrow; }
    float flexShrink() const { return rareNonInheritedData->m_flexibleBox->m_flexShrink; }
    const Length& flexBasis() const { return rareNonInheritedData->m_flexibleBox->m_flexBasis; }
    const StyleContentAlignmentData& alignContent() const { return rareNonInheritedData->m_alignContent; }
    ContentPosition alignContentPosition() const { return rareNonInheritedData->m_alignContent.position(); }
    ContentDistributionType alignContentDistribution() const { return rareNonInheritedData->m_alignContent.distribution(); }
    OverflowAlignment alignContentOverflowAlignment() const { return rareNonInheritedData->m_alignContent.overflow(); }
    const StyleSelfAlignmentData& alignItems() const { return rareNonInheritedData->m_alignItems; }
    ItemPosition alignItemsPosition() const { return rareNonInheritedData->m_alignItems.position(); }
    OverflowAlignment alignItemsOverflowAlignment() const { return rareNonInheritedData->m_alignItems.overflow(); }
    const StyleSelfAlignmentData& alignSelf() const { return rareNonInheritedData->m_alignSelf; }
    ItemPosition alignSelfPosition() const { return rareNonInheritedData->m_alignSelf.position(); }
    OverflowAlignment alignSelfOverflowAlignment() const { return rareNonInheritedData->m_alignSelf.overflow(); }
    EFlexDirection flexDirection() const { return static_cast<EFlexDirection>(rareNonInheritedData->m_flexibleBox->m_flexDirection); }
    bool isColumnFlexDirection() const { return flexDirection() == FlowColumn || flexDirection() == FlowColumnReverse; }
    bool isReverseFlexDirection() const { return flexDirection() == FlowRowReverse || flexDirection() == FlowColumnReverse; }
    EFlexWrap flexWrap() const { return static_cast<EFlexWrap>(rareNonInheritedData->m_flexibleBox->m_flexWrap); }
    const StyleContentAlignmentData& justifyContent() const { return rareNonInheritedData->m_justifyContent; }
    ContentPosition justifyContentPosition() const { return rareNonInheritedData->m_justifyContent.position(); }
    ContentDistributionType justifyContentDistribution() const { return rareNonInheritedData->m_justifyContent.distribution(); }
    OverflowAlignment justifyContentOverflowAlignment() const { return rareNonInheritedData->m_justifyContent.overflow(); }
    const StyleSelfAlignmentData& justifyItems() const { return rareNonInheritedData->m_justifyItems; }
    ItemPosition justifyItemsPosition() const { return rareNonInheritedData->m_justifyItems.position(); }
    OverflowAlignment justifyItemsOverflowAlignment() const { return rareNonInheritedData->m_justifyItems.overflow(); }
    ItemPositionType justifyItemsPositionType() const { return rareNonInheritedData->m_justifyItems.positionType(); }
    const StyleSelfAlignmentData& justifySelf() const { return rareNonInheritedData->m_justifySelf; }
    ItemPosition justifySelfPosition() const { return rareNonInheritedData->m_justifySelf.position(); }
    OverflowAlignment justifySelfOverflowAlignment() const { return rareNonInheritedData->m_justifySelf.overflow(); }

    const Vector<GridTrackSize>& gridTemplateColumns() const { return rareNonInheritedData->m_grid->m_gridTemplateColumns; }
    const Vector<GridTrackSize>& gridTemplateRows() const { return rareNonInheritedData->m_grid->m_gridTemplateRows; }
    const NamedGridLinesMap& namedGridColumnLines() const { return rareNonInheritedData->m_grid->m_namedGridColumnLines; }
    const NamedGridLinesMap& namedGridRowLines() const { return rareNonInheritedData->m_grid->m_namedGridRowLines; }
    const OrderedNamedGridLines& orderedNamedGridColumnLines() const { return rareNonInheritedData->m_grid->m_orderedNamedGridColumnLines; }
    const OrderedNamedGridLines& orderedNamedGridRowLines() const { return rareNonInheritedData->m_grid->m_orderedNamedGridRowLines; }
    const NamedGridAreaMap& namedGridArea() const { return rareNonInheritedData->m_grid->m_namedGridArea; }
    size_t namedGridAreaRowCount() const { return rareNonInheritedData->m_grid->m_namedGridAreaRowCount; }
    size_t namedGridAreaColumnCount() const { return rareNonInheritedData->m_grid->m_namedGridAreaColumnCount; }
    GridAutoFlow gridAutoFlow() const { return static_cast<GridAutoFlow>(rareNonInheritedData->m_grid->m_gridAutoFlow); }
    bool isGridAutoFlowDirectionRow() const { return (rareNonInheritedData->m_grid->m_gridAutoFlow & InternalAutoFlowDirectionRow) == InternalAutoFlowDirectionRow; }
    bool isGridAutoFlowDirectionColumn() const { return (rareNonInheritedData->m_grid->m_gridAutoFlow & InternalAutoFlowDirectionColumn) == InternalAutoFlowDirectionColumn; }
    bool isGridAutoFlowAlgorithmSparse() const { return (rareNonInheritedData->m_grid->m_gridAutoFlow & InternalAutoFlowAlgorithmSparse) == InternalAutoFlowAlgorithmSparse; }
    bool isGridAutoFlowAlgorithmDense() const { return (rareNonInheritedData->m_grid->m_gridAutoFlow & InternalAutoFlowAlgorithmDense) == InternalAutoFlowAlgorithmDense; }
    const GridTrackSize& gridAutoColumns() const { return rareNonInheritedData->m_grid->m_gridAutoColumns; }
    const GridTrackSize& gridAutoRows() const { return rareNonInheritedData->m_grid->m_gridAutoRows; }

    const GridPosition& gridColumnStart() const { return rareNonInheritedData->m_gridItem->m_gridColumnStart; }
    const GridPosition& gridColumnEnd() const { return rareNonInheritedData->m_gridItem->m_gridColumnEnd; }
    const GridPosition& gridRowStart() const { return rareNonInheritedData->m_gridItem->m_gridRowStart; }
    const GridPosition& gridRowEnd() const { return rareNonInheritedData->m_gridItem->m_gridRowEnd; }

    ShadowList* boxShadow() const { return rareNonInheritedData->m_boxShadow.get(); }

    EBoxDecorationBreak boxDecorationBreak() const { return m_box->boxDecorationBreak(); }
    StyleReflection* boxReflect() const { return rareNonInheritedData->m_boxReflect.get(); }
    bool reflectionDataEquivalent(const ComputedStyle* otherStyle) const { return rareNonInheritedData->reflectionDataEquivalent(*otherStyle->rareNonInheritedData); }

    // FIXME: reflections should belong to this helper function but they are currently handled
    // through their self-painting layers. So the layout code doesn't account for them.
    bool hasVisualOverflowingEffect() const { return boxShadow() || hasBorderImageOutsets() || hasOutline(); }

    EBoxSizing boxSizing() const { return m_box->boxSizing(); }
    EUserModify userModify() const { return static_cast<EUserModify>(rareInheritedData->userModify); }
    EUserDrag userDrag() const { return static_cast<EUserDrag>(rareNonInheritedData->userDrag); }
    EUserSelect userSelect() const { return static_cast<EUserSelect>(rareInheritedData->userSelect); }
    TextOverflow textOverflow() const { return static_cast<TextOverflow>(rareNonInheritedData->textOverflow); }
    EMarginCollapse marginBeforeCollapse() const { return static_cast<EMarginCollapse>(rareNonInheritedData->marginBeforeCollapse); }
    EMarginCollapse marginAfterCollapse() const { return static_cast<EMarginCollapse>(rareNonInheritedData->marginAfterCollapse); }
    EWordBreak wordBreak() const { return static_cast<EWordBreak>(rareInheritedData->wordBreak); }
    EOverflowWrap overflowWrap() const { return static_cast<EOverflowWrap>(rareInheritedData->overflowWrap); }
    LineBreak lineBreak() const { return static_cast<LineBreak>(rareInheritedData->lineBreak); }
    const AtomicString& highlight() const { return rareInheritedData->highlight; }
    const AtomicString& hyphenationString() const { return rareInheritedData->hyphenationString; }
    const AtomicString& locale() const { return rareInheritedData->locale; }
    EResize resize() const { return static_cast<EResize>(rareNonInheritedData->m_resize); }
    bool hasInlinePaginationAxis() const
    {
        // If the pagination axis is parallel with the writing mode inline axis, columns may be laid
        // out along the inline axis, just like for regular multicol. Otherwise, we need to lay out
        // along the block axis.
        if (isOverflowPaged())
            return (overflowY() == OPAGEDX) == isHorizontalWritingMode();
        return false;
    }
    float columnWidth() const { return rareNonInheritedData->m_multiCol->m_width; }
    bool hasAutoColumnWidth() const { return rareNonInheritedData->m_multiCol->m_autoWidth; }
    unsigned short columnCount() const { return rareNonInheritedData->m_multiCol->m_count; }
    bool hasAutoColumnCount() const { return rareNonInheritedData->m_multiCol->m_autoCount; }
    bool specifiesColumns() const { return !hasAutoColumnCount() || !hasAutoColumnWidth(); }
    ColumnFill columnFill() const { return static_cast<ColumnFill>(rareNonInheritedData->m_multiCol->m_fill); }
    float columnGap() const { return rareNonInheritedData->m_multiCol->m_gap; }
    bool hasNormalColumnGap() const { return rareNonInheritedData->m_multiCol->m_normalGap; }
    EBorderStyle columnRuleStyle() const { return rareNonInheritedData->m_multiCol->m_rule.style(); }
    unsigned short columnRuleWidth() const { return rareNonInheritedData->m_multiCol->ruleWidth(); }
    bool columnRuleIsTransparent() const { return rareNonInheritedData->m_multiCol->m_rule.isTransparent(); }
    bool columnRuleEquivalent(const ComputedStyle* otherStyle) const;
    ColumnSpan columnSpan() const { return static_cast<ColumnSpan>(rareNonInheritedData->m_multiCol->m_columnSpan); }
    EPageBreak columnBreakBefore() const { return static_cast<EPageBreak>(rareNonInheritedData->m_multiCol->m_breakBefore); }
    EPageBreak columnBreakInside() const { return static_cast<EPageBreak>(rareNonInheritedData->m_multiCol->m_breakInside); }
    EPageBreak columnBreakAfter() const { return static_cast<EPageBreak>(rareNonInheritedData->m_multiCol->m_breakAfter); }
    bool hasInlineTransform() const { return rareNonInheritedData->m_hasInlineTransform; }
    bool hasCompositorProxy() const { return rareNonInheritedData->m_hasCompositorProxy; }
    const TransformOperations& transform() const { return rareNonInheritedData->m_transform->m_operations; }
    const TransformOrigin& transformOrigin() const { return rareNonInheritedData->m_transform->m_origin; }
    const Length& transformOriginX() const { return transformOrigin().x(); }
    const Length& transformOriginY() const { return transformOrigin().y(); }
    TranslateTransformOperation* translate() const { return rareNonInheritedData->m_transform->m_translate.get(); }
    RotateTransformOperation* rotate() const { return rareNonInheritedData->m_transform->m_rotate.get(); }
    ScaleTransformOperation* scale() const { return rareNonInheritedData->m_transform->m_scale.get(); }
    float transformOriginZ() const { return transformOrigin().z(); }
    bool has3DTransform() const { return rareNonInheritedData->m_transform->has3DTransform(); }
    bool hasTransform() const { return hasTransformOperations() || hasMotionPath() || hasCurrentTransformAnimation() || translate() || rotate() || scale(); }
    bool hasTransformOperations() const { return !rareNonInheritedData->m_transform->m_operations.operations().isEmpty(); }
    bool transformDataEquivalent(const ComputedStyle& otherStyle) const { return rareNonInheritedData->m_transform == otherStyle.rareNonInheritedData->m_transform; }

    StyleMotionPath* motionPath() const { return rareNonInheritedData->m_transform->m_motion.m_path.get(); }
    bool hasMotionPath() const { return rareNonInheritedData->m_transform->m_motion.m_path; }
    const Length& motionOffset() const { return rareNonInheritedData->m_transform->m_motion.m_offset; }
    float motionRotation() const { return rareNonInheritedData->m_transform->m_motion.m_rotation; }
    MotionRotationType motionRotationType() const { return rareNonInheritedData->m_transform->m_motion.m_rotationType; }

    TextEmphasisFill textEmphasisFill() const { return static_cast<TextEmphasisFill>(rareInheritedData->textEmphasisFill); }
    TextEmphasisMark textEmphasisMark() const;
    const AtomicString& textEmphasisCustomMark() const { return rareInheritedData->textEmphasisCustomMark; }
    TextEmphasisPosition textEmphasisPosition() const { return static_cast<TextEmphasisPosition>(rareInheritedData->textEmphasisPosition); }
    const AtomicString& textEmphasisMarkString() const;

    RubyPosition rubyPosition() const { return static_cast<RubyPosition>(rareInheritedData->m_rubyPosition); }

    TextOrientation textOrientation() const { return static_cast<TextOrientation>(rareInheritedData->m_textOrientation); }

    ObjectFit objectFit() const { return static_cast<ObjectFit>(rareNonInheritedData->m_objectFit); }
    LengthPoint objectPosition() const { return rareNonInheritedData->m_objectPosition; }

    // Return true if any transform related property (currently transform/motionPath, transformStyle3D or perspective)
    // indicates that we are transforming
    bool hasTransformRelatedProperty() const { return hasTransform() || preserves3D() || hasPerspective(); }

    enum ApplyTransformOrigin { IncludeTransformOrigin, ExcludeTransformOrigin };
    enum ApplyMotionPath { IncludeMotionPath, ExcludeMotionPath };
    enum ApplyIndependentTransformProperties { IncludeIndependentTransformProperties , ExcludeIndependentTransformProperties };
    void applyTransform(TransformationMatrix&, const LayoutSize& borderBoxSize, ApplyTransformOrigin, ApplyMotionPath, ApplyIndependentTransformProperties) const;
    void applyTransform(TransformationMatrix&, const FloatRect& boundingBox, ApplyTransformOrigin, ApplyMotionPath, ApplyIndependentTransformProperties) const;
    bool hasMask() const { return rareNonInheritedData->m_mask.hasImage() || rareNonInheritedData->m_maskBoxImage.hasImage(); }

    TextCombine textCombine() const { return static_cast<TextCombine>(rareInheritedData->m_textCombine); }
    bool hasTextCombine() const { return textCombine() != TextCombineNone; }

    TabSize tabSize() const { return rareInheritedData->m_tabSize; }

    // End CSS3 Getters

    // Apple-specific property getter methods
    EPointerEvents pointerEvents() const { return static_cast<EPointerEvents>(inherited_flags._pointerEvents); }
    const CSSAnimationData* animations() const { return rareNonInheritedData->m_animations.get(); }
    const CSSTransitionData* transitions() const { return rareNonInheritedData->m_transitions.get(); }

    CSSAnimationData& accessAnimations();
    CSSTransitionData& accessTransitions();

    ETransformStyle3D transformStyle3D() const { return static_cast<ETransformStyle3D>(rareNonInheritedData->m_transformStyle3D); }
    bool preserves3D() const { return rareNonInheritedData->m_transformStyle3D == TransformStyle3DPreserve3D; }

    EBackfaceVisibility backfaceVisibility() const { return static_cast<EBackfaceVisibility>(rareNonInheritedData->m_backfaceVisibility); }
    float perspective() const { return rareNonInheritedData->m_perspective; }
    bool hasPerspective() const { return rareNonInheritedData->m_perspective > 0; }
    const LengthPoint& perspectiveOrigin() const { return rareNonInheritedData->m_perspectiveOrigin; }
    const Length& perspectiveOriginX() const { return perspectiveOrigin().x(); }
    const Length& perspectiveOriginY() const { return perspectiveOrigin().y(); }
    const FloatSize& pageSize() const { return rareNonInheritedData->m_pageSize; }
    PageSizeType pageSizeType() const { return static_cast<PageSizeType>(rareNonInheritedData->m_pageSizeType); }

    bool hasCurrentOpacityAnimation() const { return rareNonInheritedData->m_hasCurrentOpacityAnimation; }
    bool hasCurrentTransformAnimation() const { return rareNonInheritedData->m_hasCurrentTransformAnimation; }
    bool hasCurrentFilterAnimation() const { return rareNonInheritedData->m_hasCurrentFilterAnimation; }
    bool shouldCompositeForCurrentAnimations() const { return hasCurrentOpacityAnimation() || hasCurrentTransformAnimation() || hasCurrentFilterAnimation(); }

    bool isRunningOpacityAnimationOnCompositor() const { return rareNonInheritedData->m_runningOpacityAnimationOnCompositor; }
    bool isRunningTransformAnimationOnCompositor() const { return rareNonInheritedData->m_runningTransformAnimationOnCompositor; }
    bool isRunningFilterAnimationOnCompositor() const { return rareNonInheritedData->m_runningFilterAnimationOnCompositor; }
    bool isRunningAnimationOnCompositor() const { return isRunningOpacityAnimationOnCompositor() || isRunningTransformAnimationOnCompositor() || isRunningFilterAnimationOnCompositor(); }

    LineBoxContain lineBoxContain() const { return rareInheritedData->m_lineBoxContain; }
    const LineClampValue& lineClamp() const { return rareNonInheritedData->lineClamp; }
    Color tapHighlightColor() const { return rareInheritedData->tapHighlightColor; }
    ETextSecurity textSecurity() const { return static_cast<ETextSecurity>(rareInheritedData->textSecurity); }

    WritingMode writingMode() const { return static_cast<WritingMode>(inherited_flags.m_writingMode); }
    bool isHorizontalWritingMode() const { return blink::isHorizontalWritingMode(writingMode()); }
    bool isFlippedLinesWritingMode() const { return blink::isFlippedLinesWritingMode(writingMode()); }
    bool isFlippedBlocksWritingMode() const { return blink::isFlippedBlocksWritingMode(writingMode()); }

    EImageRendering imageRendering() const { return static_cast<EImageRendering>(rareInheritedData->m_imageRendering); }

    ESpeak speak() const { return static_cast<ESpeak>(rareInheritedData->speak); }

    FilterOperations& mutableFilter() { return rareNonInheritedData.access()->m_filter.access()->m_operations; }
    const FilterOperations& filter() const { return rareNonInheritedData->m_filter->m_operations; }
    bool hasFilter() const { return !rareNonInheritedData->m_filter->m_operations.operations().isEmpty(); }

    WebBlendMode blendMode() const { return static_cast<WebBlendMode>(rareNonInheritedData->m_effectiveBlendMode); }
    void setBlendMode(WebBlendMode v) { rareNonInheritedData.access()->m_effectiveBlendMode = v; }
    bool hasBlendMode() const { return blendMode() != WebBlendModeNormal; }

    EIsolation isolation() const { return static_cast<EIsolation>(rareNonInheritedData->m_isolation); }
    void setIsolation(EIsolation v) { rareNonInheritedData.access()->m_isolation = v; }
    bool hasIsolation() const { return isolation() != IsolationAuto; }

    bool shouldPlaceBlockDirectionScrollbarOnLogicalLeft() const { return !isLeftToRightDirection() && isHorizontalWritingMode(); }

    TouchAction touchAction() const { return static_cast<TouchAction>(rareNonInheritedData->m_touchAction); }

    ScrollBehavior scrollBehavior() const { return static_cast<ScrollBehavior>(rareNonInheritedData->m_scrollBehavior); }
    WebScrollBlocksOn scrollBlocksOn() const { return static_cast<WebScrollBlocksOn>(rareNonInheritedData->m_scrollBlocksOn); }
    bool hasScrollBlocksOn() const { return scrollBlocksOn() != WebScrollBlocksOnNone; }

    ScrollSnapType scrollSnapType() const { return static_cast<ScrollSnapType>(rareNonInheritedData->m_scrollSnapType); }
    ScrollSnapPoints scrollSnapPointsX() const { return rareNonInheritedData->m_scrollSnap->m_xPoints; }
    ScrollSnapPoints scrollSnapPointsY() const { return rareNonInheritedData->m_scrollSnap->m_yPoints; }
    Vector<LengthPoint> scrollSnapCoordinate() const { return rareNonInheritedData->m_scrollSnap->m_coordinates; }
    LengthPoint scrollSnapDestination() const { return rareNonInheritedData->m_scrollSnap->m_destination; }

    const Vector<CSSPropertyID>& willChangeProperties() const { return rareNonInheritedData->m_willChange->m_properties; }
    bool willChangeContents() const { return rareNonInheritedData->m_willChange->m_contents; }
    bool willChangeScrollPosition() const { return rareNonInheritedData->m_willChange->m_scrollPosition; }
    bool hasWillChangeCompositingHint() const;
    bool subtreeWillChangeContents() const { return rareInheritedData->m_subtreeWillChangeContents; }

// attribute setter methods

    void setDisplay(EDisplay v) { noninherited_flags.effectiveDisplay = v; }
    void setOriginalDisplay(EDisplay v) { noninherited_flags.originalDisplay = v; }
    void setPosition(EPosition v) { noninherited_flags.position = v; }
    void setFloating(EFloat v) { noninherited_flags.floating = v; }

//     void setLeft(const Length& v) { SET_VAR(surround, offset.m_left, v); }
//     void setRight(const Length& v) { SET_VAR(surround, offset.m_right, v); }
//     void setTop(const Length& v) { SET_VAR(surround, offset.m_top, v); }
//     void setBottom(const Length& v) { SET_VAR(surround, offset.m_bottom, v); }
// 
//     void setWidth(const Length& v) { SET_VAR(m_box, m_width, v); }
//     void setHeight(const Length& v) { SET_VAR(m_box, m_height, v); }
// 
//     void setLogicalWidth(const Length& v)
//     {
//         if (isHorizontalWritingMode()) {
//             SET_VAR(m_box, m_width, v);
//         } else {
//             SET_VAR(m_box, m_height, v);
//         }
//     }
// 
//     void setLogicalHeight(const Length& v)
//     {
//         if (isHorizontalWritingMode()) {
//             SET_VAR(m_box, m_height, v);
//         } else {
//             SET_VAR(m_box, m_width, v);
//         }
//     }
// 
//     void setMinWidth(const Length& v) { SET_VAR(m_box, m_minWidth, v); }
//     void setMaxWidth(const Length& v) { SET_VAR(m_box, m_maxWidth, v); }
//     void setMinHeight(const Length& v) { SET_VAR(m_box, m_minHeight, v); }
//     void setMaxHeight(const Length& v) { SET_VAR(m_box, m_maxHeight, v); }

    void setLeft(const Length& v);
    void setRight(const Length& v);
    void setTop(const Length& v);
    void setBottom(const Length& v);

    void setWidth(const Length& v);
    void setHeight(const Length& v);

    void setLogicalWidth(const Length& v);

    void setLogicalHeight(const Length& v);

    void setMinWidth(const Length& v);
    void setMaxWidth(const Length& v);
    void setMinHeight(const Length& v);
    void setMaxHeight(const Length& v);

    DraggableRegionMode getDraggableRegionMode() const { return rareNonInheritedData->m_draggableRegionMode; }
    void setDraggableRegionMode(DraggableRegionMode v) { SET_VAR(rareNonInheritedData, m_draggableRegionMode, v); }

    void resetBorder()
    {
        resetBorderImage();
        resetBorderTop();
        resetBorderRight();
        resetBorderBottom();
        resetBorderLeft();
        resetBorderTopLeftRadius();
        resetBorderTopRightRadius();
        resetBorderBottomLeftRadius();
        resetBorderBottomRightRadius();
    }
    void resetBorderTop() { SET_VAR(surround, border.m_top, BorderValue()); }
    void resetBorderRight() { SET_VAR(surround, border.m_right, BorderValue()); }
    void resetBorderBottom() { SET_VAR(surround, border.m_bottom, BorderValue()); }
    void resetBorderLeft() { SET_VAR(surround, border.m_left, BorderValue()); }
    void resetBorderImage() { SET_VAR(surround, border.m_image, NinePieceImage()); }
    void resetBorderTopLeftRadius() { SET_VAR(surround, border.m_topLeft, initialBorderRadius()); }
    void resetBorderTopRightRadius() { SET_VAR(surround, border.m_topRight, initialBorderRadius()); }
    void resetBorderBottomLeftRadius() { SET_VAR(surround, border.m_bottomLeft, initialBorderRadius()); }
    void resetBorderBottomRightRadius() { SET_VAR(surround, border.m_bottomRight, initialBorderRadius()); }

    void setBackgroundColor(const StyleColor& v) { SET_VAR(m_background, m_color, v); }

    void setBorderImage(const NinePieceImage& b) { SET_VAR(surround, border.m_image, b); }
    void setBorderImageSource(PassRefPtr<StyleImage>);
    void setBorderImageSlices(const LengthBox&);
    void setBorderImageSlicesFill(bool);
    void setBorderImageWidth(const BorderImageLengthBox&);
    void setBorderImageOutset(const BorderImageLengthBox&);

    void setBorderTopLeftRadius(const LengthSize& s) { SET_VAR(surround, border.m_topLeft, s); }
    void setBorderTopRightRadius(const LengthSize& s) { SET_VAR(surround, border.m_topRight, s); }
    void setBorderBottomLeftRadius(const LengthSize& s) { SET_VAR(surround, border.m_bottomLeft, s); }
    void setBorderBottomRightRadius(const LengthSize& s) { SET_VAR(surround, border.m_bottomRight, s); }

    void setBorderRadius(const LengthSize& s)
    {
        setBorderTopLeftRadius(s);
        setBorderTopRightRadius(s);
        setBorderBottomLeftRadius(s);
        setBorderBottomRightRadius(s);
    }
    void setBorderRadius(const IntSize& s)
    {
        setBorderRadius(LengthSize(Length(s.width(), Fixed), Length(s.height(), Fixed)));
    }

    FloatRoundedRect getRoundedBorderFor(const LayoutRect& borderRect, bool includeLogicalLeftEdge = true,
        bool includeLogicalRightEdge = true) const;
    FloatRoundedRect getRoundedInnerBorderFor(const LayoutRect& borderRect, bool includeLogicalLeftEdge = true, bool includeLogicalRightEdge = true) const;

    FloatRoundedRect getRoundedInnerBorderFor(const LayoutRect& borderRect,
        const LayoutRectOutsets insets, bool includeLogicalLeftEdge, bool includeLogicalRightEdge) const;

    void setBorderLeftWidth(unsigned v) { SET_VAR(surround, border.m_left.m_width, v); }
    void setBorderLeftStyle(EBorderStyle v) { SET_VAR(surround, border.m_left.m_style, v); }
    void setBorderLeftColor(const StyleColor& v) { SET_BORDERVALUE_COLOR(surround, border.m_left, v); }
    void setBorderRightWidth(unsigned v) { SET_VAR(surround, border.m_right.m_width, v); }
    void setBorderRightStyle(EBorderStyle v) { SET_VAR(surround, border.m_right.m_style, v); }
    void setBorderRightColor(const StyleColor& v) { SET_BORDERVALUE_COLOR(surround, border.m_right, v); }
    void setBorderTopWidth(unsigned v) { SET_VAR(surround, border.m_top.m_width, v); }
    void setBorderTopStyle(EBorderStyle v) { SET_VAR(surround, border.m_top.m_style, v); }
    void setBorderTopColor(const StyleColor& v) { SET_BORDERVALUE_COLOR(surround, border.m_top, v); }
    void setBorderBottomWidth(unsigned v) { SET_VAR(surround, border.m_bottom.m_width, v); }
    void setBorderBottomStyle(EBorderStyle v) { SET_VAR(surround, border.m_bottom.m_style, v); }
    void setBorderBottomColor(const StyleColor& v) { SET_BORDERVALUE_COLOR(surround, border.m_bottom, v); }

    void setOutlineWidth(unsigned short v) { SET_VAR(m_background, m_outline.m_width, v); }
    void setOutlineStyleIsAuto(OutlineIsAuto isAuto) { SET_VAR(m_background, m_outline.m_isAuto, isAuto); }
    void setOutlineStyle(EBorderStyle v) { SET_VAR(m_background, m_outline.m_style, v); }
    void setOutlineColor(const StyleColor& v) { SET_BORDERVALUE_COLOR(m_background, m_outline, v); }
    bool isOutlineEquivalent(const ComputedStyle* otherStyle) const
    {
        // No other style, so we don't have an outline then we consider them to be the same.
        if (!otherStyle)
            return !hasOutline();
        return m_background->outline().visuallyEqual(otherStyle->m_background->outline());
    }
    void setOutlineFromStyle(const ComputedStyle& o)
    {
        ASSERT(!isOutlineEquivalent(&o));
        m_background.access()->m_outline = o.m_background->m_outline;
    }

    void setOverflowX(EOverflow v) { noninherited_flags.overflowX = v; }
    void setOverflowY(EOverflow v) { noninherited_flags.overflowY = v; }
    void setVisibility(EVisibility v) { inherited_flags._visibility = v; }
    void setVerticalAlign(EVerticalAlign v) { noninherited_flags.verticalAlign = v; }
    void setVerticalAlignLength(const Length& length) { setVerticalAlign(LENGTH); SET_VAR(m_box, m_verticalAlign, length); }

    void setHasAutoClip() { SET_VAR(visual, hasAutoClip, true); SET_VAR(visual, clip, ComputedStyle::initialClip()); }
    void setClip(const LengthBox& box) { SET_VAR(visual, hasAutoClip, false); SET_VAR(visual, clip, box); }

    void setUnicodeBidi(EUnicodeBidi b) { noninherited_flags.unicodeBidi = b; }

    void setClear(EClear v) { noninherited_flags.clear = v; }
    void setTableLayout(ETableLayout v) { noninherited_flags.tableLayout = v; }

    bool setFontDescription(const FontDescription&);
    void setFont(const Font&);

    void setTextAutosizingMultiplier(float);

    void setColor(const Color&);
    void setTextIndent(const Length& v) { SET_VAR(rareInheritedData, indent, v); }
    void setTextIndentLine(TextIndentLine v) { SET_VAR(rareInheritedData, m_textIndentLine, v); }
    void setTextIndentType(TextIndentType v) { SET_VAR(rareInheritedData, m_textIndentType, v); }
    void setTextAlign(ETextAlign v) { inherited_flags._text_align = v; }
    void setTextAlignLast(TextAlignLast v) { SET_VAR(rareInheritedData, m_textAlignLast, v); }
    void setTextJustify(TextJustify v) { SET_VAR(rareInheritedData, m_textJustify, v); }
    void setTextTransform(ETextTransform v) { inherited_flags._text_transform = v; }
    void applyTextDecorations();
    void clearAppliedTextDecorations();
    void setTextDecoration(TextDecoration v) { SET_VAR(visual, textDecoration, v); }
    void setTextUnderlinePosition(TextUnderlinePosition v) { SET_VAR(rareInheritedData, m_textUnderlinePosition, v); }
    void setTextDecorationStyle(TextDecorationStyle v) { SET_VAR(rareNonInheritedData, m_textDecorationStyle, v); }
    void setDirection(TextDirection v) { inherited_flags._direction = v; }
    void setSelfOrAncestorHasDirAutoAttribute(bool v) { SET_VAR(rareInheritedData, m_selfOrAncestorHasDirAutoAttribute, v); }
    void setLineHeight(const Length& specifiedLineHeight);
    bool setZoom(float);
    bool setEffectiveZoom(float);
    void clearMultiCol();

    void setImageRendering(EImageRendering v) { SET_VAR(rareInheritedData, m_imageRendering, v); }

    void setWhiteSpace(EWhiteSpace v) { inherited_flags._white_space = v; }

    // FIXME: Remove these two and replace them with respective FontBuilder calls.
    void setWordSpacing(float);
    void setLetterSpacing(float);

    void adjustBackgroundLayers()
    {
        if (backgroundLayers().next()) {
            accessBackgroundLayers().cullEmptyLayers();
            accessBackgroundLayers().fillUnsetProperties();
        }
    }

    void adjustMaskLayers()
    {
        if (maskLayers().next()) {
            accessMaskLayers().cullEmptyLayers();
            accessMaskLayers().fillUnsetProperties();
        }
    }

    void setMaskBoxImage(const NinePieceImage& b) { SET_VAR(rareNonInheritedData, m_maskBoxImage, b); }
    void setMaskBoxImageSource(PassRefPtr<StyleImage> v) { rareNonInheritedData.access()->m_maskBoxImage.setImage(v); }
    void setMaskBoxImageSlices(const LengthBox& slices)
    {
        rareNonInheritedData.access()->m_maskBoxImage.setImageSlices(slices);
    }
    void setMaskBoxImageSlicesFill(bool fill)
    {
        rareNonInheritedData.access()->m_maskBoxImage.setFill(fill);
    }
    void setMaskBoxImageWidth(const BorderImageLengthBox& slices)
    {
        rareNonInheritedData.access()->m_maskBoxImage.setBorderSlices(slices);
    }
    void setMaskBoxImageOutset(const BorderImageLengthBox& outset)
    {
        rareNonInheritedData.access()->m_maskBoxImage.setOutset(outset);
    }

    void setBorderCollapse(EBorderCollapse collapse) { inherited_flags._border_collapse = collapse; }
    void setHorizontalBorderSpacing(short);
    void setVerticalBorderSpacing(short);
    void setEmptyCells(EEmptyCell v) { inherited_flags._empty_cells = v; }
    void setCaptionSide(ECaptionSide v) { inherited_flags._caption_side = v; }

    void setListStyleType(EListStyleType v) { inherited_flags._list_style_type = v; }
    void setListStyleImage(PassRefPtr<StyleImage>);
    void setListStylePosition(EListStylePosition v) { inherited_flags._list_style_position = v; }

    void setMarginTop(const Length& v) { SET_VAR(surround, margin.m_top, v); }
    void setMarginBottom(const Length& v) { SET_VAR(surround, margin.m_bottom, v); }
    void setMarginLeft(const Length& v) { SET_VAR(surround, margin.m_left, v); }
    void setMarginRight(const Length& v) { SET_VAR(surround, margin.m_right, v); }
    void setMarginStart(const Length&);
    void setMarginEnd(const Length&);

    void resetPadding() { SET_VAR(surround, padding, LengthBox(Fixed)); }
    void setPaddingBox(const LengthBox& b) { SET_VAR(surround, padding, b); }
    void setPaddingTop(const Length& v) { SET_VAR(surround, padding.m_top, v); }
    void setPaddingBottom(const Length& v) { SET_VAR(surround, padding.m_bottom, v); }
    void setPaddingLeft(const Length& v) { SET_VAR(surround, padding.m_left, v); }
    void setPaddingRight(const Length& v) { SET_VAR(surround, padding.m_right, v); }

    void setCursor(ECursor c) { inherited_flags._cursor_style = c; }
    void addCursor(PassRefPtr<StyleImage>, bool hotSpotSpecified, const IntPoint& hotSpot = IntPoint());
    void setCursorList(PassRefPtr<CursorList>);
    void clearCursorList();

    void setInsideLink(EInsideLink insideLink) { inherited_flags._insideLink = insideLink; }
    void setIsLink(bool b) { noninherited_flags.isLink = b; }

    PrintColorAdjust printColorAdjust() const { return static_cast<PrintColorAdjust>(inherited_flags.m_printColorAdjust); }
    void setPrintColorAdjust(PrintColorAdjust value) { inherited_flags.m_printColorAdjust = value; }

    // A stacking context is painted atomically and defines a stacking order, whereas
    // a containing stacking context defines in which order the stacking contexts
    // below are painted. In Blink, a stacking context is defined by non-auto
    // z-index'. This invariant is enforced by the logic in StyleAdjuster
    // See CSS 2.1, Appendix E for more details.
    bool isStackingContext() const { return !hasAutoZIndex(); }

    // Some elements are "treated as if they create a new stacking context" for the purpose
    // of painting and hit testing. However they don't determine the stacking of the stacking
    // context underneath them. That means that they are painted atomically.
    bool isTreatedAsStackingContextForPainting() const
    {
        // FIXME: Floating objects are also considered stacking contexts for painting.
        return isStackingContext() || position() != StaticPosition;
    }

    bool hasAutoZIndex() const { return m_box->hasAutoZIndex(); }
    void setHasAutoZIndex() { SET_VAR(m_box, m_hasAutoZIndex, true); SET_VAR(m_box, m_zIndex, 0); }
    int zIndex() const { return m_box->zIndex(); }
    void setZIndex(int v) { SET_VAR(m_box, m_hasAutoZIndex, false); SET_VAR(m_box, m_zIndex, v); }

    void setHasAutoWidows() { SET_VAR(rareInheritedData, widows, initialWidows()); }
    void setWidows(short w) { SET_VAR(rareInheritedData, widows, w); }

    void setHasAutoOrphans() { SET_VAR(rareInheritedData, m_hasAutoOrphans, true); SET_VAR(rareInheritedData, orphans, initialOrphans()); }
    void setOrphans(short o) { SET_VAR(rareInheritedData, m_hasAutoOrphans, false); SET_VAR(rareInheritedData, orphans, o); }

    // For valid values of page-break-inside see http://www.w3.org/TR/CSS21/page.html#page-break-props
    void setPageBreakInside(EPageBreak b) { ASSERT(b == PBAUTO || b == PBAVOID); noninherited_flags.pageBreakInside = b; }
    void setPageBreakBefore(EPageBreak b) { noninherited_flags.pageBreakBefore = b; }
    void setPageBreakAfter(EPageBreak b) { noninherited_flags.pageBreakAfter = b; }

    // CSS3 Setters
    void setOutlineOffset(int v) { SET_VAR(m_background, m_outline.m_offset, v); }
    void setTextShadow(PassRefPtr<ShadowList>);
    void setTextStrokeColor(const StyleColor& c) { SET_VAR_WITH_SETTER(rareInheritedData, textStrokeColor, setTextStrokeColor, c); }
    void setTextStrokeWidth(float w) { SET_VAR(rareInheritedData, textStrokeWidth, w); }
    void setTextFillColor(const StyleColor& c) { SET_VAR_WITH_SETTER(rareInheritedData, textFillColor, setTextFillColor, c); }
    void setOpacity(float f) { float v = clampTo<float>(f, 0, 1); SET_VAR(rareNonInheritedData, opacity, v); }
    void setAppearance(ControlPart a) { SET_VAR(rareNonInheritedData, m_appearance, a); }
    // For valid values of box-align see http://www.w3.org/TR/2009/WD-css3-flexbox-20090723/#alignment
    void setBoxAlign(EBoxAlignment a) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, align, a); }
    void setBoxDecorationBreak(EBoxDecorationBreak b) { SET_VAR(m_box, m_boxDecorationBreak, b); }
    void setBoxDirection(EBoxDirection d) { inherited_flags._box_direction = d; }
    void setBoxFlex(float f) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, flex, f); }
    void setBoxFlexGroup(unsigned fg) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, flexGroup, fg); }
    void setBoxLines(EBoxLines l) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, lines, l); }
    void setBoxOrdinalGroup(unsigned og) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, ordinalGroup, og); }
    void setBoxOrient(EBoxOrient o) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, orient, o); }
    void setBoxPack(EBoxPack p) { SET_VAR(rareNonInheritedData.access()->m_deprecatedFlexibleBox, pack, p); }
    void setBoxShadow(PassRefPtr<ShadowList>);
    void setBoxReflect(PassRefPtr<StyleReflection> reflect) { if (rareNonInheritedData->m_boxReflect != reflect) rareNonInheritedData.access()->m_boxReflect = reflect; }
    void setBoxSizing(EBoxSizing s) { SET_VAR(m_box, m_boxSizing, s); }
    void setFlexGrow(float f) { SET_VAR(rareNonInheritedData.access()->m_flexibleBox, m_flexGrow, f); }
    void setFlexShrink(float f) { SET_VAR(rareNonInheritedData.access()->m_flexibleBox, m_flexShrink, f); }
    void setFlexBasis(const Length& length) { SET_VAR(rareNonInheritedData.access()->m_flexibleBox, m_flexBasis, length); }
    // We restrict the smallest value to int min + 2 because we use int min and int min + 1 as special values in a hash set.
    void setOrder(int o) { SET_VAR(rareNonInheritedData, m_order, max(std::numeric_limits<int>::min() + 2, o)); }
    void addCallbackSelector(const String& selector);
    void setAlignContent(const StyleContentAlignmentData& data) { SET_VAR(rareNonInheritedData, m_alignContent, data); }
    void setAlignContentPosition(ContentPosition position) { rareNonInheritedData.access()->m_alignContent.setPosition(position); }
    void setAlignContentDistribution(ContentDistributionType distribution) { rareNonInheritedData.access()->m_alignContent.setDistribution(distribution); }
    void setAlignContentOverflow(OverflowAlignment overflow) { rareNonInheritedData.access()->m_alignContent.setOverflow(overflow); }
    void setAlignItems(const StyleSelfAlignmentData& data) { SET_VAR(rareNonInheritedData, m_alignItems, data); }
    void setAlignItemsPosition(ItemPosition position) { rareNonInheritedData.access()->m_alignItems.setPosition(position); }
    void setAlignItemsOverflow(OverflowAlignment overflow) { rareNonInheritedData.access()->m_alignItems.setOverflow(overflow); }
    void setAlignSelf(const StyleSelfAlignmentData& data) { SET_VAR(rareNonInheritedData, m_alignSelf, data); }
    void setAlignSelfPosition(ItemPosition position) { rareNonInheritedData.access()->m_alignSelf.setPosition(position); }
    void setAlignSelfOverflow(OverflowAlignment overflow) { rareNonInheritedData.access()->m_alignSelf.setOverflow(overflow); }
    void setFlexDirection(EFlexDirection direction) { SET_VAR(rareNonInheritedData.access()->m_flexibleBox, m_flexDirection, direction); }
    void setFlexWrap(EFlexWrap w) { SET_VAR(rareNonInheritedData.access()->m_flexibleBox, m_flexWrap, w); }
    void setJustifyContent(const StyleContentAlignmentData& data) { SET_VAR(rareNonInheritedData, m_justifyContent, data); }
    void setJustifyContentPosition(ContentPosition position) { rareNonInheritedData.access()->m_justifyContent.setPosition(position); }
    void setJustifyContentDistribution(ContentDistributionType distribution) { rareNonInheritedData.access()->m_justifyContent.setDistribution(distribution); }
    void setJustifyContentOverflow(OverflowAlignment overflow) { rareNonInheritedData.access()->m_justifyContent.setOverflow(overflow); }
    void setJustifyItems(const StyleSelfAlignmentData& data) { SET_VAR(rareNonInheritedData, m_justifyItems, data); }
    void setJustifyItemsPosition(ItemPosition position) { rareNonInheritedData.access()->m_justifyItems.setPosition(position); }
    void setJustifyItemsOverflow(OverflowAlignment overflow) { rareNonInheritedData.access()->m_justifyItems.setOverflow(overflow); }
    void setJustifyItemsPositionType(ItemPositionType positionType) { rareNonInheritedData.access()->m_justifyItems.setPositionType(positionType); }
    void setJustifySelf(const StyleSelfAlignmentData& data) { SET_VAR(rareNonInheritedData, m_justifySelf, data); }
    void setJustifySelfPosition(ItemPosition position) { rareNonInheritedData.access()->m_justifySelf.setPosition(position); }
    void setJustifySelfOverflow(OverflowAlignment overflow) { rareNonInheritedData.access()->m_justifySelf.setOverflow(overflow); }
    void setGridAutoColumns(const GridTrackSize& length) { SET_VAR(rareNonInheritedData.access()->m_grid, m_gridAutoColumns, length); }
    void setGridAutoRows(const GridTrackSize& length) { SET_VAR(rareNonInheritedData.access()->m_grid, m_gridAutoRows, length); }
    void setGridTemplateColumns(const Vector<GridTrackSize>& lengths) { SET_VAR(rareNonInheritedData.access()->m_grid, m_gridTemplateColumns, lengths); }
    void setGridTemplateRows(const Vector<GridTrackSize>& lengths) { SET_VAR(rareNonInheritedData.access()->m_grid, m_gridTemplateRows, lengths); }
    void setNamedGridColumnLines(const NamedGridLinesMap& namedGridColumnLines) { SET_VAR(rareNonInheritedData.access()->m_grid, m_namedGridColumnLines, namedGridColumnLines); }
    void setNamedGridRowLines(const NamedGridLinesMap& namedGridRowLines) { SET_VAR(rareNonInheritedData.access()->m_grid, m_namedGridRowLines, namedGridRowLines); }
    void setOrderedNamedGridColumnLines(const OrderedNamedGridLines& orderedNamedGridColumnLines) { SET_VAR(rareNonInheritedData.access()->m_grid, m_orderedNamedGridColumnLines, orderedNamedGridColumnLines); }
    void setOrderedNamedGridRowLines(const OrderedNamedGridLines& orderedNamedGridRowLines) { SET_VAR(rareNonInheritedData.access()->m_grid, m_orderedNamedGridRowLines, orderedNamedGridRowLines); }
    void setNamedGridArea(const NamedGridAreaMap& namedGridArea) { SET_VAR(rareNonInheritedData.access()->m_grid, m_namedGridArea, namedGridArea); }
    void setNamedGridAreaRowCount(size_t rowCount) { SET_VAR(rareNonInheritedData.access()->m_grid, m_namedGridAreaRowCount, rowCount); }
    void setNamedGridAreaColumnCount(size_t columnCount) { SET_VAR(rareNonInheritedData.access()->m_grid, m_namedGridAreaColumnCount, columnCount); }
    void setGridAutoFlow(GridAutoFlow flow) { SET_VAR(rareNonInheritedData.access()->m_grid, m_gridAutoFlow, flow); }

    void setGridColumnStart(const GridPosition& columnStartPosition) { SET_VAR(rareNonInheritedData.access()->m_gridItem, m_gridColumnStart, columnStartPosition); }
    void setGridColumnEnd(const GridPosition& columnEndPosition) { SET_VAR(rareNonInheritedData.access()->m_gridItem, m_gridColumnEnd, columnEndPosition); }
    void setGridRowStart(const GridPosition& rowStartPosition) { SET_VAR(rareNonInheritedData.access()->m_gridItem, m_gridRowStart, rowStartPosition); }
    void setGridRowEnd(const GridPosition& rowEndPosition) { SET_VAR(rareNonInheritedData.access()->m_gridItem, m_gridRowEnd, rowEndPosition); }

    void setUserModify(EUserModify u) { SET_VAR(rareInheritedData, userModify, u); }
    void setUserDrag(EUserDrag d) { SET_VAR(rareNonInheritedData, userDrag, d); }
    void setUserSelect(EUserSelect s) { SET_VAR(rareInheritedData, userSelect, s); }
    void setTextOverflow(TextOverflow overflow) { SET_VAR(rareNonInheritedData, textOverflow, overflow); }
    void setMarginBeforeCollapse(EMarginCollapse c) { SET_VAR(rareNonInheritedData, marginBeforeCollapse, c); }
    void setMarginAfterCollapse(EMarginCollapse c) { SET_VAR(rareNonInheritedData, marginAfterCollapse, c); }
    void setWordBreak(EWordBreak b) { SET_VAR(rareInheritedData, wordBreak, b); }
    void setOverflowWrap(EOverflowWrap b) { SET_VAR(rareInheritedData, overflowWrap, b); }
    void setLineBreak(LineBreak b) { SET_VAR(rareInheritedData, lineBreak, b); }
    void setHighlight(const AtomicString& h) { SET_VAR(rareInheritedData, highlight, h); }
    void setHyphenationString(const AtomicString& h) { SET_VAR(rareInheritedData, hyphenationString, h); }
    void setLocale(const AtomicString& locale) { SET_VAR(rareInheritedData, locale, locale); }
    void setResize(EResize r) { SET_VAR(rareNonInheritedData, m_resize, r); }
    void setColumnWidth(float f) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_autoWidth, false); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_width, f); }
    void setHasAutoColumnWidth() { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_autoWidth, true); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_width, 0); }
    void setColumnCount(unsigned short c) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_autoCount, false); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_count, c); }
    void setHasAutoColumnCount() { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_autoCount, true); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_count, initialColumnCount()); }
    void setColumnFill(ColumnFill columnFill) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_fill, columnFill); }
    void setColumnGap(float f) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_normalGap, false); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_gap, f); }
    void setHasNormalColumnGap() { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_normalGap, true); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_gap, 0); }
    void setColumnRuleColor(const StyleColor& c) { SET_BORDERVALUE_COLOR(rareNonInheritedData.access()->m_multiCol, m_rule, c); }
    void setColumnRuleStyle(EBorderStyle b) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_rule.m_style, b); }
    void setColumnRuleWidth(unsigned short w) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_rule.m_width, w); }
    void setColumnSpan(ColumnSpan columnSpan) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_columnSpan, columnSpan); }
    void setColumnBreakBefore(EPageBreak p) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_breakBefore, p); }
    // For valid values of column-break-inside see http://www.w3.org/TR/css3-multicol/#break-before-break-after-break-inside
    void setColumnBreakInside(EPageBreak p) { ASSERT(p == PBAUTO || p == PBAVOID); SET_VAR(rareNonInheritedData.access()->m_multiCol, m_breakInside, p); }
    void setColumnBreakAfter(EPageBreak p) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_breakAfter, p); }
    void inheritColumnPropertiesFrom(const ComputedStyle& parent) { rareNonInheritedData.access()->m_multiCol = parent.rareNonInheritedData->m_multiCol; }
    void setHasInlineTransform(bool b) { SET_VAR(rareNonInheritedData, m_hasInlineTransform, b); }
    void setHasCompositorProxy(bool b) { SET_VAR(rareNonInheritedData, m_hasCompositorProxy, b); }
    void setTransform(const TransformOperations& ops) { SET_VAR(rareNonInheritedData.access()->m_transform, m_operations, ops); }
    void setTransformOriginX(const Length& v) { setTransformOrigin(TransformOrigin(v, transformOriginY(), transformOriginZ())); }
    void setTransformOriginY(const Length& v) { setTransformOrigin(TransformOrigin(transformOriginX(), v, transformOriginZ())); }
    void setTransformOriginZ(float f) { setTransformOrigin(TransformOrigin(transformOriginX(), transformOriginY(), f)); }
    void setTransformOrigin(const TransformOrigin& o) { SET_VAR(rareNonInheritedData.access()->m_transform, m_origin, o); }
    void setTranslate(PassRefPtr<TranslateTransformOperation> v) { rareNonInheritedData.access()->m_transform.access()->m_translate = v; }
    void setRotate(PassRefPtr<RotateTransformOperation> v) { rareNonInheritedData.access()->m_transform.access()->m_rotate = v; }
    void setScale(PassRefPtr<ScaleTransformOperation> v) { rareNonInheritedData.access()->m_transform.access()->m_scale = v; }

    void setSpeak(ESpeak s) { SET_VAR(rareInheritedData, speak, s); }
    void setTextCombine(TextCombine v) { SET_VAR(rareInheritedData, m_textCombine, v); }
    void setTextDecorationColor(const StyleColor& c) { SET_VAR(rareNonInheritedData, m_textDecorationColor, c); }
    void setTextEmphasisColor(const StyleColor& c) { SET_VAR_WITH_SETTER(rareInheritedData, textEmphasisColor, setTextEmphasisColor, c); }
    void setTextEmphasisFill(TextEmphasisFill fill) { SET_VAR(rareInheritedData, textEmphasisFill, fill); }
    void setTextEmphasisMark(TextEmphasisMark mark) { SET_VAR(rareInheritedData, textEmphasisMark, mark); }
    void setTextEmphasisCustomMark(const AtomicString& mark) { SET_VAR(rareInheritedData, textEmphasisCustomMark, mark); }
    void setTextEmphasisPosition(TextEmphasisPosition position) { SET_VAR(rareInheritedData, textEmphasisPosition, position); }
    bool setTextOrientation(TextOrientation);

    void setMotionPath(PassRefPtr<StyleMotionPath>);
    void resetMotionPath();
    void setMotionOffset(const Length& motionOffset) { SET_VAR(rareNonInheritedData.access()->m_transform, m_motion.m_offset, motionOffset); }
    void setMotionRotation(float motionRotation) { SET_VAR(rareNonInheritedData.access()->m_transform, m_motion.m_rotation, motionRotation); }
    void setMotionRotationType(MotionRotationType motionRotationType) { SET_VAR(rareNonInheritedData.access()->m_transform, m_motion.m_rotationType, motionRotationType); }

    void setObjectFit(ObjectFit f) { SET_VAR(rareNonInheritedData, m_objectFit, f); }
    void setObjectPosition(LengthPoint position) { SET_VAR(rareNonInheritedData, m_objectPosition, position); }

    void setRubyPosition(RubyPosition position) { SET_VAR(rareInheritedData, m_rubyPosition, position); }

    void setFilter(const FilterOperations& ops) { SET_VAR(rareNonInheritedData.access()->m_filter, m_operations, ops); }

    void setTabSize(TabSize size) { SET_VAR(rareInheritedData, m_tabSize, size); }

    // End CSS3 Setters

    void setWrapFlow(WrapFlow wrapFlow) { SET_VAR(rareNonInheritedData, m_wrapFlow, wrapFlow); }
    void setWrapThrough(WrapThrough wrapThrough) { SET_VAR(rareNonInheritedData, m_wrapThrough, wrapThrough); }

    // Apple-specific property setters
    void setPointerEvents(EPointerEvents p) { inherited_flags._pointerEvents = p; }

    void setTransformStyle3D(ETransformStyle3D b) { SET_VAR(rareNonInheritedData, m_transformStyle3D, b); }
    void setBackfaceVisibility(EBackfaceVisibility b) { SET_VAR(rareNonInheritedData, m_backfaceVisibility, b); }
    void setPerspective(float p) { SET_VAR(rareNonInheritedData, m_perspective, p); }
    void setPerspectiveOriginX(const Length& v) { setPerspectiveOrigin(LengthPoint(v, perspectiveOriginY())); }
    void setPerspectiveOriginY(const Length& v) { setPerspectiveOrigin(LengthPoint(perspectiveOriginX(), v)); }
    void setPerspectiveOrigin(const LengthPoint& p) { SET_VAR(rareNonInheritedData, m_perspectiveOrigin, p); }
    void setPageSize(const FloatSize& s) { SET_VAR(rareNonInheritedData, m_pageSize, s); }
    void setPageSizeType(PageSizeType t) { SET_VAR(rareNonInheritedData, m_pageSizeType, t); }
    void resetPageSizeType() { SET_VAR(rareNonInheritedData, m_pageSizeType, PAGE_SIZE_AUTO); }

    void setHasCurrentOpacityAnimation(bool b = true) { SET_VAR(rareNonInheritedData, m_hasCurrentOpacityAnimation, b); }
    void setHasCurrentTransformAnimation(bool b = true) { SET_VAR(rareNonInheritedData, m_hasCurrentTransformAnimation, b); }
    void setHasCurrentFilterAnimation(bool b = true) { SET_VAR(rareNonInheritedData, m_hasCurrentFilterAnimation, b); }

    void setIsRunningOpacityAnimationOnCompositor(bool b = true) { SET_VAR(rareNonInheritedData, m_runningOpacityAnimationOnCompositor, b); }
    void setIsRunningTransformAnimationOnCompositor(bool b = true) { SET_VAR(rareNonInheritedData, m_runningTransformAnimationOnCompositor, b); }
    void setIsRunningFilterAnimationOnCompositor(bool b = true) { SET_VAR(rareNonInheritedData, m_runningFilterAnimationOnCompositor, b); }

    void setLineBoxContain(LineBoxContain c) { SET_VAR(rareInheritedData, m_lineBoxContain, c); }
    void setLineClamp(LineClampValue c) { SET_VAR(rareNonInheritedData, lineClamp, c); }
    void setTapHighlightColor(const Color& c) { SET_VAR(rareInheritedData, tapHighlightColor, c); }
    void setTextSecurity(ETextSecurity aTextSecurity) { SET_VAR(rareInheritedData, textSecurity, aTextSecurity); }
    void setTouchAction(TouchAction t) { SET_VAR(rareNonInheritedData, m_touchAction, t); }

    void setScrollBehavior(ScrollBehavior b) { SET_VAR(rareNonInheritedData, m_scrollBehavior, b); }
    void setScrollBlocksOn(WebScrollBlocksOn b) { SET_VAR(rareNonInheritedData, m_scrollBlocksOn, b); }

    void setScrollSnapType(ScrollSnapType b) { SET_VAR(rareNonInheritedData, m_scrollSnapType, b); }
    void setScrollSnapPointsX(const ScrollSnapPoints& b) { SET_VAR(rareNonInheritedData.access()->m_scrollSnap, m_xPoints, b); }
    void setScrollSnapPointsY(const ScrollSnapPoints& b) { SET_VAR(rareNonInheritedData.access()->m_scrollSnap, m_yPoints, b); }
    void setScrollSnapDestination(const LengthPoint& b) { SET_VAR(rareNonInheritedData.access()->m_scrollSnap, m_destination, b); }
    void setScrollSnapCoordinate(const Vector<LengthPoint>& b) { SET_VAR(rareNonInheritedData.access()->m_scrollSnap, m_coordinates, b); }

    void setWillChangeProperties(const Vector<CSSPropertyID>& properties) { SET_VAR(rareNonInheritedData.access()->m_willChange, m_properties, properties); }
    void setWillChangeContents(bool b) { SET_VAR(rareNonInheritedData.access()->m_willChange, m_contents, b); }
    void setWillChangeScrollPosition(bool b) { SET_VAR(rareNonInheritedData.access()->m_willChange, m_scrollPosition, b); }
    void setSubtreeWillChangeContents(bool b) { SET_VAR(rareInheritedData, m_subtreeWillChangeContents, b); }

    bool requiresAcceleratedCompositingForExternalReasons(bool b) { return rareNonInheritedData->m_requiresAcceleratedCompositingForExternalReasons; }
    void setRequiresAcceleratedCompositingForExternalReasons(bool b) { SET_VAR(rareNonInheritedData, m_requiresAcceleratedCompositingForExternalReasons, b); }

    const SVGComputedStyle& svgStyle() const { return *m_svgStyle.get(); }
    SVGComputedStyle& accessSVGStyle() { return *m_svgStyle.access(); }

    const SVGPaintType& fillPaintType() const { return svgStyle().fillPaintType(); }
    Color fillPaintColor() const { return svgStyle().fillPaintColor(); }
    float fillOpacity() const { return svgStyle().fillOpacity(); }
    void setFillOpacity(float f) { accessSVGStyle().setFillOpacity(f); }

    const SVGPaintType& strokePaintType() const { return svgStyle().strokePaintType(); }
    Color strokePaintColor() const { return svgStyle().strokePaintColor(); }
    float strokeOpacity() const { return svgStyle().strokeOpacity(); }
    void setStrokeOpacity(float f) { accessSVGStyle().setStrokeOpacity(f); }
    const UnzoomedLength& strokeWidth() const { return svgStyle().strokeWidth(); }
    void setStrokeWidth(const UnzoomedLength& w) { accessSVGStyle().setStrokeWidth(w); }
    SVGDashArray* strokeDashArray() const { return svgStyle().strokeDashArray(); }
    void setStrokeDashArray(PassRefPtr<SVGDashArray> array) { accessSVGStyle().setStrokeDashArray(array); }
    const Length& strokeDashOffset() const { return svgStyle().strokeDashOffset(); }
    void setStrokeDashOffset(const Length& d) { accessSVGStyle().setStrokeDashOffset(d); }
    float strokeMiterLimit() const { return svgStyle().strokeMiterLimit(); }
    void setStrokeMiterLimit(float f) { accessSVGStyle().setStrokeMiterLimit(f); }

    void setCx(const Length& cx) { accessSVGStyle().setCx(cx); }
    void setCy(const Length& cy) { accessSVGStyle().setCy(cy); }
    void setX(const Length& x) { accessSVGStyle().setX(x); }
    void setY(const Length& y) { accessSVGStyle().setY(y); }
    void setR(const Length& r) { accessSVGStyle().setR(r); }
    void setRx(const Length& rx) { accessSVGStyle().setRx(rx); }
    void setRy(const Length& ry) { accessSVGStyle().setRy(ry); }

    float floodOpacity() const { return svgStyle().floodOpacity(); }
    void setFloodOpacity(float f) { accessSVGStyle().setFloodOpacity(f); }

    float stopOpacity() const { return svgStyle().stopOpacity(); }
    void setStopOpacity(float f) { accessSVGStyle().setStopOpacity(f); }

    void setStopColor(const Color& c) { accessSVGStyle().setStopColor(c); }
    void setFloodColor(const Color& c) { accessSVGStyle().setFloodColor(c); }
    void setLightingColor(const Color& c) { accessSVGStyle().setLightingColor(c); }

    const Length& baselineShiftValue() const { return svgStyle().baselineShiftValue(); }
    void setBaselineShiftValue(const Length& s) { accessSVGStyle().setBaselineShiftValue(s); }

    void setShapeOutside(PassRefPtr<ShapeValue> value)
    {
        if (rareNonInheritedData->m_shapeOutside == value)
            return;
        rareNonInheritedData.access()->m_shapeOutside = value;
    }
    ShapeValue* shapeOutside() const { return rareNonInheritedData->m_shapeOutside.get(); }

    static ShapeValue* initialShapeOutside() { return 0; }

    void setClipPath(PassRefPtr<ClipPathOperation> operation)
    {
        if (rareNonInheritedData->m_clipPath != operation)
            rareNonInheritedData.access()->m_clipPath = operation;
    }
    ClipPathOperation* clipPath() const { return rareNonInheritedData->m_clipPath.get(); }

    static ClipPathOperation* initialClipPath() { return 0; }

    const Length& shapeMargin() const { return rareNonInheritedData->m_shapeMargin; }
    void setShapeMargin(const Length& shapeMargin) { SET_VAR(rareNonInheritedData, m_shapeMargin, shapeMargin); }
    static Length initialShapeMargin() { return Length(0, Fixed); }

    float shapeImageThreshold() const { return rareNonInheritedData->m_shapeImageThreshold; }
    void setShapeImageThreshold(float shapeImageThreshold)
    {
        float clampedShapeImageThreshold = clampTo<float>(shapeImageThreshold, 0, 1);
        SET_VAR(rareNonInheritedData, m_shapeImageThreshold, clampedShapeImageThreshold);
    }
    static float initialShapeImageThreshold() { return 0; }

    bool hasContent() const { return contentData(); }
    const ContentData* contentData() const { return rareNonInheritedData->m_content.get(); }
    bool contentDataEquivalent(const ComputedStyle* otherStyle) const { return const_cast<ComputedStyle*>(this)->rareNonInheritedData->contentDataEquivalent(*const_cast<ComputedStyle*>(otherStyle)->rareNonInheritedData); }
    void clearContent();
    void setContent(const String&, bool add = false);
    void setContent(PassRefPtr<StyleImage>, bool add = false);
    void setContent(PassOwnPtr<CounterContent>, bool add = false);
    void setContent(QuoteType, bool add = false);

    const CounterDirectiveMap* counterDirectives() const;
    CounterDirectiveMap& accessCounterDirectives();
    const CounterDirectives getCounterDirectives(const AtomicString& identifier) const;
    void clearIncrementDirectives();
    void clearResetDirectives();

    QuotesData* quotes() const { return rareInheritedData->quotes.get(); }
    void setQuotes(PassRefPtr<QuotesData>);

    const AtomicString& hyphenString() const;

    bool inheritedNotEqual(const ComputedStyle&) const;
    bool inheritedDataShared(const ComputedStyle&) const;

    bool isDisplayReplacedType() const { return isDisplayReplacedType(display()); }
    bool isDisplayInlineType() const { return isDisplayInlineType(display()); }
    bool isOriginalDisplayInlineType() const { return isDisplayInlineType(originalDisplay()); }
    bool isDisplayFlexibleOrGridBox() const { return isDisplayFlexibleBox(display()) || isDisplayGridBox(display()); }
    bool isDisplayFlexibleBox() const { return isDisplayFlexibleBox(display()); }


    bool setWritingMode(WritingMode v)
    {
        if (v == writingMode())
            return false;

        inherited_flags.m_writingMode = v;
        return true;
    }

    // A unique style is one that has matches something that makes it impossible to share.
    bool unique() const { return noninherited_flags.unique; }
    void setUnique() { noninherited_flags.unique = true; }

    bool isSharable() const;

    bool emptyState() const { return noninherited_flags.emptyState; }
    void setEmptyState(bool b) { setUnique(); noninherited_flags.emptyState = b; }

    Color visitedDependentColor(int colorProperty) const;

    void setHasExplicitlyInheritedProperties() { noninherited_flags.explicitInheritance = true; }
    bool hasExplicitlyInheritedProperties() const { return noninherited_flags.explicitInheritance; }

    bool hasChildDependentFlags() const { return emptyState() || hasExplicitlyInheritedProperties(); }
    void copyChildDependentFlagsFrom(const ComputedStyle&);

    bool hasBoxDecorations() const { return hasBorderDecoration() || hasBorderRadius() || hasOutline() || hasAppearance() || boxShadow() || hasFilter() || resize() != RESIZE_NONE; }

    bool borderObscuresBackground() const;
    void getBorderEdgeInfo(BorderEdge edges[], bool includeLogicalLeftEdge = true, bool includeLogicalRightEdge = true) const;

    // Initial values for all the properties
    static EBorderCollapse initialBorderCollapse() { return BSEPARATE; }
    static EBorderStyle initialBorderStyle() { return BNONE; }
    static OutlineIsAuto initialOutlineStyleIsAuto() { return AUTO_OFF; }
    static NinePieceImage initialNinePieceImage() { return NinePieceImage(); }
    static LengthSize initialBorderRadius() { return LengthSize(Length(0, Fixed), Length(0, Fixed)); }
    static ECaptionSide initialCaptionSide() { return CAPTOP; }
    static EClear initialClear() { return CNONE; }
    static LengthBox initialClip() { return LengthBox(); }
    static TextDirection initialDirection() { return LTR; }
    static WritingMode initialWritingMode() { return TopToBottomWritingMode; }
    static TextCombine initialTextCombine() { return TextCombineNone; }
    static TextOrientation initialTextOrientation() { return TextOrientationVerticalRight; }
    static ObjectFit initialObjectFit() { return ObjectFitFill; }
    static LengthPoint initialObjectPosition() { return LengthPoint(Length(50.0, Percent), Length(50.0, Percent)); }
    static EDisplay initialDisplay() { return INLINE; }
    static EEmptyCell initialEmptyCells() { return SHOW; }
    static EFloat initialFloating() { return NoFloat; }
    static EListStylePosition initialListStylePosition() { return OUTSIDE; }
    static EListStyleType initialListStyleType() { return Disc; }
    static EOverflow initialOverflowX() { return OVISIBLE; }
    static EOverflow initialOverflowY() { return OVISIBLE; }
    static EPageBreak initialPageBreak() { return PBAUTO; }
    static EPosition initialPosition() { return StaticPosition; }
    static ETableLayout initialTableLayout() { return TAUTO; }
    static EUnicodeBidi initialUnicodeBidi() { return UBNormal; }
    static ETextTransform initialTextTransform() { return TTNONE; }
    static EVisibility initialVisibility() { return VISIBLE; }
    static EWhiteSpace initialWhiteSpace() { return NORMAL; }
    static short initialHorizontalBorderSpacing() { return 0; }
    static short initialVerticalBorderSpacing() { return 0; }
    static ECursor initialCursor() { return CURSOR_AUTO; }
    static Color initialColor() { return Color::black; }
    static StyleImage* initialListStyleImage() { return 0; }
    static unsigned initialBorderWidth() { return 3; }
    static unsigned short initialColumnRuleWidth() { return 3; }
    static unsigned short initialOutlineWidth() { return 3; }
    static float initialLetterWordSpacing() { return 0.0f; }
    static Length initialSize() { return Length(); }
    static Length initialMinSize() { return Length(); }
    static Length initialMaxSize() { return Length(MaxSizeNone); }
    static Length initialOffset() { return Length(); }
    static Length initialMargin() { return Length(Fixed); }
    static Length initialPadding() { return Length(Fixed); }
    static Length initialTextIndent() { return Length(Fixed); }
    static TextIndentLine initialTextIndentLine() { return TextIndentFirstLine; }
    static TextIndentType initialTextIndentType() { return TextIndentNormal; }
    static EVerticalAlign initialVerticalAlign() { return BASELINE; }
    static short initialWidows() { return 1; }
    static short initialOrphans() { return 2; }
    static Length initialLineHeight() { return Length(-100.0, Percent); }
    static ETextAlign initialTextAlign() { return TASTART; }
    static TextAlignLast initialTextAlignLast() { return TextAlignLastAuto; }
    static TextJustify initialTextJustify() { return TextJustifyAuto; }
    static TextDecoration initialTextDecoration() { return TextDecorationNone; }
    static TextUnderlinePosition initialTextUnderlinePosition() { return TextUnderlinePositionAuto; }
    static TextDecorationStyle initialTextDecorationStyle() { return TextDecorationStyleSolid; }
    static float initialZoom() { return 1.0f; }
    static int initialOutlineOffset() { return 0; }
    static float initialOpacity() { return 1.0f; }
    static EBoxAlignment initialBoxAlign() { return BSTRETCH; }
    static EBoxDecorationBreak initialBoxDecorationBreak() { return DSLICE; }
    static EBoxDirection initialBoxDirection() { return BNORMAL; }
    static EBoxLines initialBoxLines() { return SINGLE; }
    static EBoxOrient initialBoxOrient() { return HORIZONTAL; }
    static EBoxPack initialBoxPack() { return Start; }
    static float initialBoxFlex() { return 0.0f; }
    static unsigned initialBoxFlexGroup() { return 1; }
    static unsigned initialBoxOrdinalGroup() { return 1; }
    static EBoxSizing initialBoxSizing() { return CONTENT_BOX; }
    static StyleReflection* initialBoxReflect() { return 0; }
    static float initialFlexGrow() { return 0; }
    static float initialFlexShrink() { return 1; }
    static Length initialFlexBasis() { return Length(Auto); }
    static int initialOrder() { return 0; }
    static StyleContentAlignmentData initialContentAlignment() { return StyleContentAlignmentData(ContentPositionAuto, ContentDistributionDefault, OverflowAlignmentDefault); }
    static StyleSelfAlignmentData initialSelfAlignment() { return StyleSelfAlignmentData(ItemPositionAuto, OverflowAlignmentDefault); }
    static EFlexDirection initialFlexDirection() { return FlowRow; }
    static EFlexWrap initialFlexWrap() { return FlexNoWrap; }
    static EUserModify initialUserModify() { return READ_ONLY; }
    static EUserDrag initialUserDrag() { return DRAG_AUTO; }
    static EUserSelect initialUserSelect() { return SELECT_TEXT; }
    static TextOverflow initialTextOverflow() { return TextOverflowClip; }
    static EMarginCollapse initialMarginBeforeCollapse() { return MCOLLAPSE; }
    static EMarginCollapse initialMarginAfterCollapse() { return MCOLLAPSE; }
    static EWordBreak initialWordBreak() { return NormalWordBreak; }
    static EOverflowWrap initialOverflowWrap() { return NormalOverflowWrap; }
    static LineBreak initialLineBreak() { return LineBreakAuto; }
    static const AtomicString& initialHighlight() { return nullAtom; }
    static ESpeak initialSpeak() { return SpeakNormal; }
    static const AtomicString& initialHyphenationString() { return nullAtom; }
    static const AtomicString& initialLocale() { return nullAtom; }
    static EResize initialResize() { return RESIZE_NONE; }
    static ControlPart initialAppearance() { return NoControlPart; }
    static Order initialRTLOrdering() { return LogicalOrder; }
    static float initialTextStrokeWidth() { return 0; }
    static unsigned short initialColumnCount() { return 1; }
    static ColumnFill initialColumnFill() { return ColumnFillBalance; }
    static ColumnSpan initialColumnSpan() { return ColumnSpanNone; }
    static const TransformOperations& initialTransform() { DEFINE_STATIC_LOCAL(TransformOperations, ops, ()); return ops; }
    static PassRefPtr<TranslateTransformOperation> initialTranslate() { return TranslateTransformOperation::create(Length(0, Fixed), Length(0, Fixed), 0, TransformOperation::Translate3D); }
    static PassRefPtr<RotateTransformOperation> initialRotate() { return RotateTransformOperation::create(0, 0, 1, 0, TransformOperation::Rotate3D); }
    static PassRefPtr<ScaleTransformOperation> initialScale() { return ScaleTransformOperation::create(1, 1, 1, TransformOperation::Scale3D); }
    static Length initialTransformOriginX() { return Length(50.0, Percent); }
    static Length initialTransformOriginY() { return Length(50.0, Percent); }
    static float initialTransformOriginZ() { return 0; }
    static TransformOrigin initialTransformOrigin() { return TransformOrigin(Length(50.0, Percent), Length(50.0, Percent), 0); }
    static EPointerEvents initialPointerEvents() { return PE_AUTO; }
    static ETransformStyle3D initialTransformStyle3D() { return TransformStyle3DFlat; }
    static const StyleMotionPath* initialMotionPath() { return nullptr; }
    static Length initialMotionOffset() { return Length(0, Fixed); }
    static float initialMotionRotation() { return 0; }
    static MotionRotationType initialMotionRotationType() { return MotionRotationAuto; }
    static EBackfaceVisibility initialBackfaceVisibility() { return BackfaceVisibilityVisible; }
    static float initialPerspective() { return 0; }
    static Length initialPerspectiveOriginX() { return Length(50.0, Percent); }
    static Length initialPerspectiveOriginY() { return Length(50.0, Percent); }
    static LengthPoint initialPerspectiveOrigin() { return LengthPoint(Length(50.0, Percent), Length(50.0, Percent)); }
    static Color initialBackgroundColor() { return Color::transparent; }
    static TextEmphasisFill initialTextEmphasisFill() { return TextEmphasisFillFilled; }
    static TextEmphasisMark initialTextEmphasisMark() { return TextEmphasisMarkNone; }
    static const AtomicString& initialTextEmphasisCustomMark() { return nullAtom; }
    static TextEmphasisPosition initialTextEmphasisPosition() { return TextEmphasisPositionOver; }
    static RubyPosition initialRubyPosition() { return RubyPositionBefore; }
    static LineBoxContain initialLineBoxContain() { return LineBoxContainBlock | LineBoxContainInline | LineBoxContainReplaced; }
    static ImageOrientationEnum initialImageOrientation() { return OriginTopLeft; }
    static EImageRendering initialImageRendering() { return ImageRenderingAuto; }
    static ImageResolutionSource initialImageResolutionSource() { return ImageResolutionSpecified; }
    static ImageResolutionSnap initialImageResolutionSnap() { return ImageResolutionNoSnap; }
    static float initialImageResolution() { return 1; }
    static StyleImage* initialBorderImageSource() { return 0; }
    static StyleImage* initialMaskBoxImageSource() { return 0; }
    static PrintColorAdjust initialPrintColorAdjust() { return PrintColorAdjustEconomy; }
    static TouchAction initialTouchAction() { return TouchActionAuto; }
    static ShadowList* initialBoxShadow() { return 0; }
    static ShadowList* initialTextShadow() { return 0; }
	static StyleVariableData* initialVariables() { return nullptr; }
    static ScrollBehavior initialScrollBehavior() { return ScrollBehaviorAuto; }
    static WebScrollBlocksOn initialScrollBlocksOn() { return WebScrollBlocksOnNone; }
    static ScrollSnapType initialScrollSnapType() { return ScrollSnapTypeNone; }
    static ScrollSnapPoints initialScrollSnapPointsX() { return ScrollSnapPoints(); }
    static ScrollSnapPoints initialScrollSnapPointsY() { return ScrollSnapPoints(); }
    static LengthPoint initialScrollSnapDestination() { return LengthPoint(Length(0, Fixed), Length(0, Fixed)); }
    static Vector<LengthPoint> initialScrollSnapCoordinate() { return Vector<LengthPoint>(); }

    // The initial value is 'none' for grid tracks.
    static Vector<GridTrackSize> initialGridTemplateColumns() { return Vector<GridTrackSize>(); }
    static Vector<GridTrackSize> initialGridTemplateRows() { return Vector<GridTrackSize>(); }

    static GridAutoFlow initialGridAutoFlow() { return AutoFlowRow; }

    static GridTrackSize initialGridAutoColumns() { return GridTrackSize(Length(Auto)); }
    static GridTrackSize initialGridAutoRows() { return GridTrackSize(Length(Auto)); }

    static NamedGridLinesMap initialNamedGridColumnLines() { return NamedGridLinesMap(); }
    static NamedGridLinesMap initialNamedGridRowLines() { return NamedGridLinesMap(); }

    static OrderedNamedGridLines initialOrderedNamedGridColumnLines() { return OrderedNamedGridLines(); }
    static OrderedNamedGridLines initialOrderedNamedGridRowLines() { return OrderedNamedGridLines(); }

    static NamedGridAreaMap initialNamedGridArea() { return NamedGridAreaMap(); }
    static size_t initialNamedGridAreaCount() { return 0; }

    // 'auto' is the default.
    static GridPosition initialGridColumnStart() { return GridPosition(); }
    static GridPosition initialGridColumnEnd() { return GridPosition(); }
    static GridPosition initialGridRowStart() { return GridPosition(); }
    static GridPosition initialGridRowEnd() { return GridPosition(); }

    static TabSize initialTabSize() { return TabSize(8); }

    static WrapFlow initialWrapFlow() { return WrapFlowAuto; }
    static WrapThrough initialWrapThrough() { return WrapThroughWrap; }

    static QuotesData* initialQuotes() { return 0; }

    // Keep these at the end.
    // FIXME: Why? Seems these should all be one big sorted list.
    static LineClampValue initialLineClamp() { return LineClampValue(); }
    static ETextSecurity initialTextSecurity() { return TSNONE; }
    static Color initialTapHighlightColor();
#if ENABLE(OILPAN)
    static const FilterOperations& initialFilter();
#else
    static const FilterOperations& initialFilter() { DEFINE_STATIC_LOCAL(FilterOperations, ops, ()); return ops; }
#endif
    static WebBlendMode initialBlendMode() { return WebBlendModeNormal; }
    static EIsolation initialIsolation() { return IsolationAuto; }
private:
    void setVisitedLinkColor(const Color&);
    void setVisitedLinkBackgroundColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkBackgroundColor, v); }
    void setVisitedLinkBorderLeftColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkBorderLeftColor, v); }
    void setVisitedLinkBorderRightColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkBorderRightColor, v); }
    void setVisitedLinkBorderBottomColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkBorderBottomColor, v); }
    void setVisitedLinkBorderTopColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkBorderTopColor, v); }
    void setVisitedLinkOutlineColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkOutlineColor, v); }
    void setVisitedLinkColumnRuleColor(const StyleColor& v) { SET_VAR(rareNonInheritedData.access()->m_multiCol, m_visitedLinkColumnRuleColor, v); }
    void setVisitedLinkTextDecorationColor(const StyleColor& v) { SET_VAR(rareNonInheritedData, m_visitedLinkTextDecorationColor, v); }
    void setVisitedLinkTextEmphasisColor(const StyleColor& v) { SET_VAR_WITH_SETTER(rareInheritedData, visitedLinkTextEmphasisColor, setVisitedLinkTextEmphasisColor, v); }
    void setVisitedLinkTextFillColor(const StyleColor& v) { SET_VAR_WITH_SETTER(rareInheritedData, visitedLinkTextFillColor, setVisitedLinkTextFillColor, v); }
    void setVisitedLinkTextStrokeColor(const StyleColor& v) { SET_VAR_WITH_SETTER(rareInheritedData, visitedLinkTextStrokeColor, setVisitedLinkTextStrokeColor, v); }

    void inheritUnicodeBidiFrom(const ComputedStyle& parent) { noninherited_flags.unicodeBidi = parent.noninherited_flags.unicodeBidi; }

    bool isDisplayFlexibleBox(EDisplay display) const
    {
        return display == FLEX || display == INLINE_FLEX;
    }

    bool isDisplayGridBox(EDisplay display) const
    {
        return display == GRID || display == INLINE_GRID;
    }

    bool isDisplayReplacedType(EDisplay display) const
    {
        return display == INLINE_BLOCK || display == INLINE_BOX || display == INLINE_FLEX
            || display == INLINE_TABLE || display == INLINE_GRID;
    }

    bool isDisplayInlineType(EDisplay display) const
    {
        return display == INLINE || isDisplayReplacedType(display);
    }

    // Color accessors are all private to make sure callers use visitedDependentColor instead to access them.
    StyleColor borderLeftColor() const { return surround->border.left().color(); }
    StyleColor borderRightColor() const { return surround->border.right().color(); }
    StyleColor borderTopColor() const { return surround->border.top().color(); }
    StyleColor borderBottomColor() const { return surround->border.bottom().color(); }
    StyleColor backgroundColor() const { return m_background->color(); }
    Color color() const;
    StyleColor columnRuleColor() const { return rareNonInheritedData->m_multiCol->m_rule.color(); }
    StyleColor outlineColor() const { return m_background->outline().color(); }
    StyleColor textEmphasisColor() const { return rareInheritedData->textEmphasisColor(); }
    StyleColor textFillColor() const { return rareInheritedData->textFillColor(); }
    StyleColor textStrokeColor() const { return rareInheritedData->textStrokeColor(); }
    Color visitedLinkColor() const;
    StyleColor visitedLinkBackgroundColor() const { return rareNonInheritedData->m_visitedLinkBackgroundColor; }
    StyleColor visitedLinkBorderLeftColor() const { return rareNonInheritedData->m_visitedLinkBorderLeftColor; }
    StyleColor visitedLinkBorderRightColor() const { return rareNonInheritedData->m_visitedLinkBorderRightColor; }
    StyleColor visitedLinkBorderBottomColor() const { return rareNonInheritedData->m_visitedLinkBorderBottomColor; }
    StyleColor visitedLinkBorderTopColor() const { return rareNonInheritedData->m_visitedLinkBorderTopColor; }
    StyleColor visitedLinkOutlineColor() const { return rareNonInheritedData->m_visitedLinkOutlineColor; }
    StyleColor visitedLinkColumnRuleColor() const { return rareNonInheritedData->m_multiCol->m_visitedLinkColumnRuleColor; }
    StyleColor textDecorationColor() const { return rareNonInheritedData->m_textDecorationColor; }
    StyleColor visitedLinkTextDecorationColor() const { return rareNonInheritedData->m_visitedLinkTextDecorationColor; }
    StyleColor visitedLinkTextEmphasisColor() const { return rareInheritedData->visitedLinkTextEmphasisColor(); }
    StyleColor visitedLinkTextFillColor() const { return rareInheritedData->visitedLinkTextFillColor(); }
    StyleColor visitedLinkTextStrokeColor() const { return rareInheritedData->visitedLinkTextStrokeColor(); }

    StyleColor decorationColorIncludingFallback(bool visitedLink) const;
    Color colorIncludingFallback(int colorProperty, bool visitedLink) const;

    Color stopColor() const { return svgStyle().stopColor(); }
    Color floodColor() const { return svgStyle().floodColor(); }
    Color lightingColor() const { return svgStyle().lightingColor(); }

    void appendContent(PassOwnPtr<ContentData>);
    void addAppliedTextDecoration(const AppliedTextDecoration&);
    void applyMotionPathTransform(float originX, float originY, TransformationMatrix&) const;

    bool diffNeedsFullLayoutAndPaintInvalidation(const ComputedStyle& other) const;
    bool diffNeedsFullLayout(const ComputedStyle& other) const;
    bool diffNeedsPaintInvalidationLayer(const ComputedStyle& other) const;
    bool diffNeedsPaintInvalidationObject(const ComputedStyle& other) const;
    void updatePropertySpecificDifferences(const ComputedStyle& other, StyleDifference&) const;

    bool requireTransformOrigin(ApplyTransformOrigin applyOrigin, ApplyMotionPath) const;
};

// FIXME: Reduce/remove the dependency on zoom adjusted int values.
// The float or LayoutUnit versions of layout values should be used.
inline int adjustForAbsoluteZoom(int value, float zoomFactor)
{
    if (zoomFactor == 1)
        return value;
    // Needed because computeLengthInt truncates (rather than rounds) when scaling up.
    float fvalue = value;
    if (zoomFactor > 1) {
        if (value < 0)
            fvalue -= 0.5f;
        else
            fvalue += 0.5f;
    }

    return roundForImpreciseConversion<int>(fvalue / zoomFactor);
}

inline int adjustForAbsoluteZoom(int value, const ComputedStyle* style)
{
    return adjustForAbsoluteZoom(value, style->effectiveZoom());
}

inline float adjustFloatForAbsoluteZoom(float value, const ComputedStyle& style)
{
    return value / style.effectiveZoom();
}

inline double adjustDoubleForAbsoluteZoom(double value, const ComputedStyle& style)
{
    return value / style.effectiveZoom();
}

inline LayoutUnit adjustLayoutUnitForAbsoluteZoom(LayoutUnit value, const ComputedStyle& style)
{
    return value / style.effectiveZoom();
}

inline double adjustScrollForAbsoluteZoom(double scrollOffset, float zoomFactor)
{
    return scrollOffset / zoomFactor;
}

inline double adjustScrollForAbsoluteZoom(double scrollOffset, const ComputedStyle& style)
{
    return adjustScrollForAbsoluteZoom(scrollOffset, style.effectiveZoom());
}

inline bool ComputedStyle::setZoom(float f)
{
    if (compareEqual(visual->m_zoom, f))
        return false;
    visual.access()->m_zoom = f;
    setEffectiveZoom(effectiveZoom() * zoom());
    return true;
}

inline bool ComputedStyle::setEffectiveZoom(float f)
{
    // Clamp the effective zoom value to a smaller (but hopeful still large
    // enough) range, to avoid overflow in derived computations.
    float clampedEffectiveZoom = clampTo<float>(f, 1e-6, 1e6);
    if (compareEqual(rareInheritedData->m_effectiveZoom, clampedEffectiveZoom))
        return false;
    rareInheritedData.access()->m_effectiveZoom = clampedEffectiveZoom;
    return true;
}

inline bool ComputedStyle::isSharable() const
{
    if (unique())
        return false;
    if (hasUniquePseudoStyle())
        return false;
    return true;
}

inline bool ComputedStyle::setTextOrientation(TextOrientation textOrientation)
{
    if (compareEqual(rareInheritedData->m_textOrientation, textOrientation))
        return false;

    rareInheritedData.access()->m_textOrientation = textOrientation;
    return true;
}

inline bool ComputedStyle::hasAnyPublicPseudoStyles() const
{
    return PUBLIC_PSEUDOID_MASK & noninherited_flags.pseudoBits;
}

inline bool ComputedStyle::hasPseudoStyle(PseudoId pseudo) const
{
    ASSERT(pseudo > NOPSEUDO);
    ASSERT(pseudo < FIRST_INTERNAL_PSEUDOID);
    return (1 << (pseudo - 1)) & noninherited_flags.pseudoBits;
}

inline void ComputedStyle::setHasPseudoStyle(PseudoId pseudo)
{
    ASSERT(pseudo > NOPSEUDO);
    ASSERT(pseudo < FIRST_INTERNAL_PSEUDOID);
    noninherited_flags.pseudoBits |= 1 << (pseudo - 1);
}

inline bool ComputedStyle::hasPseudoElementStyle() const
{
    return noninherited_flags.pseudoBits & PSEUDO_ELEMENT_MASK;
}

} // namespace blink

#endif // ComputedStyle_h
