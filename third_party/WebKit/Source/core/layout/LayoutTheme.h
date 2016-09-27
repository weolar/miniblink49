/*
 * This file is part of the theme implementation for form controls in WebCore.
 *
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Computer, Inc.
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

#ifndef LayoutTheme_h
#define LayoutTheme_h

#include "core/CoreExport.h"
#include "core/layout/LayoutObject.h"
#include "platform/ThemeTypes.h"
#include "platform/scroll/ScrollTypes.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class AuthorStyleInfo;
class ComputedStyle;
class Element;
class FileList;
class HTMLInputElement;
class LayoutMeter;
class Theme;
class ThemePainter;

class CORE_EXPORT LayoutTheme : public RefCounted<LayoutTheme> {
protected:
    LayoutTheme();

public:
    virtual ~LayoutTheme() { }

    // This function is to be implemented in your platform-specific theme implementation to hand back the
    // appropriate platform theme.
    static LayoutTheme& theme();

    virtual ThemePainter& painter() = 0;

    static void setSizeIfAuto(ComputedStyle&, const IntSize&);

    // This method is called whenever style has been computed for an element and the appearance
    // property has been set to a value other than "none".  The theme should map in all of the appropriate
    // metrics and defaults given the contents of the style.  This includes sophisticated operations like
    // selection of control size based off the font, the disabling of appearance when certain other properties like
    // "border" are set, or if the appearance is not supported by the theme.
    void adjustStyle(ComputedStyle&, Element*, const AuthorStyleInfo&);

    // The remaining methods should be implemented by the platform-specific portion of the theme, e.g.,
    // LayoutThemeMac.cpp for Mac OS X.

    // These methods return the theme's extra style sheets rules, to let each platform
    // adjust the default CSS rules in html.css, quirks.css or mediaControls.css.
    virtual String extraDefaultStyleSheet();
    virtual String extraQuirksStyleSheet() { return String(); }
    virtual String extraMediaControlsStyleSheet() { return String(); }
    virtual String extraFullScreenStyleSheet() { return String(); }

    // A method to obtain the baseline position for a "leaf" control.  This will only be used if a baseline
    // position cannot be determined by examining child content. Checkboxes and radio buttons are examples of
    // controls that need to do this.
    virtual int baselinePosition(const LayoutObject*) const;

    // A method for asking if a control is a container or not.  Leaf controls have to have some special behavior (like
    // the baseline position API above).
    bool isControlContainer(ControlPart) const;

    // Whether or not the control has been styled enough by the author to disable the native appearance.
    virtual bool isControlStyled(const ComputedStyle&, const AuthorStyleInfo&) const;

    // Some controls may spill out of their containers (e.g., the check on an OSX 10.9 checkbox). Add this
    // "visual overflow" to the object's border box rect.
    virtual void addVisualOverflow(const LayoutObject&, IntRect& borderBox);

    // This method is called whenever a control state changes on a particular themed object, e.g., the mouse becomes pressed
    // or a control becomes disabled. The ControlState parameter indicates which state has changed (from having to not having,
    // or vice versa).
    bool controlStateChanged(LayoutObject&, ControlState) const;

    bool shouldDrawDefaultFocusRing(LayoutObject*) const;

    // A method asking if the theme's controls actually care about redrawing when hovered.
    virtual bool supportsHover(const ComputedStyle&) const { return false; }

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    // A method asking if the platform is able to show a calendar picker for a given input type.
    virtual bool supportsCalendarPicker(const AtomicString&) const;
#endif

    // Text selection colors.
    Color activeSelectionBackgroundColor() const;
    Color inactiveSelectionBackgroundColor() const;
    Color activeSelectionForegroundColor() const;
    Color inactiveSelectionForegroundColor() const;

    // List box selection colors
    Color activeListBoxSelectionBackgroundColor() const;
    Color activeListBoxSelectionForegroundColor() const;
    Color inactiveListBoxSelectionBackgroundColor() const;
    Color inactiveListBoxSelectionForegroundColor() const;

    // Highlighting colors for TextMatches.
    virtual Color platformActiveTextSearchHighlightColor() const;
    virtual Color platformInactiveTextSearchHighlightColor() const;

    Color focusRingColor() const;
    virtual Color platformFocusRingColor() const { return Color(0, 0, 0); }
    void setCustomFocusRingColor(const Color&);
    static Color tapHighlightColor();
    virtual Color platformTapHighlightColor() const { return LayoutTheme::defaultTapHighlightColor; }
    virtual Color platformDefaultCompositionBackgroundColor() const { return defaultCompositionBackgroundColor; }
    virtual void platformColorsDidChange();

    virtual double caretBlinkInterval() const { return 0.5; }

    // System fonts and colors for CSS.
    virtual void systemFont(CSSValueID systemFontID, FontStyle&, FontWeight&, float& fontSize, AtomicString& fontFamily) const = 0;
    void systemFont(CSSValueID systemFontID, FontDescription&);
    virtual Color systemColor(CSSValueID) const;

    virtual int minimumMenuListSize(const ComputedStyle&) const { return 0; }

    virtual void adjustSliderThumbSize(ComputedStyle&, Element*) const;

    virtual int popupInternalPaddingLeft(const ComputedStyle&) const { return 0; }
    virtual int popupInternalPaddingRight(const ComputedStyle&) const { return 0; }
    virtual int popupInternalPaddingTop(const ComputedStyle&) const { return 0; }
    virtual int popupInternalPaddingBottom(const ComputedStyle&) const { return 0; }
    virtual bool popupOptionSupportsTextIndent() const { return false; }

    virtual ScrollbarControlSize scrollbarControlSizeForPart(ControlPart) { return RegularScrollbar; }

    // Returns the repeat interval of the animation for the progress bar.
    virtual double animationRepeatIntervalForProgressBar() const;
    // Returns the duration of the animation for the progress bar.
    virtual double animationDurationForProgressBar() const;

    // Media controls
    String formatMediaControlsTime(float time) const;
    String formatMediaControlsCurrentTime(float currentTime, float duration) const;

    virtual IntSize meterSizeForBounds(const LayoutMeter*, const IntRect&) const;
    virtual bool supportsMeter(ControlPart) const;

    // Returns size of one slider tick mark for a horizontal track.
    // For vertical tracks we rotate it and use it. i.e. Width is always length along the track.
    virtual IntSize sliderTickSize() const = 0;
    // Returns the distance of slider tick origin from the slider track center.
    virtual int sliderTickOffsetFromTrackCenter() const = 0;

    virtual bool shouldShowPlaceholderWhenFocused() const { return false; }
    virtual bool shouldHaveSpinButton(HTMLInputElement*) const;

    // Functions for <select> elements.
    virtual bool delegatesMenuListRendering() const { return false; }
    virtual bool popsMenuByArrowKeys() const { return false; }
    virtual bool popsMenuBySpaceKey() const { return false; }
    virtual bool popsMenuByReturnKey() const { return false; }
    virtual bool popsMenuByAltDownUpOrF4Key() const { return false; }

    virtual String fileListNameForWidth(Locale&, const FileList*, const Font&, int width) const;

    virtual bool shouldOpenPickerWithF4Key() const;

    virtual bool supportsSelectionForegroundColors() const { return true; }

    virtual bool isModalColorChooser() const { return true; }

    virtual bool shouldUseFallbackTheme(const ComputedStyle&) const;

protected:
    // The platform selection color.
    virtual Color platformActiveSelectionBackgroundColor() const;
    virtual Color platformInactiveSelectionBackgroundColor() const;
    virtual Color platformActiveSelectionForegroundColor() const;
    virtual Color platformInactiveSelectionForegroundColor() const;

    virtual Color platformActiveListBoxSelectionBackgroundColor() const;
    virtual Color platformInactiveListBoxSelectionBackgroundColor() const;
    virtual Color platformActiveListBoxSelectionForegroundColor() const;
    virtual Color platformInactiveListBoxSelectionForegroundColor() const;

    // A method asking if the theme is able to draw the focus ring.
    virtual bool supportsFocusRing(const ComputedStyle&) const;

#if !USE(NEW_THEME)
    // Methods for each appearance value.
    virtual void adjustCheckboxStyle(ComputedStyle&, Element*) const;
    virtual void setCheckboxSize(ComputedStyle&) const { }

    virtual void adjustRadioStyle(ComputedStyle&, Element*) const;
    virtual void setRadioSize(ComputedStyle&) const { }

    virtual void adjustButtonStyle(ComputedStyle&, Element*) const;
    virtual void adjustInnerSpinButtonStyle(ComputedStyle&, Element*) const;
#endif

    virtual void adjustMenuListStyle(ComputedStyle&, Element*) const;
    virtual void adjustMenuListButtonStyle(ComputedStyle&, Element*) const;
    virtual void adjustSliderThumbStyle(ComputedStyle&, Element*) const;
    virtual void adjustSearchFieldStyle(ComputedStyle&, Element*) const;
    virtual void adjustSearchFieldCancelButtonStyle(ComputedStyle&, Element*) const;
    virtual void adjustSearchFieldDecorationStyle(ComputedStyle&, Element*) const;
    virtual void adjustSearchFieldResultsDecorationStyle(ComputedStyle&, Element*) const;
    void adjustStyleUsingFallbackTheme(ComputedStyle&, Element*);
    void adjustCheckboxStyleUsingFallbackTheme(ComputedStyle&, Element*) const;
    void adjustRadioStyleUsingFallbackTheme(ComputedStyle&, Element*) const;

public:
    // Methods for state querying
    static ControlStates controlStatesForLayoutObject(const LayoutObject*);
    static bool isActive(const LayoutObject*);
    static bool isChecked(const LayoutObject*);
    static bool isIndeterminate(const LayoutObject*);
    static bool isEnabled(const LayoutObject*);
    static bool isFocused(const LayoutObject*);
    static bool isPressed(const LayoutObject*);
    static bool isSpinUpButtonPartPressed(const LayoutObject*);
    static bool isHovered(const LayoutObject*);
    static bool isSpinUpButtonPartHovered(const LayoutObject*);
    static bool isReadOnlyControl(const LayoutObject*);

private:
    Color m_customFocusRingColor;
    bool m_hasCustomFocusRingColor;

    // This color is expected to be drawn on a semi-transparent overlay,
    // making it more transparent than its alpha value indicates.
    static const RGBA32 defaultTapHighlightColor = 0x66000000;

    static const RGBA32 defaultCompositionBackgroundColor = 0xFFFFDD55;

#if USE(NEW_THEME)
    Theme* m_platformTheme; // The platform-specific theme.
#endif
};

} // namespace blink

#endif // LayoutTheme_h
