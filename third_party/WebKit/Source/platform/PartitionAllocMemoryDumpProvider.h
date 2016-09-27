// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PartitionAllocMemoryDumpProvider_h
#define PartitionAllocMemoryDumpProvider_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebMemoryDumpProvider.h"

namespace blink {

class BLINK_PLATFORM_EXPORT PartitionAllocMemoryDumpProvider final : public WebMemoryDumpProvider {
public:
    static PartitionAllocMemoryDumpProvider* instance();
    ~PartitionAllocMemoryDumpProvider() override;

    // WebMemoryDumpProvider implementation.
    bool onMemoryDump(WebProcessMemoryDump*) override;

private:
    PartitionAllocMemoryDumpProvider();
};

} // namespace blink

#endif // PartitionAllocMemoryDumpProvider_h
