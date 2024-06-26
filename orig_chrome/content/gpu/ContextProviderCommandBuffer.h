// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_COMMON_GPU_CLIENT_CONTEXT_PROVIDER_COMMAND_BUFFER_H_
#define CONTENT_COMMON_GPU_CLIENT_CONTEXT_PROVIDER_COMMAND_BUFFER_H_

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "base/threading/thread_checker.h"
#include "cc/blink/context_provider_web_context.h"
#include "cc/output/context_provider.h"
#include "content/gpu/CommandBufferMetrics.h"
#include "content/gpu/WgContext3dCmdBufImpl.h"
#include "gpu/blink/webgraphicscontext3d_in_process_command_buffer_impl.h"
#include "skia/ext/refptr.h"

namespace content {

class GrContextForWebGraphicsContext3D;
class GrGLInterfaceForWebGraphicsContext3D;
class CommandBufferClientImpl;

// Implementation of cc::ContextProvider that provides a
// WebGraphicsContext3DInProcessCommandBufferImpl context and a GrContext.
class ContextProviderCommandBuffer : public cc_blink::ContextProviderWebContext {
public:
    static scoped_refptr<ContextProviderCommandBuffer> Create(
        scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> context3d,
        CommandBufferContextType type);

    //gpu::InProcessCommandBuffer* GetCommandBufferProxy();

    // cc_blink::ContextProviderWebContext implementation.
    gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl* WebContext3D() override;

    // cc::ContextProvider implementation.
    virtual bool BindToCurrentThread() override;
    virtual void DetachFromThread() override;
    virtual gpu::gles2::GLES2Interface* ContextGL() override;
    virtual gpu::ContextSupport* ContextSupport() override;
    virtual class GrContext* GrContext() override;
    virtual void InvalidateGrContext(uint32_t state) override;
    virtual void SetupLock() override;
    virtual base::Lock* GetLock() override;
    virtual Capabilities ContextCapabilities() override;
    virtual void DeleteCachedResources() override;
    virtual void SetLostContextCallback(const LostContextCallback& lost_context_callback) override;

    virtual void VerifyContexts() override;
    virtual bool DestroyedOnMainThread() override;
    virtual void SetMemoryPolicyChangedCallback(const cc::ContextProvider::MemoryPolicyChangedCallback& memory_policy_changed_callback) override;

protected:
    ContextProviderCommandBuffer(scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> context3d, CommandBufferContextType type);
    ~ContextProviderCommandBuffer() override;

    void OnLostContext();

private:
    gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl* WebContext3DNoChecks();
    void InitializeCapabilities();

    base::ThreadChecker main_thread_checker_;
    base::ThreadChecker context_thread_checker_;

    skia::RefPtr<GrGLInterfaceForWebGraphicsContext3D> gr_interface_;
    scoped_ptr<GrContextForWebGraphicsContext3D> gr_context_;

    cc::ContextProvider::Capabilities capabilities_;
    CommandBufferContextType context_type_;
    std::string debug_name_;

    LostContextCallback lost_context_callback_;

    base::Lock context_lock_;

    class LostContextCallbackProxy;
    friend class LostContextCallbackProxy;
    scoped_ptr<LostContextCallbackProxy> lost_context_callback_proxy_;
};

} // namespace content

#endif // CONTENT_COMMON_GPU_CLIENT_CONTEXT_PROVIDER_COMMAND_BUFFER_H_
