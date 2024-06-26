// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_TEST_SHARED_BITMAP_MANAGER_H_
#define CC_TEST_TEST_SHARED_BITMAP_MANAGER_H_

#include <map>

#include "base/synchronization/lock.h"
#include "cc/resources/shared_bitmap_manager.h"

namespace base {
class SharedMemory;
} // namespace base

namespace cc {

class TestSharedBitmapManager : public SharedBitmapManager {
public:
    TestSharedBitmapManager();
    ~TestSharedBitmapManager() override;

    scoped_ptr<SharedBitmap> AllocateSharedBitmap(const gfx::Size& size) override;

    scoped_ptr<SharedBitmap> GetSharedBitmapFromId(
        const gfx::Size&,
        const SharedBitmapId& id) override;

private:
    base::Lock lock_;
    std::map<SharedBitmapId, base::SharedMemory*> bitmap_map_;
};

} // namespace cc

#endif // CC_TEST_TEST_SHARED_BITMAP_MANAGER_H_
