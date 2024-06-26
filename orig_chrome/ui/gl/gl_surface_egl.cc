// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_surface_egl.h"

#if defined(OS_ANDROID)
#include <android/native_window_jni.h>
#endif

#include "base/command_line.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/metrics/histogram_macros.h"
#include "base/trace_event/trace_event.h"
#include "build/build_config.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gl/egl_util.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_surface_stub.h"
#include "ui/gl/gl_switches.h"
#include "ui/gl/scoped_make_current.h"
#include "ui/gl/sync_control_vsync_provider.h"

#if defined(USE_X11)
extern "C" {
#include <X11/Xlib.h>
}
#endif

#if defined(USE_OZONE)
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#endif

#if !defined(EGL_FIXED_SIZE_ANGLE)
#define EGL_FIXED_SIZE_ANGLE 0x3201
#endif

#if !defined(EGL_OPENGL_ES3_BIT)
#define EGL_OPENGL_ES3_BIT 0x00000040
#endif

// From ANGLE's egl/eglext.h.

#ifndef EGL_ANGLE_platform_angle
#define EGL_ANGLE_platform_angle 1
#define EGL_PLATFORM_ANGLE_ANGLE 0x3202
#define EGL_PLATFORM_ANGLE_TYPE_ANGLE 0x3203
#define EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE 0x3204
#define EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE 0x3205
#define EGL_PLATFORM_ANGLE_TYPE_DEFAULT_ANGLE 0x3206
#endif /* EGL_ANGLE_platform_angle */

#ifndef EGL_ANGLE_platform_angle_d3d
#define EGL_ANGLE_platform_angle_d3d 1
#define EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE 0x3207
#define EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE 0x3208
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE 0x3209
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_HARDWARE_ANGLE 0x320A
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE 0x320B
#define EGL_PLATFORM_ANGLE_DEVICE_TYPE_REFERENCE_ANGLE 0x320C
#endif /* EGL_ANGLE_platform_angle_d3d */

#ifndef EGL_ANGLE_platform_angle_opengl
#define EGL_ANGLE_platform_angle_opengl 1
#define EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE 0x320D
#define EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE 0x320E
#endif /* EGL_ANGLE_platform_angle_opengl */

using ui::GetLastEGLErrorString;

namespace gfx {

#if defined(OS_WIN)
unsigned int NativeViewGLSurfaceEGL::current_swap_generation_ = 0;
unsigned int NativeViewGLSurfaceEGL::swaps_this_generation_ = 0;
unsigned int NativeViewGLSurfaceEGL::last_multiswap_generation_ = 0;

const unsigned int MULTISWAP_FRAME_VSYNC_THRESHOLD = 60;
#endif

namespace {

    EGLConfig g_config;
    EGLDisplay g_display;
    EGLNativeDisplayType g_native_display;

    const char* g_egl_extensions = NULL;
    bool g_egl_create_context_robustness_supported = false;
    bool g_egl_sync_control_supported = false;
    bool g_egl_window_fixed_size_supported = false;
    bool g_egl_surfaceless_context_supported = false;

    class EGLSyncControlVSyncProvider
        : public gfx::SyncControlVSyncProvider {
    public:
        explicit EGLSyncControlVSyncProvider(EGLSurface surface)
            : SyncControlVSyncProvider()
            , surface_(surface)
        {
        }

        ~EGLSyncControlVSyncProvider() override { }

    protected:
        bool GetSyncValues(int64* system_time,
            int64* media_stream_counter,
            int64* swap_buffer_counter) override
        {
            uint64 u_system_time, u_media_stream_counter, u_swap_buffer_counter;
            bool result = eglGetSyncValuesCHROMIUM(
                              g_display, surface_, &u_system_time,
                              &u_media_stream_counter, &u_swap_buffer_counter)
                == EGL_TRUE;
            if (result) {
                *system_time = static_cast<int64>(u_system_time);
                *media_stream_counter = static_cast<int64>(u_media_stream_counter);
                *swap_buffer_counter = static_cast<int64>(u_swap_buffer_counter);
            }
            return result;
        }

        bool GetMscRate(int32* numerator, int32* denominator) override
        {
            return false;
        }

    private:
        EGLSurface surface_;

        DISALLOW_COPY_AND_ASSIGN(EGLSyncControlVSyncProvider);
    };

    EGLDisplay GetPlatformANGLEDisplay(EGLNativeDisplayType native_display,
        EGLenum platform_type,
        bool warpDevice)
    {
        std::vector<EGLint> display_attribs;

        display_attribs.push_back(EGL_PLATFORM_ANGLE_TYPE_ANGLE);
        display_attribs.push_back(platform_type);

        if (warpDevice) {
            display_attribs.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE);
            display_attribs.push_back(EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE);
        }

        display_attribs.push_back(EGL_NONE);

        return eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE,
            reinterpret_cast<void*>(native_display),
            &display_attribs[0]);
    }

    EGLDisplay GetDisplayFromType(DisplayType display_type,
        EGLNativeDisplayType native_display)
    {
        switch (display_type) {
        case DEFAULT:
        case SWIFT_SHADER:
            return eglGetDisplay(native_display);
        case ANGLE_WARP:
            return GetPlatformANGLEDisplay(native_display,
                EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE, true);
        case ANGLE_D3D9:
            return GetPlatformANGLEDisplay(native_display,
                EGL_PLATFORM_ANGLE_TYPE_D3D9_ANGLE, false);
        case ANGLE_D3D11:
            return GetPlatformANGLEDisplay(
                native_display, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE, false);
        case ANGLE_OPENGL:
            return GetPlatformANGLEDisplay(
                native_display, EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE, false);
        case ANGLE_OPENGLES:
            return GetPlatformANGLEDisplay(
                native_display, EGL_PLATFORM_ANGLE_TYPE_OPENGLES_ANGLE, false);
        default:
            NOTREACHED();
            return EGL_NO_DISPLAY;
        }
    }

    const char* DisplayTypeString(DisplayType display_type)
    {
        switch (display_type) {
        case DEFAULT:
            return "Default";
        case SWIFT_SHADER:
            return "SwiftShader";
        case ANGLE_WARP:
            return "WARP";
        case ANGLE_D3D9:
            return "D3D9";
        case ANGLE_D3D11:
            return "D3D11";
        case ANGLE_OPENGL:
            return "OpenGL";
        case ANGLE_OPENGLES:
            return "OpenGLES";
        default:
            NOTREACHED();
            return "Err";
        }
    }

} // namespace

void GetEGLInitDisplays(bool supports_angle_d3d,
    bool supports_angle_opengl,
    const base::CommandLine* command_line,
    std::vector<DisplayType>* init_displays)
{
    // SwiftShader does not use the platform extensions
    if (command_line->GetSwitchValueASCII(switches::kUseGL) == kGLImplementationSwiftShaderName) {
        init_displays->push_back(SWIFT_SHADER);
        return;
    }

    std::string requested_renderer = command_line->GetSwitchValueASCII(switches::kUseANGLE);

    if (supports_angle_d3d) {
        bool use_angle_default = !command_line->HasSwitch(switches::kUseANGLE) || requested_renderer == kANGLEImplementationDefaultName;

        if (use_angle_default) {
            // Default mode for ANGLE - try D3D11, else try D3D9
            if (!command_line->HasSwitch(switches::kDisableD3D11)) {
                init_displays->push_back(ANGLE_D3D11);
            }
            init_displays->push_back(ANGLE_D3D9);
        } else {
            if (requested_renderer == kANGLEImplementationD3D11Name) {
                init_displays->push_back(ANGLE_D3D11);
            }
            if (requested_renderer == kANGLEImplementationD3D9Name) {
                init_displays->push_back(ANGLE_D3D9);
            }
            if (requested_renderer == kANGLEImplementationWARPName) {
                init_displays->push_back(ANGLE_WARP);
            }
        }
    }

    if (supports_angle_opengl) {
        if (requested_renderer == kANGLEImplementationOpenGLName) {
            init_displays->push_back(ANGLE_OPENGL);
        }
        if (requested_renderer == kANGLEImplementationOpenGLESName) {
            init_displays->push_back(ANGLE_OPENGLES);
        }
    }

    // If no displays are available due to missing angle extensions or invalid
    // flags, request the default display.
    if (init_displays->empty()) {
        init_displays->push_back(DEFAULT);
    }
}

GLSurfaceEGL::GLSurfaceEGL() { }

bool GLSurfaceEGL::InitializeOneOff()
{
    static bool initialized = false;
    if (initialized)
        return true;

    InitializeDisplay();
    if (g_display == EGL_NO_DISPLAY)
        return false;

    // Choose an EGL configuration.
    // On X this is only used for PBuffer surfaces.
    EGLint renderable_type = EGL_OPENGL_ES2_BIT;
    //   if (base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kEnableUnsafeES3APIs)) {
    //     renderable_type = EGL_OPENGL_ES3_BIT;
    //   }
    const EGLint kConfigAttribs[] = {
        EGL_BUFFER_SIZE, 32,
        EGL_ALPHA_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_RENDERABLE_TYPE, renderable_type,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
        EGL_NONE
    };

#if defined(USE_OZONE)
    const EGLint* config_attribs = ui::OzonePlatform::GetInstance()
                                       ->GetSurfaceFactoryOzone()
                                       ->GetEGLSurfaceProperties(kConfigAttribs);
#else
    const EGLint* config_attribs = kConfigAttribs;
#endif

    EGLint num_configs;
    if (!eglChooseConfig(g_display,
            config_attribs,
            NULL,
            0,
            &num_configs)) {
        LOG(ERROR) << "eglChooseConfig failed with error "
                   << GetLastEGLErrorString();
        return false;
    }

    if (num_configs == 0) {
        LOG(ERROR) << "No suitable EGL configs found.";
        return false;
    }

    if (!eglChooseConfig(g_display,
            config_attribs,
            &g_config,
            1,
            &num_configs)) {
        LOG(ERROR) << "eglChooseConfig failed with error "
                   << GetLastEGLErrorString();
        return false;
    }

    g_egl_extensions = eglQueryString(g_display, EGL_EXTENSIONS);
    g_egl_create_context_robustness_supported = HasEGLExtension("EGL_EXT_create_context_robustness");
    g_egl_sync_control_supported = HasEGLExtension("EGL_CHROMIUM_sync_control");
    g_egl_window_fixed_size_supported = HasEGLExtension("EGL_ANGLE_window_fixed_size");

    // TODO(oetuaho@nvidia.com): Surfaceless is disabled on Android as a temporary
    // workaround, since code written for Android WebView takes different paths
    // based on whether GL surface objects have underlying EGL surface handles,
    // conflicting with the use of surfaceless. See https://crbug.com/382349
#if defined(OS_ANDROID)
    DCHECK(!g_egl_surfaceless_context_supported);
#else
    // Check if SurfacelessEGL is supported.
    g_egl_surfaceless_context_supported = HasEGLExtension("EGL_KHR_surfaceless_context");
    if (g_egl_surfaceless_context_supported) {
        // EGL_KHR_surfaceless_context is supported but ensure
        // GL_OES_surfaceless_context is also supported. We need a current context
        // to query for supported GL extensions.
        scoped_refptr<GLSurface> surface = new SurfacelessEGL(Size(1, 1));
        scoped_refptr<GLContext> context = GLContext::CreateGLContext(
            NULL, surface.get(), PreferIntegratedGpu);
        if (!context->MakeCurrent(surface.get()))
            g_egl_surfaceless_context_supported = false;

        // Ensure context supports GL_OES_surfaceless_context.
        if (g_egl_surfaceless_context_supported) {
            g_egl_surfaceless_context_supported = context->HasExtension(
                "GL_OES_surfaceless_context");
            context->ReleaseCurrent(surface.get());
        }
    }
#endif

    initialized = true;

    return true;
}

EGLDisplay GLSurfaceEGL::GetDisplay()
{
    return g_display;
}

EGLDisplay GLSurfaceEGL::GetHardwareDisplay()
{
    return g_display;
}

EGLNativeDisplayType GLSurfaceEGL::GetNativeDisplay()
{
    return g_native_display;
}

const char* GLSurfaceEGL::GetEGLExtensions()
{
    return g_egl_extensions;
}

bool GLSurfaceEGL::HasEGLExtension(const char* name)
{
    return ExtensionsContain(GetEGLExtensions(), name);
}

bool GLSurfaceEGL::IsCreateContextRobustnessSupported()
{
    return g_egl_create_context_robustness_supported;
}

bool GLSurfaceEGL::IsEGLSurfacelessContextSupported()
{
    return g_egl_surfaceless_context_supported;
}

GLSurfaceEGL::~GLSurfaceEGL() { }

// InitializeDisplay is necessary because the static binding code
// needs a full Display init before it can query the Display extensions.
// static
EGLDisplay GLSurfaceEGL::InitializeDisplay()
{
    if (g_display != EGL_NO_DISPLAY) {
        return g_display;
    }

    g_native_display = GetPlatformDefaultEGLNativeDisplay();

    // If EGL_EXT_client_extensions not supported this call to eglQueryString
    // will return NULL.
    const char* client_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    bool supports_angle_d3d = false;
    bool supports_angle_opengl = false;
    // Check for availability of ANGLE extensions.
    if (client_extensions && ExtensionsContain(client_extensions, "EGL_ANGLE_platform_angle")) {
        supports_angle_d3d = ExtensionsContain(client_extensions, "EGL_ANGLE_platform_angle_d3d");
        supports_angle_opengl = ExtensionsContain(client_extensions, "EGL_ANGLE_platform_angle_opengl");
    }

    std::vector<DisplayType> init_displays;
    GetEGLInitDisplays(supports_angle_d3d, supports_angle_opengl,
        base::CommandLine::ForCurrentProcess(), &init_displays);

    for (size_t disp_index = 0; disp_index < init_displays.size(); ++disp_index) {
        DisplayType display_type = init_displays[disp_index];
        EGLDisplay display = GetDisplayFromType(display_type, g_native_display);
        if (display == EGL_NO_DISPLAY) {
            LOG(ERROR) << "EGL display query failed with error "
                       << GetLastEGLErrorString();
        }

        if (!eglInitialize(display, nullptr, nullptr)) {
            bool is_last = disp_index == init_displays.size() - 1;

            LOG(ERROR) << "eglInitialize " << DisplayTypeString(display_type)
                       << " failed with error " << GetLastEGLErrorString()
                       << (is_last ? "" : ", trying next display type");
        } else {
            UMA_HISTOGRAM_ENUMERATION("GPU.EGLDisplayType", display_type,
                DISPLAY_TYPE_MAX);
            g_display = display;
            break;
        }
    }

    return g_display;
}

NativeViewGLSurfaceEGL::NativeViewGLSurfaceEGL(EGLNativeWindowType window)
    : window_(window)
    , surface_(NULL)
    , supports_post_sub_buffer_(false)
    , config_(NULL)
    , size_(1, 1)
    , swap_interval_(1)
{
#if defined(OS_ANDROID)
    if (window)
        ANativeWindow_acquire(window);
#endif

#if defined(OS_WIN)
    vsync_override_ = false;
    swap_generation_ = 0;
    RECT windowRect;
    if (GetClientRect(window_, &windowRect))
        size_ = gfx::Rect(windowRect).size();
#endif
}

bool NativeViewGLSurfaceEGL::Initialize()
{
    return Initialize(nullptr);
}

bool NativeViewGLSurfaceEGL::Initialize(
    scoped_ptr<VSyncProvider> sync_provider)
{
    DCHECK(!surface_);

    if (!GetDisplay()) {
        LOG(ERROR) << "Trying to create surface with invalid display.";
        return false;
    }

    std::vector<EGLint> egl_window_attributes;

    if (g_egl_window_fixed_size_supported) {
        egl_window_attributes.push_back(EGL_FIXED_SIZE_ANGLE);
        egl_window_attributes.push_back(EGL_TRUE);
        egl_window_attributes.push_back(EGL_WIDTH);
        egl_window_attributes.push_back(size_.width());
        egl_window_attributes.push_back(EGL_HEIGHT);
        egl_window_attributes.push_back(size_.height());
    }

    if (gfx::g_driver_egl.ext.b_EGL_NV_post_sub_buffer) {
        egl_window_attributes.push_back(EGL_POST_SUB_BUFFER_SUPPORTED_NV);
        egl_window_attributes.push_back(EGL_TRUE);
    }

    egl_window_attributes.push_back(EGL_NONE);
    // Create a surface for the native window.
    surface_ = eglCreateWindowSurface(
        GetDisplay(), GetConfig(), window_, &egl_window_attributes[0]);

    if (!surface_) {
        LOG(ERROR) << "eglCreateWindowSurface failed with error "
                   << GetLastEGLErrorString();
        Destroy();
        return false;
    }

    if (gfx::g_driver_egl.ext.b_EGL_NV_post_sub_buffer) {
        EGLint surfaceVal;
        EGLBoolean retVal = eglQuerySurface(
            GetDisplay(), surface_, EGL_POST_SUB_BUFFER_SUPPORTED_NV, &surfaceVal);
        supports_post_sub_buffer_ = (surfaceVal && retVal) == EGL_TRUE;
    }

    if (sync_provider)
        vsync_provider_.reset(sync_provider.release());
    else if (g_egl_sync_control_supported)
        vsync_provider_.reset(new EGLSyncControlVSyncProvider(surface_));
    return true;
}

void NativeViewGLSurfaceEGL::Destroy()
{
    if (surface_) {
        if (!eglDestroySurface(GetDisplay(), surface_)) {
            LOG(ERROR) << "eglDestroySurface failed with error "
                       << GetLastEGLErrorString();
        }
        surface_ = NULL;
    }
}

EGLConfig NativeViewGLSurfaceEGL::GetConfig()
{
#if !defined(USE_X11)
    return g_config;
#else
    if (!config_) {
        // Get a config compatible with the window
        DCHECK(window_);
        XWindowAttributes win_attribs;
        if (!XGetWindowAttributes(GetNativeDisplay(), window_, &win_attribs)) {
            return NULL;
        }

        // Try matching the window depth with an alpha channel,
        // because we're worried the destination alpha width could
        // constrain blending precision.
        const int kBufferSizeOffset = 1;
        const int kAlphaSizeOffset = 3;
        EGLint config_attribs[] = {
            EGL_BUFFER_SIZE, ~0,
            EGL_ALPHA_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PBUFFER_BIT,
            EGL_NONE
        };
        config_attribs[kBufferSizeOffset] = win_attribs.depth;

        EGLint num_configs;
        if (!eglChooseConfig(g_display,
                config_attribs,
                &config_,
                1,
                &num_configs)) {
            LOG(ERROR) << "eglChooseConfig failed with error "
                       << GetLastEGLErrorString();
            return NULL;
        }

        if (num_configs) {
            EGLint config_depth;
            if (!eglGetConfigAttrib(g_display,
                    config_,
                    EGL_BUFFER_SIZE,
                    &config_depth)) {
                LOG(ERROR) << "eglGetConfigAttrib failed with error "
                           << GetLastEGLErrorString();
                return NULL;
            }

            if (config_depth == win_attribs.depth) {
                return config_;
            }
        }

        // Try without an alpha channel.
        config_attribs[kAlphaSizeOffset] = 0;
        if (!eglChooseConfig(g_display,
                config_attribs,
                &config_,
                1,
                &num_configs)) {
            LOG(ERROR) << "eglChooseConfig failed with error "
                       << GetLastEGLErrorString();
            return NULL;
        }

        if (num_configs == 0) {
            LOG(ERROR) << "No suitable EGL configs found.";
            return NULL;
        }
    }
    return config_;
#endif
}

bool NativeViewGLSurfaceEGL::IsOffscreen()
{
    return false;
}

gfx::SwapResult NativeViewGLSurfaceEGL::SwapBuffers()
{
    TRACE_EVENT2("gpu", "NativeViewGLSurfaceEGL:RealSwapBuffers",
        "width", GetSize().width(),
        "height", GetSize().height());

#if defined(OS_WIN)
    if (swap_interval_ != 0) {
        // This code is a simple way of enforcing that we only vsync if one surface
        // is swapping per frame. This provides single window cases a stable refresh
        // while allowing multi-window cases to not slow down due to multiple syncs
        // on a single thread. A better way to fix this problem would be to have
        // each surface present on its own thread.

        if (current_swap_generation_ == swap_generation_) {
            if (swaps_this_generation_ > 1)
                last_multiswap_generation_ = current_swap_generation_;
            swaps_this_generation_ = 0;
            current_swap_generation_++;
        }

        swap_generation_ = current_swap_generation_;

        if (swaps_this_generation_ != 0 || (current_swap_generation_ - last_multiswap_generation_ < MULTISWAP_FRAME_VSYNC_THRESHOLD)) {
            // Override vsync settings and switch it off
            if (!vsync_override_) {
                eglSwapInterval(GetDisplay(), 0);
                vsync_override_ = true;
            }
        } else if (vsync_override_) {
            // Only one window swapping, so let the normal vsync setting take over
            eglSwapInterval(GetDisplay(), swap_interval_);
            vsync_override_ = false;
        }

        swaps_this_generation_++;
    }
#endif

    if (!eglSwapBuffers(GetDisplay(), surface_)) {
        DVLOG(1) << "eglSwapBuffers failed with error "
                 << GetLastEGLErrorString();
        return gfx::SwapResult::SWAP_FAILED;
    }

    return gfx::SwapResult::SWAP_ACK;
}

gfx::Size NativeViewGLSurfaceEGL::GetSize()
{
    EGLint width;
    EGLint height;
    if (!eglQuerySurface(GetDisplay(), surface_, EGL_WIDTH, &width) || !eglQuerySurface(GetDisplay(), surface_, EGL_HEIGHT, &height)) {
        NOTREACHED() << "eglQuerySurface failed with error "
                     << GetLastEGLErrorString();
        return gfx::Size();
    }

    return gfx::Size(width, height);
}

bool NativeViewGLSurfaceEGL::Resize(const gfx::Size& size, float scale_factor)
{
    if (size == GetSize())
        return true;

    size_ = size;

    scoped_ptr<ui::ScopedMakeCurrent> scoped_make_current;
    GLContext* current_context = GLContext::GetCurrent();
    bool was_current = current_context && current_context->IsCurrent(this);
    if (was_current) {
        scoped_make_current.reset(
            new ui::ScopedMakeCurrent(current_context, this));
        current_context->ReleaseCurrent(this);
    }

    Destroy();

    if (!Initialize()) {
        LOG(ERROR) << "Failed to resize window.";
        return false;
    }

    return true;
}

bool NativeViewGLSurfaceEGL::Recreate()
{
    Destroy();
    if (!Initialize()) {
        LOG(ERROR) << "Failed to create surface.";
        return false;
    }
    return true;
}

EGLSurface NativeViewGLSurfaceEGL::GetHandle()
{
    return surface_;
}

bool NativeViewGLSurfaceEGL::SupportsPostSubBuffer()
{
    return supports_post_sub_buffer_;
}

gfx::SwapResult NativeViewGLSurfaceEGL::PostSubBuffer(int x,
    int y,
    int width,
    int height)
{
    DCHECK(supports_post_sub_buffer_);
    if (!eglPostSubBufferNV(GetDisplay(), surface_, x, y, width, height)) {
        DVLOG(1) << "eglPostSubBufferNV failed with error "
                 << GetLastEGLErrorString();
        return gfx::SwapResult::SWAP_FAILED;
    }
    return gfx::SwapResult::SWAP_ACK;
}

VSyncProvider* NativeViewGLSurfaceEGL::GetVSyncProvider()
{
    return vsync_provider_.get();
}

void NativeViewGLSurfaceEGL::OnSetSwapInterval(int interval)
{
    swap_interval_ = interval;
}

NativeViewGLSurfaceEGL::~NativeViewGLSurfaceEGL()
{
    Destroy();
#if defined(OS_ANDROID)
    if (window_)
        ANativeWindow_release(window_);
#endif
}

PbufferGLSurfaceEGL::PbufferGLSurfaceEGL(const gfx::Size& size)
    : size_(size)
    , surface_(NULL)
{
    // Some implementations of Pbuffer do not support having a 0 size. For such
    // cases use a (1, 1) surface.
    if (size_.GetArea() == 0)
        size_.SetSize(1, 1);
}

bool PbufferGLSurfaceEGL::Initialize()
{
    EGLSurface old_surface = surface_;

    EGLDisplay display = GetDisplay();
    if (!display) {
        LOG(ERROR) << "Trying to create surface with invalid display.";
        return false;
    }

    // Allocate the new pbuffer surface before freeing the old one to ensure
    // they have different addresses. If they have the same address then a
    // future call to MakeCurrent might early out because it appears the current
    // context and surface have not changed.
    const EGLint pbuffer_attribs[] = {
        EGL_WIDTH, size_.width(),
        EGL_HEIGHT, size_.height(),
        EGL_NONE
    };

    EGLSurface new_surface = eglCreatePbufferSurface(display,
        GetConfig(),
        pbuffer_attribs);
    if (!new_surface) {
        LOG(ERROR) << "eglCreatePbufferSurface failed with error "
                   << GetLastEGLErrorString();
        return false;
    }

    if (old_surface)
        eglDestroySurface(display, old_surface);

    surface_ = new_surface;
    return true;
}

void PbufferGLSurfaceEGL::Destroy()
{
    if (surface_) {
        if (!eglDestroySurface(GetDisplay(), surface_)) {
            LOG(ERROR) << "eglDestroySurface failed with error "
                       << GetLastEGLErrorString();
        }
        surface_ = NULL;
    }
}

EGLConfig PbufferGLSurfaceEGL::GetConfig()
{
    return g_config;
}

bool PbufferGLSurfaceEGL::IsOffscreen()
{
    return true;
}

gfx::SwapResult PbufferGLSurfaceEGL::SwapBuffers()
{
    NOTREACHED() << "Attempted to call SwapBuffers on a PbufferGLSurfaceEGL.";
    return gfx::SwapResult::SWAP_FAILED;
}

gfx::Size PbufferGLSurfaceEGL::GetSize()
{
    return size_;
}

bool PbufferGLSurfaceEGL::Resize(const gfx::Size& size, float scale_factor)
{
    if (size == size_)
        return true;

    scoped_ptr<ui::ScopedMakeCurrent> scoped_make_current;
    GLContext* current_context = GLContext::GetCurrent();
    bool was_current = current_context && current_context->IsCurrent(this);
    if (was_current) {
        scoped_make_current.reset(
            new ui::ScopedMakeCurrent(current_context, this));
    }

    size_ = size;

    if (!Initialize()) {
        LOG(ERROR) << "Failed to resize pbuffer.";
        return false;
    }

    return true;
}

EGLSurface PbufferGLSurfaceEGL::GetHandle()
{
    return surface_;
}

void* PbufferGLSurfaceEGL::GetShareHandle()
{
#if defined(OS_ANDROID)
    NOTREACHED();
    return NULL;
#else
    if (!gfx::g_driver_egl.ext.b_EGL_ANGLE_query_surface_pointer)
        return NULL;

    if (!gfx::g_driver_egl.ext.b_EGL_ANGLE_surface_d3d_texture_2d_share_handle)
        return NULL;

    void* handle;
    if (!eglQuerySurfacePointerANGLE(g_display,
            GetHandle(),
            EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE,
            &handle)) {
        return NULL;
    }

    return handle;
#endif
}

PbufferGLSurfaceEGL::~PbufferGLSurfaceEGL()
{
    Destroy();
}

SurfacelessEGL::SurfacelessEGL(const gfx::Size& size)
    : size_(size)
{
}

bool SurfacelessEGL::Initialize()
{
    return true;
}

void SurfacelessEGL::Destroy()
{
}

EGLConfig SurfacelessEGL::GetConfig()
{
    return g_config;
}

bool SurfacelessEGL::IsOffscreen()
{
    return true;
}

bool SurfacelessEGL::IsSurfaceless() const
{
    return true;
}

gfx::SwapResult SurfacelessEGL::SwapBuffers()
{
    LOG(ERROR) << "Attempted to call SwapBuffers with SurfacelessEGL.";
    return gfx::SwapResult::SWAP_FAILED;
}

gfx::Size SurfacelessEGL::GetSize()
{
    return size_;
}

bool SurfacelessEGL::Resize(const gfx::Size& size, float scale_factor)
{
    size_ = size;
    return true;
}

EGLSurface SurfacelessEGL::GetHandle()
{
    return EGL_NO_SURFACE;
}

void* SurfacelessEGL::GetShareHandle()
{
    return NULL;
}

SurfacelessEGL::~SurfacelessEGL()
{
}

} // namespace gfx
