// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef libcef_common_CefTaskImpl_h
#define libcef_common_CefTaskImpl_h

#include "include/cef_task.h"

#include "third_party/WebKit/public/platform/WebTraceLocation.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/Functional.h"

bool CefPostBlinkTask(const blink::WebTraceLocation& location, CefThreadId threadId, WTF::PassOwnPtr<WTF::Function<void()>> task);

bool CefPostBlinkDelayedTask(const blink::WebTraceLocation& location, CefThreadId threadId, WTF::PassOwnPtr<WTF::Function<void()>> task, int64 delayMs);

#endif // libcef_common_CefTaskImpl_h