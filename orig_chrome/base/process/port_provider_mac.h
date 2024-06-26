// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_PROCESS_PORT_PROVIDER_MAC_H_
#define BASE_PROCESS_PORT_PROVIDER_MAC_H_

#include <mach/mach.h>

#include "base/base_export.h"
#include "base/process/process_handle.h"

namespace base {

// Abstract base class that provides a mapping from ProcessHandle (pid_t) to the
// Mach task port. This replicates task_for_pid(), which requires root
// privileges.
class BASE_EXPORT PortProvider {
public:
    virtual ~PortProvider() { }

    // Returns the mach task port for |process| if possible, or else
    // |MACH_PORT_NULL|.
    virtual mach_port_t TaskForPid(ProcessHandle process) const = 0;
};

} // namespace base

#endif // BASE_PROCESS_PORT_PROVIDER_MAC_H_
