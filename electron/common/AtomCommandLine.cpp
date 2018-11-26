// Copyright (c) 2013 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/AtomCommandLine.h"

#include "uv.h"

#if defined(OS_WIN)
#include <windows.h>
#include <ShellAPI.h>
#endif

namespace atom {

// static
std::vector<std::string> AtomCommandLine::argv_;

#if defined(OS_WIN)
// static
std::vector<std::wstring> AtomCommandLine::wargv_;
#endif

// static
void AtomCommandLine::init(int argc, const char* const* argv) {
    // Hack around with the argv pointer. Used for process.title = "blah"
    char** new_argv = uv_setup_args(argc, const_cast<char**>(argv));
    for (int i = 0; i < argc; ++i) {
        argv_.push_back(new_argv[i]);
    }
}

#if defined(OS_WIN)
// static
void AtomCommandLine::initW(int argc, const wchar_t* const* argv) {
    for (int i = 0; i < argc; ++i) {
        std::wstring arg = argv[i];
        if (arg.length() > 0 && arg[0] >= L'a' && arg[0] <= L'z')
            arg[0] += L'A' - L'a';

        wargv_.push_back(arg);
    }
}

void AtomCommandLine::initAW() {
    int argc = 0;
    wchar_t** argvW = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

    initW(argc, argvW);

    // Convert argv to to UTF8
    char** argvA = new char*[argc];
    for (int i = 0; i < argc; i++) {
        // Compute the size of the required buffer
        DWORD size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
        if (size == 0) {
            // This should never happen.
            fprintf(stderr, "Could not convert arguments to utf8.");
            return;
        }

        // Do the actual conversion
        argvA[i] = new char[size];
        DWORD result = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, argvA[i], size, NULL, NULL);
        if (result == 0) {
            // This should never happen.
            fprintf(stderr, "Could not convert arguments to utf8.");
            return;
        }

        char* argvStrA = argvA[i];
        if (argvStrA[0] >= 'a' && argvStrA[0] <= 'z')
            argvStrA[0] += 'A' - 'a';
        argvStrA = argvStrA;
    }

    atom::AtomCommandLine::init(argc, argvA);
}
#endif

#if defined(OS_LINUX)
// static
void AtomCommandLine::initializeFromCommandLine() {
    argv_ = base::CommandLine::ForCurrentProcess()->argv();
}
#endif

}  // namespace atom
