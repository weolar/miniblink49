/*
 * Copyright (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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

#include "config.h"
#include "core/style/StyleRareInheritedData.h"

#include "core/style/AppliedTextDecoration.h"
#include "core/style/CursorData.h"
#include "core/style/DataEquivalency.h"
#include "core/style/ComputedStyle.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/style/QuotesData.h"
#include "core/style/ShadowList.h"
#include "core/style/StyleImage.h"

namespace blink {

struct SameSizeAsStyleRareInheritedData : public RefCounted<SameSizeAsStyleRareInheritedData> {
    void* styleImage;
    Color firstColor;
    float firstFloat;
    Color colors[5];
    void* ownPtrs[1];
    AtomicString atomicStrings[4];
    void* refPtrs[3];
    Length lengths[1];
    float secondFloat;
    unsigned m_bitfields[2];
    short pagedMediaShorts[2];
    short hyphenationShorts[3];

    Color touchColors;
    TabSize tabSize;
    void* variables[1];
};

static_assert(sizeof(StyleRareInheritedData) == sizeof(SameSizeAsStyleRareInheritedData), "StyleRareInheritedData should stay small");

StyleRareInheritedData::StyleRareInheritedData()
    : listStyleImage(ComputedStyle::initialListStyleImage())
    , textStrokeWidth(ComputedStyle::initialTextStrokeWidth())
    , indent(ComputedStyle::initialTextIndent())
    , m_effectiveZoom(ComputedStyle::initialZoom())
    , widows(ComputedStyle::initialWidows())
    , orphans(ComputedStyle::initialOrphans())
    , m_hasAutoOrphans(true)
    , m_textStrokeColorIsCurrentColor(true)
    , m_textFillColorIsCurrentColor(true)
    , m_textEmphasisColorIsCurrentColor(true)
    , m_visitedLinkTextStrokeColorIsCurrentColor(true)
    , m_visitedLinkTextFillColorIsCurrentColor(true)
    , m_visitedLinkTextEmphasisColorIsCurrentColor(true)
    , textSecurity(ComputedStyle::initialTextSecurity())
    , userModify(READ_ONLY)
    , wordBreak(ComputedStyle::initialWordBreak())
    , overflowWrap(ComputedStyle::initialOverflowWrap())
    , lineBreak(LineBreakAuto)
    , userSelect(ComputedStyle::initialUserSelect())
    , speak(SpeakNormal)
    , hyphens(HyphensManual)
    , textEmphasisFill(TextEmphasisFillFilled)
    , textEmphasisMark(TextEmphasisMarkNone)
    , textEmphasisPosition(TextEmphasisPositionOver)
    , m_textAlignLast(ComputedStyle::initialTextAlignLast())
    , m_textJustify(ComputedStyle::initialTextJustify())
    , m_textOrientation(TextOrientationVerticalRight)
    , m_textCombine(ComputedStyle::initialTextCombine())
    , m_textIndentLine(ComputedStyle::initialTextIndentLine())
    , m_textIndentType(ComputedStyle::initialTextIndentLine())
    , m_lineBoxContain(ComputedStyle::initialLineBoxContain())
    , m_imageRendering(ComputedStyle::initialImageRendering())
    , m_textUnderlinePosition(ComputedStyle::initialTextUnderlinePosition())
    , m_rubyPosition(ComputedStyle::initialRubyPosition())
    , m_subtreeWillChangeContents(false)
    , m_selfOrAncestorHasDirAutoAttribute(false)
    , hyphenationLimitBefore(-1)
    , hyphenationLimitAfter(-1)
    , hyphenationLimitLines(-1)
    , tapHighlightColor(ComputedStyle::initialTapHighlightColor())
    , m_tabSize(ComputedStyle::initialTabSize())
{
}

StyleRareInheritedData::StyleRareInheritedData(const StyleRareInheritedData& o)
    : RefCounted<StyleRareInheritedData>()
    , listStyleImage(o.listStyleImage)
    , m_textStrokeColor(o.m_textStrokeColor)
    , textStrokeWidth(o.textStrokeWidth)
    , m_textFillColor(o.m_textFillColor)
    , m_textEmphasisColor(o.m_textEmphasisColor)
    , m_visitedLinkTextStrokeColor(o.m_visitedLinkTextStrokeColor)
    , m_visitedLinkTextFillColor(o.m_visitedLinkTextFillColor)
    , m_visitedLinkTextEmphasisColor(o.m_visitedLinkTextEmphasisColor)
    , textShadow(o.textShadow)
    , highlight(o.highlight)
    , cursorData(o.cursorData)
    , indent(o.indent)
    , m_effectiveZoom(o.m_effectiveZoom)
    , widows(o.widows)
    , orphans(o.orphans)
    , m_hasAutoOrphans(o.m_hasAutoOrphans)
    , m_textStrokeColorIsCurrentColor(o.m_textStrokeColorIsCurrentColor)
    , m_textFillColorIsCurrentColor(o.m_textFillColorIsCurrentColor)
    , m_textEmphasisColorIsCurrentColor(o.m_textEmphasisColorIsCurrentColor)
    , m_visitedLinkTextStrokeColorIsCurrentColor(o.m_visitedLinkTextStrokeColorIsCurrentColor)
    , m_visitedLinkTextFillColorIsCurrentColor(o.m_visitedLinkTextFillColorIsCurrentColor)
    , m_visitedLinkTextEmphasisColorIsCurrentColor(o.m_visitedLinkTextEmphasisColorIsCurrentColor)
    , textSecurity(o.textSecurity)
    , userModify(o.userModify)
    , wordBreak(o.wordBreak)
    , overflowWrap(o.overflowWrap)
    , lineBreak(o.lineBreak)
    , userSelect(o.userSelect)
    , speak(o.speak)
    , hyphens(o.hyphens)
    , textEmphasisFill(o.textEmphasisFill)
    , textEmphasisMark(o.textEmphasisMark)
    , textEmphasisPosition(o.textEmphasisPosition)
    , m_textAlignLast(o.m_textAlignLast)
    , m_textJustify(o.m_textJustify)
    , m_textOrientation(o.m_textOrientation)
    , m_textCombine(o.m_textCombine)
    , m_textIndentLine(o.m_textIndentLine)
    , m_textIndentType(o.m_textIndentType)
    , m_lineBoxContain(o.m_lineBoxContain)
    , m_imageRendering(o.m_imageRendering)
    , m_textUnderlinePosition(o.m_textUnderlinePosition)
    , m_rubyPosition(o.m_rubyPosition)
    , m_subtreeWillChangeContents(o.m_subtreeWillChangeContents)
    , m_selfOrAncestorHasDirAutoAttribute(o.m_selfOrAncestorHasDirAutoAttribute)
    , hyphenationString(o.hyphenationString)
    , hyphenationLimitBefore(o.hyphenationLimitBefore)
    , hyphenationLimitAfter(o.hyphenationLimitAfter)
    , hyphenationLimitLines(o.hyphenationLimitLines)
    , locale(o.locale)
    , textEmphasisCustomMark(o.textEmphasisCustomMark)
    , tapHighlightColor(o.tapHighlightColor)
    , appliedTextDecorations(o.appliedTextDecorations)
    , m_tabSize(o.m_tabSize)
    , variables(o.variables)
{
}

StyleRareInheritedData::~StyleRareInheritedData()
{
}

bool StyleRareInheritedData::operator==(const StyleRareInheritedData& o) const
{
    return m_textStrokeColor == o.m_textStrokeColor
        && textStrokeWidth == o.textStrokeWidth
        && m_textFillColor == o.m_textFillColor
        && m_textEmphasisColor == o.m_textEmphasisColor
        && m_visitedLinkTextStrokeColor == o.m_visitedLinkTextStrokeColor
        && m_visitedLinkTextFillColor == o.m_visitedLinkTextFillColor
        && m_visitedLinkTextEmphasisColor == o.m_visitedLinkTextEmphasisColor
        && tapHighlightColor == o.tapHighlightColor
        && shadowDataEquivalent(o)
        && highlight == o.highlight
        && dataEquivalent(cursorData.get(), o.cursorData.get())
        && indent == o.indent
        && m_effectiveZoom == o.m_effectiveZoom
        && widows == o.widows
        && orphans == o.orphans
        && m_hasAutoOrphans == o.m_hasAutoOrphans
        && m_textStrokeColorIsCurrentColor == o.m_textStrokeColorIsCurrentColor
        && m_textFillColorIsCurrentColor == o.m_textFillColorIsCurrentColor
        && m_textEmphasisColorIsCurrentColor == o.m_textEmphasisColorIsCurrentColor
        && m_visitedLinkTextStrokeColorIsCurrentColor == o.m_visitedLinkTextStrokeColorIsCurrentColor
        && m_visitedLinkTextFillColorIsCurrentColor == o.m_visitedLinkTextFillColorIsCurrentColor
        && m_visitedLinkTextEmphasisColorIsCurrentColor == o.m_visitedLinkTextEmphasisColorIsCurrentColor
        && textSecurity == o.textSecurity
        && userModify == o.userModify
        && wordBreak == o.wordBreak
        && overflowWrap == o.overflowWrap
        && lineBreak == o.lineBreak
        && userSelect == o.userSelect
        && speak == o.speak
        && hyphens == o.hyphens
        && hyphenationLimitBefore == o.hyphenationLimitBefore
        && hyphenationLimitAfter == o.hyphenationLimitAfter
        && hyphenationLimitLines == o.hyphenationLimitLines
        && textEmphasisFill == o.textEmphasisFill
        && textEmphasisMark == o.textEmphasisMark
        && textEmphasisPosition == o.textEmphasisPosition
        && m_textAlignLast == o.m_textAlignLast
        && m_textJustify == o.m_textJustify
        && m_textOrientation == o.m_textOrientation
        && m_textCombine == o.m_textCombine
        && m_textIndentLine == o.m_textIndentLine
        && m_textIndentType == o.m_textIndentType
        && m_lineBoxContain == o.m_lineBoxContain
        && m_subtreeWillChangeContents == o.m_subtreeWillChangeContents
        && m_selfOrAncestorHasDirAutoAttribute == o.m_selfOrAncestorHasDirAutoAttribute
        && hyphenationString == o.hyphenationString
        && locale == o.locale
        && textEmphasisCustomMark == o.textEmphasisCustomMark
        && quotesDataEquivalent(o)
        && m_tabSize == o.m_tabSize
        && m_imageRendering == o.m_imageRendering
        && m_textUnderlinePosition == o.m_textUnderlinePosition
        && m_rubyPosition == o.m_rubyPosition
        && dataEquivalent(listStyleImage.get(), o.listStyleImage.get())
        && dataEquivalent(appliedTextDecorations, o.appliedTextDecorations)
        && variables == o.variables;
}

bool StyleRareInheritedData::shadowDataEquivalent(const StyleRareInheritedData& o) const
{
    return dataEquivalent(textShadow.get(), o.textShadow.get());
}

bool StyleRareInheritedData::quotesDataEquivalent(const StyleRareInheritedData& o) const
{
    return dataEquivalent(quotes, o.quotes);
}

} // namespace blink
