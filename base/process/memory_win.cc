// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/memory.h"

#ifndef USING_VC6RT
#include <psapi.h>
#endif

#include "base/logging.h"

// extern "C" 
// WINBASEAPI
// BOOL WINAPI GetModuleHandleExA(DWORD dwFlags, LPCSTR lpModuleName, HMODULE * phModule);

#define GET_MODULE_HANDLE_EX_FLAG_PIN                 (0x00000001)
#define GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT  (0x00000002)
#define GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS        (0x00000004)

namespace base {

namespace {

// void OnNoMemory() {
//   // Kill the process. This is important for security, since WebKit doesn't
//   // NULL-check many memory allocations. If a malloc fails, returns NULL, and
//   // the buffer is then used, it provides a handy mapping of memory starting at
//   // address 0 for an attacker to utilize.
//   __debugbreak();
//   _exit(1);
// }

// HeapSetInformation function pointer.
typedef BOOL (WINAPI* HeapSetFn)(HANDLE, HEAP_INFORMATION_CLASS, PVOID, SIZE_T);

}  // namespace

bool EnableLowFragmentationHeap() {
#if 0
  HMODULE kernel32 = GetModuleHandle(L"kernel32.dll");
  HeapSetFn heap_set = reinterpret_cast<HeapSetFn>(GetProcAddress(
      kernel32,
      "HeapSetInformation"));

  // On Windows 2000, the function is not exported. This is not a reason to
  // fail.
  if (!heap_set)
    return true;

  unsigned number_heaps = GetProcessHeaps(0, NULL);
  if (!number_heaps)
    return false;

  // Gives us some extra space in the array in case a thread is creating heaps
  // at the same time we're querying them.
  static const int MARGIN = 8;
  HANDLE* heaps = (new HANDLE[number_heaps + MARGIN]);
  number_heaps = GetProcessHeaps(number_heaps + MARGIN, heaps);
  if (!number_heaps) {
      delete heaps;
      return false;
  }

  for (unsigned i = 0; i < number_heaps; ++i) {
    ULONG lfh_flag = 2;
    // Don't bother with the result code. It may fails on heaps that have the
    // HEAP_NO_SERIALIZE flag. This is expected and not a problem at all.
    heap_set(heaps[i],
             HeapCompatibilityInformation,
             &lfh_flag,
             sizeof(lfh_flag));
  }
  delete heaps;
#endif
  DebugBreak();
  return true;
}

void EnableTerminationOnHeapCorruption() {
#if 0
  // Ignore the result code. Supported on XP SP3 and Vista.
  HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
#endif
  DebugBreak();
}

void EnableTerminationOnOutOfMemory() {
  //std::set_new_handler(&OnNoMemory);
  DebugBreak();
}

HMODULE GetModuleFromAddress(void* address) {
  HMODULE instance = NULL;
#if 0
  if (!::GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            static_cast<char*>(address),
                            &instance)) {
    __debugbreak();
  }
#endif
  __debugbreak();
  return instance;
}

// TODO(b.kelemen): implement it with the required semantics. On Linux this is
// implemented with a weak symbol that is overridden by tcmalloc. This is
// neccessary because base cannot have a direct dependency on tcmalloc. Since
// weak symbols are not supported on Windows this will involve some build time
// magic, much like what is done for libcrt in order to override the allocation
// functions.
bool UncheckedMalloc(size_t size, void** result) {
  *result = malloc(size);
  return *result != NULL;
}

}  // namespace base
