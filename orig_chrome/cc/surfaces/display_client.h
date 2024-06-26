// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_DISPLAY_CLIENT_H_
#define CC_SURFACES_DISPLAY_CLIENT_H_

#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"

namespace cc {

struct ManagedMemoryPolicy;

class DisplayClient {
public:
    virtual void CommitVSyncParameters(base::TimeTicks timebase,
        base::TimeDelta interval)
        = 0;
    virtual void OutputSurfaceLost() = 0;
    virtual void SetMemoryPolicy(const ManagedMemoryPolicy& policy) = 0;

protected:
    virtual ~DisplayClient() { }
};

} // namespace cc

#endif // CC_SURFACES_DISPLAY_CLIENT_H_
