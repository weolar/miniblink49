// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BASE_LOGGING_H_
#define THIRD_PARTY_BASE_LOGGING_H_

#include <assert.h>
#include <stdlib.h>

#include "build/build_config.h"
#include "third_party/base/compiler_specific.h"

#if defined(COMPILER_GCC)

#if defined(ARCH_CPU_X86_FAMILY)
// int 3 will generate a SIGTRAP.
#define TRAP_SEQUENCE() \
  asm volatile(         \
      "int3; ud2; push %0;" ::"i"(static_cast<unsigned char>(__COUNTER__)))

#elif defined(ARCH_CPU_ARMEL)
// bkpt will generate a SIGBUS when running on armv7 and a SIGTRAP when running
// as a 32 bit userspace app on arm64. There doesn't seem to be any way to
// cause a SIGTRAP from userspace without using a syscall (which would be a
// problem for sandboxing).
#define TRAP_SEQUENCE() \
  asm volatile("bkpt #0; udf %0;" ::"i"(__COUNTER__ % 256))

#elif defined(ARCH_CPU_ARM64)
// This will always generate a SIGTRAP on arm64.
#define TRAP_SEQUENCE() \
  asm volatile("brk #0; hlt %0;" ::"i"(__COUNTER__ % 65536))

#else
// Crash report accuracy will not be guaranteed on other architectures, but at
// least this will crash as expected.
#define TRAP_SEQUENCE() __builtin_trap()
#endif  // ARCH_CPU_*

#elif defined(COMPILER_MSVC)

// Clang is cleverer about coalescing int3s, so we need to add a unique-ish
// instruction following the __debugbreak() to have it emit distinct locations
// for CHECKs rather than collapsing them all together. It would be nice to use
// a short intrinsic to do this (and perhaps have only one implementation for
// both clang and MSVC), however clang-cl currently does not support intrinsics.
// On the flip side, MSVC x64 doesn't support inline asm. So, we have to have
// two implementations. Normally clang-cl's version will be 5 bytes (1 for
// `int3`, 2 for `ud2`, 2 for `push byte imm`, however, TODO(scottmg):
// https://crbug.com/694670 clang-cl doesn't currently support %'ing
// __COUNTER__, so eventually it will emit the dword form of push.
// TODO(scottmg): Reinvestigate a short sequence that will work on both
// compilers once clang supports more intrinsics. See https://crbug.com/693713.
#if !defined(__clang__)
#define TRAP_SEQUENCE() __debugbreak()
#elif defined(ARCH_CPU_ARM64)
#define TRAP_SEQUENCE() \
  __asm volatile("brk #0\n hlt %0\n" ::"i"(__COUNTER__ % 65536));
#else
#define TRAP_SEQUENCE() ({ {__asm int 3 __asm ud2 __asm push __COUNTER__}; })
#endif  // __clang__

#else
#error Port
#endif  // COMPILER_GCC

// CHECK() and the trap sequence can be invoked from a constexpr function.
// This could make compilation fail on GCC, as it forbids directly using inline
// asm inside a constexpr function. However, it allows calling a lambda
// expression including the same asm.
// The side effect is that the top of the stacktrace will not point to the
// calling function, but to this anonymous lambda. This is still useful as the
// full name of the lambda will typically include the name of the function that
// calls CHECK() and the debugger will still break at the right line of code.
#if !defined(COMPILER_GCC)
#define WRAPPED_TRAP_SEQUENCE() TRAP_SEQUENCE()
#else
#define WRAPPED_TRAP_SEQUENCE() \
  do {                          \
    [] { TRAP_SEQUENCE(); }();  \
  } while (false)
#endif

#if defined(__clang__) || defined(COMPILER_GCC)
#define IMMEDIATE_CRASH()    \
  ({                         \
    WRAPPED_TRAP_SEQUENCE(); \
    __builtin_unreachable(); \
  })
#else
// This is supporting non-chromium user of logging.h to build with MSVC, like
// pdfium. On MSVC there is no __builtin_unreachable().
#define IMMEDIATE_CRASH() WRAPPED_TRAP_SEQUENCE()
#endif

#define CHECK(condition)        \
  if (UNLIKELY(!(condition))) { \
    IMMEDIATE_CRASH();          \
  }

// TODO(palmer): These are quick hacks to import PartitionAlloc with minimum
// hassle. Look into pulling in the real DCHECK definition. It might be more
// than we need, or have more dependencies than we want. In the meantime, this
// is safe, at the cost of some performance.
#define DCHECK CHECK
#define DCHECK_EQ(x, y) CHECK((x) == (y))
#define DCHECK_IS_ON() true

#define NOTREACHED() assert(false)

#endif  // THIRD_PARTY_BASE_LOGGING_H_
