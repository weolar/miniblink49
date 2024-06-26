// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/memory_history.h"

#include <limits>

namespace cc {

// static
scoped_ptr<MemoryHistory> MemoryHistory::Create()
{
    return make_scoped_ptr(new MemoryHistory());
}

MemoryHistory::MemoryHistory() { }

void MemoryHistory::SaveEntry(const MemoryHistory::Entry& entry)
{
    ring_buffer_.SaveToBuffer(entry);
}

} // namespace cc
