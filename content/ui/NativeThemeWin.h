
#ifndef NativeThemeWin_h
#define NativeThemeWin_h

#include <uxtheme.h>
#include <VSStyle.h>
#include "platform/geometry/win/IntRectWin.h"
#include "skia/ext/skia_utils_win.h"
#include "skia/ext/platform_canvas.h"
#include "public/platform/WebRect.h"

namespace content {

// These sizes match the sizes in Chromium Win.
const int kSliderThumbWidth = 11;
const int kSliderThumbHeight = 21;

class NativeThemeWin {
public:
    enum ThemeName {
        BUTTON,
        LIST,
        MENU,
        MENULIST,
        SCROLLBAR,
        STATUS,
        TAB,
        TEXTFIELD,
        TRACKBAR,
        WINDOW,
        PROGRESS,
        SPIN,
        LAST
    };

    typedef HRESULT(WINAPI* DrawThemeBackgroundPtr)(HANDLE theme,
        HDC hdc,
        int part_id,
        int state_id,
        const RECT* rect,
        const RECT* clip_rect);
    typedef HRESULT(WINAPI* DrawThemeBackgroundExPtr)(HANDLE theme,
        HDC hdc,
        int part_id,
        int state_id,
        const RECT* rect,
        const DTBGOPTS* opts);
    typedef HRESULT(WINAPI* GetThemeColorPtr)(HANDLE hTheme,
        int part_id,
        int state_id,
        int prop_id,
        COLORREF* color);
    typedef HRESULT(WINAPI* GetThemeContentRectPtr)(HANDLE hTheme,
        HDC hdc,
        int part_id,
        int state_id,
        const RECT* rect,
        RECT* content_rect);
    typedef HRESULT(WINAPI* GetThemePartSizePtr)(HANDLE hTheme,
        HDC hdc,
        int part_id,
        int state_id,
        RECT* rect,
        int ts,
        SIZE* size);
    typedef HANDLE(WINAPI* OpenThemeDataPtr)(HWND window,
        LPCWSTR class_list);
    typedef HRESULT(WINAPI* CloseThemeDataPtr)(HANDLE theme);

    typedef void (WINAPI* SetThemeAppPropertiesPtr) (DWORD flags);
    typedef BOOL(WINAPI* IsThemeActivePtr)();
    typedef HRESULT(WINAPI* GetThemeIntPtr)(HANDLE hTheme,
        int part_id,
        int state_id,
        int prop_id,
        int *value);

    // Function pointers into uxtheme.dll.
    DrawThemeBackgroundPtr draw_theme_;
    DrawThemeBackgroundExPtr draw_theme_ex_;
    GetThemeColorPtr get_theme_color_;
    GetThemeContentRectPtr get_theme_content_rect_;
    GetThemePartSizePtr get_theme_part_size_;
    OpenThemeDataPtr open_theme_;
    CloseThemeDataPtr close_theme_;
    SetThemeAppPropertiesPtr set_theme_properties_;
    IsThemeActivePtr is_theme_active_;
    GetThemeIntPtr get_theme_int_;

    // Handle to uxtheme.dll.
    HMODULE theme_dll_;

    // A cache of open theme handles.
    mutable HANDLE theme_handles_[LAST];

    NativeThemeWin::NativeThemeWin()
        : theme_dll_(LoadLibrary(L"uxtheme.dll")),
        draw_theme_(NULL),
        draw_theme_ex_(NULL),
        get_theme_color_(NULL),
        get_theme_content_rect_(NULL),
        get_theme_part_size_(NULL),
        open_theme_(NULL),
        close_theme_(NULL),
        set_theme_properties_(NULL),
        is_theme_active_(NULL),
        get_theme_int_(NULL) {
        if (theme_dll_) {
            draw_theme_ = reinterpret_cast<DrawThemeBackgroundPtr>(
                GetProcAddress(theme_dll_, "DrawThemeBackground"));
            draw_theme_ex_ = reinterpret_cast<DrawThemeBackgroundExPtr>(
                GetProcAddress(theme_dll_, "DrawThemeBackgroundEx"));
            get_theme_color_ = reinterpret_cast<GetThemeColorPtr>(
                GetProcAddress(theme_dll_, "GetThemeColor"));
            get_theme_content_rect_ = reinterpret_cast<GetThemeContentRectPtr>(
                GetProcAddress(theme_dll_, "GetThemeBackgroundContentRect"));
            get_theme_part_size_ = reinterpret_cast<GetThemePartSizePtr>(
                GetProcAddress(theme_dll_, "GetThemePartSize"));
            open_theme_ = reinterpret_cast<OpenThemeDataPtr>(
                GetProcAddress(theme_dll_, "OpenThemeData"));
            close_theme_ = reinterpret_cast<CloseThemeDataPtr>(
                GetProcAddress(theme_dll_, "CloseThemeData"));
            set_theme_properties_ = reinterpret_cast<SetThemeAppPropertiesPtr>(
                GetProcAddress(theme_dll_, "SetThemeAppProperties"));
            is_theme_active_ = reinterpret_cast<IsThemeActivePtr>(
                GetProcAddress(theme_dll_, "IsThemeActive"));
            get_theme_int_ = reinterpret_cast<GetThemeIntPtr>(
                GetProcAddress(theme_dll_, "GetThemeInt"));
        }
        memset(theme_handles_, 0, sizeof(theme_handles_));
    }

    static ThemeName GetThemeName(blink::WebThemeEngine::Part part) {
        ThemeName name;
        switch (part) {
        case blink::WebThemeEngine::PartCheckbox:
        case blink::WebThemeEngine::PartRadio:
        case blink::WebThemeEngine::PartButton:
            name = BUTTON;
            break;
        case blink::WebThemeEngine::PartMenuList:
            name = MENU;
            break;
        case blink::WebThemeEngine::PartScrollbarDownArrow:
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
        case blink::WebThemeEngine::PartScrollbarRightArrow:
        case blink::WebThemeEngine::PartScrollbarUpArrow:
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
        case blink::WebThemeEngine::PartScrollbarVerticalTrack:
        case blink::WebThemeEngine::PartScrollbarCorner:
            name = SCROLLBAR;
            break;
        case blink::WebThemeEngine::PartTextField:
            name = TEXTFIELD;
            break;
        default:
            notImplemented();
            break;
        }
        return name;
    }

    HRESULT GetThemePartSize(ThemeName theme_name,
        HDC hdc,
        int part_id,
        int state_id,
        RECT* rect,
        int ts,
        SIZE* size) const {
        HANDLE handle = GetThemeHandle(theme_name);
        if (handle && get_theme_part_size_)
            return get_theme_part_size_(handle, hdc, part_id, state_id, rect, ts, size);

        return E_NOTIMPL;
    }

    HANDLE GetThemeHandle(ThemeName theme_name) const {
        if (!open_theme_ || theme_name < 0 || theme_name >= LAST)
            return 0;

        if (theme_handles_[theme_name])
            return theme_handles_[theme_name];

        // Not found, try to load it.
        HANDLE handle = 0;
        switch (theme_name) {
        case BUTTON:
            handle = open_theme_(NULL, L"Button");
            break;
        case LIST:
            handle = open_theme_(NULL, L"Listview");
            break;
        case MENU:
            handle = open_theme_(NULL, L"Menu");
            break;
        case MENULIST:
            handle = open_theme_(NULL, L"Combobox");
            break;
        case SCROLLBAR:
            handle = open_theme_(NULL, L"Scrollbar");
            break;
        case STATUS:
            handle = open_theme_(NULL, L"Status");
            break;
        case TAB:
            handle = open_theme_(NULL, L"Tab");
            break;
        case TEXTFIELD:
            handle = open_theme_(NULL, L"Edit");
            break;
        case TRACKBAR:
            handle = open_theme_(NULL, L"Trackbar");
            break;
        case WINDOW:
            handle = open_theme_(NULL, L"Window");
            break;
        case PROGRESS:
            handle = open_theme_(NULL, L"Progress");
            break;
        case SPIN:
            handle = open_theme_(NULL, L"Spin");
            break;
        default:
            notImplemented();
        }
        theme_handles_[theme_name] = handle;
        return handle;
    }

    static int GetWindowsPart(blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::WebThemeEngine::ExtraParams& extra) {
        int part_id;
        switch (part) {
        case blink::WebThemeEngine::PartCheckbox:
            part_id = BP_CHECKBOX;
            break;
        case blink::WebThemeEngine::PartMenuList: // kMenuCheck :
            part_id = MENU_POPUPCHECK;
            break;
//         case blink::WebThemeEngine::PartMenuList: // kMenuPopupArrow :
//             part_id = MENU_POPUPSUBMENU;
//             break;
//         case blink::WebThemeEngine::PartMenuList: // kMenuPopupGutter :
//             part_id = MENU_POPUPGUTTER;
//             break;
//         case blink::WebThemeEngine::PartMenuList: // kMenuPopupSeparator :
//             part_id = MENU_POPUPSEPARATOR;
//             break;
        case blink::WebThemeEngine::PartButton:
            part_id = BP_PUSHBUTTON;
            break;
        case blink::WebThemeEngine::PartRadio:
            part_id = BP_RADIOBUTTON;
            break;
//         case kWindowResizeGripper:
//             part_id = SP_GRIPPER;
//             break;
        case blink::WebThemeEngine::PartScrollbarDownArrow:
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
        case blink::WebThemeEngine::PartScrollbarRightArrow:
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            part_id = SBP_ARROWBTN;
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
            part_id = SBP_THUMBBTNHORZ;
            break;
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
            part_id = SBP_THUMBBTNVERT;
            break;
        default:
            notImplemented();
            break;
        }
        return part_id;
    }

    static int GetWindowsState(blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::WebThemeEngine::ExtraParams& extra) {
        int state_id;
        switch (part) {
        case blink::WebThemeEngine::PartCheckbox:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = CBS_UNCHECKEDNORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                state_id = CBS_UNCHECKEDHOT;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = CBS_UNCHECKEDPRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = CBS_UNCHECKEDDISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        case blink::WebThemeEngine::PartButton:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = PBS_NORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                state_id = PBS_HOT;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = PBS_PRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = PBS_DISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        case blink::WebThemeEngine::PartRadio:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = RBS_UNCHECKEDNORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                state_id = RBS_UNCHECKEDHOT;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = RBS_UNCHECKEDPRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = RBS_UNCHECKEDDISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        
        case blink::WebThemeEngine::PartScrollbarDownArrow:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = ABS_DOWNNORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                // Mimic ScrollbarThemeChromiumWin.cpp in WebKit.
                state_id = //base::win::GetVersion() < base::win::VERSION_VISTA ?
                    ABS_DOWNHOT;// : ABS_DOWNHOVER;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = ABS_DOWNPRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = ABS_DOWNDISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = ABS_LEFTNORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                // Mimic ScrollbarThemeChromiumWin.cpp in WebKit.
                state_id = //base::win::GetVersion() < base::win::VERSION_VISTA ?
                    ABS_LEFTHOT;// : ABS_LEFTHOVER;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = ABS_LEFTPRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = ABS_LEFTDISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        case blink::WebThemeEngine::PartScrollbarRightArrow:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = ABS_RIGHTNORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                // Mimic ScrollbarThemeChromiumWin.cpp in WebKit.
                state_id = //base::win::GetVersion() < base::win::VERSION_VISTA ?
                    ABS_RIGHTHOT;// : ABS_RIGHTHOVER;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = ABS_RIGHTPRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = ABS_RIGHTDISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = ABS_UPNORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                // Mimic ScrollbarThemeChromiumWin.cpp in WebKit.
                state_id = //base::win::GetVersion() < base::win::VERSION_VISTA ?
                    ABS_UPHOT;//: ABS_UPHOVER;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = ABS_UPPRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = ABS_UPDISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
            switch (state) {
            case blink::WebThemeEngine::StateNormal:
                state_id = SCRBS_NORMAL;
                break;
            case blink::WebThemeEngine::StateHover:
                // Mimic WebKit's behaviour in ScrollbarThemeChromiumWin.cpp.
                state_id = //base::win::GetVersion() < base::win::VERSION_VISTA ?
                    SCRBS_HOT;// : SCRBS_HOVER;
                break;
            case blink::WebThemeEngine::StatePressed:
                state_id = SCRBS_PRESSED;
                break;
            case blink::WebThemeEngine::StateDisabled:
                state_id = SCRBS_DISABLED;
                break;
            default:
                notImplemented();
                break;
            }
            break;
        default:
            notImplemented();
            break;
        }
        return state_id;
    }

    blink::WebSize getSize(blink::WebThemeEngine::Part part)
    {
        // The GetThemePartSize call below returns the default size without
        // accounting for user customization (crbug/218291).
        switch (part) {
        case blink::WebThemeEngine::PartScrollbarDownArrow:
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
        case blink::WebThemeEngine::PartScrollbarRightArrow:
        case blink::WebThemeEngine::PartScrollbarUpArrow:
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
        case blink::WebThemeEngine::PartScrollbarVerticalTrack: {
            int size = gfx::win::GetSystemMetricsInDIP(SM_CXVSCROLL);
            if (size == 0)
                size = 17;
            return blink::WebSize(size, size);
        }
        case blink::WebThemeEngine::PartSliderThumb:
            return blink::WebSize(kSliderThumbWidth, kSliderThumbHeight);  // No default size.
        case blink::WebThemeEngine::PartSliderTrack:
            return blink::WebSize();  // No default size.
        }

        int part_id = NativeThemeWin::GetWindowsPart(part, blink::WebThemeEngine::StateNormal, blink::WebThemeEngine::ExtraParams());
        int state_id = NativeThemeWin::GetWindowsState(part, blink::WebThemeEngine::StateNormal, blink::WebThemeEngine::ExtraParams());

        SIZE size;
        HDC hdc = GetDC(NULL);
        HRESULT hr = NativeThemeWin::GetThemePartSize(NativeThemeWin::GetThemeName(part), hdc, part_id, state_id,
            NULL, TS_TRUE, &size);
        ReleaseDC(NULL, hdc);

        if (FAILED(hr)) {
            // TODO(rogerta): For now, we need to support radio buttons and checkboxes
            // when theming is not enabled.  Support for other parts can be added
            // if/when needed.
            switch (part) {
            case blink::WebThemeEngine::PartCheckbox:
            case blink::WebThemeEngine::PartRadio:
                // TODO(rogerta): I was not able to find any API to get the default
                // size of these controls, so determined these values empirically.
                size.cx = 13;
                size.cy = 13;
                break;
            default:
                size.cx = 0;
                size.cy = 0;
                break;
            }
        }

        return blink::WebSize();
    }

    HRESULT PaintTextField(
        HDC hdc,
        blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::TextFieldExtraParams& extra) const {
        int part_id = EP_EDITTEXT;
        int state_id = ETS_NORMAL;
        switch (state) {
        case blink::WebThemeEngine::StateReadonly:
            state_id = ETS_NORMAL;
            break;
        case blink::WebThemeEngine::StateFocused:
            state_id = ETS_FOCUSED;
            break;
        case blink::WebThemeEngine::StateNormal:
            state_id = ETS_NORMAL;
            break;
        case blink::WebThemeEngine::StateHover:
            state_id = ETS_HOT;
            break;
        case blink::WebThemeEngine::StatePressed:
            state_id = ETS_SELECTED;
            break;
        case blink::WebThemeEngine::StateDisabled:
            state_id = ETS_DISABLED;
            break;
        default:
            notImplemented();
            break;
        }

        RECT rect_win = blink::intRectToWinRect(rect);
        return PaintTextField(hdc, part_id, state_id, 0,
            &rect_win,
            skia::SkColorToCOLORREF(extra.backgroundColor),
            true, true);
    }

    HRESULT PaintTextField(
        HDC hdc,
        int part_id,
        int state_id,
        int classic_state,
        RECT* rect,
        COLORREF color,
        bool fill_content_area,
        bool draw_edges) const {
        // TODO(ojan): http://b/1210017 Figure out how to give the ability to
        // exclude individual edges from being drawn.

        HANDLE handle = GetThemeHandle(TEXTFIELD);
        // TODO(mpcomplete): can we detect if the color is specified by the user,
        // and if not, just use the system color?
        // CreateSolidBrush() accepts a RGB value but alpha must be 0.
        HBRUSH bg_brush = CreateSolidBrush(color);
        HRESULT hr;
        // DrawThemeBackgroundEx was introduced in XP SP2, so that it's possible
        // draw_theme_ex_ is NULL and draw_theme_ is non-null.
        if (handle && (draw_theme_ex_ || (draw_theme_ && draw_edges))) {
            if (draw_theme_ex_) {
                static const DTBGOPTS omit_border_options = {
                    sizeof(DTBGOPTS),
                    DTBG_OMITBORDER,
                    { 0, 0, 0, 0 }
                };
                const DTBGOPTS* draw_opts = draw_edges ? NULL : &omit_border_options;
                hr = draw_theme_ex_(handle, hdc, part_id, state_id, rect, draw_opts);
            }
            else {
                hr = draw_theme_(handle, hdc, part_id, state_id, rect, NULL);
            }

            // TODO(maruel): Need to be fixed if get_theme_content_rect_ is NULL.
//             if (fill_content_area && get_theme_content_rect_) {
//                 RECT content_rect;
//                 hr = get_theme_content_rect_(handle, hdc, part_id, state_id, rect,
//                     &content_rect);
//                 FillRect(hdc, &content_rect, bg_brush);
//             }
        }
        else {
            // Draw it manually.
            if (draw_edges)
                DrawEdge(hdc, rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

            if (fill_content_area) {
                FillRect(hdc, rect, (classic_state & DFCS_INACTIVE) ?
                    reinterpret_cast<HBRUSH>(COLOR_BTNFACE + 1) : bg_brush);
            }
            hr = S_OK;
        }
        DeleteObject(bg_brush);
        return hr;
    }

    HRESULT PaintTextField2(blink::WebCanvas* canvas, const blink::IntRect& rect, blink::WebColor color) const {
        SkRect skrect = (blink::IntRect)rect;
        SkPaint paint;

        paint.setColor((SkColor)color);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRect(skrect, paint);

        paint.setColor(0xffA9A9A9);
        paint.setStrokeWidth(1);
        paint.setStyle(SkPaint::kStroke_Style);
        canvas->drawRect(skrect, paint);

        return S_OK;
    }

    void DrawRectAndBound(blink::WebCanvas* canvas, const blink::IntRect& rect, blink::WebColor bdColor, blink::WebColor boundColor) const {
        SkPaint paint;
        SkRect skrect = (blink::IntRect)rect;
        paint.setColor((SkColor)bdColor);
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawRect(skrect, paint);
        paint.setColor(boundColor);
        paint.setStrokeWidth(1);
        paint.setStyle(SkPaint::kStroke_Style);
        canvas->drawRect(skrect, paint);
    }

    HRESULT PaintButton2(blink::WebCanvas* canvas, HDC hdc,
        blink::WebThemeEngine::State state,
        const blink::WebThemeEngine::ButtonExtraParams& extra,
        int part_id,
        int state_id,
        RECT* rect) const {
        switch (part_id) {
        case BP_PUSHBUTTON:
            switch (state) {
            case blink::WebThemeEngine::StateDisabled:
                DrawRectAndBound(canvas, blink::winRectToIntRect(*rect), 0xffd4d0c8, 0xffA9A9A9);
                break;
            case blink::WebThemeEngine::StatePressed:
                DrawRectAndBound(canvas, blink::winRectToIntRect(*rect), 0xfff3f3f3, 0xffA9A9A9);
                break;
            case blink::WebThemeEngine::StateNormal:
                DrawRectAndBound(canvas, blink::winRectToIntRect(*rect), 0xffe3e3e3, 0xffA9A9A9);
                break;
            case blink::WebThemeEngine::StateHover:
                DrawRectAndBound(canvas, blink::winRectToIntRect(*rect), 0xffc3c3c3, 0xffA9A9A9);
                break;
            default:
                notImplemented();
                break;
            }
            break;
        }
        return S_OK;
    }

    HRESULT PaintButton(HDC hdc,
        blink::WebThemeEngine::State state,
        const blink::WebThemeEngine::ButtonExtraParams& extra,
        int part_id,
        int state_id,
        RECT* rect) const {
        HANDLE handle = GetThemeHandle(BUTTON);
        if (handle && draw_theme_)
            return draw_theme_(handle, hdc, part_id, state_id, rect, NULL);

        // Adjust classic_state based on part, state, and extras.
        int classic_state = 0; //  extra.classic_state;
        switch (part_id) {
        case BP_CHECKBOX:
            classic_state |= DFCS_BUTTONCHECK;
            break;
        case BP_RADIOBUTTON:
            classic_state |= DFCS_BUTTONRADIO;
            break;
        case BP_PUSHBUTTON:
            classic_state |= DFCS_BUTTONPUSH;
            break;
        default:
            notImplemented();
            break;
        }

        switch (state) {
        case blink::WebThemeEngine::StateDisabled:
            classic_state |= DFCS_INACTIVE;
            break;
        case blink::WebThemeEngine::StatePressed:
            classic_state |= DFCS_PUSHED;
            break;
        case blink::WebThemeEngine::StateNormal:
        case blink::WebThemeEngine::StateHover:
            break;
        default:
            notImplemented();
            break;
        }

        //if (extra.checked)
        //    classic_state |= DFCS_CHECKED;

        // Draw it manually.
        // All pressed states have both low bits set, and no other states do.
        const bool focused = ((state_id & ETS_FOCUSED) == ETS_FOCUSED);
        const bool pressed = ((state_id & PBS_PRESSED) == PBS_PRESSED);
        if ((BP_PUSHBUTTON == part_id) && (pressed || focused)) {
            // BP_PUSHBUTTON has a focus rect drawn around the outer edge, and the
            // button itself is shrunk by 1 pixel.
            HBRUSH brush = GetSysColorBrush(COLOR_3DDKSHADOW);
            if (brush) {
                FrameRect(hdc, rect, brush);
                InflateRect(rect, -1, -1);
            }
        }
        DrawFrameControl(hdc, rect, DFC_BUTTON, classic_state);

        // Draw the focus rectangle (the dotted line box) only on buttons.  For radio
        // and checkboxes, we let webkit draw the focus rectangle (orange glow).
        if ((BP_PUSHBUTTON == part_id) && focused) {
            // The focus rect is inside the button.  The exact number of pixels depends
            // on whether we're in classic mode or using uxtheme.
            if (handle && get_theme_content_rect_) {
                get_theme_content_rect_(handle, hdc, part_id, state_id, rect, rect);
            }
            else {
                InflateRect(rect, -GetSystemMetrics(SM_CXEDGE),
                    -GetSystemMetrics(SM_CYEDGE));
            }
            DrawFocusRect(hdc, rect);
        }

        // Classic theme doesn't support indeterminate checkboxes.  We draw
        // a recangle inside a checkbox like IE10 does.
        if (part_id == BP_CHECKBOX && extra.indeterminate) {
            RECT inner_rect = *rect;
            // "4 / 13" is same as IE10 in classic theme.
            int padding = (inner_rect.right - inner_rect.left) * 4 / 13;
            InflateRect(&inner_rect, -padding, -padding);
            int color_index = state == blink::WebThemeEngine::StateDisabled ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT;
            FillRect(hdc, &inner_rect, GetSysColorBrush(color_index));
        }

        return S_OK;
    }

    HRESULT PaintPushButton(blink::WebCanvas* canvas, HDC hdc,
        blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ButtonExtraParams& extra) const {
        int state_id;
        switch (state) {
        case blink::WebThemeEngine::StateDisabled:
            state_id = PBS_DISABLED;
            break;
        case blink::WebThemeEngine::StateHover:
            state_id = PBS_HOT;
            break;
        case blink::WebThemeEngine::StateNormal:
            state_id = extra.isDefault ? PBS_DEFAULTED : PBS_NORMAL;
            break;
        case blink::WebThemeEngine::StatePressed:
            state_id = PBS_PRESSED;
            break;
        default:
            notImplemented();
            break;
        }

        RECT rect_win = intRectToWinRect(rect);
        return PaintButton2(canvas, hdc, state, extra, BP_PUSHBUTTON, state_id, &rect_win);
    }

    HRESULT PaintScrollbarArrow(
        HDC hdc,
        blink::WebThemeEngine::Part part,
        const blink::WebThemeEngine::State& state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ScrollbarTrackExtraParams& extra) const
    {
        RECT rect_win = intRectToWinRect(rect);
//         static const int state_id_matrix[4][kMaxState] = {
//             ABS_DOWNDISABLED, ABS_DOWNHOT, ABS_DOWNNORMAL, ABS_DOWNPRESSED,
//             ABS_LEFTDISABLED, ABS_LEFTHOT, ABS_LEFTNORMAL, ABS_LEFTPRESSED,
//             ABS_RIGHTDISABLED, ABS_RIGHTHOT, ABS_RIGHTNORMAL, ABS_RIGHTPRESSED,
//             ABS_UPDISABLED, ABS_UPHOT, ABS_UPNORMAL, ABS_UPPRESSED
//         };
//         HANDLE handle = GetThemeHandle(SCROLLBAR);
//         
//         if (handle && draw_theme_) {
//             int index = part - kScrollbarDownArrow;
//             DCHECK(index >= 0 && index < 4);
//             int state_id = state_id_matrix[index][state];
// 
//             // Hovering means that the cursor is over the scroolbar, but not over the
//             // specific arrow itself.  We don't want to show it "hot" mode, but only
//             // in "hover" mode.
//             if (state == kHovered && extra.is_hovering) {
//                 switch (part) {
//                 case kScrollbarDownArrow:
//                     state_id = ABS_DOWNHOVER;
//                     break;
//                 case kScrollbarLeftArrow:
//                     state_id = ABS_LEFTHOVER;
//                     break;
//                 case kScrollbarRightArrow:
//                     state_id = ABS_RIGHTHOVER;
//                     break;
//                 case kScrollbarUpArrow:
//                     state_id = ABS_UPHOVER;
//                     break;
//                 default:
//                     NOTREACHED() << "Invalid part: " << part;
//                     break;
//                 }
//             }
//             return PaintScaledTheme(handle, hdc, SBP_ARROWBTN, state_id, rect);
//         }

        int classic_state = DFCS_SCROLLDOWN;
        switch (part) {
        case blink::WebThemeEngine::PartScrollbarDownArrow:
            classic_state = DFCS_SCROLLDOWN;
            break;
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
            classic_state = DFCS_SCROLLLEFT;
            break;
        case blink::WebThemeEngine::PartScrollbarRightArrow:
            classic_state = DFCS_SCROLLRIGHT;
            break;
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            classic_state = DFCS_SCROLLUP;
            break;
        default:
            notImplemented();
            break;
        }
        switch (state) {
        case blink::WebThemeEngine::StateDisabled:
            classic_state |= DFCS_INACTIVE;
            break;
        case blink::WebThemeEngine::StateHover:
            classic_state |= DFCS_HOT;
            break;
        case blink::WebThemeEngine::StateNormal:
            break;
        case blink::WebThemeEngine::StatePressed:
            classic_state |= DFCS_PUSHED;
            break;
        default:
            notImplemented();
            break;
        }
        DrawFrameControl(hdc, &rect_win, DFC_SCROLL, classic_state);
        return S_OK;
    }

    HRESULT PaintScrollbarThumb(
        blink::WebCanvas* canvas,
        HDC hdc,
        blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ScrollbarTrackExtraParams& extra
        ) const 
    {
        HANDLE handle = GetThemeHandle(SCROLLBAR);
        RECT rect_win = intRectToWinRect(rect);
        int part_id;
        int state_id;

        switch (part) {
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
            part_id = SBP_THUMBBTNHORZ;
            break;
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
            part_id = SBP_THUMBBTNVERT;
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
            part_id = SBP_GRIPPERHORZ;
            break;
        case blink::WebThemeEngine::PartScrollbarVerticalTrack:
            part_id = SBP_GRIPPERVERT;
            break;
        default:
            notImplemented();
            break;
        }

        switch (state) {
        case blink::WebThemeEngine::StateDisabled:
            state_id = SCRBS_DISABLED;
            break;
        case blink::WebThemeEngine::StateHover:
            state_id = /*extra.is_hovering ? SCRBS_HOVER :*/ SCRBS_HOT;
            break;
        case blink::WebThemeEngine::StateNormal:
            state_id = SCRBS_NORMAL;
            break;
        case blink::WebThemeEngine::StatePressed:
            state_id = SCRBS_PRESSED;
            break;
        default:
            notImplemented();
            break;
        }

//         if (handle && draw_theme_)
//             return PaintScaledTheme(handle, hdc, part_id, state_id, rect);

//         SkPaint paint;
//         paint.setARGB(0xff, 0xBC, 0xBC, 0xBC);
//         canvas->drawIRect(rect, paint);

        // Draw it manually.
        if ((part_id == SBP_THUMBBTNHORZ) || (part_id == SBP_THUMBBTNVERT))
            DrawEdge(hdc, &rect_win, EDGE_RAISED, BF_RECT | BF_MIDDLE);
        // Classic mode doesn't have a gripper.
        return S_OK;
    }

    HRESULT PaintScrollbarTrack(
        SkCanvas* canvas,
        HDC hdc,
        blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::ScrollbarTrackExtraParams& extra) const {
        HANDLE handle = GetThemeHandle(SCROLLBAR);
        RECT rect_win = intRectToWinRect(rect);
        int part_id;
        int state_id;

        switch (part) {
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
            part_id = extra.isBack ? SBP_UPPERTRACKHORZ : SBP_LOWERTRACKHORZ;
            break;
        case blink::WebThemeEngine::PartScrollbarVerticalTrack:
            part_id = extra.isBack ? SBP_UPPERTRACKVERT : SBP_LOWERTRACKVERT;
            break;
        default:
            notImplemented();
            break;
        }

        switch (state) {
        case blink::WebThemeEngine::StateDisabled:
            state_id = SCRBS_DISABLED;
            break;
        case blink::WebThemeEngine::StateHover:
            state_id = SCRBS_HOVER;
            break;
        case blink::WebThemeEngine::StateNormal:
            state_id = SCRBS_NORMAL;
            break;
        case blink::WebThemeEngine::StatePressed:
            state_id = SCRBS_PRESSED;
            break;
        default:
            notImplemented();
            break;
        }

        if (handle && draw_theme_)
            return draw_theme_(handle, hdc, part_id, state_id, &rect_win, NULL);

//         // Draw it manually.
//         if ((system_colors_[COLOR_SCROLLBAR] != system_colors_[COLOR_3DFACE]) &&
//             (system_colors_[COLOR_SCROLLBAR] != system_colors_[COLOR_WINDOW])) {
//             FillRect(hdc, &rect_win, reinterpret_cast<HBRUSH>(COLOR_SCROLLBAR + 1));
//         }
//         else {
//             SkPaint paint;
//             RECT align_rect = gfx::Rect(extra.track_x, extra.track_y, extra.track_width,
//                 extra.track_height).ToRECT();
//             SetCheckerboardShader(&paint, align_rect);
//             canvas->drawIRect(skia::RECTToSkIRect(rect_win), paint);
//         }
//         if (extra.classic_state & DFCS_PUSHED)
//             InvertRect(hdc, &rect_win);
        return S_OK;
    }

    HRESULT PaintMenuList(HDC hdc,
        blink::WebThemeEngine::State state,
        const blink::IntRect& rect,
        const blink::WebThemeEngine::MenuListExtraParams& extra) const {
        HANDLE handle = GetThemeHandle(MENULIST);
        RECT rect_win = intRectToWinRect(rect);
        int state_id;
        switch (state) {
        case blink::WebThemeEngine::StateNormal:
            state_id = CBXS_NORMAL;
            break;
        case blink::WebThemeEngine::StateDisabled:
            state_id = CBXS_DISABLED;
            break;
        case blink::WebThemeEngine::StateHover:
            state_id = CBXS_HOT;
            break;
        case blink::WebThemeEngine::StatePressed:
            state_id = CBXS_PRESSED;
            break;
        default:
            notImplemented();
            break;
        }

        if (handle && draw_theme_)
            return draw_theme_(handle, hdc, CP_DROPDOWNBUTTON, state_id, &rect_win,
            NULL);

        // Draw it manually.
        ::DrawFrameControl(hdc, &rect_win, DFC_SCROLL, DFCS_SCROLLCOMBOBOX /*| extra.classic_state*/);
        return S_OK;
    }

    HRESULT PaintScrollbarCorner(blink::WebCanvas* canvas, const blink::IntRect& rect) const {
        SkPaint paint;
        paint.setColor(SK_ColorWHITE);
        paint.setXfermodeMode(SkXfermode::kSrc_Mode);
        canvas->drawRect(rect, paint);
        return S_OK;
    }
    

    void PaintDirect(blink::WebCanvas* canvas,
        blink::WebThemeEngine::Part part,
        blink::WebThemeEngine::State state,
        const blink::WebRect& rect,
        const blink::WebThemeEngine::ExtraParams* extra) const {
        skia::ScopedPlatformPaint scoped_platform_paint(canvas);
        HDC hdc = scoped_platform_paint.GetPlatformSurface();
        blink::IntRect rc(rect.x, rect.y, rect.width, rect.height);

        switch (part) {
        case blink::WebThemeEngine::PartTextField:
            //PaintTextField(hdc, part, state, rc, extra->textField);
            PaintTextField2(canvas, rc, extra->textField.backgroundColor);
            break;
        case blink::WebThemeEngine::PartButton:
            PaintPushButton(canvas, hdc, part, state, rc, extra->button);
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalThumb:
        case blink::WebThemeEngine::PartScrollbarVerticalThumb:
            PaintScrollbarThumb(canvas, hdc, part, state, rc, extra->scrollbarTrack);
            break;
        case blink::WebThemeEngine::PartScrollbarDownArrow:
        case blink::WebThemeEngine::PartScrollbarLeftArrow:
        case blink::WebThemeEngine::PartScrollbarRightArrow:
        case blink::WebThemeEngine::PartScrollbarUpArrow:
            PaintScrollbarArrow(hdc, part, state, rc, extra->scrollbarTrack);
            break;
        case blink::WebThemeEngine::PartScrollbarHorizontalTrack:
        case blink::WebThemeEngine::PartScrollbarVerticalTrack:
            PaintScrollbarTrack(canvas, hdc, part, state, rc, extra->scrollbarTrack);
            break;
        case blink::WebThemeEngine::PartScrollbarCorner:
            PaintScrollbarCorner(canvas, rc);
            break;
        case blink::WebThemeEngine::PartMenuList:
            PaintMenuList(hdc, state, rc, extra->menuList);
            break;
        default:
            notImplemented();
        }
    }
};

} // content

#endif // NativeThemeWin_h