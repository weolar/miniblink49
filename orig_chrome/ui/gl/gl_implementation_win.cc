// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <vector>

#include "base/at_exit.h"
#include "base/base_paths.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/native_library.h"
#include "base/path_service.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"
#include "base/trace_event/trace_event.h"
#include "base/win/windows_version.h"
// TODO(jmadill): Apply to all platforms eventually
#include "ui/gl/angle_platform_impl.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_context_stub_with_extensions.h"
#include "ui/gl/gl_egl_api_implementation.h"
#include "ui/gl/gl_gl_api_implementation.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_osmesa_api_implementation.h"
#include "ui/gl/gl_surface_wgl.h"
#include "ui/gl/gl_wgl_api_implementation.h"

#if defined(ENABLE_SWIFTSHADER)
#include "software_renderer.h"
#endif

//#include <blpwtk2_products.h>
#include <shlwapi.h>

namespace gfx {

namespace {

    const wchar_t kD3DCompiler[] = L"D3DCompiler_47.dll";

    void GL_BINDING_CALL MarshalClearDepthToClearDepthf(GLclampd depth)
    {
        glClearDepthf(static_cast<GLclampf>(depth));
    }

    void GL_BINDING_CALL MarshalDepthRangeToDepthRangef(GLclampd z_near,
        GLclampd z_far)
    {
        glDepthRangef(static_cast<GLclampf>(z_near), static_cast<GLclampf>(z_far));
    }

    bool LoadD3DXLibrary(const base::FilePath& module_path,
        const base::FilePath::StringType& name)
    {
        base::NativeLibrary library = base::LoadNativeLibrary(base::FilePath(name), NULL);
        if (!library) {
            library = base::LoadNativeLibrary(module_path.Append(name), NULL);
            if (!library) {
                DVLOG(1) << name << " not found.";
                return false;
            }
        }
        return true;
    }

    // TODO(jmadill): Apply to all platforms eventually
    base::LazyInstance<ANGLEPlatformImpl> g_angle_platform_impl = LAZY_INSTANCE_INITIALIZER;

    ANGLEPlatformShutdownFunc g_angle_platform_shutdown = nullptr;

} // namespace

void GetAllowedGLImplementations(std::vector<GLImplementation>* impls)
{
    impls->push_back(kGLImplementationEGLGLES2);
    impls->push_back(kGLImplementationDesktopGL);
    impls->push_back(kGLImplementationOSMesaGL);
}

// const wchar_t* glesv2Path = L"E:\\mycode\\chromium.bb-master\\src\\out\\Debug\\blpcr_glesv2.dll";
// const wchar_t* eglPath = L"E:\\mycode\\chromium.bb-master\\src\\out\\Debug\\blpcr_egl.dll";

// const wchar_t* glesv2Path = L"G:\\m75_xp\\src\\out\\release\\swiftshader\\libGLESv2.dll";
// const wchar_t* eglPath = L"G:\\m75_xp\\src\\out\\release\\swiftshader\\libEGL.dll";

const wchar_t* kGlesv2Path = L"plugins\\egl\\libGLESv2.dll";
const wchar_t* kEglPath = L"plugins\\egl\\libEGL.dll";

const wchar_t* kSwiftShaderGlesv2Path = L"plugins\\swiftshader\\libGLESv2.dll";
const wchar_t* kSwiftShaderEglPath = L"plugins\\swiftshader\\libEGL.dll";

static bool InitEGLGLES2Library(base::NativeLibrary gles_library, base::NativeLibrary egl_library)
{
    ANGLEPlatformInitializeFunc angle_platform_init = (ANGLEPlatformInitializeFunc)(base::GetFunctionPointerFromNativeLibrary(gles_library, "ANGLEPlatformInitialize"));
    if (angle_platform_init) {
        angle_platform_init(&g_angle_platform_impl.Get());
    }
    g_angle_platform_shutdown = reinterpret_cast<ANGLEPlatformShutdownFunc>(base::GetFunctionPointerFromNativeLibrary(gles_library, "ANGLEPlatformShutdown"));

    GLGetProcAddressProc get_proc_address = reinterpret_cast<GLGetProcAddressProc>(base::GetFunctionPointerFromNativeLibrary(egl_library, "eglGetProcAddress"));
    if (!get_proc_address) {
        OutputDebugStringA("eglGetProcAddress not found.\n");
        return false;
    }

    SetGLGetProcAddressProc(get_proc_address);
    AddGLNativeLibrary(egl_library);
    AddGLNativeLibrary(gles_library);
    SetGLImplementation(kGLImplementationEGLGLES2);

    InitializeStaticGLBindingsGL();
    InitializeStaticGLBindingsEGL();

    // These two functions take single precision float rather than double
    // precision float parameters in GLES.
    ::gfx::g_driver_gl.fn.glClearDepthFn = MarshalClearDepthToClearDepthf;
    ::gfx::g_driver_gl.fn.glDepthRangeFn = MarshalDepthRangeToDepthRangef;

    OutputDebugStringA("InitEGLGLES2Library ok\n");
    return true;
}

static bool LoadEgles2LibraryImpl(const wchar_t* glesv2_name, const wchar_t* egl_name)
{
    std::vector<wchar_t> module_buffer;
    module_buffer.resize(MAX_PATH);
    ::GetModuleFileNameW(nullptr, &module_buffer[0], MAX_PATH);
    ::PathRemoveFileSpecW(&module_buffer[0]);

    base::FilePath module_path(&module_buffer[0]);

    if (!LoadD3DXLibrary(module_path, kD3DCompiler)) {
        OutputDebugStringA("D3DCompiler_47.dll not found\n");
    }

    base::FilePath glesv2_path = module_path.Append(glesv2_name);
    base::NativeLibrary gles_library = base::LoadNativeLibrary(glesv2_path, NULL);
    if (!gles_library) {
        OutputDebugStringA("GLESV2_DLL not found\n");
        return false;
    }

    base::FilePath egl_path = module_path.Append(egl_name);
    base::NativeLibrary egl_library = base::LoadNativeLibrary(egl_path, NULL);
    if (!egl_library) {
        OutputDebugStringA("EGL_DLL not found\n");
        return false;
    }

    return InitEGLGLES2Library(gles_library, egl_library);
}

static bool LoadSwitfShaderLibrary()
{
    return LoadEgles2LibraryImpl(kSwiftShaderGlesv2Path, kSwiftShaderEglPath);
}

static bool LoadEgles2Library()
{
    if (LoadEgles2LibraryImpl(kGlesv2Path, kEglPath))
        return true;

    return LoadSwitfShaderLibrary();
}

bool InitializeStaticGLBindings(GLImplementation implementation)
{
    // Prevent reinitialization with a different implementation. Once the gpu
    // unit tests have initialized with kGLImplementationMock, we don't want to
    // later switch to another GL implementation.
    DCHECK_EQ(kGLImplementationNone, GetGLImplementation());

    // Allow the main thread or another to initialize these bindings
    // after instituting restrictions on I/O. Going forward they will
    // likely be used in the browser process on most platforms. The
    // one-time initialization cost is small, between 2 and 5 ms.
    base::ThreadRestrictions::ScopedAllowIO allow_io;

    switch (implementation) {
    case kGLImplementationOSMesaGL: {
        //       base::FilePath module_path;
        //       PathService::Get(base::DIR_MODULE, &module_path);
        //       base::NativeLibrary library = base::LoadNativeLibrary(
        //           module_path.Append(L"osmesa.dll"), NULL);
        //       if (!library) {
        //         PathService::Get(base::DIR_EXE, &module_path);
        //         library = base::LoadNativeLibrary(
        //             module_path.Append(L"osmesa.dll"), NULL);
        //         if (!library) {
        //           DVLOG(1) << "osmesa.dll not found";
        //           return false;
        //         }
        //       }
        //
        //       GLGetProcAddressProc get_proc_address =
        //           reinterpret_cast<GLGetProcAddressProc>(
        //               base::GetFunctionPointerFromNativeLibrary(
        //                   library, "OSMesaGetProcAddress"));
        //       if (!get_proc_address) {
        //         DLOG(ERROR) << "OSMesaGetProcAddress not found.";
        //         base::UnloadNativeLibrary(library);
        //         return false;
        //       }
        //
        //       SetGLGetProcAddressProc(get_proc_address);
        //       AddGLNativeLibrary(library);
        //       SetGLImplementation(kGLImplementationOSMesaGL);
        //
        //       InitializeStaticGLBindingsGL();
        //       InitializeStaticGLBindingsOSMESA();
        DebugBreak();
        break;
    }
    case kGLImplementationEGLGLES2: {
        return LoadEgles2Library();
        //       base::FilePath module_path;
        //       if (!PathService::Get(base::DIR_MODULE, &module_path))
        //         return false;
        //
        //       // Attempt to load the D3DX shader compiler using the default search path
        //       // and if that fails, using an absolute path. This is to ensure these DLLs
        //       // are loaded before ANGLE is loaded in case they are not in the default
        //       // search path.
        //       LoadD3DXLibrary(module_path, kD3DCompiler);
        //
        //       base::FilePath gles_path;
        //       const base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
        //       bool using_swift_shader = command_line->GetSwitchValueASCII(switches::kUseGL) == "swiftshader";
        //       if (using_swift_shader) {
        //         if (!command_line->HasSwitch(switches::kSwiftShaderPath))
        //           return false;
        //         gles_path = command_line->GetSwitchValuePath(switches::kSwiftShaderPath);
        //         // Preload library
        //         LoadLibrary(L"ddraw.dll");
        //       } else {
        //         gles_path = module_path;
        //       }
        //
        //       // Load libglesv2.dll before libegl.dll because the latter is dependent on
        //       // the former and if there is another version of libglesv2.dll in the dll
        //       // search path, it will get loaded instead.
        //       //base::NativeLibrary gles_library = base::LoadNativeLibrary(gles_path.AppendASCII(BLPCR_GLESV2_DLL_NAME), NULL);
        //       base::NativeLibrary gles_library = base::LoadNativeLibrary(base::FilePath(glesv2Path), NULL);
        //       if (!gles_library) {
        //         LOG(WARNING) << BLPCR_GLESV2_DLL_NAME << " not found";
        //         return false;
        //       }
        //
        //       // When using EGL, first try eglGetProcAddress and then Windows
        //       // GetProcAddress on both the EGL and GLES2 DLLs.
        //       //base::NativeLibrary egl_library = base::LoadNativeLibrary(gles_path.AppendASCII(BLPCR_EGL_DLL_NAME), NULL);
        //       base::NativeLibrary egl_library = base::LoadNativeLibrary(base::FilePath(eglPath), NULL);
        //       if (!egl_library) {
        //         LOG(WARNING) << BLPCR_EGL_DLL_NAME << " not found.";
        //         base::UnloadNativeLibrary(gles_library);
        //         return false;
        //       }
        //
        // #if defined(ENABLE_SWIFTSHADER)
        //       if (using_swift_shader) {
        //         SetupSoftwareRenderer(gles_library);
        //       }
        // #endif
        //
        //       if (!using_swift_shader) {
        //         // Init ANGLE platform here, before we call GetPlatformDisplay().
        //         // TODO(jmadill): Apply to all platforms eventually
        //         ANGLEPlatformInitializeFunc angle_platform_init =
        //             reinterpret_cast<ANGLEPlatformInitializeFunc>(base::GetFunctionPointerFromNativeLibrary(gles_library, "ANGLEPlatformInitialize"));
        //         if (angle_platform_init) {
        //           angle_platform_init(&g_angle_platform_impl.Get());
        //
        //           g_angle_platform_shutdown = reinterpret_cast<ANGLEPlatformShutdownFunc>(base::GetFunctionPointerFromNativeLibrary(gles_library, "ANGLEPlatformShutdown"));
        //         }
        //       }
        //
        //       GLGetProcAddressProc get_proc_address = reinterpret_cast<GLGetProcAddressProc>(base::GetFunctionPointerFromNativeLibrary(egl_library, "eglGetProcAddress"));
        //       if (!get_proc_address) {
        //         LOG(ERROR) << "eglGetProcAddress not found.";
        //         base::UnloadNativeLibrary(egl_library);
        //         base::UnloadNativeLibrary(gles_library);
        //         return false;
        //       }
        //
        //       SetGLGetProcAddressProc(get_proc_address);
        //       AddGLNativeLibrary(egl_library);
        //       AddGLNativeLibrary(gles_library);
        //       SetGLImplementation(kGLImplementationEGLGLES2);
        //
        //       InitializeStaticGLBindingsGL();
        //       InitializeStaticGLBindingsEGL();
        //
        //       // These two functions take single precision float rather than double
        //       // precision float parameters in GLES.
        //       ::gfx::g_driver_gl.fn.glClearDepthFn = MarshalClearDepthToClearDepthf;
        //       ::gfx::g_driver_gl.fn.glDepthRangeFn = MarshalDepthRangeToDepthRangef;
        break;
    }
    case kGLImplementationEGLGLES2SwiftShader:
        return LoadSwitfShaderLibrary();
        break;
    case kGLImplementationDesktopGL: {
        //       base::NativeLibrary library = base::LoadNativeLibrary(
        //           base::FilePath(L"opengl32.dll"), NULL);
        //       if (!library) {
        //         DVLOG(1) << "opengl32.dll not found";
        //         return false;
        //       }
        //
        //       GLGetProcAddressProc get_proc_address =
        //           reinterpret_cast<GLGetProcAddressProc>(
        //               base::GetFunctionPointerFromNativeLibrary(
        //                   library, "wglGetProcAddress"));
        //       if (!get_proc_address) {
        //         LOG(ERROR) << "wglGetProcAddress not found.";
        //         base::UnloadNativeLibrary(library);
        //         return false;
        //       }
        //
        //       SetGLGetProcAddressProc(get_proc_address);
        //       AddGLNativeLibrary(library);
        //       SetGLImplementation(kGLImplementationDesktopGL);
        //
        //       // Initialize GL surface and get some functions needed for the context
        //       // creation below.
        //       if (!GLSurfaceWGL::InitializeOneOff()) {
        //         LOG(ERROR) << "GLSurfaceWGL::InitializeOneOff failed.";
        //         return false;
        //       }
        //       wglCreateContextProc wglCreateContextFn =
        //           reinterpret_cast<wglCreateContextProc>(
        //               GetGLProcAddress("wglCreateContext"));
        //       wglDeleteContextProc wglDeleteContextFn =
        //           reinterpret_cast<wglDeleteContextProc>(
        //               GetGLProcAddress("wglDeleteContext"));
        //       wglMakeCurrentProc wglMakeCurrentFn =
        //           reinterpret_cast<wglMakeCurrentProc>(
        //               GetGLProcAddress("wglMakeCurrent"));
        //
        //       // Create a temporary GL context to bind to entry points. This is needed
        //       // because wglGetProcAddress is specified to return NULL for all queries
        //       // if a context is not current in MSDN documentation, and the static
        //       // bindings may contain functions that need to be queried with
        //       // wglGetProcAddress. OpenGL wiki further warns that other error values
        //       // than NULL could also be returned from wglGetProcAddress on some
        //       // implementations, so we need to clear the WGL bindings and reinitialize
        //       // them after the context creation.
        //       HGLRC gl_context = wglCreateContextFn(GLSurfaceWGL::GetDisplayDC());
        //       if (!gl_context) {
        //         LOG(ERROR) << "Failed to create temporary context.";
        //         return false;
        //       }
        //       if (!wglMakeCurrentFn(GLSurfaceWGL::GetDisplayDC(), gl_context)) {
        //         LOG(ERROR) << "Failed to make temporary GL context current.";
        //         wglDeleteContextFn(gl_context);
        //         return false;
        //       }
        //
        //       InitializeStaticGLBindingsGL();
        //       InitializeStaticGLBindingsWGL();
        //
        //       wglMakeCurrent(NULL, NULL);
        //       wglDeleteContext(gl_context);
        DebugBreak();

        break;
    }
    case kGLImplementationMockGL: {
        SetGLImplementation(kGLImplementationMockGL);
        InitializeStaticGLBindingsGL();
        break;
    }
    default:
        return false;
    }

    return true;
}

bool InitializeDynamicGLBindings(GLImplementation implementation,
    GLContext* context)
{
    switch (implementation) {
    case kGLImplementationOSMesaGL:
    case kGLImplementationEGLGLES2:
    case kGLImplementationDesktopGL:
        InitializeDynamicGLBindingsGL(context);
        break;
    case kGLImplementationMockGL:
        if (!context) {
            scoped_refptr<GLContextStubWithExtensions> mock_context(
                new GLContextStubWithExtensions());
            mock_context->SetGLVersionString("3.0");
            InitializeDynamicGLBindingsGL(mock_context.get());
        } else
            InitializeDynamicGLBindingsGL(context);
        break;
    default:
        return false;
    }

    return true;
}

void InitializeDebugGLBindings()
{
    InitializeDebugGLBindingsEGL();
    InitializeDebugGLBindingsGL();
    //   InitializeDebugGLBindingsOSMESA();
    //   InitializeDebugGLBindingsWGL();
}

void ClearGLBindings()
{
    // TODO(jmadill): Apply to all platforms eventually
    if (g_angle_platform_shutdown) {
        g_angle_platform_shutdown();
    }

    ClearGLBindingsEGL();
    ClearGLBindingsGL();
    //   ClearGLBindingsOSMESA();
    //   ClearGLBindingsWGL();
    SetGLImplementation(kGLImplementationNone);
    UnloadGLNativeLibraries();
}

bool GetGLWindowSystemBindingInfo(GLWindowSystemBindingInfo* info)
{
    switch (GetGLImplementation()) {
    case kGLImplementationDesktopGL:
        //return GetGLWindowSystemBindingInfoWGL(info);
        DebugBreak();
        return false;
    case kGLImplementationEGLGLES2:
        return GetGLWindowSystemBindingInfoEGL(info);
    default:
        return false;
    }
}

} // namespace gfx
