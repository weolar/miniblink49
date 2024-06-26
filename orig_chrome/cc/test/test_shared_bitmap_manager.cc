// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/test_shared_bitmap_manager.h"

#include "base/memory/shared_memory.h"

namespace cc {

namespace {
    class OwnedSharedBitmap : public SharedBitmap {
    public:
        OwnedSharedBitmap(scoped_ptr<base::SharedMemory> shared_memory,
            const SharedBitmapId& id)
            : SharedBitmap(static_cast<uint8*>(shared_memory->memory()), id)
            , shared_memory_(shared_memory.Pass())
        {
        }

        ~OwnedSharedBitmap() override { }

    private:
        scoped_ptr<base::SharedMemory> shared_memory_;
    };

} // namespace

TestSharedBitmapManager::TestSharedBitmapManager() { }

TestSharedBitmapManager::~TestSharedBitmapManager() { }

scoped_ptr<SharedBitmap> TestSharedBitmapManager::AllocateSharedBitmap(
    const gfx::Size& size)
{
    base::AutoLock lock(lock_);
    scoped_ptr<base::SharedMemory> memory(new base::SharedMemory);
    memory->CreateAndMapAnonymous(size.GetArea() * 4);
    SharedBitmapId id = SharedBitmap::GenerateId();
    bitmap_map_[id] = memory.get();
    return make_scoped_ptr(new OwnedSharedBitmap(memory.Pass(), id));
}

scoped_ptr<SharedBitmap> TestSharedBitmapManager::GetSharedBitmapFromId(
    const gfx::Size&,
    const SharedBitmapId& id)
{
    base::AutoLock lock(lock_);
    if (bitmap_map_.find(id) == bitmap_map_.end())
        return nullptr;
    uint8* pixels = static_cast<uint8*>(bitmap_map_[id]->memory());
    return make_scoped_ptr(new SharedBitmap(pixels, id));
}

} // namespace cc
