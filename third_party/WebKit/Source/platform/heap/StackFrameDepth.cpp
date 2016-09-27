// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/StackFrameDepth.h"

#include "public/platform/Platform.h"

#if OS(WIN)
#include <stddef.h>
#include <windows.h>
#include <winnt.h>
#elif defined(__GLIBC__)
extern "C" void* __libc_stack_end;  // NOLINT
#endif

namespace blink {

static const char* s_avoidOptimization = nullptr;

uintptr_t StackFrameDepth::s_stackFrameLimit = 0;
#if ENABLE(ASSERT)
bool StackFrameDepth::s_isEnabled = false;
#endif

// NEVER_INLINE ensures that |dummy| array on configureLimit() is not optimized away,
// and the stack frame base register is adjusted |kSafeStackFrameSize|.
NEVER_INLINE static uintptr_t currentStackFrameBaseOnCallee(const char* dummy)
{
    s_avoidOptimization = dummy;
    return StackFrameDepth::currentStackFrame();
}

void StackFrameDepth::enableStackLimit()
{
#if ENABLE(ASSERT)
    s_isEnabled = true;
#endif

    static const int kStackRoomSize = 1024;

    size_t stackSize = getUnderestimatedStackSize();
    if (stackSize) {
        size_t stackBase = reinterpret_cast<size_t>(getStackStart());
        s_stackFrameLimit = stackBase - stackSize + kStackRoomSize;
        return;
    }

    // Fallback version
    // Allocate a 32KB object on stack and query stack frame base after it.
    char dummy[kSafeStackFrameSize];
    s_stackFrameLimit = currentStackFrameBaseOnCallee(dummy);

    // Assert that the stack frame can be used.
    dummy[sizeof(dummy) - 1] = 0;
}

size_t StackFrameDepth::getUnderestimatedStackSize()
{
    // FIXME: ASAN bot uses a fake stack as a thread stack frame,
    // and its size is different from the value which APIs tells us.
#if defined(ADDRESS_SANITIZER)
    return 0;
#endif

    // FIXME: On Mac OSX and Linux, this method cannot estimate stack size
    // correctly for the main thread.

#if defined(__GLIBC__) || OS(ANDROID) || OS(FREEBSD)
    // pthread_getattr_np() can fail if the thread is not invoked by
    // pthread_create() (e.g., the main thread of webkit_unit_tests).
    // In this case, this method returns 0 and the caller must handle it.

    pthread_attr_t attr;
    int error;
#if OS(FREEBSD)
    pthread_attr_init(&attr);
    error = pthread_attr_get_np(pthread_self(), &attr);
#else
    error = pthread_getattr_np(pthread_self(), &attr);
#endif
    if (!error) {
        void* base;
        size_t size;
        error = pthread_attr_getstack(&attr, &base, &size);
        RELEASE_ASSERT(!error);
        pthread_attr_destroy(&attr);
        return size;
    }
#if OS(FREEBSD)
    pthread_attr_destroy(&attr);
#endif

    return 0;
#elif OS(MACOSX)
    // FIXME: pthread_get_stacksize_np() returns shorter size than actual stack
    // size for the main thread on Mavericks(10.9).
    return 0;
#elif OS(WIN) && COMPILER(MSVC)
    // On Windows stack limits for the current thread are available in
    // the thread information block (TIB). Its fields can be accessed through
    // FS segment register on x86 and GS segment register on x86_64.
#ifdef _WIN64
    return __readgsqword(offsetof(NT_TIB64, StackBase)) - __readgsqword(offsetof(NT_TIB64, StackLimit));
#else
    return __readfsdword(offsetof(NT_TIB, StackBase)) - __readfsdword(offsetof(NT_TIB, StackLimit));
#endif
#else
#error "Stack frame size estimation not supported on this platform."
    return 0;
#endif
}

void* StackFrameDepth::getStackStart()
{
#if defined(__GLIBC__) || OS(ANDROID) || OS(FREEBSD)
    pthread_attr_t attr;
    int error;
#if OS(FREEBSD)
    pthread_attr_init(&attr);
    error = pthread_attr_get_np(pthread_self(), &attr);
#else
    error = pthread_getattr_np(pthread_self(), &attr);
#endif
    if (!error) {
        void* base;
        size_t size;
        error = pthread_attr_getstack(&attr, &base, &size);
        RELEASE_ASSERT(!error);
        pthread_attr_destroy(&attr);
        return reinterpret_cast<uint8_t*>(base) + size;
    }
#if OS(FREEBSD)
    pthread_attr_destroy(&attr);
#endif
#if defined(__GLIBC__)
    // pthread_getattr_np can fail for the main thread. In this case
    // just like NaCl we rely on the __libc_stack_end to give us
    // the start of the stack.
    // See https://code.google.com/p/nativeclient/issues/detail?id=3431.
    return __libc_stack_end;
#else
    ASSERT_NOT_REACHED();
    return nullptr;
#endif
#elif OS(MACOSX)
    return pthread_get_stackaddr_np(pthread_self());
#elif OS(WIN) && COMPILER(MSVC)
    // On Windows stack limits for the current thread are available in
    // the thread information block (TIB). Its fields can be accessed through
    // FS segment register on x86 and GS segment register on x86_64.
#ifdef _WIN64
    return reinterpret_cast<void*>(__readgsqword(offsetof(NT_TIB64, StackBase)));
#else
    return reinterpret_cast<void*>(__readfsdword(offsetof(NT_TIB, StackBase)));
#endif
#else
#error Unsupported getStackStart on this platform.
#endif
}

} // namespace blink
