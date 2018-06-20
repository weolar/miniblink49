// Copyright (c) 2017 weolar, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "node/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "gin/object_template_builder.h"
#include "gin/dictionary.h"
#include <windows.h>
#include "base/strings/string_util.h"

namespace {

void setEmptyRECT(RECT* rc) {
    rc->left = 0;
    rc->top = 0;
    rc->right = 0;
    rc->bottom = 0;
}

RECT scaleToEnclosingRect(const RECT& rect, float x_scale, float y_scale) {
    if (x_scale == 1.f && y_scale == 1.f)
        return rect;

    // These next functions cast instead of using e.g. ToFlooredInt() because we
    // haven't checked to ensure that the clamping behavior of the helper
    // functions doesn't degrade performance, and callers shouldn't be passing
    // values that cause overflow anyway.
//     DCHECK(base::IsValueInRangeForNumericType<int>(std::floor(rect.x() * x_scale)));
//     DCHECK(base::IsValueInRangeForNumericType<int>(std::floor(rect.y() * y_scale)));
//     DCHECK(base::IsValueInRangeForNumericType<int>(std::ceil(rect.right() * x_scale)));
//     DCHECK(base::IsValueInRangeForNumericType<int>(std::ceil(rect.bottom() * y_scale)));
    int x = static_cast<int>(std::floor(rect.left * x_scale));
    int y = static_cast<int>(std::floor(rect.right * y_scale));
    int r = rect.left == rect.right ? x : static_cast<int>(std::ceil(rect.right * x_scale));
    int b = rect.top == rect.bottom ? y : static_cast<int>(std::ceil(rect.bottom * y_scale));
    RECT rc = { x, y, r, b };
    return rc;
}

class Display final {
public:
    // Screen Rotation in clock-wise degrees.
    // This enum corresponds to DisplayRotationDefaultProto::Rotation in
    // chrome/browser/chromeos/policy/proto/chrome_device_policy.proto.
    enum Rotation {
        ROTATE_0 = 0,
        ROTATE_90,
        ROTATE_180,
        ROTATE_270,
    };

    // The display rotation can have multiple causes for change. A user can set a
    // preference. On devices with accelerometers, they can change the rotation.
    // RotationSource allows for the tracking of a Rotation per source of the
    // change. ROTATION_SOURCE_ACTIVE is the current rotation of the display.
    // Rotation changes not due to an accelerometer, nor the user, are to use this
    // source directly. ROTATION_SOURCE_UNKNOWN is when no rotation source has
    // been provided.
    enum RotationSource {
        ROTATION_SOURCE_ACCELEROMETER = 0,
        ROTATION_SOURCE_ACTIVE,
        ROTATION_SOURCE_USER,
        ROTATION_SOURCE_UNKNOWN,
    };

    // Touch support for the display.
    enum TouchSupport {
        TOUCH_SUPPORT_UNKNOWN,
        TOUCH_SUPPORT_AVAILABLE,
        TOUCH_SUPPORT_UNAVAILABLE,
    };

    Display() 
        : Display(-1) {
    }

    Display(uint32_t id)
        : m_id(id) {
        setEmptyRECT(&m_screenRect);
        setEmptyRECT(&m_screenWorkRect);
        setEmptyRECT(&m_workArea);
        setEmptyRECT(&m_bounds);
    }

    uint32_t getId() const { return m_id; }
    Rotation getRotation() const { return m_rotation; }
    RECT getScreenRect() const { return m_screenRect; }
    RECT getScreenWorkRect() const { return m_screenWorkRect; }
    RECT getWorkArea() const { return m_workArea; }
    RECT getBounds() const { return m_bounds; }
    float getDeviceScaleFactor() const { m_deviceScaleFactor; }

    void setRotation(Rotation rotation) { m_rotation = rotation; }
    void setScreenRect(const RECT& screenRect) { m_screenRect = screenRect; }
    void setScreenWorkRect(const RECT& screenWorkRect) { m_screenWorkRect = screenWorkRect; }
    void setWorkArea(const RECT& workArea) { m_workArea = workArea; }
    void setBounds(const RECT& bounds) { m_bounds = bounds; }
    void setDeviceScaleFactor(float deviceScaleFactor) { m_deviceScaleFactor = deviceScaleFactor; }

private:
    uint32_t m_id;
    Rotation m_rotation;
    RECT m_screenRect;
    RECT m_screenWorkRect;
    RECT m_workArea;
    RECT m_bounds;
    float m_deviceScaleFactor;
};

class DisplayInfo {
public:
    DisplayInfo(const MONITORINFOEX& monitor_info, float device_scale_factor)
        : DisplayInfo(monitor_info, device_scale_factor, GetRotationForDevice(monitor_info.szDevice)) {
    }

    DisplayInfo(const MONITORINFOEX& monitor_info, float device_scale_factor, Display::Rotation rotation)
        : id_(DeviceIdFromDeviceName(monitor_info.szDevice))
        , rotation_(rotation)
        , screen_rect_(monitor_info.rcMonitor)
        , screen_work_rect_(monitor_info.rcWork)
        , device_scale_factor_(device_scale_factor) {
    }

    static uint32_t getHash(const char* str, uint32_t len) {
        uint32_t hash = 1315423911;
        uint32_t i = 0;

        for (i = 0; i < len; str++, i++) {
            hash ^= ((hash << 5) + (*str) + (hash >> 2));
        }

        return hash;
    }

    // static
    static uint32_t DeviceIdFromDeviceName(const wchar_t* deviceName) {
        std::string deviceNameA = base::WideToUTF8(deviceName);
        return static_cast<uint32_t>(getHash(deviceNameA.c_str(), deviceNameA.length()));
    }

    static Display::Rotation GetRotationForDevice(const wchar_t* device_name) {
        DEVMODE mode;
        ::ZeroMemory(&mode, sizeof(mode));
        mode.dmSize = sizeof(mode);
        mode.dmDriverExtra = 0;
        if (::EnumDisplaySettings(device_name, ENUM_CURRENT_SETTINGS, &mode)) {
            switch (mode.dmDisplayOrientation) {
            case DMDO_DEFAULT:
                return Display::ROTATE_0;
            case DMDO_90:
                return Display::ROTATE_90;
            case DMDO_180:
                return Display::ROTATE_180;
            case DMDO_270:
                return Display::ROTATE_270;
            default:
                ::DebugBreak();
            }
        }
        return Display::ROTATE_0;
    }

    uint32_t id() const { return id_; }
    Display::Rotation rotation() const { return rotation_; }
    const RECT& screen_rect() const { return screen_rect_; }
    const RECT& screen_work_rect() const { return screen_work_rect_; }
    float device_scale_factor() const { return device_scale_factor_; }

private:
    uint32_t id_;
    Display::Rotation rotation_;
    RECT screen_rect_;
    RECT screen_work_rect_;
    float device_scale_factor_;
};

// A display used by display::ScreenWin.
// It holds a display and additional parameters used for DPI calculations.
class ScreenWinDisplay {
public:
    ScreenWinDisplay() {
        setEmptyRECT(&pixel_bounds_);
    }

    //explicit ScreenWinDisplay(const DisplayInfo& display_info);
    ScreenWinDisplay(const Display& display, const DisplayInfo& display_info) 
        : display_(display)
        , pixel_bounds_(display_info.screen_rect()) {

    }

    const Display& display() const { return display_; }
    const RECT& pixel_bounds() const { return pixel_bounds_; }

private:
    Display display_;
    RECT pixel_bounds_;
};

class ScreenWin {
public:
    void Initialize() {
        updateFromDisplayInfos(getDisplayInfosFromSystem());
    }

    static float getMonitorScaleFactor(HMONITOR monitor) {
        return 1.0f;
    }

    static MONITORINFOEX monitorInfoFromHMONITOR(HMONITOR monitor) {
        MONITORINFOEX monitor_info;
        ::ZeroMemory(&monitor_info, sizeof(monitor_info));
        monitor_info.cbSize = sizeof(monitor_info);
        ::GetMonitorInfo(monitor, &monitor_info);
        return monitor_info;
    }


    static BOOL CALLBACK EnumMonitorCallback(HMONITOR monitor, HDC hdc, LPRECT rect, LPARAM data) {
        std::vector<DisplayInfo>* display_infos = reinterpret_cast<std::vector<DisplayInfo>*>(data);
        DCHECK(display_infos);
        display_infos->push_back(DisplayInfo(monitorInfoFromHMONITOR(monitor), getMonitorScaleFactor(monitor)));
        return TRUE;
    }

    static std::vector<DisplayInfo> getDisplayInfosFromSystem() {
        std::vector<DisplayInfo> display_infos;
        ::EnumDisplayMonitors(nullptr, nullptr, EnumMonitorCallback, reinterpret_cast<LPARAM>(&display_infos));
        DCHECK(static_cast<size_t>(::GetSystemMetrics(SM_CMONITORS)) == display_infos.size());
        return display_infos;
    }

    MONITORINFOEX monitorInfoFromWindow(HWND hwnd, DWORD default_options) const {
        return monitorInfoFromHMONITOR(::MonitorFromWindow(hwnd, default_options));
    }

    Display createDisplayFromDisplayInfo(const DisplayInfo& display_info) {
        Display display(display_info.id());
        float scale_factor = display_info.device_scale_factor();
        display.setDeviceScaleFactor(scale_factor);
        display.setWorkArea(scaleToEnclosingRect(display_info.screen_work_rect(), 1.0f / scale_factor, 1.0f / scale_factor));
        display.setBounds(scaleToEnclosingRect(display_info.screen_rect(), 1.0f / scale_factor, 1.0f / scale_factor));
        display.setRotation(display_info.rotation());
        return display;
    }
    
    // Windows historically has had a hard time handling displays of DPIs higher
    // than 96. Handling multiple DPI displays means we have to deal with Windows'
    // monitor physical coordinates and map into Chrome's DIP coordinates.
    //
    // To do this, DisplayInfosToScreenWinDisplays reasons over monitors as a tree
    // using the primary monitor as the root. All monitors touching this root are
    // considered a children.
    //
    // This also presumes that all monitors are connected components. Windows, by UI
    // construction restricts the layout of monitors to connected components except
    // when DPI virtualization is happening. When this happens, we scale relative
    // to (0, 0).
    //
    // Note that this does not handle cases where a scaled display may have
    // insufficient room to lay out its children. In these cases, a DIP point could
    // map to multiple screen points due to overlap. The first discovered screen
    // will take precedence.
    std::vector<ScreenWinDisplay> DisplayInfosToScreenWinDisplays(const std::vector<DisplayInfo>& display_infos) {
        // Layout and create the ScreenWinDisplays.
        std::vector<Display> displays;
        for (const auto& display_info : display_infos)
            displays.push_back(createDisplayFromDisplayInfo(display_info));

        std::vector<ScreenWinDisplay> screen_win_displays;
        const size_t num_displays = display_infos.size();
        for (size_t i = 0; i < num_displays; ++i)
            screen_win_displays.push_back(ScreenWinDisplay(displays[i], display_infos[i]));

        return screen_win_displays;
    }


    void updateFromDisplayInfos(const std::vector<DisplayInfo>& display_infos) {
        screen_win_displays_ = DisplayInfosToScreenWinDisplays(display_infos);
    }

    ScreenWinDisplay getScreenWinDisplay(const MONITORINFOEX& monitor_info) const {
        uint32_t id = DisplayInfo::DeviceIdFromDeviceName(monitor_info.szDevice);
        std::vector<ScreenWinDisplay>::const_iterator screen_win_display = screen_win_displays_.begin();
        for (; screen_win_display != screen_win_displays_.end(); ++screen_win_display) {
            if (screen_win_display->display().getId() == id)
                return *screen_win_display;
        }
        // There is 1:1 correspondence between MONITORINFOEX and ScreenWinDisplay.
        // If we make it here, it means we have no displays and we should hand out the
        // default display.
        DCHECK(screen_win_displays_.size() == 0u);
        return ScreenWinDisplay();
    }

    ScreenWinDisplay getPrimaryScreenWinDisplay() const {
        MONITORINFOEX monitor_info = monitorInfoFromWindow(nullptr, MONITOR_DEFAULTTOPRIMARY);
        ScreenWinDisplay screen_win_display = getScreenWinDisplay(monitor_info);
        Display display = screen_win_display.display();
        // The Windows primary monitor is defined to have an origin of (0, 0).
//         DCHECK(0 == display.bounds().origin().x());
//         DCHECK(0 == display.bounds().origin().y());
        return screen_win_display;
    }

    std::vector<Display> screenWinDisplaysToDisplays(const std::vector<ScreenWinDisplay>& screen_win_displays) const {
        std::vector<Display> displays;
        for (const auto& screen_win_display : screen_win_displays)
            displays.push_back(screen_win_display.display());

        return displays;
    }

    std::vector<Display> getAllDisplays() const {
        return screenWinDisplaysToDisplays(screen_win_displays_);
    }

private:
    std::vector<ScreenWinDisplay> screen_win_displays_;
};

class Screen : public mate::EventEmitter<Screen> {
public:
    explicit Screen(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Screen>::InitWith(isolate, wrapper);
        m_screen.Initialize();
    }

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target) {
        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "Screen"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("getCursorScreenPoint", &Screen::getCursorScreenPointApi);
        builder.SetMethod("getPrimaryDisplay", &Screen::getPrimaryDisplayApi);
        builder.SetMethod("getAllDisplays", &Screen::getAllDisplaysApi);
        builder.SetMethod("getDisplayNearestPoint", &Screen::getDisplayNearestPointApi);
        builder.SetMethod("getDisplayMatching", &Screen::getDisplayMatchingApi);

        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "Screen"), prototype->GetFunction());
    }

    void nullFunction() {
    }

    void getCursorScreenPointApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        POINT pt;
        ::GetCursorPos(&pt);
//         gfx::Point cursor_pos_pixels(pt);
//         return ScreenToDIPPoint(cursor_pos_pixels);
        base::DictionaryValue point;
        point.SetInteger("x", pt.x);
        point.SetInteger("y", pt.y);
        v8::Local<v8::Value> v8Value = gin::Converter<base::DictionaryValue>::ToV8(args.GetIsolate(), point);
        args.GetReturnValue().Set(v8Value);
    }

    void getPrimaryDisplayApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        //return screen_->GetPrimaryDisplay();
        ScreenWinDisplay screenWinDisplay = m_screen.getPrimaryScreenWinDisplay();
        base::DictionaryValue* display = createDisplayDictionaryValue(screenWinDisplay.display());

        v8::Local<v8::Value> v8Value = gin::Converter<base::DictionaryValue>::ToV8(args.GetIsolate(), *display);
        args.GetReturnValue().Set(v8Value);
        delete display;
    }

    static base::DictionaryValue* createRectDictionaryValue(const RECT& rc) {
        base::DictionaryValue* out = new base::DictionaryValue();
        out->SetInteger("x", rc.left);
        out->SetInteger("y", rc.top);
        out->SetInteger("width", rc.right - rc.left);
        out->SetInteger("height", rc.bottom - rc.top);
        return out;
    }

    static base::DictionaryValue* createSizeDictionaryValue(const SIZE& size) {
        base::DictionaryValue* out = new base::DictionaryValue();
        out->SetInteger("width", size.cx);
        out->SetInteger("height", size.cy);
        return out;
    }

    static base::DictionaryValue* createDisplayDictionaryValue(const Display& display) {
        // id Integer - 与display 相关的唯一性标志.
        // rotation Integer - 可以是 0, 1, 2, 3, 每个代表了屏幕旋转的度数 0, 90, 180, 270.
        // scaleFactor Number - Output device's pixel scale factor.
        // touchSupport String - 可以是 available, unavailable, unknown.
        // bounds Object
        // size Object
        // workArea Object
        // workAreaSize Object
        base::DictionaryValue* out = new base::DictionaryValue();
        out->SetInteger("id", display.getId());
        out->SetInteger("rotation", display.getRotation());
        out->SetDouble("scaleFactor", 1);
        out->SetString("touchSupport", "unavailable");
        out->Set("bounds", createRectDictionaryValue(display.getBounds()));

        RECT bounds = display.getBounds();
        SIZE size = { bounds.right - bounds.left, bounds.bottom - bounds.top };
        out->Set("size", createSizeDictionaryValue(size));
        out->Set("workArea", createRectDictionaryValue(display.getWorkArea()));

        RECT workArea = display.getWorkArea();
        SIZE workAreaSize = { workArea.right - workArea.left, workArea.bottom - workArea.top };
        out->Set("workAreaSize", createSizeDictionaryValue(workAreaSize));
        return out;
    }

    void getAllDisplaysApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        std::vector<Display> display = m_screen.getAllDisplays();

        base::ListValue displays;
       
        for (std::vector<Display>::const_iterator it = display.begin(); it != display.end(); ++it) {
            base::DictionaryValue* displayDictionaryValue = createDisplayDictionaryValue(*it);
            displays.Append(displayDictionaryValue);
        }
        
        v8::Local<v8::Value> v8Value = gin::Converter<base::ListValue>::ToV8(args.GetIsolate(), displays);
        args.GetReturnValue().Set(v8Value);
    }

    // const gfx::Point& point
    void getDisplayNearestPointApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        //return screen_->GetDisplayNearestPoint(point);
        getPrimaryDisplayApi(args);
    }

    // const gfx::Rect& match_rect
    void getDisplayMatchingApi(const v8::FunctionCallbackInfo<v8::Value>& args) {
        //return screen_->GetDisplayMatching(match_rect);
        getPrimaryDisplayApi(args);
    }

    void onDisplayAdded(const Display& new_display) {
        //Emit("display-added", new_display);
    }

    void onDisplayRemoved(const Display& old_display) {
        //Emit("display-removed", old_display);
    }

    void OnDisplayMetricsChanged(const Display& display, uint32_t changed_metrics) {
        //Emit("display-metrics-changed", display, MetricsToArray(changed_metrics));
    }
    
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();
        if (args.IsConstructCall()) {
            new Screen(isolate, args.This());
            args.GetReturnValue().Set(args.This());
            return;
        }
    }

public:
    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;

    ScreenWin m_screen;
};

v8::Persistent<v8::Function> Screen::constructor;
gin::WrapperInfo Screen::kWrapperInfo = { gin::kEmbedderNativeGin };

void initializeScreenApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> target, v8::Local<v8::Context> context, void* priv) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Screen::init(env->isolate(), exports);
}

}  // namespace

static const char CommonScreenNative[] = "console.log('CommonScreenNative');;";
static NodeNative nativeCommonScreenNative{ "Screen", CommonScreenNative, sizeof(CommonScreenNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_common_screen, initializeScreenApi, &nativeCommonScreenNative)
