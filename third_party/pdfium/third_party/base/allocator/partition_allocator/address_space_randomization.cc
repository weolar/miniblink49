// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/base/allocator/partition_allocator/address_space_randomization.h"

#include "build/build_config.h"
#include "third_party/base/allocator/partition_allocator/page_allocator.h"
#include "third_party/base/allocator/partition_allocator/spin_lock.h"
#include "third_party/base/logging.h"

#if defined(OS_WIN)
#include <windows.h>  // Must be in front of other Windows header files.

//#include <VersionHelpers.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

namespace pdfium {
namespace base {

namespace {

// This is the same PRNG as used by tcmalloc for mapping address randomness;
// see http://burtleburtle.net/bob/rand/smallprng.html
struct RandomContext {
  subtle::SpinLock lock;
  bool initialized;
  uint32_t a;
  uint32_t b;
  uint32_t c;
  uint32_t d;
};

RandomContext* GetRandomContext() {
  static RandomContext* s_RandomContext = nullptr;
  if (!s_RandomContext)
    s_RandomContext = new RandomContext();
  return s_RandomContext;
}

#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

uint32_t RandomValueInternal(RandomContext* x) {
  uint32_t e = x->a - rot(x->b, 27);
  x->a = x->b ^ rot(x->c, 17);
  x->b = x->c + x->d;
  x->c = x->d + e;
  x->d = e + x->a;
  return x->d;
}

#undef rot

uint32_t RandomValue(RandomContext* x) {
  subtle::SpinLock::Guard guard(x->lock);
  if (UNLIKELY(!x->initialized)) {
    x->initialized = true;
    char c;
    uint32_t seed = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&c));
    uint32_t pid;
    uint32_t usec;
#if defined(OS_WIN)
    pid = GetCurrentProcessId();
    SYSTEMTIME st;
    GetSystemTime(&st);
    usec = static_cast<uint32_t>(st.wMilliseconds * 1000);
#else
    pid = static_cast<uint32_t>(getpid());
    struct timeval tv;
    gettimeofday(&tv, 0);
    usec = static_cast<uint32_t>(tv.tv_usec);
#endif
    seed ^= pid;
    seed ^= usec;
    x->a = 0xf1ea5eed;
    x->b = x->c = x->d = seed;
    for (int i = 0; i < 20; ++i) {
      RandomValueInternal(x);
    }
  }

  return RandomValueInternal(x);
}

}  // namespace

void SetRandomPageBaseSeed(int64_t seed) {
  RandomContext* x = GetRandomContext();
  subtle::SpinLock::Guard guard(x->lock);
  // Set RNG to initial state.
  x->initialized = true;
  x->a = x->b = static_cast<uint32_t>(seed);
  x->c = x->d = static_cast<uint32_t>(seed >> 32);
}

BOOL MyIsWindows8Point1OrGreater() {
  return FALSE;
}

void* GetRandomPageBase() {
  uintptr_t random = static_cast<uintptr_t>(RandomValue(GetRandomContext()));

#if defined(ARCH_CPU_64_BITS)
  random <<= 32ULL;
  random |= static_cast<uintptr_t>(RandomValue(GetRandomContext()));

// The kASLRMask and kASLROffset constants will be suitable for the
// OS and build configuration.
#if defined(OS_WIN) && !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
  // Windows >= 8.1 has the full 47 bits. Use them where available.
  static bool windows_81 = false;
  static bool windows_81_initialized = false;
  if (!windows_81_initialized) {
    windows_81 = MyIsWindows8Point1OrGreater();
    windows_81_initialized = true;
  }
  if (!windows_81) {
    random &= internal::kASLRMaskBefore8_10;
  } else {
    random &= internal::kASLRMask;
  }
  random += internal::kASLROffset;
#else
  random &= internal::kASLRMask;
  random += internal::kASLROffset;
#endif  // defined(OS_WIN) && !defined(MEMORY_TOOL_REPLACES_ALLOCATOR)
#else   // defined(ARCH_CPU_32_BITS)
#if defined(OS_WIN)
  // On win32 host systems the randomization plus huge alignment causes
  // excessive fragmentation. Plus most of these systems lack ASLR, so the
  // randomization isn't buying anything. In that case we just skip it.
  // TODO(jschuh): Just dump the randomization when HE-ASLR is present.
  static BOOL is_wow64 = -1;
  if (is_wow64 == -1 && !IsWow64Process(GetCurrentProcess(), &is_wow64))
    is_wow64 = FALSE;
  if (!is_wow64)
    return nullptr;
#endif  // defined(OS_WIN)
  random &= internal::kASLRMask;
  random += internal::kASLROffset;
#endif  // defined(ARCH_CPU_32_BITS)

  DCHECK_EQ(0ULL, (random & kPageAllocationGranularityOffsetMask));
  return reinterpret_cast<void*>(random);
}

}  // namespace base
}  // namespace pdfium
