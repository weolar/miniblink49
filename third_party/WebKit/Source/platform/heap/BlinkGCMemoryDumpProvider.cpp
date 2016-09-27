// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "Source/platform/heap/BlinkGCMemoryDumpProvider.h"

#include "platform/heap/Handle.h"
#include "public/platform/Platform.h"
#include "public/platform/WebMemoryAllocatorDump.h"
#include "public/platform/WebProcessMemoryDump.h"
#include "wtf/StdLibExtras.h"
#include "wtf/Threading.h"

namespace blink {

BlinkGCMemoryDumpProvider* BlinkGCMemoryDumpProvider::instance()
{
    DEFINE_STATIC_LOCAL(BlinkGCMemoryDumpProvider, instance, ());
    return &instance;
}

BlinkGCMemoryDumpProvider::~BlinkGCMemoryDumpProvider()
{
}

bool BlinkGCMemoryDumpProvider::onMemoryDump(blink::WebProcessMemoryDump* memoryDump)
{
    Heap::collectGarbage(ThreadState::NoHeapPointersOnStack, ThreadState::TakeSnapshot, Heap::ForcedGC);
    String dumpName = String::format("blink_gc/thread_%lu", static_cast<unsigned long>(WTF::currentThread()));
    WebMemoryAllocatorDump* allocatorDump = memoryDump->createMemoryAllocatorDump(dumpName);
    allocatorDump->AddScalar("size", "bytes", Heap::allocatedSpace());

    dumpName.append("/allocated_objects");
    WebMemoryAllocatorDump* objectsDump = memoryDump->createMemoryAllocatorDump(dumpName);
    objectsDump->AddScalar("size", "bytes", Heap::allocatedObjectSize() + Heap::markedObjectSize());
    objectsDump->AddScalar("estimated_live_object_size", "bytes", Heap::estimatedLiveObjectSize());

    // Merge all dumps collected by Heap::collectGarbage.
    memoryDump->takeAllDumpsFrom(m_currentProcessMemoryDump.get());
    return true;
}

WebMemoryAllocatorDump* BlinkGCMemoryDumpProvider::createMemoryAllocatorDumpForCurrentGC(const String& absoluteName)
{
    return m_currentProcessMemoryDump->createMemoryAllocatorDump(absoluteName);
}

void BlinkGCMemoryDumpProvider::clearProcessDumpForCurrentGC()
{
    m_currentProcessMemoryDump->clear();
}

BlinkGCMemoryDumpProvider::BlinkGCMemoryDumpProvider()
    : m_currentProcessMemoryDump(adoptPtr(Platform::current()->createProcessMemoryDump()))
{
}

} // namespace blink
