// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_TEST_IN_PROCESS_CONTEXT_PROVIDER_H_
#define CC_TEST_TEST_IN_PROCESS_CONTEXT_PROVIDER_H_

#include "base/synchronization/lock.h"
#include "cc/output/context_provider.h"
#include "cc/test/test_gpu_memory_buffer_manager.h"
#include "cc/test/test_image_factory.h"
#include "skia/ext/refptr.h"

class GrContext;

namespace gpu {
class GLInProcessContext;
}

namespace cc {

scoped_ptr<gpu::GLInProcessContext> CreateTestInProcessContext();
scoped_ptr<gpu::GLInProcessContext> CreateTestInProcessContext(
    TestGpuMemoryBufferManager* gpu_memory_buffer_manager,
    TestImageFactory* image_factory);

class TestInProcessContextProvider : public ContextProvider {
public:
    TestInProcessContextProvider();

    bool BindToCurrentThread() override;
    gpu::gles2::GLES2Interface* ContextGL() override;
    gpu::ContextSupport* ContextSupport() override;
    class GrContext* GrContext() override;
    void InvalidateGrContext(uint32_t state) override;
    void SetupLock() override;
    base::Lock* GetLock() override;
    Capabilities ContextCapabilities() override;
    void VerifyContexts() override;
    void DeleteCachedResources() override;
    bool DestroyedOnMainThread() override;
    void SetLostContextCallback(
        const LostContextCallback& lost_context_callback) override;
    void SetMemoryPolicyChangedCallback(
        const MemoryPolicyChangedCallback& memory_policy_changed_callback)
        override;

protected:
    friend class base::RefCountedThreadSafe<TestInProcessContextProvider>;
    ~TestInProcessContextProvider() override;

private:
    TestGpuMemoryBufferManager gpu_memory_buffer_manager_;
    TestImageFactory image_factory_;
    scoped_ptr<gpu::GLInProcessContext> context_;
    skia::RefPtr<class GrContext> gr_context_;
    base::Lock context_lock_;
};

} // namespace cc

#endif // CC_TEST_TEST_IN_PROCESS_CONTEXT_PROVIDER_H_
