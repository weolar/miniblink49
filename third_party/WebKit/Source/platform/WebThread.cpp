// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebThread.h"

#include "platform/Task.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/Assertions.h"
#include "wtf/OwnPtr.h"

#if OS(WIN)
#include <windows.h>
#elif OS(POSIX)
#include <unistd.h>
#endif

namespace blink {

#if OS(WIN)
static_assert(sizeof(blink::PlatformThreadId) >= sizeof(DWORD), "size of platform thread id is too small");
#elif OS(POSIX)
static_assert(sizeof(blink::PlatformThreadId) >= sizeof(pid_t), "size of platform thread id is too small");
#else
#error Unexpected platform
#endif

void WebThread::postTask(const WebTraceLocation& location, PassOwnPtr<Function<void()>> function)
{
    postTask(location, new blink::Task(function));
}

void WebThread::postDelayedTask(const WebTraceLocation& location, PassOwnPtr<Function<void()>> function, long long delayMs)
{
    postDelayedTask(location, new blink::Task(function), delayMs);
}

} // namespace blink
