// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_IMPLEMENTATION_H_
#define UI_GL_GL_IMPLEMENTATION_H_

#include <string>
#include <vector>

#include "base/native_library.h"
#include "build/build_config.h"
#include "ui/gl/gl_export.h"
#include "ui/gl/gl_switches.h"

namespace gfx {

class GLContext;

// The GL implementation currently in use.
enum GLImplementation {
    kGLImplementationNone,
    kGLImplementationDesktopGL,
    kGLImplementationDesktopGLCoreProfile,
    kGLImplementationOSMesaGL,
    kGLImplementationAppleGL,
    kGLImplementationEGLGLES2,
    kGLImplementationEGLGLES2SwiftShader,
    kGLImplementationMockGL
};

struct GL_EXPORT GLWindowSystemBindingInfo {
    GLWindowSystemBindingInfo();
    std::string vendor;
    std::string version;
    std::string extensions;
    bool direct_rendering;
};

void GL_EXPORT
GetAllowedGLImplementations(std::vector<GLImplementation>* impls);

#if defined(OS_WIN)
typedef void*(WINAPI* GLGetProcAddressProc)(const char* name);
#else
typedef void* (*GLGetProcAddressProc)(const char* name);
#endif

// Initialize a particular GL implementation.
GL_EXPORT bool InitializeStaticGLBindings(GLImplementation implementation);

// Initialize function bindings that depend on the context for a GL
// implementation.
GL_EXPORT bool InitializeDynamicGLBindings(GLImplementation implementation,
    GLContext* context);

// Initialize Debug logging wrappers for GL bindings.
void InitializeDebugGLBindings();

// Initialize stub methods for drawing operations in the GL bindings. The
// null draw bindings default to enabled, so that draw operations do nothing.
void InitializeNullDrawGLBindings();

// TODO(danakj): Remove this when all test suites are using null-draw.
GL_EXPORT bool HasInitializedNullDrawGLBindings();

// Filter a list of disabled_extensions from GL style space-separated
// extension_list, returning a space separated list of filtered extensions, in
// the same order as the input.
GL_EXPORT std::string FilterGLExtensionList(
    const char* extension_list,
    const std::vector<std::string>& disabled_extensions);

// Once initialized, instantiating this turns the stub methods for drawing
// operations off allowing drawing will occur while the object is alive.
class GL_EXPORT DisableNullDrawGLBindings {
public:
    DisableNullDrawGLBindings();
    ~DisableNullDrawGLBindings();

private:
    bool initial_enabled_;
};

GL_EXPORT void ClearGLBindings();

// Set the current GL implementation.
GL_EXPORT void SetGLImplementation(GLImplementation implementation);

// Get the current GL implementation.
GL_EXPORT GLImplementation GetGLImplementation();

// Does the underlying GL support all features from Desktop GL 2.0 that were
// removed from the ES 2.0 spec without requiring specific extension strings.
GL_EXPORT bool HasDesktopGLFeatures();

// Get the GL implementation with a given name.
GLImplementation GetNamedGLImplementation(const std::string& name);

// Get the name of a GL implementation.
const char* GetGLImplementationName(GLImplementation implementation);

// Add a native library to those searched for GL entry points.
void AddGLNativeLibrary(base::NativeLibrary library);

// Unloads all native libraries.
void UnloadGLNativeLibraries();

// Set an additional function that will be called to find GL entry points.
// Exported so that tests may set the function used in the mock implementation.
GL_EXPORT void SetGLGetProcAddressProc(GLGetProcAddressProc proc);

// Find an entry point in the current GL implementation. Note that the function
// may return a non-null pointer to something else than the GL function if an
// unsupported function is queried. Spec-compliant eglGetProcAddress and
// glxGetProcAddress are allowed to return garbage for unsupported functions,
// and when querying functions from the EGL library supplied by Android, it may
// return a function that prints a log message about the function being
// unsupported.
void* GetGLProcAddress(const char* name);

// Return information about the GL window system binding implementation (e.g.,
// EGL, GLX, WGL). Returns true if the information was retrieved successfully.
GL_EXPORT bool GetGLWindowSystemBindingInfo(GLWindowSystemBindingInfo* info);

// Helper for fetching the OpenGL extensions from the current context.
// This helper abstracts over differences between the desktop OpenGL
// core profile, and OpenGL ES and the compatibility profile.  It's
// intended for users of the bindings, not the implementation of the
// bindings themselves. This is a relatively expensive call, so
// callers should cache the result.
GL_EXPORT std::string GetGLExtensionsFromCurrentContext();

// Helper for the GL bindings implementation to understand whether
// glGetString(GL_EXTENSIONS) or glGetStringi(GL_EXTENSIONS, i) will
// be used in the function above.
GL_EXPORT bool WillUseGLGetStringForExtensions();

} // namespace gfx

#endif // UI_GL_GL_IMPLEMENTATION_H_
