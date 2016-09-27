/*
 * Copyright (C) 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
 */

#import "config.h"
#import "core/layout/LayoutThemeMac.h"

#import "core/CSSValueKeywords.h"
#import "core/HTMLNames.h"
#import "core/fileapi/FileList.h"
#import "core/html/HTMLMeterElement.h"
#import "core/layout/LayoutMeter.h"
#import "core/layout/LayoutProgress.h"
#import "core/layout/LayoutView.h"
#import "core/paint/MediaControlsPainter.h"
#import "core/style/AuthorStyleInfo.h"
#import "core/style/ShadowList.h"
#import "platform/LayoutTestSupport.h"
#import "platform/PlatformResourceLoader.h"
#import "platform/graphics/BitmapImage.h"
#import "platform/mac/ColorMac.h"
#import "platform/mac/LocalCurrentGraphicsContext.h"
#import "platform/mac/ThemeMac.h"
#import "platform/mac/WebCoreNSCellExtras.h"
#import "platform/text/PlatformLocale.h"
#import "platform/text/StringTruncator.h"
#import <AvailabilityMacros.h>
#import <Carbon/Carbon.h>
#import <Cocoa/Cocoa.h>
#import <math.h>

// The methods in this file are specific to the Mac OS X platform.

@interface LayoutThemeNotificationObserver : NSObject
{
    blink::LayoutTheme *_theme;
}

- (id)initWithTheme:(blink::LayoutTheme *)theme;
- (void)systemColorsDidChange:(NSNotification *)notification;

@end

@implementation LayoutThemeNotificationObserver

- (id)initWithTheme:(blink::LayoutTheme *)theme
{
    if (!(self = [super init]))
        return nil;

    _theme = theme;
    return self;
}

- (void)systemColorsDidChange:(NSNotification *)unusedNotification
{
    ASSERT_UNUSED(unusedNotification, [[unusedNotification name] isEqualToString:NSSystemColorsDidChangeNotification]);
    _theme->platformColorsDidChange();
}

@end

@interface NSTextFieldCell (WKDetails)
- (CFDictionaryRef)_coreUIDrawOptionsWithFrame:(NSRect)cellFrame inView:(NSView *)controlView includeFocus:(BOOL)includeFocus;
@end


@interface WebCoreTextFieldCell : NSTextFieldCell
- (CFDictionaryRef)_coreUIDrawOptionsWithFrame:(NSRect)cellFrame inView:(NSView *)controlView includeFocus:(BOOL)includeFocus;
@end

@implementation WebCoreTextFieldCell
- (CFDictionaryRef)_coreUIDrawOptionsWithFrame:(NSRect)cellFrame inView:(NSView *)controlView includeFocus:(BOOL)includeFocus
{
    // FIXME: This is a post-Lion-only workaround for <rdar://problem/11385461>. When that bug is resolved, we should remove this code.
    CFMutableDictionaryRef coreUIDrawOptions = CFDictionaryCreateMutableCopy(NULL, 0, [super _coreUIDrawOptionsWithFrame:cellFrame inView:controlView includeFocus:includeFocus]);
    CFDictionarySetValue(coreUIDrawOptions, @"borders only", kCFBooleanTrue);
    return (CFDictionaryRef)[NSMakeCollectable(coreUIDrawOptions) autorelease];
}
@end

@interface RTCMFlippedView : NSView
{}

- (BOOL)isFlipped;
- (NSText *)currentEditor;

@end

@implementation RTCMFlippedView

- (BOOL)isFlipped {
    return [[NSGraphicsContext currentContext] isFlipped];
}

- (NSText *)currentEditor {
    return nil;
}

@end

namespace blink {

using namespace HTMLNames;

LayoutThemeMac::LayoutThemeMac()
    : m_notificationObserver(AdoptNS, [[LayoutThemeNotificationObserver alloc] initWithTheme:this])
    , m_painter(*this)
{
    [[NSNotificationCenter defaultCenter] addObserver:m_notificationObserver.get()
                                             selector:@selector(systemColorsDidChange:)
                                                 name:NSSystemColorsDidChangeNotification
                                               object:nil];
}

LayoutThemeMac::~LayoutThemeMac()
{
    [[NSNotificationCenter defaultCenter] removeObserver:m_notificationObserver.get()];
}

Color LayoutThemeMac::platformActiveSelectionBackgroundColor() const
{
    NSColor* color = [[NSColor selectedTextBackgroundColor] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    return Color(static_cast<int>(255.0 * [color redComponent]), static_cast<int>(255.0 * [color greenComponent]), static_cast<int>(255.0 * [color blueComponent]));
}

Color LayoutThemeMac::platformInactiveSelectionBackgroundColor() const
{
    NSColor* color = [[NSColor secondarySelectedControlColor] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    return Color(static_cast<int>(255.0 * [color redComponent]), static_cast<int>(255.0 * [color greenComponent]), static_cast<int>(255.0 * [color blueComponent]));
}

Color LayoutThemeMac::platformActiveSelectionForegroundColor() const
{
    return Color::black;
}

Color LayoutThemeMac::platformActiveListBoxSelectionBackgroundColor() const
{
    NSColor* color = [[NSColor alternateSelectedControlColor] colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    return Color(static_cast<int>(255.0 * [color redComponent]), static_cast<int>(255.0 * [color greenComponent]), static_cast<int>(255.0 * [color blueComponent]));
}

Color LayoutThemeMac::platformActiveListBoxSelectionForegroundColor() const
{
    return Color::white;
}

Color LayoutThemeMac::platformInactiveListBoxSelectionForegroundColor() const
{
    return Color::black;
}

Color LayoutThemeMac::platformFocusRingColor() const
{
    static const RGBA32 oldAquaFocusRingColor = 0xFF7DADD9;
    if (usesTestModeFocusRingColor())
        return oldAquaFocusRingColor;

    return systemColor(CSSValueWebkitFocusRingColor);
}

Color LayoutThemeMac::platformInactiveListBoxSelectionBackgroundColor() const
{
    return platformInactiveSelectionBackgroundColor();
}

static FontWeight toFontWeight(NSInteger appKitFontWeight)
{
    ASSERT(appKitFontWeight > 0 && appKitFontWeight < 15);
    if (appKitFontWeight > 14)
        appKitFontWeight = 14;
    else if (appKitFontWeight < 1)
        appKitFontWeight = 1;

    static FontWeight fontWeights[] = {
        FontWeight100,
        FontWeight100,
        FontWeight200,
        FontWeight300,
        FontWeight400,
        FontWeight500,
        FontWeight600,
        FontWeight600,
        FontWeight700,
        FontWeight800,
        FontWeight800,
        FontWeight900,
        FontWeight900,
        FontWeight900
    };
    return fontWeights[appKitFontWeight - 1];
}

static inline NSFont* systemNSFont(CSSValueID systemFontID)
{
    switch (systemFontID) {
    case CSSValueSmallCaption:
        return [NSFont systemFontOfSize:[NSFont smallSystemFontSize]];
    case CSSValueMenu:
        return [NSFont menuFontOfSize:[NSFont systemFontSize]];
    case CSSValueStatusBar:
        return [NSFont labelFontOfSize:[NSFont labelFontSize]];
    case CSSValueWebkitMiniControl:
        return [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSMiniControlSize]];
    case CSSValueWebkitSmallControl:
        return [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSSmallControlSize]];
    case CSSValueWebkitControl:
        return [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:NSRegularControlSize]];
    default:
        return [NSFont systemFontOfSize:[NSFont systemFontSize]];
    }
}

void LayoutThemeMac::systemFont(CSSValueID systemFontID, FontStyle& fontStyle, FontWeight& fontWeight, float& fontSize, AtomicString& fontFamily) const
{
    NSFont* font = systemNSFont(systemFontID);
    if (!font)
        return;

    NSFontManager *fontManager = [NSFontManager sharedFontManager];
    fontStyle = ([fontManager traitsOfFont:font] & NSItalicFontMask) ? FontStyleItalic : FontStyleNormal;
    fontWeight = toFontWeight([fontManager weightOfFont:font]);
    fontSize = [font pointSize];
    fontFamily = [font webCoreFamilyName];
}

static RGBA32 convertNSColorToColor(NSColor *color)
{
    NSColor *colorInColorSpace = [color colorUsingColorSpaceName:NSDeviceRGBColorSpace];
    if (colorInColorSpace) {
        static const double scaleFactor = nextafter(256.0, 0.0);
        return makeRGB(static_cast<int>(scaleFactor * [colorInColorSpace redComponent]),
            static_cast<int>(scaleFactor * [colorInColorSpace greenComponent]),
            static_cast<int>(scaleFactor * [colorInColorSpace blueComponent]));
    }

    // This conversion above can fail if the NSColor in question is an NSPatternColor
    // (as many system colors are). These colors are actually a repeating pattern
    // not just a solid color. To work around this we simply draw a 1x1 image of
    // the color and use that pixel's color. It might be better to use an average of
    // the colors in the pattern instead.
    NSBitmapImageRep *offscreenRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil
                                                                             pixelsWide:1
                                                                             pixelsHigh:1
                                                                          bitsPerSample:8
                                                                        samplesPerPixel:4
                                                                               hasAlpha:YES
                                                                               isPlanar:NO
                                                                         colorSpaceName:NSDeviceRGBColorSpace
                                                                            bytesPerRow:4
                                                                           bitsPerPixel:32];

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:[NSGraphicsContext graphicsContextWithBitmapImageRep:offscreenRep]];
    NSEraseRect(NSMakeRect(0, 0, 1, 1));
    [color drawSwatchInRect:NSMakeRect(0, 0, 1, 1)];
    [NSGraphicsContext restoreGraphicsState];

    NSUInteger pixel[4];
    [offscreenRep getPixel:pixel atX:0 y:0];

    [offscreenRep release];

    return makeRGB(pixel[0], pixel[1], pixel[2]);
}

static RGBA32 menuBackgroundColor()
{
    NSBitmapImageRep *offscreenRep = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nil
                                                                             pixelsWide:1
                                                                             pixelsHigh:1
                                                                          bitsPerSample:8
                                                                        samplesPerPixel:4
                                                                               hasAlpha:YES
                                                                               isPlanar:NO
                                                                         colorSpaceName:NSDeviceRGBColorSpace
                                                                            bytesPerRow:4
                                                                           bitsPerPixel:32];

    CGContextRef context = static_cast<CGContextRef>([[NSGraphicsContext graphicsContextWithBitmapImageRep:offscreenRep] graphicsPort]);
    CGRect rect = CGRectMake(0, 0, 1, 1);
    HIThemeMenuDrawInfo drawInfo;
    drawInfo.version =  0;
    drawInfo.menuType = kThemeMenuTypePopUp;
    HIThemeDrawMenuBackground(&rect, &drawInfo, context, kHIThemeOrientationInverted);

    NSUInteger pixel[4];
    [offscreenRep getPixel:pixel atX:0 y:0];

    [offscreenRep release];

    return makeRGB(pixel[0], pixel[1], pixel[2]);
}

void LayoutThemeMac::platformColorsDidChange()
{
    m_systemColorCache.clear();
    LayoutTheme::platformColorsDidChange();
}

Color LayoutThemeMac::systemColor(CSSValueID cssValueId) const
{
    {
        HashMap<int, RGBA32>::iterator it = m_systemColorCache.find(cssValueId);
        if (it != m_systemColorCache.end())
            return it->value;
    }

    Color color;
    bool needsFallback = false;
    switch (cssValueId) {
    case CSSValueActiveborder:
        color = convertNSColorToColor([NSColor keyboardFocusIndicatorColor]);
        break;
    case CSSValueActivecaption:
        color = convertNSColorToColor([NSColor windowFrameTextColor]);
        break;
    case CSSValueAppworkspace:
        color = convertNSColorToColor([NSColor headerColor]);
        break;
    case CSSValueBackground:
        // Use theme independent default
        needsFallback = true;
        break;
    case CSSValueButtonface:
        // We use this value instead of NSColor's controlColor to avoid website
        // incompatibilities. We may want to change this to use the NSColor in
        // future.
        color = 0xFFC0C0C0;
        break;
    case CSSValueButtonhighlight:
        color = convertNSColorToColor([NSColor controlHighlightColor]);
        break;
    case CSSValueButtonshadow:
        color = convertNSColorToColor([NSColor controlShadowColor]);
        break;
    case CSSValueButtontext:
        color = convertNSColorToColor([NSColor controlTextColor]);
        break;
    case CSSValueCaptiontext:
        color = convertNSColorToColor([NSColor textColor]);
        break;
    case CSSValueGraytext:
        color = convertNSColorToColor([NSColor disabledControlTextColor]);
        break;
    case CSSValueHighlight:
        color = convertNSColorToColor([NSColor selectedTextBackgroundColor]);
        break;
    case CSSValueHighlighttext:
        color = convertNSColorToColor([NSColor selectedTextColor]);
        break;
    case CSSValueInactiveborder:
        color = convertNSColorToColor([NSColor controlBackgroundColor]);
        break;
    case CSSValueInactivecaption:
        color = convertNSColorToColor([NSColor controlBackgroundColor]);
        break;
    case CSSValueInactivecaptiontext:
        color = convertNSColorToColor([NSColor textColor]);
        break;
    case CSSValueInfobackground:
        // There is no corresponding NSColor for this so we use a hard coded
        // value.
        color = 0xFFFBFCC5;
        break;
    case CSSValueInfotext:
        color = convertNSColorToColor([NSColor textColor]);
        break;
    case CSSValueMenu:
        color = menuBackgroundColor();
        break;
    case CSSValueMenutext:
        color = convertNSColorToColor([NSColor selectedMenuItemTextColor]);
        break;
    case CSSValueScrollbar:
        color = convertNSColorToColor([NSColor scrollBarColor]);
        break;
    case CSSValueText:
        color = convertNSColorToColor([NSColor textColor]);
        break;
    case CSSValueThreeddarkshadow:
        color = convertNSColorToColor([NSColor controlDarkShadowColor]);
        break;
    case CSSValueThreedshadow:
        color = convertNSColorToColor([NSColor shadowColor]);
        break;
    case CSSValueThreedface:
        // We use this value instead of NSColor's controlColor to avoid website
        // incompatibilities. We may want to change this to use the NSColor in
        // future.
        color = 0xFFC0C0C0;
        break;
    case CSSValueThreedhighlight:
        color = convertNSColorToColor([NSColor highlightColor]);
        break;
    case CSSValueThreedlightshadow:
        color = convertNSColorToColor([NSColor controlLightHighlightColor]);
        break;
    case CSSValueWebkitFocusRingColor:
        color = convertNSColorToColor([NSColor keyboardFocusIndicatorColor]);
        break;
    case CSSValueWindow:
        color = convertNSColorToColor([NSColor windowBackgroundColor]);
        break;
    case CSSValueWindowframe:
        color = convertNSColorToColor([NSColor windowFrameColor]);
        break;
    case CSSValueWindowtext:
        color = convertNSColorToColor([NSColor windowFrameTextColor]);
        break;
    default:
        needsFallback = true;
        break;
    }

    if (needsFallback)
        color = LayoutTheme::systemColor(cssValueId);

    m_systemColorCache.set(cssValueId, color.rgb());

    return color;
}

bool LayoutThemeMac::isControlStyled(const ComputedStyle& style, const AuthorStyleInfo& authorStyle) const
{
    if (style.appearance() == TextFieldPart || style.appearance() == TextAreaPart)
        return authorStyle.specifiesBorder() || style.boxShadow();

    // FIXME: This is horrible, but there is not much else that can be done.
    // Menu lists cannot draw properly when scaled. They can't really draw
    // properly when transformed either. We can't detect the transform case at
    // style adjustment time so that will just have to stay broken.  We can
    // however detect that we're zooming. If zooming is in effect we treat it
    // like the control is styled.
    if (style.appearance() == MenulistPart && style.effectiveZoom() != 1.0f)
        return true;
    // FIXME: NSSearchFieldCell doesn't work well when scaled.
    if (style.appearance() == SearchFieldPart && style.effectiveZoom() != 1)
        return true;

    return LayoutTheme::isControlStyled(style, authorStyle);
}

void LayoutThemeMac::addVisualOverflow(const LayoutObject& object, IntRect& rect)
{
    ControlPart part = object.style()->appearance();

#if USE(NEW_THEME)
    switch (part) {
    case CheckboxPart:
    case RadioPart:
    case PushButtonPart:
    case SquareButtonPart:
    case ButtonPart:
    case InnerSpinButtonPart:
        return LayoutTheme::addVisualOverflow(object, rect);
    default:
        break;
    }
#endif

    float zoomLevel = object.style()->effectiveZoom();

    if (part == MenulistPart) {
        setPopupButtonCellState(&object, rect);
        IntSize size = popupButtonSizes()[[popupButton() controlSize]];
        size.setHeight(size.height() * zoomLevel);
        size.setWidth(rect.width());
        rect = ThemeMac::inflateRect(rect, size, popupButtonMargins(), zoomLevel);
    } else if (part == SliderThumbHorizontalPart || part == SliderThumbVerticalPart) {
        rect.setHeight(rect.height() + sliderThumbShadowBlur);
    }
}

void LayoutThemeMac::updateCheckedState(NSCell* cell, const LayoutObject* o)
{
    bool oldIndeterminate = [cell state] == NSMixedState;
    bool indeterminate = isIndeterminate(o);
    bool checked = isChecked(o);

    if (oldIndeterminate != indeterminate) {
        [cell setState:indeterminate ? NSMixedState : (checked ? NSOnState : NSOffState)];
        return;
    }

    bool oldChecked = [cell state] == NSOnState;
    if (checked != oldChecked)
        [cell setState:checked ? NSOnState : NSOffState];
}

void LayoutThemeMac::updateEnabledState(NSCell* cell, const LayoutObject* o)
{
    bool oldEnabled = [cell isEnabled];
    bool enabled = isEnabled(o);
    if (enabled != oldEnabled)
        [cell setEnabled:enabled];
}

void LayoutThemeMac::updateFocusedState(NSCell* cell, const LayoutObject* o)
{
    bool oldFocused = [cell showsFirstResponder];
    bool focused = isFocused(o) && o->style()->outlineStyleIsAuto();
    if (focused != oldFocused)
        [cell setShowsFirstResponder:focused];
}

void LayoutThemeMac::updatePressedState(NSCell* cell, const LayoutObject* o)
{
    bool oldPressed = [cell isHighlighted];
    bool pressed = o->node() && o->node()->active();
    if (pressed != oldPressed)
        [cell setHighlighted:pressed];
}

NSControlSize LayoutThemeMac::controlSizeForFont(const ComputedStyle& style) const
{
    int fontSize = style.fontSize();
    if (fontSize >= 16)
        return NSRegularControlSize;
    if (fontSize >= 11)
        return NSSmallControlSize;
    return NSMiniControlSize;
}

void LayoutThemeMac::setControlSize(NSCell* cell, const IntSize* sizes, const IntSize& minSize, float zoomLevel)
{
    NSControlSize size;
    if (minSize.width() >= static_cast<int>(sizes[NSRegularControlSize].width() * zoomLevel) &&
        minSize.height() >= static_cast<int>(sizes[NSRegularControlSize].height() * zoomLevel))
        size = NSRegularControlSize;
    else if (minSize.width() >= static_cast<int>(sizes[NSSmallControlSize].width() * zoomLevel) &&
             minSize.height() >= static_cast<int>(sizes[NSSmallControlSize].height() * zoomLevel))
        size = NSSmallControlSize;
    else
        size = NSMiniControlSize;
    // Only update if we have to, since AppKit does work even if the size is the
    // same.
    if (size != [cell controlSize])
        [cell setControlSize:size];
}

IntSize LayoutThemeMac::sizeForFont(const ComputedStyle& style, const IntSize* sizes) const
{
    if (style.effectiveZoom() != 1.0f) {
        IntSize result = sizes[controlSizeForFont(style)];
        return IntSize(result.width() * style.effectiveZoom(), result.height() * style.effectiveZoom());
    }
    return sizes[controlSizeForFont(style)];
}

IntSize LayoutThemeMac::sizeForSystemFont(const ComputedStyle& style, const IntSize* sizes) const
{
    if (style.effectiveZoom() != 1.0f) {
        IntSize result = sizes[controlSizeForSystemFont(style)];
        return IntSize(result.width() * style.effectiveZoom(), result.height() * style.effectiveZoom());
    }
    return sizes[controlSizeForSystemFont(style)];
}

void LayoutThemeMac::setSizeFromFont(ComputedStyle& style, const IntSize* sizes) const
{
    // FIXME: Check is flawed, since it doesn't take min-width/max-width into
    // account.
    IntSize size = sizeForFont(style, sizes);
    if (style.width().isIntrinsicOrAuto() && size.width() > 0)
        style.setWidth(Length(size.width(), Fixed));
    if (style.height().isAuto() && size.height() > 0)
        style.setHeight(Length(size.height(), Fixed));
}

void LayoutThemeMac::setFontFromControlSize(ComputedStyle& style, NSControlSize controlSize) const
{
    FontDescription fontDescription;
    fontDescription.setIsAbsoluteSize(true);
    fontDescription.setGenericFamily(FontDescription::SerifFamily);

    NSFont* font = [NSFont systemFontOfSize:[NSFont systemFontSizeForControlSize:controlSize]];
    fontDescription.firstFamily().setFamily([font webCoreFamilyName]);
    fontDescription.setComputedSize([font pointSize] * style.effectiveZoom());
    fontDescription.setSpecifiedSize([font pointSize] * style.effectiveZoom());

    // Reset line height.
    style.setLineHeight(ComputedStyle::initialLineHeight());

    // TODO(esprehn): The fontSelector manual management is buggy and error prone.
    FontSelector* fontSelector = style.font().fontSelector();
    if (style.setFontDescription(fontDescription))
        style.font().update(fontSelector);
}

NSControlSize LayoutThemeMac::controlSizeForSystemFont(const ComputedStyle& style) const
{
    float fontSize = style.fontSize();
    float zoomLevel = style.effectiveZoom();
    if (zoomLevel != 1)
        fontSize /= zoomLevel;
    if (fontSize >= [NSFont systemFontSizeForControlSize:NSRegularControlSize])
        return NSRegularControlSize;
    if (fontSize >= [NSFont systemFontSizeForControlSize:NSSmallControlSize])
        return NSSmallControlSize;
    return NSMiniControlSize;
}

const int* LayoutThemeMac::popupButtonMargins() const
{
    static const int margins[3][4] =
    {
        { 0, 3, 1, 3 },
        { 0, 3, 2, 3 },
        { 0, 1, 0, 1 }
    };
    return margins[[popupButton() controlSize]];
}

const IntSize* LayoutThemeMac::popupButtonSizes() const
{
    static const IntSize sizes[3] = { IntSize(0, 21), IntSize(0, 18), IntSize(0, 15) };
    return sizes;
}

const int* LayoutThemeMac::popupButtonPadding(NSControlSize size) const
{
    static const int padding[3][4] =
    {
        { 2, 26, 3, 8 },
        { 2, 23, 3, 8 },
        { 2, 22, 3, 10 }
    };
    return padding[size];
}

IntSize LayoutThemeMac::meterSizeForBounds(const LayoutMeter* layoutMeter, const IntRect& bounds) const
{
    if (NoControlPart == layoutMeter->style()->appearance())
        return bounds.size();

    NSLevelIndicatorCell* cell = levelIndicatorFor(layoutMeter);
    // Makes enough room for cell's intrinsic size.
    NSSize cellSize = [cell cellSizeForBounds:IntRect(IntPoint(), bounds.size())];
    return IntSize(bounds.width() < cellSize.width ? cellSize.width : bounds.width(),
                   bounds.height() < cellSize.height ? cellSize.height : bounds.height());
}

bool LayoutThemeMac::supportsMeter(ControlPart part) const
{
    switch (part) {
    case RelevancyLevelIndicatorPart:
    case DiscreteCapacityLevelIndicatorPart:
    case RatingLevelIndicatorPart:
    case MeterPart:
    case ContinuousCapacityLevelIndicatorPart:
        return true;
    default:
        return false;
    }
}

NSLevelIndicatorStyle LayoutThemeMac::levelIndicatorStyleFor(ControlPart part) const
{
    switch (part) {
    case RelevancyLevelIndicatorPart:
        return NSRelevancyLevelIndicatorStyle;
    case DiscreteCapacityLevelIndicatorPart:
        return NSDiscreteCapacityLevelIndicatorStyle;
    case RatingLevelIndicatorPart:
        return NSRatingLevelIndicatorStyle;
    case MeterPart:
    case ContinuousCapacityLevelIndicatorPart:
    default:
        return NSContinuousCapacityLevelIndicatorStyle;
    }
}

NSLevelIndicatorCell* LayoutThemeMac::levelIndicatorFor(const LayoutMeter* layoutMeter) const
{
    const ComputedStyle& style = layoutMeter->styleRef();
    ASSERT(style.appearance() != NoControlPart);

    if (!m_levelIndicator)
        m_levelIndicator.adoptNS([[NSLevelIndicatorCell alloc] initWithLevelIndicatorStyle:NSContinuousCapacityLevelIndicatorStyle]);
    NSLevelIndicatorCell* cell = m_levelIndicator.get();

    HTMLMeterElement* element = layoutMeter->meterElement();
    double value = element->value();

    // Because NSLevelIndicatorCell does not support optimum-in-the-middle type
    // coloring, we explicitly control the color instead giving low and high
    // value to NSLevelIndicatorCell as is.
    switch (element->gaugeRegion()) {
    case HTMLMeterElement::GaugeRegionOptimum:
        // Make meter the green.
        [cell setWarningValue:value + 1];
        [cell setCriticalValue:value + 2];
        break;
    case HTMLMeterElement::GaugeRegionSuboptimal:
        // Make the meter yellow.
        [cell setWarningValue:value - 1];
        [cell setCriticalValue:value + 1];
        break;
    case HTMLMeterElement::GaugeRegionEvenLessGood:
        // Make the meter red.
        [cell setWarningValue:value - 2];
        [cell setCriticalValue:value - 1];
        break;
    }

    [cell setLevelIndicatorStyle:levelIndicatorStyleFor(style.appearance())];
    [cell setBaseWritingDirection:style.isLeftToRightDirection() ? NSWritingDirectionLeftToRight : NSWritingDirectionRightToLeft];
    [cell setMinValue:element->min()];
    [cell setMaxValue:element->max()];
    RetainPtr<NSNumber> valueObject = [NSNumber numberWithDouble:value];
    [cell setObjectValue:valueObject.get()];

    return cell;
}

const IntSize* LayoutThemeMac::progressBarSizes() const
{
    static const IntSize sizes[3] = { IntSize(0, 20), IntSize(0, 12), IntSize(0, 12) };
    return sizes;
}

const int* LayoutThemeMac::progressBarMargins(NSControlSize controlSize) const
{
    static const int margins[3][4] =
    {
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
        { 0, 0, 1, 0 },
    };
    return margins[controlSize];
}

int LayoutThemeMac::minimumProgressBarHeight(const ComputedStyle& style) const
{
    return sizeForSystemFont(style, progressBarSizes()).height();
}

double LayoutThemeMac::animationRepeatIntervalForProgressBar() const
{
    return progressAnimationFrameRate;
}

double LayoutThemeMac::animationDurationForProgressBar() const
{
    return progressAnimationNumFrames * progressAnimationFrameRate;
}

static const IntSize* menuListButtonSizes()
{
    static const IntSize sizes[3] = { IntSize(0, 21), IntSize(0, 18), IntSize(0, 15) };
    return sizes;
}

void LayoutThemeMac::adjustMenuListStyle(ComputedStyle& style, Element* e) const
{
    NSControlSize controlSize = controlSizeForFont(style);

    style.resetBorder();
    style.resetPadding();

    // Height is locked to auto.
    style.setHeight(Length(Auto));

    // White-space is locked to pre.
    style.setWhiteSpace(PRE);

    // Set the foreground color to black or gray when we have the aqua look.
    // Cast to RGB32 is to work around a compiler bug.
    style.setColor(e && !e->isDisabledFormControl() ? static_cast<RGBA32>(Color::black) : Color::darkGray);

    // Set the button's vertical size.
    setSizeFromFont(style, menuListButtonSizes());

    // Our font is locked to the appropriate system font size for the
    // control. To clarify, we first use the CSS-specified font to figure out a
    // reasonable control size, but once that control size is determined, we
    // throw that font away and use the appropriate system font for the control
    // size instead.
    setFontFromControlSize(style, controlSize);
}

static const int autofillPopupHorizontalPadding = 4;
static const int paddingBeforeSeparator = 4;
static const int baseBorderRadius = 5;
static const int styledPopupPaddingLeft = 8;
static const int styledPopupPaddingTop = 1;
static const int styledPopupPaddingBottom = 2;

// These functions are called with MenuListPart or MenulistButtonPart appearance
// by LayoutMenuList, or with TextFieldPart appearance by
// AutofillPopupMenuClient. We assume only AutofillPopupMenuClient gives
// TexfieldPart appearance here. We want to change only Autofill padding.  In
// the future, we have to separate Autofill popup window logic from WebKit to
// Chromium.
int LayoutThemeMac::popupInternalPaddingLeft(const ComputedStyle& style) const
{
    if (style.appearance() == TextFieldPart)
        return autofillPopupHorizontalPadding;

    if (style.appearance() == MenulistPart)
        return popupButtonPadding(controlSizeForFont(style))[ThemeMac::LeftMargin] * style.effectiveZoom();
    if (style.appearance() == MenulistButtonPart)
        return styledPopupPaddingLeft * style.effectiveZoom();
    return 0;
}

int LayoutThemeMac::popupInternalPaddingRight(const ComputedStyle& style) const
{
    if (style.appearance() == TextFieldPart)
        return autofillPopupHorizontalPadding;

    if (style.appearance() == MenulistPart)
        return popupButtonPadding(controlSizeForFont(style))[ThemeMac::RightMargin] * style.effectiveZoom();
    if (style.appearance() == MenulistButtonPart) {
        float fontScale = style.fontSize() / baseFontSize;
        float arrowWidth = menuListBaseArrowWidth * fontScale;
        return static_cast<int>(ceilf(arrowWidth + (menuListArrowPaddingLeft + menuListArrowPaddingRight + paddingBeforeSeparator) * style.effectiveZoom()));
    }
    return 0;
}

int LayoutThemeMac::popupInternalPaddingTop(const ComputedStyle& style) const
{
    if (style.appearance() == MenulistPart)
        return popupButtonPadding(controlSizeForFont(style))[ThemeMac::TopMargin] * style.effectiveZoom();
    if (style.appearance() == MenulistButtonPart)
        return styledPopupPaddingTop * style.effectiveZoom();
    return 0;
}

int LayoutThemeMac::popupInternalPaddingBottom(const ComputedStyle& style) const
{
    if (style.appearance() == MenulistPart)
        return popupButtonPadding(controlSizeForFont(style))[ThemeMac::BottomMargin] * style.effectiveZoom();
    if (style.appearance() == MenulistButtonPart)
        return styledPopupPaddingBottom * style.effectiveZoom();
    return 0;
}

void LayoutThemeMac::adjustMenuListButtonStyle(ComputedStyle& style, Element*) const
{
    float fontScale = style.fontSize() / baseFontSize;

    style.resetPadding();
    style.setBorderRadius(IntSize(int(baseBorderRadius + fontScale - 1), int(baseBorderRadius + fontScale - 1))); // FIXME: Round up?

    const int minHeight = 15;
    style.setMinHeight(Length(minHeight, Fixed));

    style.setLineHeight(ComputedStyle::initialLineHeight());
}

void LayoutThemeMac::setPopupButtonCellState(const LayoutObject* object, const IntRect& rect)
{
    NSPopUpButtonCell* popupButton = this->popupButton();

    // Set the control size based off the rectangle we're painting into.
    setControlSize(popupButton, popupButtonSizes(), rect.size(), object->style()->effectiveZoom());

    // Update the various states we respond to.
    updateActiveState(popupButton, object);
    updateCheckedState(popupButton, object);
    updateEnabledState(popupButton, object);
    updatePressedState(popupButton, object);
    if (ThemeMac::drawWithFrameDrawsFocusRing())
        updateFocusedState(popupButton, object);
}

const IntSize* LayoutThemeMac::menuListSizes() const
{
    static const IntSize sizes[3] = { IntSize(9, 0), IntSize(5, 0), IntSize(0, 0) };
    return sizes;
}

int LayoutThemeMac::minimumMenuListSize(const ComputedStyle& style) const
{
    return sizeForSystemFont(style, menuListSizes()).width();
}

void LayoutThemeMac::setSearchCellState(LayoutObject* o, const IntRect&)
{
    NSSearchFieldCell* search = this->search();

    // Update the various states we respond to.
    updateActiveState(search, o);
    updateEnabledState(search, o);
    updateFocusedState(search, o);
}

const IntSize* LayoutThemeMac::searchFieldSizes() const
{
    static const IntSize sizes[3] = { IntSize(0, 22), IntSize(0, 19), IntSize(0, 15) };
    return sizes;
}

static const int* searchFieldHorizontalPaddings()
{
    static const int sizes[3] = { 3, 2, 1 };
    return sizes;
}

void LayoutThemeMac::setSearchFieldSize(ComputedStyle& style) const
{
    // If the width and height are both specified, then we have nothing to do.
    if (!style.width().isIntrinsicOrAuto() && !style.height().isAuto())
        return;

    // Use the font size to determine the intrinsic width of the control.
    setSizeFromFont(style, searchFieldSizes());
}

const int searchFieldBorderWidth = 2;
void LayoutThemeMac::adjustSearchFieldStyle(ComputedStyle& style, Element*) const
{
    // Override border.
    style.resetBorder();
    const short borderWidth = searchFieldBorderWidth * style.effectiveZoom();
    style.setBorderLeftWidth(borderWidth);
    style.setBorderLeftStyle(INSET);
    style.setBorderRightWidth(borderWidth);
    style.setBorderRightStyle(INSET);
    style.setBorderBottomWidth(borderWidth);
    style.setBorderBottomStyle(INSET);
    style.setBorderTopWidth(borderWidth);
    style.setBorderTopStyle(INSET);

    // Override height.
    style.setHeight(Length(Auto));
    setSearchFieldSize(style);

    NSControlSize controlSize = controlSizeForFont(style);

    // Override padding size to match AppKit text positioning.
    const int verticalPadding = 1 * style.effectiveZoom();
    const int horizontalPadding = searchFieldHorizontalPaddings()[controlSize] * style.effectiveZoom();
    style.setPaddingLeft(Length(horizontalPadding, Fixed));
    style.setPaddingRight(Length(horizontalPadding, Fixed));
    style.setPaddingTop(Length(verticalPadding, Fixed));
    style.setPaddingBottom(Length(verticalPadding, Fixed));

    setFontFromControlSize(style, controlSize);

    style.setBoxShadow(nullptr);
}

const IntSize* LayoutThemeMac::cancelButtonSizes() const
{
    static const IntSize sizes[3] = { IntSize(14, 14), IntSize(11, 11), IntSize(9, 9) };
    return sizes;
}

void LayoutThemeMac::adjustSearchFieldCancelButtonStyle(ComputedStyle& style, Element*) const
{
    IntSize size = sizeForSystemFont(style, cancelButtonSizes());
    style.setWidth(Length(size.width(), Fixed));
    style.setHeight(Length(size.height(), Fixed));
    style.setBoxShadow(nullptr);
}

const IntSize* LayoutThemeMac::resultsButtonSizes() const
{
    static const IntSize sizes[3] = { IntSize(15, 14), IntSize(16, 13), IntSize(14, 11) };
    return sizes;
}

void LayoutThemeMac::adjustSearchFieldDecorationStyle(ComputedStyle& style, Element*) const
{
    NSControlSize controlSize = controlSizeForSystemFont(style);
    IntSize searchFieldSize = searchFieldSizes()[controlSize];
    int width = searchFieldSize.height() / 2 - searchFieldBorderWidth - searchFieldHorizontalPaddings()[controlSize];
    style.setWidth(Length(width, Fixed));
    style.setHeight(Length(0, Fixed));
    style.setBoxShadow(nullptr);
}

void LayoutThemeMac::adjustSearchFieldResultsDecorationStyle(ComputedStyle& style, Element*) const
{
    IntSize size = sizeForSystemFont(style, resultsButtonSizes());
    style.setWidth(Length(size.width(), Fixed));
    style.setHeight(Length(size.height(), Fixed));
    style.setBoxShadow(nullptr);
}

IntSize LayoutThemeMac::sliderTickSize() const
{
    return IntSize(1, 3);
}

int LayoutThemeMac::sliderTickOffsetFromTrackCenter() const
{
    return -9;
}

void LayoutThemeMac::adjustSliderThumbSize(ComputedStyle& style, Element*) const
{
    float zoomLevel = style.effectiveZoom();
    if (style.appearance() == SliderThumbHorizontalPart || style.appearance() == SliderThumbVerticalPart) {
        style.setWidth(Length(static_cast<int>(sliderThumbWidth * zoomLevel), Fixed));
        style.setHeight(Length(static_cast<int>(sliderThumbHeight * zoomLevel), Fixed));
    }

    adjustMediaSliderThumbSize(style);
}

NSPopUpButtonCell* LayoutThemeMac::popupButton() const
{
    if (!m_popupButton) {
        m_popupButton.adoptNS([[NSPopUpButtonCell alloc] initTextCell:@"" pullsDown:NO]);
        [m_popupButton.get() setUsesItemFromMenu:NO];
        [m_popupButton.get() setFocusRingType:NSFocusRingTypeExterior];
    }

    return m_popupButton.get();
}

NSSearchFieldCell* LayoutThemeMac::search() const
{
    if (!m_search) {
        m_search.adoptNS([[NSSearchFieldCell alloc] initTextCell:@""]);
        [m_search.get() setBezelStyle:NSTextFieldRoundedBezel];
        [m_search.get() setBezeled:YES];
        [m_search.get() setEditable:YES];
        [m_search.get() setFocusRingType:NSFocusRingTypeExterior];
        SEL sel = @selector(setCenteredLook:);
        if ([m_search.get() respondsToSelector:sel]) {
            BOOL boolValue = NO;
            NSMethodSignature* signature = [NSSearchFieldCell instanceMethodSignatureForSelector:sel];
            NSInvocation* invocation = [NSInvocation invocationWithMethodSignature:signature];
            [invocation setTarget:m_search.get()];
            [invocation setSelector:sel];
            [invocation setArgument:&boolValue atIndex:2];
            [invocation invoke];
        }
    }

    return m_search.get();
}

NSTextFieldCell* LayoutThemeMac::textField() const
{
    if (!m_textField) {
        m_textField.adoptNS([[WebCoreTextFieldCell alloc] initTextCell:@""]);
        [m_textField.get() setBezeled:YES];
        [m_textField.get() setEditable:YES];
        [m_textField.get() setFocusRingType:NSFocusRingTypeExterior];
#if __MAC_OS_X_VERSION_MIN_REQUIRED <= 1070
        [m_textField.get() setDrawsBackground:YES];
        [m_textField.get() setBackgroundColor:[NSColor whiteColor]];
#else
        // Post-Lion, WebCore can be in charge of paintinng the background
        // thanks to the workaround in place for <rdar://problem/11385461>,
        // which is implemented above as _coreUIDrawOptionsWithFrame.
        [m_textField.get() setDrawsBackground:NO];
#endif
    }

    return m_textField.get();
}

String LayoutThemeMac::fileListNameForWidth(Locale& locale, const FileList* fileList, const Font& font, int width) const
{
    if (width <= 0)
        return String();

    String strToTruncate;
    if (fileList->isEmpty()) {
        strToTruncate = locale.queryString(WebLocalizedString::FileButtonNoFileSelectedLabel);
    } else if (fileList->length() == 1) {
        File* file = fileList->item(0);
        if (file->userVisibility() == File::IsUserVisible)
            strToTruncate = [[NSFileManager defaultManager] displayNameAtPath:(fileList->item(0)->path())];
        else
            strToTruncate = file->name();
    } else {
        // FIXME: Localization of fileList->length().
        return StringTruncator::rightTruncate(locale.queryString(WebLocalizedString::MultipleFileUploadText, String::number(fileList->length())), width, font);
    }

    return StringTruncator::centerTruncate(strToTruncate, width, font);
}

NSView* FlippedView()
{
    static NSView* view = [[RTCMFlippedView alloc] init];
    return view;
}

LayoutTheme& LayoutTheme::theme()
{
    DEFINE_STATIC_REF(LayoutTheme, layoutTheme, (LayoutThemeMac::create()));
    return *layoutTheme;
}

PassRefPtr<LayoutTheme> LayoutThemeMac::create()
{
    return adoptRef(new LayoutThemeMac);
}

bool LayoutThemeMac::usesTestModeFocusRingColor() const
{
    return LayoutTestSupport::isRunningLayoutTest();
}

NSView* LayoutThemeMac::documentViewFor(LayoutObject*) const
{
    return FlippedView();
}

// Updates the control tint (a.k.a. active state) of |cell| (from |o|).  In the
// Chromium port, the layoutObject runs as a background process and controls'
// NSCell(s) lack a parent NSView. Therefore controls don't have their tint
// color updated correctly when the application is activated/deactivated.
// FocusController's setActive() is called when the application is
// activated/deactivated, which causes a paint invalidation at which time this
// code is called.
// This function should be called before drawing any NSCell-derived controls,
// unless you're sure it isn't needed.
void LayoutThemeMac::updateActiveState(NSCell* cell, const LayoutObject* o)
{
    NSControlTint oldTint = [cell controlTint];
    NSControlTint tint = isActive(o) ? [NSColor currentControlTint] :
                                       static_cast<NSControlTint>(NSClearControlTint);

    if (tint != oldTint)
        [cell setControlTint:tint];
}

bool LayoutThemeMac::shouldShowPlaceholderWhenFocused() const
{
    return true;
}

void LayoutThemeMac::adjustMediaSliderThumbSize(ComputedStyle& style) const
{
    MediaControlsPainter::adjustMediaSliderThumbSize(style);
}

String LayoutThemeMac::extraFullScreenStyleSheet()
{
    // FIXME: Chromium may wish to style its default media controls differently in fullscreen.
    return String();
}

String LayoutThemeMac::extraDefaultStyleSheet()
{
    return LayoutTheme::extraDefaultStyleSheet() +
        loadResourceAsASCIIString("themeChromium.css") +
        loadResourceAsASCIIString("themeInputMultipleFields.css") +
        loadResourceAsASCIIString("themeMac.css");
}

bool LayoutThemeMac::shouldUseFallbackTheme(const ComputedStyle& style) const
{
    ControlPart part = style.appearance();
    if (part == CheckboxPart || part == RadioPart)
        return style.effectiveZoom() != 1;
    return false;
}

} // namespace blink
