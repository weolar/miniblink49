// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_PUBLIC_V8_PLATFORM_H_
#define GIN_PUBLIC_V8_PLATFORM_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "gin/gin_export.h"
#include "v8.h"
#include "v8-platform.h"

namespace gin {

#if V8_MAJOR_VERSION >= 7
class DefaultPlatformWrap;

class TracingControllerImpl : public v8::TracingController {
public:
    TracingControllerImpl() = default;
    ~TracingControllerImpl() override = default;

    // TracingController implementation.
    const uint8_t* GetCategoryGroupEnabled(const char* name) override;
    uint64_t AddTraceEvent(
        char phase,
        const uint8_t* category_enabled_flag,
        const char* name,
        const char* scope,
        uint64_t id,
        uint64_t bind_id,
        int32_t num_args,
        const char** arg_names,
        const uint8_t* arg_types,
        const uint64_t* arg_values,
        std::unique_ptr<v8::ConvertableToTraceFormat>* arg_convertables,
        unsigned int flags) override;

    uint64_t AddTraceEventWithTimestamp(
        char phase,
        const uint8_t* category_enabled_flag,
        const char* name,
        const char* scope,
        uint64_t id,
        uint64_t bind_id,
        int32_t num_args,
        const char** arg_names,
        const uint8_t* arg_types,
        const uint64_t* arg_values,
        std::unique_ptr<v8::ConvertableToTraceFormat>* arg_convertables,
        unsigned int flags,
        int64_t timestampMicroseconds) override;

    void UpdateTraceEventDuration(const uint8_t* category_enabled_flag, const char* name, uint64_t handle) override;
    void AddTraceStateObserver(TraceStateObserver* observer) override;
    void RemoveTraceStateObserver(TraceStateObserver* observer) override;

private:
    DISALLOW_COPY_AND_ASSIGN(TracingControllerImpl);
};
#endif

class V8ForegroundTaskRunner;

// A v8::Platform implementation to use with gin.
class GIN_EXPORT V8Platform : public NON_EXPORTED_BASE(v8::Platform) {
public:
    static V8Platform* Get();

    // v8::Platform implementation.
#if V8_MAJOR_VERSION >= 7
    virtual void CallOnWorkerThread(std::unique_ptr<v8::Task> task) override;
    virtual void CallDelayedOnWorkerThread(std::unique_ptr<v8::Task> task, double delay_in_seconds) override;
    virtual double CurrentClockTimeMillis() override;
    virtual v8::TracingController* GetTracingController() override
    {
        return m_tracingControllerImpl;
    }
    int NumberOfWorkerThreads(void) override;
    std::shared_ptr<v8::TaskRunner> GetForegroundTaskRunner(v8::Isolate*) override;
#else
    void CallOnBackgroundThread(v8::Task* task, v8::Platform::ExpectedRuntime expected_runtime) override;
#endif
    void CallOnForegroundThread(v8::Isolate* isolate, v8::Task* task) override;
    void CallDelayedOnForegroundThread(v8::Isolate* isolate, v8::Task* task, double delay_in_seconds) override;

    double MonotonicallyIncreasingTime() override;

private:
    //friend struct base::DefaultLazyInstanceTraits<V8Platform>;

    V8Platform();
    ~V8Platform() override;

#if V8_MAJOR_VERSION >= 7
    TracingControllerImpl* m_tracingControllerImpl;
    DefaultPlatformWrap* m_defaultPlatformWrap;
    std::shared_ptr<V8ForegroundTaskRunner> m_foregroundTaskRunner;
#endif

    DISALLOW_COPY_AND_ASSIGN(V8Platform);
};

}  // namespace gin

#endif  // GIN_PUBLIC_V8_PLATFORM_H_
