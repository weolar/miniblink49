// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/gl_version_info.h"

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/string_util.h"

namespace {

bool DesktopCoreCommonCheck(
    bool is_es, unsigned major_version, unsigned minor_version)
{
    return (!is_es && ((major_version == 3 && minor_version >= 2) || major_version > 3));
}

}

namespace gfx {

GLVersionInfo::GLVersionInfo(const char* version_str, const char* renderer_str,
    const char* extensions_str)
    : GLVersionInfo(version_str, renderer_str)
{
    is_desktop_core_profile = DesktopCoreCommonCheck(is_es, major_version, minor_version) && !strstr(extensions_str, "GL_ARB_compatibility");
}

GLVersionInfo::GLVersionInfo(const char* version_str, const char* renderer_str,
    const std::set<std::string>& extensions)
    : GLVersionInfo(version_str, renderer_str)
{
    is_desktop_core_profile = DesktopCoreCommonCheck(is_es, major_version, minor_version) && extensions.find("GL_ARB_compatibility") == extensions.end();
}

GLVersionInfo::GLVersionInfo(const char* version_str, const char* renderer_str)
    : is_es(false)
    , is_angle(false)
    , major_version(0)
    , minor_version(0)
    , is_es3(false)
    , is_desktop_core_profile(false)
{
    if (version_str) {
        ParseVersionString(version_str, &major_version, &minor_version,
            &is_es, &is_es3);
    }
    if (renderer_str) {
        is_angle = base::StartsWith(renderer_str, "ANGLE",
            base::CompareCase::SENSITIVE);
    }
}

void GLVersionInfo::ParseVersionString(const char* version_str,
    unsigned* major_version,
    unsigned* minor_version,
    bool* is_es,
    bool* is_es3)
{
    // Make sure the outputs are always initialized.
    *major_version = 0;
    *minor_version = 0;
    *is_es = false;
    *is_es3 = false;
    if (!version_str)
        return;
    std::string lstr(base::ToLowerASCII(version_str));
    *is_es = (lstr.length() > 12) && (lstr.substr(0, 9) == "opengl es");
    if (*is_es)
        lstr = lstr.substr(10, 3);
    //   base::StringTokenizer tokenizer(lstr.begin(), lstr.end(), ".");
    //   unsigned major, minor;
    //   if (tokenizer.GetNext() && base::StringToUint(tokenizer.token_piece(), &major)) {
    //     *major_version = major;
    //     if (tokenizer.GetNext() && base::StringToUint(tokenizer.token_piece(), &minor)) {
    //       *minor_version = minor;
    //     }
    //   }
    *major_version = 2;
    *minor_version = 0;

    if (*is_es && *major_version == 3)
        *is_es3 = true;
}

} // namespace gfx
