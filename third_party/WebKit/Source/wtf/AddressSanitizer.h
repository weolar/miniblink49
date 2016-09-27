// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WTF_AddressSanitizer_h
#define WTF_AddressSanitizer_h

// TODO(sof): Add SyZyASan support?
#if defined(ADDRESS_SANITIZER)
#include <sanitizer/asan_interface.h>
#else
#define ASAN_POISON_MEMORY_REGION(addr, size)   \
    ((void)(addr), (void)(size))
#define ASAN_UNPOISON_MEMORY_REGION(addr, size) \
    ((void)(addr), (void)(size))
#endif

#if defined(LEAK_SANITIZER)
#include <sanitizer/lsan_interface.h>
#else
#define __lsan_register_root_region(addr, size) ((void)(addr), (void)(size))
#define __lsan_unregister_root_region(addr, size) ((void)(addr), (void)(size))
#endif

// TODO(sof): Have to handle (ADDRESS_SANITIZER && _WIN32) differently as it
// uses both Clang (which supports the __attribute__ syntax) and CL (which doesn't)
// as long as we use "clang-cl /fallback". This shouldn't be needed when Clang
// handles all the code without falling back to CL.
#if defined(ADDRESS_SANITIZER) && (!OS(WIN) || COMPILER(CLANG))
#define NO_SANITIZE_ADDRESS __attribute__((no_sanitize_address))
#if ENABLE(LAZY_SWEEPING)
#define NO_LAZY_SWEEP_SANITIZE_ADDRESS NO_SANITIZE_ADDRESS
#else
#define NO_LAZY_SWEEP_SANITIZE_ADDRESS
#endif
#else
#define NO_SANITIZE_ADDRESS
#define NO_LAZY_SWEEP_SANITIZE_ADDRESS
#endif

#endif // WTF_AddressSanitizer_h
