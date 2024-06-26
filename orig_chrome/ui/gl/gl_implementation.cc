// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_implementation.h"

#include <algorithm>
#include <string>

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/logging.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "ui/gl/gl_bindings.h"
#include "ui/gl/gl_gl_api_implementation.h"
#include "ui/gl/gl_version_info.h"

namespace gfx {

namespace {

    const struct {
        const char* name;
        GLImplementation implementation;
    } kGLImplementationNamePairs[] = {
        { kGLImplementationDesktopName, kGLImplementationDesktopGL },
        { kGLImplementationOSMesaName, kGLImplementationOSMesaGL },
#if defined(OS_MACOSX)
        { kGLImplementationAppleName, kGLImplementationAppleGL },
#endif
        { kGLImplementationEGLName, kGLImplementationEGLGLES2 },
        { kGLImplementationMockName, kGLImplementationMockGL }
    };

    typedef std::vector<base::NativeLibrary> LibraryArray;

    GLImplementation g_gl_implementation = kGLImplementationNone;
    LibraryArray* g_libraries;
    GLGetProcAddressProc g_get_proc_address;

    void CleanupNativeLibraries(void* unused)
    {
        if (g_libraries) {
            // We do not call base::UnloadNativeLibrary() for these libraries as
            // unloading libGL without closing X display is not allowed. See
            // crbug.com/250813 for details.
            delete g_libraries;
            g_libraries = NULL;
        }
    }

}

base::ThreadLocalPointer<GLApi>* g_current_gl_context_tls = NULL;
OSMESAApi* g_current_osmesa_context;

#if defined(OS_WIN)

EGLApi* g_current_egl_context;
WGLApi* g_current_wgl_context;

#elif defined(USE_X11)

EGLApi* g_current_egl_context;
GLXApi* g_current_glx_context;

#elif defined(USE_OZONE)

EGLApi* g_current_egl_context;

#elif defined(OS_ANDROID)

EGLApi* g_current_egl_context;

#endif

GLImplementation GetNamedGLImplementation(const std::string& name)
{
    for (size_t i = 0; i < arraysize(kGLImplementationNamePairs); ++i) {
        if (name == kGLImplementationNamePairs[i].name)
            return kGLImplementationNamePairs[i].implementation;
    }

    return kGLImplementationNone;
}

const char* GetGLImplementationName(GLImplementation implementation)
{
    for (size_t i = 0; i < arraysize(kGLImplementationNamePairs); ++i) {
        if (implementation == kGLImplementationNamePairs[i].implementation)
            return kGLImplementationNamePairs[i].name;
    }

    return "unknown";
}

void SetGLImplementation(GLImplementation implementation)
{
    g_gl_implementation = implementation;
}

GLImplementation GetGLImplementation()
{
    return g_gl_implementation;
}

bool HasDesktopGLFeatures()
{
    return kGLImplementationDesktopGL == g_gl_implementation || kGLImplementationDesktopGLCoreProfile == g_gl_implementation || kGLImplementationOSMesaGL == g_gl_implementation || kGLImplementationAppleGL == g_gl_implementation;
}

void AddGLNativeLibrary(base::NativeLibrary library)
{
    DCHECK(library);

    if (!g_libraries) {
        g_libraries = new LibraryArray;
        base::AtExitManager::RegisterCallback(CleanupNativeLibraries, NULL);
    }

    g_libraries->push_back(library);
}

void UnloadGLNativeLibraries()
{
    CleanupNativeLibraries(NULL);
}

void SetGLGetProcAddressProc(GLGetProcAddressProc proc)
{
    DCHECK(proc);
    g_get_proc_address = proc;
}

void* GetGLProcAddress(const char* name)
{
    DCHECK(g_gl_implementation != kGLImplementationNone);

    if (g_libraries) {
        for (size_t i = 0; i < g_libraries->size(); ++i) {
            void* proc = base::GetFunctionPointerFromNativeLibrary((*g_libraries)[i],
                name);
            if (proc)
                return proc;
        }
    }
    if (g_get_proc_address) {
        void* proc = g_get_proc_address(name);
        if (proc)
            return proc;
    }

    return NULL;
}

void InitializeNullDrawGLBindings()
{
    // This is platform independent, so it does not need to live in a platform
    // specific implementation file.
    InitializeNullDrawGLBindingsGL();
}

bool HasInitializedNullDrawGLBindings()
{
    return HasInitializedNullDrawGLBindingsGL();
}

std::string FilterGLExtensionList(
    const char* extensions,
    const std::vector<std::string>& disabled_extensions)
{
    if (extensions == NULL)
        return "";

    std::vector<std::string> extension_vec = base::SplitString(
        extensions, " ", base::TRIM_WHITESPACE, base::SPLIT_WANT_ALL);

    auto is_disabled = [&disabled_extensions](const std::string& ext) {
        return std::find(disabled_extensions.begin(), disabled_extensions.end(),
                   ext)
            != disabled_extensions.end();
    };
    extension_vec.erase(
        std::remove_if(extension_vec.begin(), extension_vec.end(), is_disabled),
        extension_vec.end());

    return base::JoinString(extension_vec, " ");
}

DisableNullDrawGLBindings::DisableNullDrawGLBindings()
{
    initial_enabled_ = SetNullDrawGLBindingsEnabledGL(false);
}

DisableNullDrawGLBindings::~DisableNullDrawGLBindings()
{
    SetNullDrawGLBindingsEnabledGL(initial_enabled_);
}

GLWindowSystemBindingInfo::GLWindowSystemBindingInfo()
    : direct_rendering(true)
{
}

std::string GetGLExtensionsFromCurrentContext()
{
    if (WillUseGLGetStringForExtensions()) {
        return reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    }

    std::vector<std::string> exts;
    GLint num_extensions = 0;
    glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
    for (GLint i = 0; i < num_extensions; ++i) {
        const char* extension = reinterpret_cast<const char*>(
            glGetStringi(GL_EXTENSIONS, i));
        DCHECK(extension != NULL);
        exts.push_back(extension);
    }
    return base::JoinString(exts, " ");
}

bool WillUseGLGetStringForExtensions()
{
    const char* version_str = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    unsigned major_version, minor_version;
    bool is_es, is_es3;
    gfx::GLVersionInfo::ParseVersionString(
        version_str, &major_version, &minor_version, &is_es, &is_es3);
    return is_es || major_version < 3;
}

} // namespace gfx
