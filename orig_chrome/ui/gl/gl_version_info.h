// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_GL_VERSION_INFO_H_
#define UI_GL_GL_VERSION_INFO_H_

#include "base/basictypes.h"
#include "ui/gl/gl_export.h"
#include <set>
#include <string>

namespace gfx {

struct GL_EXPORT GLVersionInfo {
    GLVersionInfo(const char* version_str, const char* renderer_str,
        const char* extensions_str);

    GLVersionInfo(const char* version_str, const char* renderer_str,
        const std::set<std::string>& exts);

    bool IsAtLeastGL(unsigned major, unsigned minor) const
    {
        return !is_es && (major_version > major || (major_version == major && minor_version >= minor));
    }

    bool IsLowerThanGL(unsigned major, unsigned minor) const
    {
        return !is_es && (major_version < major || (major_version == major && minor_version < minor));
    }

    bool IsAtLeastGLES(unsigned major, unsigned minor) const
    {
        return is_es && (major_version > major || (major_version == major && minor_version >= minor));
    }

    bool BehavesLikeGLES() const
    {
        return is_es || is_desktop_core_profile;
    }

    bool IsES3Capable() const
    {
        if (IsAtLeastGLES(3, 0) || IsAtLeastGL(4, 2))
            return true;
#if defined(OS_MACOSX)
        // TODO(zmo): For experimentation purpose on MacOSX with core profile,
        // allow 3.2 or plus for now.
        if (IsAtLeastGL(3, 2))
            return true;
#endif
        return false;
    }

    static void ParseVersionString(const char* version_str,
        unsigned* major_version,
        unsigned* minor_version,
        bool* is_es,
        bool* is_es3);

    bool is_es;
    bool is_angle;
    unsigned major_version;
    unsigned minor_version;
    bool is_es3;
    bool is_desktop_core_profile;

private:
    GLVersionInfo(const char* version_str, const char* renderer_str);

    DISALLOW_COPY_AND_ASSIGN(GLVersionInfo);
};

} // namespace gfx

#endif // UI_GL_GL_VERSION_INFO_H_
