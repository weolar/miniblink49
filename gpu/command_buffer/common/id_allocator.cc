// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file contains the implementation of IdAllocator.

#include "gpu/command_buffer/common/id_allocator.h"

#include "base/logging.h"

namespace gpu {

IdAllocatorInterface::~IdAllocatorInterface() {
}

IdAllocator::IdAllocator() {}

IdAllocator::~IdAllocator() {}

ResourceId IdAllocator::AllocateID() {
  ResourceId id;
  ResourceIdSet::iterator iter = free_ids_.begin();
  if (iter != free_ids_.end()) {
    id = *iter;
  } else {
    id = LastUsedId() + 1;
    if (!id) {
      // We wrapped around to 0.
      id = FindFirstUnusedId();
    }
  }
  MarkAsUsed(id);
  return id;
}

ResourceId IdAllocator::AllocateIDAtOrAbove(ResourceId desired_id) {
  ResourceId id;
  ResourceIdSet::iterator iter = free_ids_.lower_bound(desired_id);
  if (iter != free_ids_.end()) {
    id = *iter;
  } else if (LastUsedId() < desired_id) {
    id = desired_id;
  } else {
    id = LastUsedId() + 1;
    if (!id) {
      // We wrapped around to 0.
      id = FindFirstUnusedId();
    }
  }
  MarkAsUsed(id);
  return id;
}

bool IdAllocator::MarkAsUsed(ResourceId id) {
  DCHECK(id);
  free_ids_.erase(id);
  std::pair<ResourceIdSet::iterator, bool> result = used_ids_.insert(id);
  return result.second;
}

void IdAllocator::FreeID(ResourceId id) {
  if (id) {
    used_ids_.erase(id);
    free_ids_.insert(id);
  }
}

bool IdAllocator::InUse(ResourceId id) const {
  return id == kInvalidResource || used_ids_.find(id) != used_ids_.end();
}

ResourceId IdAllocator::LastUsedId() const {
  if (used_ids_.empty()) {
    return 0u;
  } else {
    return *used_ids_.rbegin();
  }
}

ResourceId IdAllocator::FindFirstUnusedId() const {
  ResourceId id = 1;
  for (ResourceIdSet::const_iterator it = used_ids_.begin();
       it != used_ids_.end(); ++it) {
    if ((*it) != id) {
      return id;
    }
    ++id;
  }
  return id;
}

NonReusedIdAllocator::NonReusedIdAllocator() : last_id_(0) {
}

NonReusedIdAllocator::~NonReusedIdAllocator() {
}

ResourceId NonReusedIdAllocator::AllocateID() {
  return ++last_id_;
}

ResourceId NonReusedIdAllocator::AllocateIDAtOrAbove(ResourceId desired_id) {
  if (desired_id > last_id_)
    last_id_ = desired_id;

  return ++last_id_;
}

bool NonReusedIdAllocator::MarkAsUsed(ResourceId id) {
  NOTREACHED();
  return false;
}

void NonReusedIdAllocator::FreeID(ResourceId id) {
}

bool NonReusedIdAllocator::InUse(ResourceId id) const {
  NOTREACHED();
  return false;
}

}  // namespace gpu
