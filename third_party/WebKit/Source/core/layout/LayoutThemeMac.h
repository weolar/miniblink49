/*
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2005 Apple Computer, Inc.
 * Copyright (C) 2008, 2009 Google, Inc.
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

#ifndef LayoutThemeMac_h
#define LayoutThemeMac_h

#import "core/layout/LayoutTheme.h"
#import "core/paint/ThemePainterMac.h"
#import "wtf/HashMap.h"
#import "wtf/RetainPtr.h"

OBJC_CLASS LayoutThemeNotificationObserver;

namespace blink {

class LayoutThemeMac final : public LayoutTheme {
public:
    static PassRefPtr<LayoutTheme> create();

    virtual void addVisualOverflow(const LayoutObject&, IntRect& borderBox) override;

    virtual bool isControlStyled(const ComputedStyle&, const AuthorStyleInfo&) const override;

    virtual Color platformActiveSelectionBackgroundColor() const override;
    virtual Color platformInactiveSelectionBackgroundColor() const override;
    virtual Color platformActiveSelectionForegroundColor() const override;
    virtual Color platformActiveListBoxSelectionBackgroundColor() const override;
    virtual Color platformActiveListBoxSelectionForegroundColor() const override;
    virtual Color platformInactiveListBoxSelectionBackgroundColor() const override;
    virtual Color platformInactiveListBoxSelectionForegroundColor() const override;
    virtual Color platformFocusRingColor() const override;

    virtual ScrollbarControlSize scrollbarControlSizeForPart(ControlPart part) override { return part == ListboxPart ? SmallScrollbar : RegularScrollbar; }

    virtual void platformColorsDidChange() override;

    // System fonts.
    virtual void systemFont(CSSValueID systemFontID, FontStyle&, FontWeight&, float& fontSize, AtomicString& fontFamily) const override;

    virtual int minimumMenuListSize(const ComputedStyle&) const override;

    virtual void adjustSliderThumbSize(ComputedStyle&, Element*) const override;

    virtual IntSize sliderTickSize() const override;
    virtual int sliderTickOffsetFromTrackCenter() const override;

    virtual int popupInternalPaddingLeft(const ComputedStyle&) const override;
    virtual int popupInternalPaddingRight(const ComputedStyle&) const override;
    virtual int popupInternalPaddingTop(const ComputedStyle&) const override;
    virtual int popupInternalPaddingBottom(const ComputedStyle&) const override;

    virtual bool popsMenuByArrowKeys() const override { return true; }
    virtual bool popsMenuBySpaceKey() const override final { return true; }

    virtual IntSize meterSizeForBounds(const LayoutMeter*, const IntRect&) const override;
    virtual bool supportsMeter(ControlPart) const override;

    // Returns the repeat interval of the animation for the progress bar.
    virtual double animationRepeatIntervalForProgressBar() const override;
    // Returns the duration of the animation for the progress bar.
    virtual double animationDurationForProgressBar() const override;

    virtual Color systemColor(CSSValueID) const override;

    virtual bool supportsSelectionForegroundColors() const override { return false; }

    virtual bool isModalColorChooser() const { return false; }

protected:
    LayoutThemeMac();
    virtual ~LayoutThemeMac();

    virtual void adjustMenuListStyle(ComputedStyle&, Element*) const override;
    virtual void adjustMenuListButtonStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldCancelButtonStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldDecorationStyle(ComputedStyle&, Element*) const override;
    virtual void adjustSearchFieldResultsDecorationStyle(ComputedStyle&, Element*) const override;

public:
    // Constants and methods shared with ThemePainterMac

    // Get the control size based off the font. Used by some of the controls (like buttons).
    NSControlSize controlSizeForFont(const ComputedStyle&) const;
    NSControlSize controlSizeForSystemFont(const ComputedStyle&) const;
    void setControlSize(NSCell*, const IntSize* sizes, const IntSize& minSize, float zoomLevel = 1.0f);
    void setSizeFromFont(ComputedStyle&, const IntSize* sizes) const;
    IntSize sizeForFont(const ComputedStyle&, const IntSize* sizes) const;
    IntSize sizeForSystemFont(const ComputedStyle&, const IntSize* sizes) const;
    void setFontFromControlSize(ComputedStyle&, NSControlSize) const;

    void updateCheckedState(NSCell*, const LayoutObject*);
    void updateEnabledState(NSCell*, const LayoutObject*);
    void updateFocusedState(NSCell*, const LayoutObject*);
    void updatePressedState(NSCell*, const LayoutObject*);

    // Helpers for adjusting appearance and for painting

    void setPopupButtonCellState(const LayoutObject*, const IntRect&);
    const IntSize* popupButtonSizes() const;
    const int* popupButtonMargins() const;
    const int* popupButtonPadding(NSControlSize) const;
    const IntSize* menuListSizes() const;

    const IntSize* searchFieldSizes() const;
    const IntSize* cancelButtonSizes() const;
    const IntSize* resultsButtonSizes() const;
    void setSearchCellState(LayoutObject*, const IntRect&);
    void setSearchFieldSize(ComputedStyle&) const;

    NSPopUpButtonCell* popupButton() const;
    NSSearchFieldCell* search() const;
    NSTextFieldCell* textField() const;

    NSLevelIndicatorStyle levelIndicatorStyleFor(ControlPart) const;
    NSLevelIndicatorCell* levelIndicatorFor(const LayoutMeter*) const;

    // A view associated to the contained document. Subclasses may not have such a view and return a fake.
    NSView* documentViewFor(LayoutObject*) const;

    int minimumProgressBarHeight(const ComputedStyle&) const;
    const IntSize* progressBarSizes() const;
    const int* progressBarMargins(NSControlSize) const;

    void updateActiveState(NSCell*, const LayoutObject*);

    // We estimate the animation rate of a Mac OS X progress bar is 33 fps.
    // Hard code the value here because we haven't found API for it.
    static constexpr double progressAnimationFrameRate = 0.033;
    // Mac OS X progress bar animation seems to have 256 frames.
    static constexpr double progressAnimationNumFrames = 256;

    static constexpr float baseFontSize = 11.0f;
    static constexpr float menuListBaseArrowHeight = 4.0f;
    static constexpr float menuListBaseArrowWidth = 5.0f;
    static constexpr float menuListBaseSpaceBetweenArrows = 2.0f;
    static const int menuListArrowPaddingLeft = 6;
    static const int menuListArrowPaddingRight = 6;
    static const int sliderThumbWidth = 15;
    static const int sliderThumbHeight = 15;
    static const int sliderThumbShadowBlur = 1;
    static const int sliderThumbBorderWidth = 1;
    static const int sliderTrackWidth = 5;
    static const int sliderTrackBorderWidth = 1;

protected:
    void adjustMediaSliderThumbSize(ComputedStyle&) const;
    virtual String extraFullScreenStyleSheet() override;

    // Controls color values returned from platformFocusRingColor(). systemColor() will be used when false.
    bool usesTestModeFocusRingColor() const;

    virtual bool shouldUseFallbackTheme(const ComputedStyle&) const override;

private:
    virtual String fileListNameForWidth(Locale&, const FileList*, const Font&, int width) const override;
    virtual String extraDefaultStyleSheet() override;
    virtual bool shouldShowPlaceholderWhenFocused() const override;

    virtual ThemePainter& painter() override { return m_painter; }

    mutable RetainPtr<NSPopUpButtonCell> m_popupButton;
    mutable RetainPtr<NSSearchFieldCell> m_search;
    mutable RetainPtr<NSMenu> m_searchMenuTemplate;
    mutable RetainPtr<NSLevelIndicatorCell> m_levelIndicator;
    mutable RetainPtr<NSTextFieldCell> m_textField;

    mutable HashMap<int, RGBA32> m_systemColorCache;

    RetainPtr<LayoutThemeNotificationObserver> m_notificationObserver;

    ThemePainterMac m_painter;
};

} // namespace blink

#endif // LayoutThemeMac_h
