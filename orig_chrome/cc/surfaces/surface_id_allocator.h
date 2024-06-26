// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_ID_ALLOCATOR_H_
#define CC_SURFACES_SURFACE_ID_ALLOCATOR_H_

#include "cc/surfaces/surface_id.h"
#include "cc/surfaces/surfaces_export.h"

namespace cc {

class SurfaceManager;

// This is a helper class for generating surface IDs within a specified
// namespace.  This is not threadsafe, to use from multiple threads wrap this
// class in a mutex.
class CC_SURFACES_EXPORT SurfaceIdAllocator {
public:
    explicit SurfaceIdAllocator(uint32_t id_namespace);
    ~SurfaceIdAllocator();

    SurfaceId GenerateId();

    static uint32_t NamespaceForId(SurfaceId id);

    // This needs to be called before any sequences with this allocator's
    // namespace will be used to enforce destruction dependencies.
    // When this SurfaceIdAllocator is destroyed, its namespace is
    // automatically invalidated and any remaining sequences with that
    // namespace will be ignored. This method does not need to be called in
    // contexts where there is no SurfaceManager (e.g. a renderer process).
    void RegisterSurfaceIdNamespace(SurfaceManager* manager);

    uint32_t id_namespace() const { return id_namespace_; }

private:
    const uint32_t id_namespace_;
    uint32_t next_id_;
    SurfaceManager* manager_;

    DISALLOW_COPY_AND_ASSIGN(SurfaceIdAllocator);
};

} // namespace cc

#endif // CC_SURFACES_SURFACE_ID_ALLOCATOR_H_
