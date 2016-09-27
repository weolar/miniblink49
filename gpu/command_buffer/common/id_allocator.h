// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the definition of the IdAllocator class.

#ifndef GPU_COMMAND_BUFFER_CLIENT_ID_ALLOCATOR_H_
#define GPU_COMMAND_BUFFER_CLIENT_ID_ALLOCATOR_H_

#include <stdint.h>

#include <set>
#include <utility>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "gpu/gpu_export.h"

namespace gpu {

// A resource ID, key to the resource maps.
typedef uint32_t ResourceId;
// Invalid resource ID.
static const ResourceId kInvalidResource = 0u;

class GPU_EXPORT IdAllocatorInterface {
 public:
  virtual ~IdAllocatorInterface();

  // Allocates a new resource ID.
  virtual ResourceId AllocateID() = 0;

  // Allocates an Id starting at or above desired_id.
  // Note: may wrap if it starts near limit.
  virtual ResourceId AllocateIDAtOrAbove(ResourceId desired_id) = 0;

  // Marks an id as used. Returns false if id was already used.
  virtual bool MarkAsUsed(ResourceId id) = 0;

  // Frees a resource ID.
  virtual void FreeID(ResourceId id) = 0;

  // Checks whether or not a resource ID is in use.
  virtual bool InUse(ResourceId id) const = 0;
};

// A class to manage the allocation of resource IDs.
class GPU_EXPORT IdAllocator : public IdAllocatorInterface {
 public:
  IdAllocator();
  virtual ~IdAllocator();

  // Implement IdAllocatorInterface.
  virtual ResourceId AllocateID() OVERRIDE;
  virtual ResourceId AllocateIDAtOrAbove(ResourceId desired_id) OVERRIDE;
  virtual bool MarkAsUsed(ResourceId id) OVERRIDE;
  virtual void FreeID(ResourceId id) OVERRIDE;
  virtual bool InUse(ResourceId id) const OVERRIDE;

 private:
  // TODO(gman): This would work much better with ranges or a hash table.
  typedef std::set<ResourceId> ResourceIdSet;

  // The highest ID on the used list.
  ResourceId LastUsedId() const;

  // Lowest ID that isn't on the used list. This is slow, use as a last resort.
  ResourceId FindFirstUnusedId() const;

  ResourceIdSet used_ids_;
  ResourceIdSet free_ids_;

  DISALLOW_COPY_AND_ASSIGN(IdAllocator);
};

// A class to manage the allocation of resource IDs that are never reused. This
// implementation does not track which IDs are currently used. It is useful for
// shared and programs which cannot be implicitly created by binding a
// previously unused ID.
class NonReusedIdAllocator : public IdAllocatorInterface {
 public:
  NonReusedIdAllocator();
  virtual ~NonReusedIdAllocator();

  // Implement IdAllocatorInterface.
  virtual ResourceId AllocateID() OVERRIDE;
  virtual ResourceId AllocateIDAtOrAbove(ResourceId desired_id) OVERRIDE;
  virtual bool MarkAsUsed(ResourceId id) OVERRIDE;
  virtual void FreeID(ResourceId id) OVERRIDE;
  virtual bool InUse(ResourceId id) const OVERRIDE;

 private:
  ResourceId last_id_;

  DISALLOW_COPY_AND_ASSIGN(NonReusedIdAllocator);
};

}  // namespace gpu

#endif  // GPU_COMMAND_BUFFER_CLIENT_ID_ALLOCATOR_H_
