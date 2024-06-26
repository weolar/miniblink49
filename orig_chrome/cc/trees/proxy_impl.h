// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_PROXY_IMPL_H_
#define CC_TREES_PROXY_IMPL_H_

#include "base/memory/weak_ptr.h"
#include "cc/base/cc_export.h"

namespace cc {

// TODO(khushalsagar): The impl side of ThreadProxy. It is currently defined as
// an interface with the implementation provided by ThreadProxy and will be
// made an independent class.
// The methods added to this interface should only use the CompositorThreadOnly
// variables from ThreadProxy.
// See crbug/527200
class CC_EXPORT ProxyImpl {
public:
    // Callback for impl side commands received from the channel.
    virtual void SetThrottleFrameProductionOnImpl(bool throttle) = 0;
    virtual void SetLayerTreeHostClientReadyOnImpl() = 0;

    // TODO(khushalsagar): Rename as GetWeakPtr() once ThreadProxy is split.
    virtual base::WeakPtr<ProxyImpl> GetImplWeakPtr() = 0;

protected:
    virtual ~ProxyImpl() { }
};

} // namespace cc

#endif // CC_TREES_PROXY_IMPL_H_
