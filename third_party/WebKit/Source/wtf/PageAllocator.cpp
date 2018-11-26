/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "wtf/PageAllocator.h"

#include "wtf/Assertions.h"
#include "wtf/Atomics.h"
#include "wtf/AddressSpaceRandomization.h"

#include <limits.h>

#if OS(POSIX)

#include <errno.h>
#include <sys/mman.h>

#ifndef MADV_FREE
#define MADV_FREE MADV_DONTNEED
#endif

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

// On POSIX memmap uses a nearby address if the hint address is blocked.
static const bool kHintIsAdvisory = true;
static uint32_t s_allocPageErrorCode = 0;

#elif OS(WIN)

#include <windows.h>

// VirtualAlloc will fail if allocation at the hint address is blocked.
static const bool kHintIsAdvisory = false;
static uint32_t s_allocPageErrorCode = ERROR_SUCCESS;

#else
#error Unknown OS
#endif // OS(POSIX)

namespace WTF {

// This internal function wraps the OS-specific page allocation call. The
// behavior of the hint address is determined by the kHintIsAdvisory constant.
// If true, a non-zero hint is advisory and the returned address may differ from
// the hint. If false, the hint is mandatory and a successful allocation will
// not differ from the hint.
static void* systemAllocPages(void* hint, size_t len, PageAccessibilityConfiguration pageAccessibility)
{
    ASSERT(!(len & kPageAllocationGranularityOffsetMask));
    ASSERT(!(reinterpret_cast<uintptr_t>(hint) & kPageAllocationGranularityOffsetMask));
    void* ret;
#if OS(WIN)
    DWORD accessFlag = pageAccessibility == PageAccessible ? PAGE_READWRITE : PAGE_NOACCESS;
    ret = VirtualAlloc(hint, len, MEM_RESERVE | MEM_COMMIT, accessFlag);
    if (!ret)
        releaseStore(&s_allocPageErrorCode, GetLastError());
#else
    int accessFlag = pageAccessibility == PageAccessible ? (PROT_READ | PROT_WRITE) : PROT_NONE;
    ret = mmap(hint, len, accessFlag, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (ret == MAP_FAILED) {
        releaseStore(&s_allocPageErrorCode, errno);
        ret = 0;
    }
#endif
    return ret;
}

// Trims base to given length and alignment. Windows returns null on failure and frees base.
static void* trimMapping(void *base, size_t baseLen, size_t trimLen, uintptr_t align, PageAccessibilityConfiguration pageAccessibility)
{
    size_t preSlack = reinterpret_cast<uintptr_t>(base) & (align - 1);
    if (preSlack)
        preSlack = align - preSlack;
    size_t postSlack = baseLen - preSlack - trimLen;
    ASSERT(baseLen >= trimLen || preSlack || postSlack);
    ASSERT(preSlack < baseLen);
    ASSERT(postSlack < baseLen);
    void* ret = base;

#if OS(POSIX) // On POSIX we can resize the allocation run.
    (void) pageAccessibility;
    if (preSlack) {
        int res = munmap(base, preSlack);
        RELEASE_ASSERT(!res);
        ret = reinterpret_cast<char*>(base) + preSlack;
    }
    if (postSlack) {
        int res = munmap(reinterpret_cast<char*>(ret) + trimLen, postSlack);
        RELEASE_ASSERT(!res);
    }
#else // On Windows we can't resize the allocation run.
    if (preSlack || postSlack) {
        ret = reinterpret_cast<char*>(base) + preSlack;
        freePages(base, baseLen);
        ret = systemAllocPages(ret, trimLen, pageAccessibility);
    }
#endif

    return ret;
}

void* allocPages(void* addr, size_t len, size_t align, PageAccessibilityConfiguration pageAccessibility)
{
    ASSERT(len >= kPageAllocationGranularity);
    ASSERT(!(len & kPageAllocationGranularityOffsetMask));
    ASSERT(align >= kPageAllocationGranularity);
    ASSERT(!(align & kPageAllocationGranularityOffsetMask));
    ASSERT(!(reinterpret_cast<uintptr_t>(addr) & kPageAllocationGranularityOffsetMask));
    uintptr_t alignOffsetMask = align - 1;
    uintptr_t alignBaseMask = ~alignOffsetMask;
    ASSERT(!(reinterpret_cast<uintptr_t>(addr) & alignOffsetMask));

    // If the client passed null as the address, choose a good one.
    if (!addr) {
        addr = getRandomPageBase();
        addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) & alignBaseMask);
    }

    // First try to force an exact-size, aligned allocation from our random base.
    for (int count = 0; count < 3; ++count) {
        void* ret = systemAllocPages(addr, len, pageAccessibility);
        if (kHintIsAdvisory || ret) {
            // If the alignment is to our liking, we're done.
            if (!(reinterpret_cast<uintptr_t>(ret)& alignOffsetMask))
                return ret;
            freePages(ret, len);
#if CPU(32BIT)
            addr = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(ret)+align) & alignBaseMask);
#endif
        } else if (!addr) { // We know we're OOM when an unhinted allocation fails.
            return nullptr;

        } else {
#if CPU(32BIT)
            addr = reinterpret_cast<char*>(addr) + align;
#endif
        }

#if !CPU(32BIT) // Keep trying random addresses on systems that have a large address space.
        addr = getRandomPageBase();
        addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) & alignBaseMask);
#endif
    }

    // Map a larger allocation so we can force alignment, but continue randomizing only on 64-bit POSIX.
    size_t tryLen = len + (align - kPageAllocationGranularity);
    RELEASE_ASSERT(tryLen >= len);
    void* ret;

    do {
        // Don't continue to burn cycles on mandatory hints (Windows).
        addr = kHintIsAdvisory ? getRandomPageBase() : nullptr;
        ret = systemAllocPages(addr, tryLen, pageAccessibility);
    // The retries are for Windows, where a race can steal our mapping on resize.
    } while (ret && !(ret = trimMapping(ret, tryLen, len, align, pageAccessibility)));

    return ret;
}

void freePages(void* addr, size_t len)
{
    ASSERT(!(reinterpret_cast<uintptr_t>(addr) & kPageAllocationGranularityOffsetMask));
    ASSERT(!(len & kPageAllocationGranularityOffsetMask));
#if OS(POSIX)
    int ret = munmap(addr, len);
    RELEASE_ASSERT(!ret);
#else
    BOOL ret = VirtualFree(addr, 0, MEM_RELEASE);
    RELEASE_ASSERT(ret);
#endif
}

void setSystemPagesInaccessible(void* addr, size_t len)
{
    ASSERT(!(len & kSystemPageOffsetMask));
#if OS(POSIX)
    int ret = mprotect(addr, len, PROT_NONE);
    RELEASE_ASSERT(!ret);
#else
    BOOL ret = VirtualFree(addr, len, MEM_DECOMMIT);
    RELEASE_ASSERT(ret);
#endif
}

bool setSystemPagesAccessible(void* addr, size_t len)
{
    ASSERT(!(len & kSystemPageOffsetMask));
#if OS(POSIX)
    return !mprotect(addr, len, PROT_READ | PROT_WRITE);
#else
    return !!VirtualAlloc(addr, len, MEM_COMMIT, PAGE_READWRITE);
#endif
}

void decommitSystemPages(void* addr, size_t len)
{
    ASSERT(!(len & kSystemPageOffsetMask));
#if OS(POSIX)
    int ret = madvise(addr, len, MADV_FREE);
    RELEASE_ASSERT(!ret);
#else
    setSystemPagesInaccessible(addr, len);
#endif
}

void recommitSystemPages(void* addr, size_t len)
{
    ASSERT(!(len & kSystemPageOffsetMask));
#if OS(POSIX)
    (void) addr;
#else
    RELEASE_ASSERT(setSystemPagesAccessible(addr, len));
#endif
}

void discardSystemPages(void* addr, size_t len)
{
    ASSERT(!(len & kSystemPageOffsetMask));
#if OS(POSIX)
    // On POSIX, the implementation detail is that discard and decommit are the
    // same, and lead to pages that are returned to the system immediately and
    // get replaced with zeroed pages when touched. So we just call
    // decommitSystemPages() here to avoid code duplication.
    decommitSystemPages(addr, len);
#else
    // On Windows discarded pages are not returned to the system immediately and
    // not guaranteed to be zeroed when returned to the application.
    using DiscardVirtualMemoryFunction = DWORD(WINAPI*)(PVOID virtualAddress, SIZE_T size);
    static DiscardVirtualMemoryFunction discardVirtualMemory = reinterpret_cast<DiscardVirtualMemoryFunction>(-1);
    if (discardVirtualMemory == reinterpret_cast<DiscardVirtualMemoryFunction>(-1))
        discardVirtualMemory = reinterpret_cast<DiscardVirtualMemoryFunction>(GetProcAddress(GetModuleHandle(L"Kernel32.dll"), "DiscardVirtualMemory"));
    // Use DiscardVirtualMemory when available because it releases faster than MEM_RESET.
    DWORD ret = 1;
    if (discardVirtualMemory)
        ret = discardVirtualMemory(addr, len);
    // DiscardVirtualMemory is buggy in Win10 SP0, so fall back to MEM_RESET on failure.
    if (ret) {
        void* ret = VirtualAlloc(addr, len, MEM_RESET, PAGE_READWRITE);
        RELEASE_ASSERT(ret);
    }
#endif
}

uint32_t getAllocPageErrorCode()
{
    return acquireLoad(&s_allocPageErrorCode);
}

} // namespace WTF

