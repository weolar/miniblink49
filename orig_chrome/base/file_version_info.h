// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_FILE_VERSION_INFO_H_
#define BASE_FILE_VERSION_INFO_H_

#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
// http://blogs.msdn.com/oldnewthing/archive/2004/10/25/247180.aspx
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif // OS_WIN

#include <string>

#include "base/base_export.h"
#include "base/strings/string16.h"

namespace base {
class FilePath;
}

// Provides an interface for accessing the version information for a file. This
// is the information you access when you select a file in the Windows Explorer,
// right-click select Properties, then click the Version tab, and on the Mac
// when you select a file in the Finder and do a Get Info.
//
// This list of properties is straight out of Win32's VerQueryValue
// <http://msdn.microsoft.com/en-us/library/ms647464.aspx> and the Mac
// version returns values from the Info.plist as appropriate. TODO(avi): make
// this a less-obvious Windows-ism.

#if defined(OS_WIN)
// Creates a FileVersionInfo for the current module. Returns NULL in case of
// error. The returned object should be deleted when you are done with it. This
// is done as a macro to force inlining of __ImageBase. It used to be inside of
// a method labeled with __forceinline, but inlining through __forceinline
// stopped working for Debug builds in VS2013 (http://crbug.com/516359).
#define CREATE_FILE_VERSION_INFO_FOR_CURRENT_MODULE() \
    FileVersionInfo::CreateFileVersionInfoForModule(  \
        reinterpret_cast<HMODULE>(&__ImageBase))
#endif

class BASE_EXPORT FileVersionInfo {
public:
    virtual ~FileVersionInfo() { }
#if defined(OS_WIN) || defined(OS_MACOSX)
    // Creates a FileVersionInfo for the specified path. Returns NULL if something
    // goes wrong (typically the file does not exit or cannot be opened). The
    // returned object should be deleted when you are done with it.
    static FileVersionInfo* CreateFileVersionInfo(
        const base::FilePath& file_path);
#endif // OS_WIN || OS_MACOSX

#if defined(OS_WIN)
    // Creates a FileVersionInfo for the specified module. Returns NULL in case
    // of error. The returned object should be deleted when you are done with it.
    // See CREATE_FILE_VERSION_INFO_FOR_CURRENT_MODULE() helper above for a
    // CreateFileVersionInfoForCurrentModule() alternative for Windows.
    static FileVersionInfo* CreateFileVersionInfoForModule(HMODULE module);
#else
    // Creates a FileVersionInfo for the current module. Returns NULL in case
    // of error. The returned object should be deleted when you are done with it.
    static FileVersionInfo* CreateFileVersionInfoForCurrentModule();
#endif // OS_WIN

    // Accessors to the different version properties.
    // Returns an empty string if the property is not found.
    virtual base::string16 company_name() = 0;
    virtual base::string16 company_short_name() = 0;
    virtual base::string16 product_name() = 0;
    virtual base::string16 product_short_name() = 0;
    virtual base::string16 internal_name() = 0;
    virtual base::string16 product_version() = 0;
    virtual base::string16 private_build() = 0;
    virtual base::string16 special_build() = 0;
    virtual base::string16 comments() = 0;
    virtual base::string16 original_filename() = 0;
    virtual base::string16 file_description() = 0;
    virtual base::string16 file_version() = 0;
    virtual base::string16 legal_copyright() = 0;
    virtual base::string16 legal_trademarks() = 0;
    virtual base::string16 last_change() = 0;
    virtual bool is_official_build() = 0;
};

#endif // BASE_FILE_VERSION_INFO_H_
