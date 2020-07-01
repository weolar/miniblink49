#ifndef content_ui_CustomTheme_h
#define content_ui_CustomTheme_h

#if USING_VC6RT != 1
#include <uxtheme.h>
#include <VSStyle.h>
#endif

#include "platform/geometry/win/IntRectWin.h"
#include "skia/ext/skia_utils_win.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/effects/SkGradientShader.h"
#include "public/platform/WebRect.h"

namespace color_utils {

// Represents an HSL color.
struct HSL {
    double h;
    double s;
    double l;
};

int calcHue(double temp1, double temp2, double hue)
{
    if (hue < 0.0)
        ++hue;
    else if (hue > 1.0)
        --hue;

    double result = temp1;
    if (hue * 6.0 < 1.0)
        result = temp1 + (temp2 - temp1) * hue * 6.0;
    else if (hue * 2.0 < 1.0)
        result = temp2;
    else if (hue * 3.0 < 2.0)
        result = temp1 + (temp2 - temp1) * (2.0 / 3.0 - hue) * 6.0;

    // Scale the result from 0 - 255 and round off the value.
    return static_cast<int>(result * 255 + .5);
}

void SkColorToHSL(SkColor c, HSL* hsl)
{
    double r = static_cast<double>(SkColorGetR(c)) / 255.0;
    double g = static_cast<double>(SkColorGetG(c)) / 255.0;
    double b = static_cast<double>(SkColorGetB(c)) / 255.0;
    double vmax = std::max(std::max(r, g), b);
    double vmin = std::min(std::min(r, g), b);
    double delta = vmax - vmin;
    hsl->l = (vmax + vmin) / 2;
    if (SkColorGetR(c) == SkColorGetG(c) && SkColorGetR(c) == SkColorGetB(c)) {
        hsl->h = hsl->s = 0;
    } else {
        double dr = (((vmax - r) / 6.0) + (delta / 2.0)) / delta;
        double dg = (((vmax - g) / 6.0) + (delta / 2.0)) / delta;
        double db = (((vmax - b) / 6.0) + (delta / 2.0)) / delta;
        // We need to compare for the max value because comparing vmax to r, g, or b
        // can sometimes result in values overflowing registers.
        if (r >= g && r >= b)
            hsl->h = db - dg;
        else if (g >= r && g >= b)
            hsl->h = (1.0 / 3.0) + dr - db;
        else  // (b >= r && b >= g)
            hsl->h = (2.0 / 3.0) + dg - dr;

        if (hsl->h < 0.0)
            ++hsl->h;
        else if (hsl->h > 1.0)
            --hsl->h;

        hsl->s = delta / ((hsl->l < 0.5) ? (vmax + vmin) : (2 - vmax - vmin));
    }
}

SkColor HSLToSkColor(const HSL& hsl, SkAlpha alpha)
{
    double hue = hsl.h;
    double saturation = hsl.s;
    double lightness = hsl.l;

    // If there's no color, we don't care about hue and can do everything based on
    // brightness.
    if (!saturation) {
        uint8 light;

        if (lightness < 0)
            light = 0;
        else if (lightness >= 1.0)
            light = 255;
        else
            light = SkDoubleToFixed(lightness) >> 8;

        return SkColorSetARGB(alpha, light, light, light);
    }

    double temp2 = (lightness < 0.5) ?
        (lightness * (1.0 + saturation)) :
        (lightness + saturation - (lightness * saturation));
    double temp1 = 2.0 * lightness - temp2;
    return SkColorSetARGB(alpha,
        calcHue(temp1, temp2, hue + 1.0 / 3.0),
        calcHue(temp1, temp2, hue),
        calcHue(temp1, temp2, hue - 1.0 / 3.0));
}

}

namespace {

// These are the default dimensions of radio buttons and checkboxes.
const int kCheckboxAndRadioWidth = 13;
const int kCheckboxAndRadioHeight = 13;

// These sizes match the sizes in Chromium Win.
const int kSliderThumbWidth = 11;
const int kSliderThumbHeight = 21;

const SkColor kSliderTrackBackgroundColor =
SkColorSetRGB(0xe3, 0xdd, 0xd8);
const SkColor kSliderThumbLightGrey = SkColorSetRGB(0xf4, 0xf2, 0xef);
const SkColor kSliderThumbDarkGrey = SkColorSetRGB(0xea, 0xe5, 0xe0);
const SkColor kSliderThumbBorderDarkGrey =
SkColorSetRGB(0x9d, 0x96, 0x8e);

const SkColor kTextBorderColor = SkColorSetRGB(0xa9, 0xa9, 0xa9);

const SkColor kMenuPopupBackgroundColor = SkColorSetRGB(210, 225, 246);

const unsigned int kDefaultScrollbarWidth = 13; // 15
const unsigned int kDefaultScrollbarButtonLength = 12; // 14

const SkColor kCheckboxTinyColor = SK_ColorGRAY;
const SkColor kCheckboxShadowColor = SkColorSetARGB(0x15, 0, 0, 0);
const SkColor kCheckboxShadowHoveredColor = SkColorSetARGB(0x1F, 0, 0, 0);
const SkColor kCheckboxShadowDisabledColor = SkColorSetARGB(0, 0, 0, 0);
const SkColor kCheckboxGradientColors[] = {
    SkColorSetRGB(0xed, 0xed, 0xed),
    SkColorSetRGB(0xde, 0xde, 0xde) };
const SkColor kCheckboxGradientPressedColors[] = {
    SkColorSetRGB(0xe7, 0xe7, 0xe7),
    SkColorSetRGB(0xd7, 0xd7, 0xd7) };
const SkColor kCheckboxGradientHoveredColors[] = {
    SkColorSetRGB(0xf0, 0xf0, 0xf0),
    SkColorSetRGB(0xe0, 0xe0, 0xe0) };
const SkColor kCheckboxGradientDisabledColors[] = {
    SkColorSetARGB(0x80, 0xed, 0xed, 0xed),
    SkColorSetARGB(0x80, 0xde, 0xde, 0xde) };
const SkColor kCheckboxBorderColor = SkColorSetARGB(0x40, 0, 0, 0);
const SkColor kCheckboxBorderHoveredColor = SkColorSetARGB(0x4D, 0, 0, 0);
const SkColor kCheckboxBorderDisabledColor = SkColorSetARGB(0x20, 0, 0, 0);
const SkColor kCheckboxStrokeColor = SkColorSetARGB(0xB3, 0, 0, 0);
const SkColor kCheckboxStrokeDisabledColor = SkColorSetARGB(0x59, 0, 0, 0);
const SkColor kRadioDotColor = SkColorSetRGB(0x66, 0x66, 0x66);
const SkColor kRadioDotDisabledColor = SkColorSetARGB(0x80, 0x66, 0x66, 0x66);

const unsigned int thumb_inactive_color_ = (0xeaeaea);
const unsigned int thumb_active_color_ = (0xf4f4f4);
const unsigned int track_color_ = (0xd3d3d3);


// Get lightness adjusted color.
SkColor BrightenColor(const color_utils::HSL& hsl, SkAlpha alpha,
    double lightness_amount)
{
    color_utils::HSL adjusted = hsl;
    adjusted.l += lightness_amount;
    if (adjusted.l > 1.0)
        adjusted.l = 1.0;
    if (adjusted.l < 0.0)
        adjusted.l = 0.0;

    return color_utils::HSLToSkColor(adjusted, alpha);
}

}

namespace content {

class CustomTheme {
public:
    unsigned int scrollbar_width_;
    unsigned int scrollbar_button_length_;

    CustomTheme() 
        : scrollbar_width_(kDefaultScrollbarWidth)
        , scrollbar_button_length_(kDefaultScrollbarButtonLength)
    {

    }

//     static ThemeName GetThemeName(blink::WebThemeEngine::Part part) {
//         ThemeName name;
//         switch (part) {
//         case blink::WebThemeEngine::PartCheckbox:
//         case blink::WebThemeEngine::PartRadio:
//         case blink::WebThemeEngine::PartButton:
//             name = BUTTON;
//             break;
//         case blink::WebThemeEngine::PartMenuList:
//             name = MENU;
//             break;
//         case blink::WebThemeEngine::PartScrollbarDownArrow:
//         case blink::WebThemeEngine::PartScrollbarLeftArrow:
//         case blink::WebThemeEngine::PartScrollbarRightArrow:
//         case blink::WebThemeEngine::PartScrollbarUpArrow:
//         case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
//         case blink::WebThemeEngine::PartScrollbarVerticalThumb:
//         case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
//         case blink::WebThemeEngine::PartScrollbarVerticalTrack:
//         case blink::WebThemeEngine::PartScrollbarCorner:
//             name = SCROLLBAR;
//             break;
//         case blink::WebThemeEngine::PartTextField:
//             name = TEXTFIELD;
//             break;
//         default:
//             notImplemented();
//             break;
//         }
//         return name;
//     }

//     blink::IntSize CommonThemeGetPartSize(blink::WebThemeEngine::Part part,
//         blink::WebThemeEngine::State state,
//         const blink::WebThemeEngine::ExtraParams& extra) {
//         blink::IntSize size;
//         switch (part) {
//         case NativeTheme::kComboboxArrow:
//             return ui::ResourceBundle::GetSharedInstance().GetImageNamed(
//                 IDR_MENU_DROPARROW).Size();
// 
//         case NativeTheme::kMenuCheck: {
//             const gfx::ImageSkia* check =
//                 ui::ResourceBundle::GetSharedInstance().GetImageNamed(
//                 IDR_MENU_CHECK_CHECKED).ToImageSkia();
//             size.SetSize(check->width(), check->height());
//             break;
//         }
//         default:
//             break;
//         }
// 
//         return size;
//     }

    blink::IntSize GetPartSize(blink::WebThemeEngine::Part part) const
    {
        blink::IntSize size;
//         size = CommonThemeGetPartSize(part, state, extra);
//         if (!size.IsEmpty())
//             return size;

        switch (part) {
            // Please keep these in the order of NativeTheme::Part.
        case blink::WebThemeEngine::PartCheckbox:
            return blink::IntSize(kCheckboxAndRadioWidth, kCheckboxAndRadioHeight);
        case blink::WebThemeEngine::PartInnerSpinButton:
            return blink::IntSize(scrollbar_width_, 0);
        case blink::WebThemeEngine::PartMenuList:
            return blink::IntSize();  // No default size.
//         case kMenuCheck:
//         case kMenuCheckBackground:
//         case kMenuPopupArrow:
//             NOTIMPLEMENTED();
//             break;
//         case kMenuPopupBackground:
//             return blink::IntSize();  // No default size.
//         case kMenuPopupGutter:
//         case kMenuPopupSeparator:
//             NOTIMPLEMENTED();
//             break;
//         case kMenuItemBackground:
        case blink::WebThemeEngine::PartProgressBar:
        case blink::WebThemeEngine::PartButton:
            return blink::IntSize();  // No default size.
        case blink::WebThemeEngine::PartRadio:
            return blink::IntSize(kCheckboxAndRadioWidth, kCheckboxAndRadioHeight);
        case blink::WebThemeEngine::PartScrollbarDownArrow:
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            return blink::IntSize(scrollbar_width_, scrollbar_button_length_);
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
        case blink::WebThemeEngine::PartScrollbarRightArrow:
            return blink::IntSize(scrollbar_button_length_, scrollbar_width_);
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
            // This matches Firefox on Linux.
            return blink::IntSize(2 * scrollbar_width_, scrollbar_width_);
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
            // This matches Firefox on Linux.
            return blink::IntSize(scrollbar_width_, 2 * scrollbar_width_);
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
            return blink::IntSize(0, scrollbar_width_);
        case blink::WebThemeEngine::PartScrollbarVerticalTrack:
            return blink::IntSize(scrollbar_width_, 0);
//         case kScrollbarHorizontalGripper:
//         case kScrollbarVerticalGripper:
//             NOTIMPLEMENTED();
//             break;
        case blink::WebThemeEngine::PartSliderTrack:
            return blink::IntSize();  // No default size.
        case blink::WebThemeEngine::PartSliderThumb:
            // These sizes match the sizes in Chromium Win.
            return blink::IntSize(kSliderThumbWidth, kSliderThumbHeight);
//         case kTabPanelBackground:
//             NOTIMPLEMENTED();
//             break;
//         case kTextField:
//             return blink::IntSize();  // No default size.
//         case kTrackbarThumb:
//         case kTrackbarTrack:
//         case kWindowResizeGripper:
//             NOTIMPLEMENTED();
//             break;
        default:
            //NOTREACHED() << "Unknown theme part: " << part;
            notImplemented();
            break;
        }
        return blink::IntSize();
    }

    void Paint(SkCanvas* canvas,
        blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ExtraParams& extra) const
    {
        if (rect.isEmpty())
            return;

        switch (part) {
            // Please keep these in the order of NativeTheme::Part.
//         case kComboboxArrow:
//             CommonThemePaintComboboxArrow(canvas, rect);
//             break;
        case blink::WebThemeEngine::PartCheckbox:
            PaintCheckbox(canvas, state, rect, extra.button);
            break;
        case blink::WebThemeEngine::PartInnerSpinButton:
            PaintInnerSpinButton(canvas, state, rect, extra.innerSpin);
            break;
        case blink::WebThemeEngine::PartMenuList:
            PaintMenuList(canvas, state, rect, extra.menuList);
            break;
//         case kMenuCheck:
//         case kMenuCheckBackground:
//         case kMenuPopupArrow:
//             NOTIMPLEMENTED();
//             break;
//         case kMenuPopupBackground:
//             PaintMenuPopupBackground(canvas, rect.size(), extra.menu_background);
//             break;
//         case kMenuPopupGutter:
//         case kMenuPopupSeparator:
//             NOTIMPLEMENTED();
//             break;
//         case kMenuItemBackground:
//             PaintMenuItemBackground(canvas, state, rect, extra.menu_list);
//             break;
        case blink::WebThemeEngine::PartProgressBar:
            PaintProgressBar(canvas, state, rect, extra.progressBar);
            break;
        case blink::WebThemeEngine::PartButton:
            PaintButton(canvas, state, rect, extra.button);
            break;
        case blink::WebThemeEngine::PartRadio:
            PaintRadio(canvas, state, rect, extra.button);
            break;
        case blink::WebThemeEngine::PartScrollbarDownArrow:
        case blink::WebThemeEngine::PartScrollbarUpArrow:
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
        case blink::WebThemeEngine::PartScrollbarRightArrow:
            PaintArrowButton(canvas, rect, part, state);
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
            PaintScrollbarThumb2(canvas, part, state, rect);
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
        case blink::WebThemeEngine::PartScrollbarVerticalTrack:
            PaintScrollbarTrack2(canvas, part, state, extra.scrollbarTrack, rect);
            break;
//         case kScrollbarHorizontalGripper:
//         case kScrollbarVerticalGripper:
//             // Invoked by views scrollbar code, don't care about for non-win
//             // implementations, so no NOTIMPLEMENTED.
//             break;
        case blink::WebThemeEngine::PartScrollbarCorner:
            PaintScrollbarCorner(canvas, state, rect);
            break;
        case blink::WebThemeEngine::PartSliderTrack:
            PaintSliderTrack(canvas, state, rect, extra.slider);
            break;
        case blink::WebThemeEngine::PartSliderThumb:
            PaintSliderThumb(canvas, state, rect, extra.slider);
            break;
//         case kTabPanelBackground:
//             NOTIMPLEMENTED();
//             break;
        case blink::WebThemeEngine::PartTextField:
            PaintTextField(canvas, state, rect, extra.textField);
            break;
//         case kTrackbarThumb:
//         case kTrackbarTrack:
//         case kWindowResizeGripper:
//             NOTIMPLEMENTED();
//             break;
        default:
            notImplemented();
            break;
        }
    }

    void PaintMenuPopupBackground(SkCanvas* canvas, const blink::IntRect& rect) const
    {
        canvas->drawColor(kMenuPopupBackgroundColor, SkXfermode::kSrc_Mode);
    }

    void PaintMenuItemBackground(SkCanvas* canvas, blink::WebThemeEngine::State state,
        const blink::IntRect& rect, const blink::WebThemeEngine::MenuListExtraParams& menu_list) const
    {
        // By default don't draw anything over the normal background.
    }

    void PaintArrowButton(
        SkCanvas* canvas,
        const blink::IntRect& rect, blink::WebThemeEngine::Part direction, blink::WebThemeEngine::State state) const
    {
        SkPaint paint;

        // Calculate button color.
        SkScalar trackHSV[3];
        SkColorToHSV(track_color_, trackHSV);
        SkColor buttonColor = SaturateAndBrighten(trackHSV, 0, 0.2f);
        SkColor backgroundColor = 0xff000000 | (RGB(236, 236, 236));// buttonColor;
        if (state == blink::WebThemeEngine::StatePressed) {
            SkScalar buttonHSV[3];
            SkColorToHSV(buttonColor, buttonHSV);
            buttonColor = SaturateAndBrighten(buttonHSV, 0, -0.1f);
            backgroundColor = 0xff000000 | (RGB(220, 220, 220));
        } else if (state == blink::WebThemeEngine::StateHover) {
            SkScalar buttonHSV[3];
            SkColorToHSV(buttonColor, buttonHSV);
            buttonColor = SaturateAndBrighten(buttonHSV, 0, 0.05f);
            backgroundColor = 0xff000000 | (RGB(220, 220, 220));
        }

        SkIRect skrect;
        skrect.set(rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height());
        // Paint the background (the area visible behind the rounded corners).
        //paint.setStyle(SkPaint::kFill_Style);
        paint.setColor(backgroundColor);
        canvas->drawIRect(skrect, paint);

        // Paint the button's outline and fill the middle
#if 0
        SkPath outline;
        switch (direction) {
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            outline.moveTo(rect.x(), rect.y() + rect.height());
            outline.rLineTo(0, -rect.height());
            outline.rLineTo(rect.width(), 0);
            outline.rLineTo(0, rect.height());
            break;
        case blink::WebThemeEngine::PartScrollbarDownArrow:
            outline.moveTo(rect.x(), rect.y());
            outline.rLineTo(0, rect.height());
            outline.rLineTo(rect.width(), 0);
            outline.rLineTo(0, -rect.height());
            break;
        case blink::WebThemeEngine::PartScrollbarRightArrow:
            outline.moveTo(rect.x(), rect.y());
            outline.rLineTo(rect.width(), 0);
            outline.rLineTo(0, rect.height());
            outline.rLineTo(-rect.width(), 0);
            break;
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
            outline.moveTo(rect.x() + rect.width(), rect.y());
            outline.rLineTo(-rect.width(), 0);
            outline.rLineTo(0, rect.height());
            outline.rLineTo(rect.width(), 0);
            break;
        default:
            break;
        }
        outline.close();

        paint.setStyle(SkPaint::kFill_Style);
        paint.setColor(buttonColor);
        canvas->drawPath(outline, paint);

        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kStroke_Style);
        SkScalar thumbHSV[3];
        SkColorToHSV(thumb_inactive_color_, thumbHSV);
        paint.setColor(OutlineColor(trackHSV, thumbHSV));
        canvas->drawPath(outline, paint);
#endif

        PaintArrow(canvas, rect, direction, GetArrowColor(state));
    }

    void PaintArrow(SkCanvas* gc,
        const blink::IntRect& rect,
        blink::WebThemeEngine::Part direction,
        SkColor color) const
    {
        int widthMiddle, lengthMiddle;
        if (direction == blink::WebThemeEngine::PartScrollbarUpArrow || direction == blink::WebThemeEngine::PartScrollbarDownArrow) {
            widthMiddle = rect.width() / 2 + 1;
            lengthMiddle = rect.height() / 2 + 1;
        } else {
            lengthMiddle = rect.width() / 2 + 1;
            widthMiddle = rect.height() / 2 + 1;
        }

        SkPaint paint;
        paint.setColor(color);
        paint.setAntiAlias(false);
        paint.setStyle(SkPaint::kFill_Style);

        SkPath path;
        // The constants in this block of code are hand-tailored to produce good
        // looking arrows without anti-aliasing.
        switch (direction) {
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            path.moveTo(rect.x() + widthMiddle - 4, rect.y() + lengthMiddle + 2);
            path.rLineTo(7, 0);
            path.rLineTo(-4, -4);
            break;
        case blink::WebThemeEngine::PartScrollbarDownArrow:
            path.moveTo(rect.x() + widthMiddle - 4, rect.y() + lengthMiddle - 3);
            path.rLineTo(7, 0);
            path.rLineTo(-4, 4);
            break;
        case blink::WebThemeEngine::PartScrollbarRightArrow:
            path.moveTo(rect.x() + lengthMiddle - 3, rect.y() + widthMiddle - 4);
            path.rLineTo(0, 7);
            path.rLineTo(4, -4);
            break;
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
            path.moveTo(rect.x() + lengthMiddle + 1, rect.y() + widthMiddle - 5);
            path.rLineTo(0, 9);
            path.rLineTo(-4, -4);
            break;
        default:
            break;
        }
        path.close();

        gc->drawPath(path, paint);
    }

    void PaintScrollbarTrack2(SkCanvas* canvas,
        blink::WebThemeEngine::Part part, blink::WebThemeEngine::State state,
        const blink::WebThemeEngine::ScrollbarTrackExtraParams& extra_params,
        const blink::IntRect& rect) const
    {
        SkPaint paint;
        SkIRect skrect;

        skrect.set(rect.x(), rect.y(), rect.maxX(), rect.maxY());

        paint.setColor(SkColorSetRGB(241, 241, 241));
        canvas->drawIRect(skrect, paint);
    }

    void PaintScrollbarThumb2(SkCanvas* canvas, blink::WebThemeEngine::Part part, blink::WebThemeEngine::State state, const blink::IntRect& rect) const
    {
        const bool hovered = state == blink::WebThemeEngine::StateHover;
        const bool vertical = part == blink::WebThemeEngine::PartScrollbarVerticalThumb;

        SkPaint paint;
        SkIRect skrect;

        int padding = 1;
        skrect.set(rect.x() + padding, rect.y() + padding, rect.x() + rect.width() - 2 * padding, rect.y() + rect.height() - 2 * padding);
        paint.setColor(hovered ? SkColorSetRGB(0xA8, 0xA8, 0xA8) : SkColorSetRGB(0xC1, 0xC1, 0xC1));
        canvas->drawIRect(skrect, paint);

#if 0
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(1);
        paint.setColor(SkColorSetRGB(212, 208, 200));
        canvas->drawIRect(skrect, paint);
#endif
    }

    void PaintScrollbarTrack(SkCanvas* canvas,
        blink::WebThemeEngine::Part part, blink::WebThemeEngine::State state,
        const blink::WebThemeEngine::ScrollbarTrackExtraParams& extra_params,
        const blink::IntRect& rect) const
    {
        SkPaint paint;
        SkIRect skrect;

        skrect.set(rect.x(), rect.y(), rect.maxX(), rect.maxY());
        SkScalar track_hsv[3];
        SkColorToHSV(track_color_, track_hsv);
        paint.setColor(SaturateAndBrighten(track_hsv, 0, 0));
        canvas->drawIRect(skrect, paint);

        SkScalar thumb_hsv[3];
        SkColorToHSV(thumb_inactive_color_, thumb_hsv);

        paint.setColor(OutlineColor(track_hsv, thumb_hsv));
        DrawBox(canvas, rect, paint);
    }

    void PaintScrollbarThumb(SkCanvas* canvas, blink::WebThemeEngine::Part part, blink::WebThemeEngine::State state, const blink::IntRect& rect) const
    {
        const bool hovered = state == blink::WebThemeEngine::StateHover;
        const int midx = rect.x() + rect.width() / 2;
        const int midy = rect.y() + rect.height() / 2;
        const bool vertical = part == blink::WebThemeEngine::PartScrollbarVerticalThumb;

        SkScalar thumb[3];
        SkColorToHSV(hovered ? thumb_active_color_ : thumb_inactive_color_, thumb);

        SkPaint paint;
        paint.setColor(SaturateAndBrighten(thumb, 0, 0.02f));

        SkIRect skrect;
        if (vertical)
            skrect.set(rect.x(), rect.y(), midx + 1, rect.y() + rect.height());
        else
            skrect.set(rect.x(), rect.y(), rect.x() + rect.width(), midy + 1);

        canvas->drawIRect(skrect, paint);

        paint.setColor(SaturateAndBrighten(thumb, 0, -0.02f));

        if (vertical) {
            skrect.set(midx + 1, rect.y(), rect.x() + rect.width(), rect.y() + rect.height());
        } else {
            skrect.set(rect.x(), midy + 1, rect.x() + rect.width(), rect.y() + rect.height());
        }

        canvas->drawIRect(skrect, paint);

        SkScalar track[3];
        SkColorToHSV(track_color_, track);
        paint.setColor(OutlineColor(track, thumb));
        DrawBox(canvas, rect, paint);

        if (rect.height() > 10 && rect.width() > 10) {
            const int grippy_half_width = 2;
            const int inter_grippy_offset = 3;
            if (vertical) {
                DrawHorizLine(canvas,
                    midx - grippy_half_width,
                    midx + grippy_half_width,
                    midy - inter_grippy_offset,
                    paint);
                DrawHorizLine(canvas,
                    midx - grippy_half_width,
                    midx + grippy_half_width,
                    midy,
                    paint);
                DrawHorizLine(canvas,
                    midx - grippy_half_width,
                    midx + grippy_half_width,
                    midy + inter_grippy_offset,
                    paint);
            } else {
                DrawVertLine(canvas,
                    midx - inter_grippy_offset,
                    midy - grippy_half_width,
                    midy + grippy_half_width,
                    paint);
                DrawVertLine(canvas,
                    midx,
                    midy - grippy_half_width,
                    midy + grippy_half_width,
                    paint);
                DrawVertLine(canvas,
                    midx + inter_grippy_offset,
                    midy - grippy_half_width,
                    midy + grippy_half_width,
                    paint);
            }
        }
    }

    void PaintScrollbarCorner(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect) const
    {
        SkPaint paint;
        paint.setColor(SK_ColorWHITE);
        paint.setStyle(SkPaint::kFill_Style);
        paint.setXfermodeMode(SkXfermode::kSrc_Mode);
        canvas->drawIRect(/*RectToSkIRect*/(rect), paint);
    }

    void PaintCheckbox(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ButtonExtraParams& button) const
    {
        SkRect skrect = PaintCheckboxRadioCommon(canvas, state, rect, SkIntToScalar(2));
        if (!skrect.isEmpty()) {
            // Draw the checkmark / dash.
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::kStroke_Style);
            if (state == blink::WebThemeEngine::StateDisabled)
                paint.setColor(kCheckboxStrokeDisabledColor);
            else
                paint.setColor(kCheckboxStrokeColor);
            if (button.indeterminate) {
                SkPath dash;
                dash.moveTo(skrect.x() + skrect.width() * 0.16,
                    (skrect.y() + skrect.bottom()) / 2);
                dash.rLineTo(skrect.width() * 0.68, 0);
                paint.setStrokeWidth(SkFloatToScalar(skrect.height() * 0.2));
                canvas->drawPath(dash, paint);
            } else if (button.checked) {
                SkPath check;
                check.moveTo(skrect.x() + skrect.width() * 0.2,
                    skrect.y() + skrect.height() * 0.5);
                check.rLineTo(skrect.width() * 0.2, skrect.height() * 0.2);
                paint.setStrokeWidth(SkFloatToScalar(skrect.height() * 0.23));
                check.lineTo(skrect.right() - skrect.width() * 0.2,
                    skrect.y() + skrect.height() * 0.2);
                canvas->drawPath(check, paint);
            }
        }
    }

    // Draws the common elements of checkboxes and radio buttons.
    // Returns the rectangle within which any additional decorations should be
    // drawn, or empty if none.
    SkRect PaintCheckboxRadioCommon(
        SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const SkScalar borderRadius) const
    {
        SkRect skrect = /*gfx::RectToSkRect*/(rect);

        // Use the largest square that fits inside the provided rectangle.
        // No other browser seems to support non-square widget, so accidentally
        // having non-square sizes is common (eg. amazon and webkit dev tools).
        if (skrect.width() != skrect.height()) {
            SkScalar size = SkMinScalar(skrect.width(), skrect.height());
            skrect.inset((skrect.width() - size) / 2, (skrect.height() - size) / 2);
        }

        // If the rectangle is too small then paint only a rectangle.  We don't want
        // to have to worry about '- 1' and '+ 1' calculations below having overflow
        // or underflow.
        if (skrect.width() <= 2) {
            SkPaint paint;
            paint.setColor(kCheckboxTinyColor);
            paint.setStyle(SkPaint::kFill_Style);
            canvas->drawRect(skrect, paint);
            // Too small to draw anything more.
            return SkRect::MakeEmpty();
        }

        // Make room for the drop shadow.
        skrect.iset(skrect.x(), skrect.y(), skrect.right() - 1, skrect.bottom() - 1);

        // Draw the drop shadow below the widget.
        if (state != blink::WebThemeEngine::StatePressed) {
            SkPaint paint;
            paint.setAntiAlias(true);
            SkRect shadowRect = skrect;
            shadowRect.offset(0, 1);
            if (state == blink::WebThemeEngine::StateDisabled)
                paint.setColor(kCheckboxShadowDisabledColor);
            else if (state == blink::WebThemeEngine::StateHover)
                paint.setColor(kCheckboxShadowHoveredColor);
            else
                paint.setColor(kCheckboxShadowColor);
            paint.setStyle(SkPaint::kFill_Style);
            canvas->drawRoundRect(shadowRect, borderRadius, borderRadius, paint);
        }

        // Draw the gradient-filled rectangle
        SkPoint gradientBounds[3];
        gradientBounds[0].set(skrect.x(), skrect.y());
        gradientBounds[1].set(skrect.x(), skrect.y() + skrect.height() * 0.38);
        gradientBounds[2].set(skrect.x(), skrect.bottom());
        const SkColor* startEndColors;
        if (state == blink::WebThemeEngine::StatePressed)
            startEndColors = kCheckboxGradientPressedColors;
        else if (state == blink::WebThemeEngine::StateHover)
            startEndColors = kCheckboxGradientHoveredColors;
        else if (state == blink::WebThemeEngine::StateDisabled)
            startEndColors = kCheckboxGradientDisabledColors;
        else /* kNormal */
            startEndColors = kCheckboxGradientColors;
        SkColor colors[3] = { startEndColors[0], startEndColors[0], startEndColors[1] };
        skia::RefPtr<SkShader> shader = skia::AdoptRef(
            SkGradientShader::CreateLinear(
            gradientBounds, colors, NULL, 3, SkShader::kClamp_TileMode));
        SkPaint paint;
        paint.setAntiAlias(true);
        paint.setShader(shader.get());
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRoundRect(skrect, borderRadius, borderRadius, paint);
        paint.setShader(NULL);

        // Draw the border.
        if (state == blink::WebThemeEngine::StateHover)
            paint.setColor(kCheckboxBorderHoveredColor);
        else if (state == blink::WebThemeEngine::StateDisabled)
            paint.setColor(kCheckboxBorderDisabledColor);
        else
            paint.setColor(kCheckboxBorderColor);
        paint.setStyle(SkPaint::kStroke_Style);
        paint.setStrokeWidth(SkIntToScalar(1));
        skrect.inset(SkFloatToScalar(.5f), SkFloatToScalar(.5f));
        canvas->drawRoundRect(skrect, borderRadius, borderRadius, paint);

        // Return the rectangle excluding the drop shadow for drawing any additional
        // decorations.
        return skrect;
    }

    void PaintRadio(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ButtonExtraParams& button) const
    {

        // Most of a radio button is the same as a checkbox, except the the rounded
        // square is a circle (i.e. border radius >= 100%).
        const SkScalar radius = SkFloatToScalar(
            static_cast<float>(std::max(rect.width(), rect.height())) / 2);
        SkRect skrect = PaintCheckboxRadioCommon(canvas, state, rect, radius);
        if (!skrect.isEmpty() && button.checked) {
            // Draw the dot.
            SkPaint paint;
            paint.setAntiAlias(true);
            paint.setStyle(SkPaint::kFill_Style);
            if (state == blink::WebThemeEngine::StateDisabled)
                paint.setColor(kRadioDotDisabledColor);
            else
                paint.setColor(kRadioDotColor);
            skrect.inset(skrect.width() * 0.25, skrect.height() * 0.25);
            // Use drawRoundedRect instead of drawOval to be completely consistent
            // with the border in PaintCheckboxRadioNewCommon.
            canvas->drawRoundRect(skrect, radius, radius, paint);
        }
    }

    void PaintButton(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ButtonExtraParams& button) const
    {
        SkPaint paint;
        const int kRight = rect.maxX();
        const int kBottom = rect.maxY();
        SkRect skrect = SkRect::MakeLTRB(rect.x(), rect.y(), kRight, kBottom);
        SkColor baseColor = button.backgroundColor;

        color_utils::HSL baseHsl;
        color_utils::SkColorToHSL(baseColor, &baseHsl);

        // Our standard gradient is from 0xdd to 0xf8. This is the amount of
        // increased luminance between those values.
        SkColor light_color(BrightenColor(baseHsl, SkColorGetA(baseColor), 0.105));

        // If the button is too small, fallback to drawing a single, solid color
        if (rect.width() < 5 || rect.height() < 5) {
            paint.setColor(baseColor);
            canvas->drawRect(skrect, paint);
            return;
        }

        paint.setColor(SK_ColorBLACK);
        const int kLightEnd = state == blink::WebThemeEngine::StatePressed ? 1 : 0;
        const int kDarkEnd = !kLightEnd;
        SkPoint gradientBounds[2];
        gradientBounds[kLightEnd].iset(rect.x(), rect.y());
        gradientBounds[kDarkEnd].iset(rect.x(), kBottom - 1);
        SkColor colors[2];
        colors[0] = light_color;
        colors[1] = baseColor;

        skia::RefPtr<SkShader> shader = skia::AdoptRef(SkGradientShader::CreateLinear(gradientBounds, colors, NULL, 2, SkShader::kClamp_TileMode));
        paint.setStyle(SkPaint::kFill_Style);
        paint.setAntiAlias(true);
        paint.setShader(shader.get());

        canvas->drawRoundRect(skrect, SkIntToScalar(1), SkIntToScalar(1), paint);
        paint.setShader(NULL);

        if (button.hasBorder) {
            int border_alpha = state == blink::WebThemeEngine::StateHover ? 0x80 : 0x55;
//             if (button.isFocused) {
//                 border_alpha = 0xff;
//                 paint.setColor(GetSystemColor(kColorId_FocusedBorderColor));
//             }
            paint.setStyle(SkPaint::kStroke_Style);
            paint.setStrokeWidth(SkIntToScalar(1));
            paint.setAlpha(border_alpha);
            skrect.inset(SkFloatToScalar(.5f), SkFloatToScalar(.5f));
            canvas->drawRoundRect(skrect, SkIntToScalar(1), SkIntToScalar(1), paint);
        }
    }

    void PaintTextField(SkCanvas* canvas, blink::WebThemeEngine::State state,
        const blink::IntRect& rect, const blink::WebThemeEngine::TextFieldExtraParams& text) const
    {
        SkRect bounds;
        bounds.set(rect.x(), rect.y(), rect.maxX() - 1, rect.maxY() - 1);

        SkPaint fill_paint;
        fill_paint.setStyle(SkPaint::kFill_Style);
        fill_paint.setColor(text.backgroundColor);
        canvas->drawRect(bounds, fill_paint);

        // Text INPUT, listbox SELECT, and TEXTAREA have consistent borders.
        // border: 1px solid #a9a9a9
        SkPaint stroke_paint;
        stroke_paint.setStyle(SkPaint::kStroke_Style);
        stroke_paint.setColor(kTextBorderColor);
        canvas->drawRect(bounds, stroke_paint);
    }

    void PaintMenuList(SkCanvas* canvas, blink::WebThemeEngine::State state, const blink::IntRect& rect,
        const blink::WebThemeEngine::MenuListExtraParams& menu_list) const
    {
        // If a border radius is specified, we let the WebCore paint the background
        // and the border of the control.
        if (!menu_list.hasBorderRadius) {
            blink::WebThemeEngine::ButtonExtraParams button = { 0 };
            button.backgroundColor = menu_list.backgroundColor;
            button.hasBorder = menu_list.hasBorder;
            PaintButton(canvas, state, rect, button);
        }

        SkPaint paint;
        paint.setColor(SK_ColorBLACK);
        paint.setAntiAlias(true);
        paint.setStyle(SkPaint::kFill_Style);

        SkPath path;
        path.moveTo(menu_list.arrowX, menu_list.arrowY - 3);
        path.rLineTo(6, 0);
        path.rLineTo(-3, 6);
        path.close();
        canvas->drawPath(path, paint);
    }

    void PaintSliderTrack(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::SliderExtraParams& slider) const
    {
        const int kMidX = rect.x() + rect.width() / 2;
        const int kMidY = rect.y() + rect.height() / 2;

        SkPaint paint;
        paint.setColor(kSliderTrackBackgroundColor);

        SkRect skrect;
        if (slider.vertical) {
            skrect.set(std::max(rect.x(), kMidX - 2),
                rect.y(),
                std::min(rect.maxX(), kMidX + 2),
                rect.maxY());
        } else {
            skrect.set(rect.x(),
                std::max(rect.y(), kMidY - 2),
                rect.maxX(),
                std::min(rect.maxY(), kMidY + 2));
        }
        canvas->drawRect(skrect, paint);
    }

    void PaintSliderThumb(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::SliderExtraParams& slider) const
    {
        const bool hovered = (state == blink::WebThemeEngine::StateHover) || slider.inDrag;
        const int kMidX = rect.x() + rect.width() / 2;
        const int kMidY = rect.y() + rect.height() / 2;

        SkPaint paint;
        paint.setColor(hovered ? SK_ColorWHITE : kSliderThumbLightGrey);

        SkIRect skrect;
        if (slider.vertical)
            skrect.set(rect.x(), rect.y(), kMidX + 1, rect.maxY());
        else
            skrect.set(rect.x(), rect.y(), rect.maxX(), kMidY + 1);

        canvas->drawIRect(skrect, paint);

        paint.setColor(hovered ? kSliderThumbLightGrey : kSliderThumbDarkGrey);

        if (slider.vertical)
            skrect.set(kMidX + 1, rect.y(), rect.maxX(), rect.maxY());
        else
            skrect.set(rect.x(), kMidY + 1, rect.maxX(), rect.maxY());

        canvas->drawIRect(skrect, paint);

        paint.setColor(kSliderThumbBorderDarkGrey);
        DrawBox(canvas, rect, paint);

        if (rect.height() > 10 && rect.width() > 10) {
            DrawHorizLine(canvas, kMidX - 2, kMidX + 2, kMidY, paint);
            DrawHorizLine(canvas, kMidX - 2, kMidX + 2, kMidY - 3, paint);
            DrawHorizLine(canvas, kMidX - 2, kMidX + 2, kMidY + 3, paint);
        }
    }

    void PaintInnerSpinButton(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::InnerSpinButtonExtraParams& spin_button) const
    {
        if (spin_button.readOnly)
            state = blink::WebThemeEngine::StateDisabled;

        blink::WebThemeEngine::State north_state = state;
        blink::WebThemeEngine::State south_state = state;
        if (spin_button.spinUp)
            south_state = south_state != blink::WebThemeEngine::StateDisabled ? blink::WebThemeEngine::StateNormal : blink::WebThemeEngine::StateDisabled;
        else
            north_state = north_state != blink::WebThemeEngine::StateDisabled ? blink::WebThemeEngine::StateNormal : blink::WebThemeEngine::StateDisabled;

        blink::IntRect half = rect;
        half.setHeight(rect.height() / 2);
        PaintArrowButton(canvas, half, blink::WebThemeEngine::PartScrollbarUpArrow, north_state);

        half.setY(rect.y() + rect.height() / 2);
        PaintArrowButton(canvas, half, blink::WebThemeEngine::PartScrollbarDownArrow, south_state);
    }

    void PaintProgressBar(SkCanvas* canvas,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ProgressBarExtraParams& progress_bar) const
    {
//         ResourceBundle& rb = ResourceBundle::GetSharedInstance();
//         gfx::ImageSkia* bar_image = rb.GetImageSkiaNamed(IDR_PROGRESS_BAR);
//         gfx::ImageSkia* left_border_image = rb.GetImageSkiaNamed(IDR_PROGRESS_BORDER_LEFT);
//         gfx::ImageSkia* right_border_image = rb.GetImageSkiaNamed(IDR_PROGRESS_BORDER_RIGHT);
// 
//         DCHECK(bar_image->width() > 0);
//         DCHECK(rect.width() > 0);
// 
//         float tile_scale_y = static_cast<float>(rect.height()) / bar_image->height();
// 
//         int dest_left_border_width = left_border_image->width();
//         int dest_right_border_width = right_border_image->width();
// 
//         // Since an implicit float -> int conversion will truncate, we want to make
//         // sure that if a border is desired, it gets at least one pixel.
//         if (dest_left_border_width > 0) {
//             dest_left_border_width = dest_left_border_width * tile_scale_y;
//             dest_left_border_width = std::max(dest_left_border_width, 1);
//         }
//         if (dest_right_border_width > 0) {
//             dest_right_border_width = dest_right_border_width * tile_scale_y;
//             dest_right_border_width = std::max(dest_right_border_width, 1);
//         }
// 
//         // Since the width of the progress bar may not be evenly divisible by the
//         // tile size, in order to make it look right we may need to draw some of the
//         // with a width of 1 pixel smaller than the rest of the tiles.
//         int new_tile_width = static_cast<int>(bar_image->width() * tile_scale_y);
//         new_tile_width = std::max(new_tile_width, 1);
// 
//         float tile_scale_x = static_cast<float>(new_tile_width) / bar_image->width();
//         if (rect.width() % new_tile_width == 0) {
//             DrawTiledImage(canvas, *bar_image, 0, 0, tile_scale_x, tile_scale_y,
//                 rect.x(), rect.y(),
//                 rect.width(), rect.height());
//         }
//         else {
//             int num_tiles = 1 + rect.width() / new_tile_width;
//             int overshoot = num_tiles * new_tile_width - rect.width();
//             // Since |overshoot| represents the number of tiles that were too big, draw
//             // |overshoot| tiles with their width reduced by 1.
//             int num_big_tiles = num_tiles - overshoot;
//             int num_small_tiles = overshoot;
//             int small_width = new_tile_width - 1;
//             float small_scale_x = static_cast<float>(small_width) / bar_image->width();
//             float big_scale_x = tile_scale_x;
// 
//             blink::IntRect big_rect = rect;
//             blink::IntRect small_rect = rect;
//             big_rect.Inset(0, 0, num_small_tiles*small_width, 0);
//             small_rect.Inset(num_big_tiles*new_tile_width, 0, 0, 0);
// 
//             DrawTiledImage(canvas, *bar_image, 0, 0, big_scale_x, tile_scale_y,
//                 big_rect.x(), big_rect.y(), big_rect.width(), big_rect.height());
//             DrawTiledImage(canvas, *bar_image, 0, 0, small_scale_x, tile_scale_y,
//                 small_rect.x(), small_rect.y(), small_rect.width(), small_rect.height());
//         }
//         if (progress_bar.valueRectWidth) {
//             gfx::ImageSkia* value_image = rb.GetImageSkiaNamed(IDR_PROGRESS_VALUE);
// 
//             new_tile_width = static_cast<int>(value_image->width() * tile_scale_y);
//             tile_scale_x = static_cast<float>(new_tile_width) /
//                 value_image->width();
// 
//             DrawTiledImage(canvas, *value_image, 0, 0, tile_scale_x, tile_scale_y,
//                 progress_bar.value_rect_x,
//                 progress_bar.value_rect_y,
//                 progress_bar.value_rect_width,
//                 progress_bar.value_rect_height);
//         }
// 
//         DrawImageInt(canvas, *left_border_image, 0, 0, left_border_image->width(),
//             left_border_image->height(), rect.x(), rect.y(), dest_left_border_width,
//             rect.height());
// 
//         int dest_x = rect.maxX() - dest_right_border_width;
//         DrawImageInt(canvas, *right_border_image, 0, 0, right_border_image->width(),
//             right_border_image->height(), dest_x, rect.y(),
//             dest_right_border_width, rect.height());
    }

    bool IntersectsClipRectInt(SkCanvas* canvas, int x, int y, int w, int h) const
    {
        SkRect clip;
        return canvas->getClipBounds(&clip) && 
            clip.intersect(SkIntToScalar(x), SkIntToScalar(y), SkIntToScalar(x + w), SkIntToScalar(y + h));
    }

//     void DrawImageInt(
//         SkCanvas* sk_canvas, const gfx::ImageSkia& image,
//         int src_x, int src_y, int src_w, int src_h,
//         int dest_x, int dest_y, int dest_w, int dest_h) const {
//         scoped_ptr<gfx::Canvas> canvas(CommonThemeCreateCanvas(sk_canvas));
//         canvas->DrawImageInt(image, src_x, src_y, src_w, src_h,
//             dest_x, dest_y, dest_w, dest_h, true);
//     }
// 
//     void DrawTiledImage(SkCanvas* sk_canvas,
//         const gfx::ImageSkia& image,
//         int src_x, int src_y, float tile_scale_x, float tile_scale_y,
//         int dest_x, int dest_y, int w, int h) const {
//         scoped_ptr<gfx::Canvas> canvas(CommonThemeCreateCanvas(sk_canvas));
//         canvas->TileImageInt(image, src_x, src_y, tile_scale_x,
//             tile_scale_y, dest_x, dest_y, w, h);
//     }

    SkColor SaturateAndBrighten(SkScalar* hsv,
        SkScalar saturate_amount,
        SkScalar brighten_amount) const
    {
        SkScalar color[3];
        color[0] = hsv[0];
        color[1] = Clamp(hsv[1] + saturate_amount, 0.0, 1.0);
        color[2] = Clamp(hsv[2] + brighten_amount, 0.0, 1.0);
        return SkHSVToColor(color);
    }

    SkColor GetArrowColor(blink::WebThemeEngine::State state) const
    {
        if (state != blink::WebThemeEngine::StateDisabled)
            return SK_ColorBLACK;

        SkScalar track_hsv[3];
        SkColorToHSV(track_color_, track_hsv);
        SkScalar thumb_hsv[3];
        SkColorToHSV(thumb_inactive_color_, thumb_hsv);
        return OutlineColor(track_hsv, thumb_hsv);
    }

    void DrawVertLine(SkCanvas* canvas,
        int x,
        int y1,
        int y2,
        const SkPaint& paint) const
    {
        SkIRect skrect;
        skrect.set(x, y1, x + 1, y2 + 1);
        canvas->drawIRect(skrect, paint);
    }

    void DrawHorizLine(SkCanvas* canvas,
        int x1,
        int x2,
        int y,
        const SkPaint& paint) const
    {
        SkIRect skrect;
        skrect.set(x1, y, x2 + 1, y + 1);
        canvas->drawIRect(skrect, paint);
    }

    void DrawBox(SkCanvas* canvas,
        const blink::IntRect& rect,
        const SkPaint& paint) const
    {
        const int right = rect.x() + rect.width() - 1;
        const int bottom = rect.y() + rect.height() - 1;
        DrawHorizLine(canvas, rect.x(), right, rect.y(), paint);
        DrawVertLine(canvas, right, rect.y(), bottom, paint);
        DrawHorizLine(canvas, rect.x(), right, bottom, paint);
        DrawVertLine(canvas, rect.x(), rect.y(), bottom, paint);
    }

    SkScalar Clamp(SkScalar value,
        SkScalar min,
        SkScalar max) const
    {
        return std::min(std::max(value, min), max);
    }

    SkColor OutlineColor(SkScalar* hsv1, SkScalar* hsv2) const
    {
        // GTK Theme engines have way too much control over the layout of
        // the scrollbar. We might be able to more closely approximate its
        // look-and-feel, if we sent whole images instead of just colors
        // from the browser to the renderer. But even then, some themes
        // would just break.
        //
        // So, instead, we don't even try to 100% replicate the look of
        // the native scrollbar. We render our own version, but we make
        // sure to pick colors that blend in nicely with the system GTK
        // theme. In most cases, we can just sample a couple of pixels
        // from the system scrollbar and use those colors to draw our
        // scrollbar.
        //
        // This works fine for the track color and the overall thumb
        // color. But it fails spectacularly for the outline color used
        // around the thumb piece.  Not all themes have a clearly defined
        // outline. For some of them it is partially transparent, and for
        // others the thickness is very unpredictable.
        //
        // So, instead of trying to approximate the system theme, we
        // instead try to compute a reasonable looking choice based on the
        // known color of the track and the thumb piece. This is difficult
        // when trying to deal both with high- and low-contrast themes,
        // and both with positive and inverted themes.
        //
        // The following code has been tested to look OK with all of the
        // default GTK themes.
        SkScalar min_diff = Clamp((hsv1[1] + hsv2[1]) * 1.2f, 0.28f, 0.5f);
        SkScalar diff = Clamp(fabs(hsv1[2] - hsv2[2]) / 2, min_diff, 0.5f);

        if (hsv1[2] + hsv2[2] > 1.0)
            diff = -diff;

        return SaturateAndBrighten(hsv2, -0.2f, diff);
    }
};


} // content

#endif // CustomTheme_h