// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/command_line.h"
#include "base/message_loop/message_loop.h"
#if defined(OS_MACOSX)
#include "base/mac/scoped_nsautorelease_pool.h"
#endif
#include "ui/gl/gl_surface.h"

extern "C" {
#if defined(GLES2_CONFORM_SUPPORT_ONLY)
#include "gpu/gles2_conform_support/gtf/gtf_stubs.h"
#else
#include "third_party/gles2_conform/GTF_ES/glsl/GTF/Source/GTFMain.h"
#endif
}

int main(int argc, char *argv[]) {
  base::AtExitManager at_exit;
  CommandLine::Init(argc, argv);
  base::MessageLoopForUI message_loop;

#if defined(OS_MACOSX)
  base::mac::ScopedNSAutoreleasePool pool;
#endif
  GTFMain(argc, argv);

  return 0;
}

