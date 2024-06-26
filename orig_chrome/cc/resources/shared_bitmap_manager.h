// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_SHARED_BITMAP_MANAGER_H_
#define CC_RESOURCES_SHARED_BITMAP_MANAGER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/resources/shared_bitmap.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT SharedBitmapManager {
public:
    SharedBitmapManager() { }
    virtual ~SharedBitmapManager() { }

    virtual scoped_ptr<SharedBitmap> AllocateSharedBitmap(const gfx::Size&) = 0;
    virtual scoped_ptr<SharedBitmap> GetSharedBitmapFromId(
        const gfx::Size&,
        const SharedBitmapId&)
        = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(SharedBitmapManager);
};

} // namespace cc

#endif // CC_RESOURCES_SHARED_BITMAP_MANAGER_H_
