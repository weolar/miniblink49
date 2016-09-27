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

#ifndef ThemePainter_h
#define ThemePainter_h

#include "platform/ThemeTypes.h"

namespace blink {

class IntRect;
class LayoutObject;

struct PaintInfo;

class ThemePainter {
public:
    // This method is called to paint the widget as a background of the LayoutObject.  A widget's foreground, e.g., the
    // text of a button, is always rendered by the engine itself.  The boolean return value indicates
    // whether the CSS border/background should also be painted.
    bool paint(LayoutObject*, const PaintInfo&, const IntRect&);
    bool paintBorderOnly(LayoutObject*, const PaintInfo&, const IntRect&);
    bool paintDecorations(LayoutObject*, const PaintInfo&, const IntRect&);

    virtual bool paintCapsLockIndicator(LayoutObject*, const PaintInfo&, const IntRect&) { return 0; }
    void paintSliderTicks(LayoutObject*, const PaintInfo&, const IntRect&);

protected:
#if !USE(NEW_THEME)
    virtual bool paintCheckbox(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintRadio(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintButton(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintInnerSpinButton(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
#endif

    virtual bool paintTextField(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintTextArea(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintMenuList(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintMenuListButton(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintMeter(LayoutObject*, const PaintInfo&, const IntRect&);
    virtual bool paintProgressBar(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintSliderTrack(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintSliderThumb(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintSearchField(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintSearchFieldCancelButton(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintSearchFieldDecoration(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }
    virtual bool paintSearchFieldResultsDecoration(LayoutObject*, const PaintInfo&, const IntRect&) { return true; }

    bool paintUsingFallbackTheme(LayoutObject*, const PaintInfo&, const IntRect&);
    bool paintCheckboxUsingFallbackTheme(LayoutObject*, const PaintInfo&, const IntRect&);
    bool paintRadioUsingFallbackTheme(LayoutObject*, const PaintInfo&, const IntRect&);
};

} // namespace blink

#endif // ThemePainter_h
