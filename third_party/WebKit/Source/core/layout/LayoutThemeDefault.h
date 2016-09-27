/*
 * This file is part of the WebKit project.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2006 Michael Emmel mike.emmel@gmail.com
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2008, 2009 Google, Inc.
 * All rights reserved.
 * Copyright (C) 2009 Kenneth Rohde Christiansen
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

#ifndef LayoutThemeDefault_h
#define LayoutThemeDefault_h

#include "core/CoreExport.h"
#include "core/layout/LayoutTheme.h"
#include "core/paint/ThemePainterDefault.h"

namespace blink {

class LayoutProgress;

class CORE_EXPORT LayoutThemeDefault : public LayoutTheme {
public:
    virtual String extraDefaultStyleSheet() override;
    virtual String extraQuirksStyleSheet() override;

    virtual Color systemColor(CSSValueID) const override;

    virtual bool supportsFocusRing(const ComputedStyle&) const override;

    // List Box selection color
    virtual Color activeListBoxSelectionBackgroundColor() const;
    virtual Color activeListBoxSelectionForegroundColor() const;
    virtual Color inactiveListBoxSelectionBackgroundColor() const;
    virtual Color inactiveListBoxSelectionForegroundColor() const;

    virtual Color platformActiveSelectionBackgroundColor() const override;
    virtual Color platformInactiveSelectionBackgroundColor() const override;
    virtual Color platformActiveSelectionForegroundColor() const override;
    virtual Color platformInactiveSelectionForegroundColor() const override;

    virtual IntSize sliderTickSize() const override;
    virtual int sliderTickOffsetFromTrackCenter() const override;
    virtual void adjustSliderThumbSize(ComputedStyle&, Element*) const override;

    static void setCaretBlinkInterval(double);

    virtual void setCheckboxSize(ComputedStyle&) const override;
    virtual void setRadioSize(ComputedStyle&) const override;
    virtual void adjustInnerSpinButtonStyle(ComputedStyle&, Element*) const override;

    virtual bool popsMenuBySpaceKey() const override final { return true; }
    virtual bool popsMenuByReturnKey() const override final { return true; }
    virtual bool popsMenuByAltDownUpOrF4Key() const override { return true; }

    virtual bool shouldOpenPickerWithF4Key() const override;

    virtual Color platformTapHighlightColor() const override
    {
        return Color(defaultTapHighlightColor);
    }

    // A method asking if the theme's controls actually care about redrawing
    // when hovered.
    virtual bool supportsHover(const ComputedStyle&) const override final;

    virtual Color platformFocusRingColor() const override;

    virtual double caretBlinkInterval() const final;

    // System fonts.
    virtual void systemFont(CSSValueID systemFontID, FontStyle&, FontWeight&, float& fontSize, AtomicString& fontFamily) const;

    virtual int minimumMenuListSize(const ComputedStyle&) const override;

    virtual void adjustButtonStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldCancelButtonStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldDecorationStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldResultsDecorationStyle(ComputedStyle&, Element*) const override;

    // MenuList refers to an unstyled menulist (meaning a menulist without
    // background-color or border set) and MenuListButton refers to a styled
    // menulist (a menulist with background-color or border set). They have
    // this distinction to support showing aqua style themes whenever they
    // possibly can, which is something we don't want to replicate.
    //
    // In short, we either go down the MenuList code path or the MenuListButton
    // codepath. We never go down both. And in both cases, they layout the
    // entire menulist.
    virtual void adjustMenuListStyle(ComputedStyle&, Element*) const override;
    virtual void adjustMenuListButtonStyle(ComputedStyle&, Element*) const override;

    virtual double animationRepeatIntervalForProgressBar() const override;
    virtual double animationDurationForProgressBar() const override;

    // These methods define the padding for the MenuList's inner block.
    virtual int popupInternalPaddingLeft(const ComputedStyle&) const override;
    virtual int popupInternalPaddingRight(const ComputedStyle&) const override;
    virtual int popupInternalPaddingTop(const ComputedStyle&) const override;
    virtual int popupInternalPaddingBottom(const ComputedStyle&) const override;

    // Provide a way to pass the default font size from the Settings object
    // to the layout theme. FIXME: http://b/1129186 A cleaner way would be
    // to remove the default font size from this object and have callers
    // that need the value to get it directly from the appropriate Settings
    // object.
    static void setDefaultFontSize(int);

    static void setSelectionColors(unsigned activeBackgroundColor, unsigned activeForegroundColor, unsigned inactiveBackgroundColor, unsigned inactiveForegroundColor);

protected:
    LayoutThemeDefault();
    virtual ~LayoutThemeDefault();
    virtual bool shouldUseFallbackTheme(const ComputedStyle&) const override;
    virtual int menuListArrowPadding() const;

    IntRect determinateProgressValueRectFor(LayoutProgress*, const IntRect&) const;
    IntRect indeterminateProgressValueRectFor(LayoutProgress*, const IntRect&) const;

private:
    virtual bool shouldShowPlaceholderWhenFocused() const override;

    virtual ThemePainter& painter() override { return m_painter; }

    int menuListInternalPadding(const ComputedStyle&, int paddingType) const;

    static const RGBA32 defaultTapHighlightColor = 0x2e000000; // 18% black.
    static double m_caretBlinkInterval;

    static unsigned m_activeSelectionBackgroundColor;
    static unsigned m_activeSelectionForegroundColor;
    static unsigned m_inactiveSelectionBackgroundColor;
    static unsigned m_inactiveSelectionForegroundColor;

    ThemePainterDefault m_painter;
};

} // namespace blink

#endif // LayoutThemeDefault_h
