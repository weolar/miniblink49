// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef  CEF_LIBCEF_COMMON_TASK_RUNNER_IMPL_H_
#define  CEF_LIBCEF_COMMON_TASK_RUNNER_IMPL_H_
#pragma once

#include "include/cef_task.h"

namespace blink {
class WebThread;
}

class CefTaskRunnerImpl : public CefTaskRunner {
public:
    explicit CefTaskRunnerImpl(blink::WebThread* task_runner);

    // Returns the task runner associated with |threadId|.
    static blink::WebThread* GetTaskRunner(CefThreadId threadId);
    // Returns the current task runner.
    static blink::WebThread* GetCurrentTaskRunner();

    // CefTaskRunner methods:
    bool IsSame(CefRefPtr<CefTaskRunner> that) override;
    bool BelongsToCurrentThread() override;
    bool BelongsToThread(CefThreadId threadId) override;
    bool PostTask(CefRefPtr<CefTask> task) override;
    bool PostDelayedTask(CefRefPtr<CefTask> task, int64 delay_ms) override;

private:
    blink::WebThread* m_taskRunner;

    //IMPLEMENT_REFCOUNTING(CefTaskRunnerImpl);
public:
    void AddRef() const OVERRIDE {
        ref_count_.AddRef();
    }
    bool Release() const OVERRIDE {
        if (ref_count_.Release()) {
            delete static_cast<const CefTaskRunnerImpl*>(this);
            return true;
        }
        return false;
    }
    bool HasOneRef() const OVERRIDE {
        return ref_count_.HasOneRef();
    }
private:
    CefRefCount ref_count_;

    DISALLOW_COPY_AND_ASSIGN(CefTaskRunnerImpl);
};

#endif  // CEF_LIBCEF_COMMON_TASK_RUNNER_IMPL_H_
