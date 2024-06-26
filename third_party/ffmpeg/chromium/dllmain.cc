// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <intrin.h>
#include <new.h>
#include <stdlib.h>
#include <windows.h>

extern "C" {
#include "../libavutil/mem.h"
}

namespace {

int OnNoMemory(size_t size) {
  // Kill the process. This is important for security, since ffmpeg doesn't
  // NULL-check many memory allocations. If a malloc fails, returns NULL, and
  // the buffer is then used, it provides a handy mapping of memory starting at
  // address 0 for an attacker to utilize.
  __debugbreak();
  _exit(1);
}

}  // namespace

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
  if (reason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(instance);
    // Remove allocation limit from ffmpeg, so calls go down to shim layer.
    av_max_alloc(0);
    // Enable OOM crashes in the shim for all malloc calls that fail.
    _set_new_mode(1);
    _set_new_handler(&OnNoMemory);
  }
  return true;
}
