// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StackFrameDepth_h
#define StackFrameDepth_h

#include "platform/PlatformExport.h"
#include "wtf/Assertions.h"
#include <stdint.h>

namespace blink {

// StackFrameDepth keeps track of current call stack frame depth.
// Use isSafeToRecurse() to query if there is a room in current
// call stack for more recursive call.
class PLATFORM_EXPORT StackFrameDepth final {
public:
    inline static bool isSafeToRecurse()
    {
        ASSERT(s_stackFrameLimit || !s_isEnabled);

        // Asssume that the stack grows towards lower addresses, which
        // all the ABIs currently supported do.
        //
        // A unit test checks that the assumption holds for a target
        // (HeapTest.StackGrowthDirection.)
        return currentStackFrame() > s_stackFrameLimit;
    }

    static void enableStackLimit();
    static void disableStackLimit()
    {
        s_stackFrameLimit = 0;
#if ENABLE(ASSERT)
        s_isEnabled = false;
#endif
    }

#if ENABLE(ASSERT)
    inline static bool isEnabled() { return s_isEnabled; }
#endif

    static size_t getUnderestimatedStackSize();
    static void* getStackStart();

#if COMPILER(MSVC)
// Ignore C4172: returning address of local variable or temporary: dummy. This
// warning suppression has to go outside of the function to take effect.
#pragma warning(push)
#pragma warning(disable: 4172)
#endif
    static uintptr_t currentStackFrame(const char* dummy = nullptr)
    {
#if COMPILER(GCC)
        return reinterpret_cast<uintptr_t>(__builtin_frame_address(0));
#elif COMPILER(MSVC)
        return reinterpret_cast<uintptr_t>(&dummy) - sizeof(void*);
#else
#error "Stack frame pointer estimation not supported on this platform."
        return 0;
#endif
    }
#if COMPILER(MSVC)
#pragma warning(pop)
#endif

private:
    // The maximum depth of eager, unrolled trace() calls that is
    // considered safe and allowed.
    static const int kSafeStackFrameSize = 32 * 1024;

    static uintptr_t s_stackFrameLimit;
#if ENABLE(ASSERT)
    static bool s_isEnabled;
#endif
};

class StackFrameDepthScope {
public:
    StackFrameDepthScope()
    {
        StackFrameDepth::enableStackLimit();
        ASSERT(StackFrameDepth::isSafeToRecurse());
    }

    ~StackFrameDepthScope()
    {
        StackFrameDepth::disableStackLimit();
    }
};

} // namespace blink

#endif // StackFrameDepth_h
