// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ConsoleMemory_h
#define ConsoleMemory_h

#include "platform/Supplementable.h"

namespace blink {

class Console;
class MemoryInfo;

class ConsoleMemory final : public GarbageCollected<ConsoleMemory>, public HeapSupplement<Console> {
    USING_GARBAGE_COLLECTED_MIXIN(ConsoleMemory);
public:
    static ConsoleMemory& from(Console&);
    static MemoryInfo* memory(Console&);
    static void setMemory(Console&, MemoryInfo*) { }

    DECLARE_VIRTUAL_TRACE();

private:
    static const char* supplementName() { return "ConsoleMemory"; }
    MemoryInfo* memory();

    Member<MemoryInfo> m_memory;
};

} // namespace blink

#endif // ConsoleMemory_h
