// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_UI_RESOURCE_CLIENT_H_
#define CC_RESOURCES_UI_RESOURCE_CLIENT_H_

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"

namespace cc {

class UIResourceBitmap;

typedef int UIResourceId;

class CC_EXPORT UIResourceClient {
public:
    // GetBitmap() will be called once soon after resource creation and then will
    // be called afterwards whenever the GL context is lost, on the same thread
    // that LayerTreeHost::CreateUIResource was called on.  It is only safe to
    // delete a UIResourceClient object after DeleteUIResource has been called for
    // all IDs associated with it.  A valid bitmap always must be returned but it
    // doesn't need to be the same size or format as the original.
    virtual UIResourceBitmap GetBitmap(UIResourceId uid,
        bool resource_lost)
        = 0;
    virtual ~UIResourceClient() { }
};

} // namespace cc

#endif // CC_RESOURCES_UI_RESOURCE_CLIENT_H_
