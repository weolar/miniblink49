// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/process/launch.h"

#include <mach/mach.h>
#include <servers/bootstrap.h>

#include "base/logging.h"

namespace base {

void RestoreDefaultExceptionHandler()
{
    // This function is tailored to remove the Breakpad exception handler.
    // exception_mask matches s_exception_mask in
    // breakpad/src/client/mac/handler/exception_handler.cc
    const exception_mask_t exception_mask = EXC_MASK_BAD_ACCESS | EXC_MASK_BAD_INSTRUCTION | EXC_MASK_ARITHMETIC | EXC_MASK_BREAKPOINT;

    // Setting the exception port to MACH_PORT_NULL may not be entirely
    // kosher to restore the default exception handler, but in practice,
    // it results in the exception port being set to Apple Crash Reporter,
    // the desired behavior.
    task_set_exception_ports(mach_task_self(), exception_mask, MACH_PORT_NULL,
        EXCEPTION_DEFAULT, THREAD_STATE_NONE);
}

} // namespace base
