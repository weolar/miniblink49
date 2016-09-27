// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BlinkGCMemoryDumpProvider_h
#define BlinkGCMemoryDumpProvider_h

#include "platform/PlatformExport.h"
#include "public/platform/WebMemoryDumpProvider.h"
#include "wtf/OwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {
class WebMemoryAllocatorDump;

class PLATFORM_EXPORT BlinkGCMemoryDumpProvider final : public WebMemoryDumpProvider {
public:
    static BlinkGCMemoryDumpProvider* instance();
    ~BlinkGCMemoryDumpProvider() override;

    // WebMemoryDumpProvider implementation.
    bool onMemoryDump(WebProcessMemoryDump*) override;

    // The returned WebMemoryAllocatorDump is owned by
    // BlinkGCMemoryDumpProvider, and should not be retained (just used to
    // dump in the current call stack).
    WebMemoryAllocatorDump* createMemoryAllocatorDumpForCurrentGC(const String& absoluteName);

    // This must be called before taking a new process-wide GC snapshot, to
    // clear the previous dumps.
    void clearProcessDumpForCurrentGC();

    WebProcessMemoryDump* currentProcessMemoryDump() { return m_currentProcessMemoryDump.get(); }

private:
    BlinkGCMemoryDumpProvider();

    OwnPtr<WebProcessMemoryDump> m_currentProcessMemoryDump;
};

} // namespace blink

#endif
