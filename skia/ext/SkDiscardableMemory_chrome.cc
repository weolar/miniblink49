// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "SkDiscardableMemory_chrome.h"

SkDiscardableMemoryChrome::~SkDiscardableMemoryChrome() {}

bool SkDiscardableMemoryChrome::lock() {
  const base::DiscardableMemoryLockStatus status = discardable_->Lock();
  switch (status) {
    case base::DISCARDABLE_MEMORY_LOCK_STATUS_SUCCESS:
      return true;
    case base::DISCARDABLE_MEMORY_LOCK_STATUS_PURGED:
      discardable_->Unlock();
      return false;
    default:
      discardable_.reset();
      return false;
  }
}

void* SkDiscardableMemoryChrome::data() {
  return discardable_->Memory();
}

void SkDiscardableMemoryChrome::unlock() {
  discardable_->Unlock();
}

SkDiscardableMemoryChrome::SkDiscardableMemoryChrome(
    scoped_ptr<base::DiscardableMemory> memory)
    : discardable_(memory.Pass()) {
}

SkDiscardableMemory* SkDiscardableMemory::Create(size_t bytes) {
  scoped_ptr<base::DiscardableMemory> discardable(
      base::DiscardableMemory::CreateLockedMemory(bytes));
  if (!discardable)
    return NULL;
  return new SkDiscardableMemoryChrome(discardable.Pass());
}
