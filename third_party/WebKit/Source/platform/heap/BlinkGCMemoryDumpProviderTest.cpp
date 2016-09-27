// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/heap/BlinkGCMemoryDumpProvider.h"

#include "public/platform/Platform.h"
#include "public/platform/WebProcessMemoryDump.h"
#include "wtf/Threading.h"

#include <gtest/gtest.h>

namespace blink {

TEST(BlinkGCDumpProviderTest, MemoryDump)
{
    WebProcessMemoryDump* dump  = Platform::current()->createProcessMemoryDump();
    ASSERT(dump);
    BlinkGCMemoryDumpProvider::instance()->onMemoryDump(dump);
    ASSERT(dump->getMemoryAllocatorDump(String::format("blink_gc/thread_%lu", static_cast<unsigned long>(WTF::currentThread()))));
    ASSERT(dump->getMemoryAllocatorDump(String::format("blink_gc/thread_%lu/allocated_objects", static_cast<unsigned long>(WTF::currentThread()))));
}

} // namespace blink
