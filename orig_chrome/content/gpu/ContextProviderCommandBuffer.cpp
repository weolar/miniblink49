// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/gpu/ContextProviderCommandBuffer.h"

#include <set>
#include <vector>

#include "base/callback_helpers.h"
#include "base/strings/stringprintf.h"
#include "cc/output/managed_memory_policy.h"
#include "content/gpu/GrcontextForWebgraphicscontext3d.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/service/in_process_command_buffer.h"
#include "third_party/skia/include/gpu/GrContext.h"

namespace content {

class ContextProviderCommandBuffer::LostContextCallbackProxy
    : public blink::WebGraphicsContext3D::WebGraphicsContextLostCallback {
public:
    explicit LostContextCallbackProxy(ContextProviderCommandBuffer* provider)
        : provider_(provider)
    {
        provider_->WebContext3DNoChecks()->setContextLostCallback(this);
    }

    ~LostContextCallbackProxy() override
    {
        provider_->WebContext3DNoChecks()->setContextLostCallback(NULL);
    }

    void onContextLost() override
    {
        provider_->OnLostContext();
    }

private:
    ContextProviderCommandBuffer* provider_;
};

scoped_refptr<ContextProviderCommandBuffer>
ContextProviderCommandBuffer::Create(scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> context3d, CommandBufferContextType type)
{
    if (!context3d)
        return NULL;

    return new ContextProviderCommandBuffer(context3d.Pass(), type);
}

ContextProviderCommandBuffer::ContextProviderCommandBuffer(scoped_ptr<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl> context3d, CommandBufferContextType type)
    : context_type_(type)
    , debug_name_(CommandBufferContextTypeToString(type))
{
    gr_interface_ = skia::AdoptRef(new GrGLInterfaceForWebGraphicsContext3D(
        context3d.Pass()));
    DCHECK(main_thread_checker_.CalledOnValidThread());
    DCHECK(gr_interface_->WebContext3D());
    context_thread_checker_.DetachFromThread();
}

ContextProviderCommandBuffer::~ContextProviderCommandBuffer()
{
    DCHECK(main_thread_checker_.CalledOnValidThread() || context_thread_checker_.CalledOnValidThread());

    // Destroy references to the context3d_ before leaking it.
    if (WebContext3DNoChecks())
        WebContext3DNoChecks()->SetLock(nullptr);
    lost_context_callback_proxy_.reset();
}

// gpu::InProcessCommandBuffer* ContextProviderCommandBuffer::GetCommandBufferProxy() {
//     return WebContext3D()->GetCommandBufferProxy();
// }

gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl* ContextProviderCommandBuffer::WebContext3D()
{
    DCHECK(gr_interface_);
    DCHECK(gr_interface_->WebContext3D());
    DCHECK(lost_context_callback_proxy_); // Is bound to thread.
    DCHECK(context_thread_checker_.CalledOnValidThread());

    return WebContext3DNoChecks();
}

gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl* ContextProviderCommandBuffer::WebContext3DNoChecks()
{
    DCHECK(gr_interface_);
    return static_cast<gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl*>(gr_interface_->WebContext3D());
}

bool ContextProviderCommandBuffer::BindToCurrentThread()
{
    // This is called on the thread the context will be used.
    DCHECK(context_thread_checker_.CalledOnValidThread());
    DCHECK(gr_interface_ && gr_interface_->WebContext3D());

    if (lost_context_callback_proxy_)
        return true;

    //WebContext3DNoChecks()->SetContextType(context_type_);
    if (!WebContext3DNoChecks()->InitializeOnCurrentThread())
        return false;

    gr_interface_->BindToCurrentThread();
    InitializeCapabilities();

    std::string unique_context_name = base::StringPrintf("%s-%p", debug_name_.c_str(), WebContext3DNoChecks());
    WebContext3DNoChecks()->traceBeginCHROMIUM("gpu_toplevel",
        unique_context_name.c_str());

    lost_context_callback_proxy_.reset(new LostContextCallbackProxy(this));
    return true;
}

void ContextProviderCommandBuffer::DetachFromThread()
{
    context_thread_checker_.DetachFromThread();
}

gpu::gles2::GLES2Interface* ContextProviderCommandBuffer::ContextGL()
{
    DCHECK(lost_context_callback_proxy_); // Is bound to thread.

    return WebContext3DNoChecks()->GetImplementation();
}

gpu::ContextSupport* ContextProviderCommandBuffer::ContextSupport()
{
    return WebContext3D()->GetContextSupport();
}

class GrContext* ContextProviderCommandBuffer::GrContext()
{
    DCHECK(lost_context_callback_proxy_); // Is bound to thread.
    DCHECK(context_thread_checker_.CalledOnValidThread());

    if (gr_context_)
        return gr_context_->get();

    gr_context_.reset(new GrContextForWebGraphicsContext3D(gr_interface_));

    // If GlContext is already lost, also abandon the new GrContext.
    if (gr_context_->get() && ContextGL()->GetGraphicsResetStatusKHR() != GL_NO_ERROR)
        gr_context_->get()->abandonContext();

    return gr_context_->get();
}

void ContextProviderCommandBuffer::InvalidateGrContext(uint32_t state)
{
    if (gr_context_) {
        DCHECK(lost_context_callback_proxy_); // Is bound to thread.
        DCHECK(context_thread_checker_.CalledOnValidThread());
        gr_context_->get()->resetContext(state);
    }
}

void ContextProviderCommandBuffer::SetupLock()
{
    WebContext3D()->SetLock(&context_lock_);
}

base::Lock* ContextProviderCommandBuffer::GetLock()
{
    return &context_lock_;
}

cc::ContextProvider::Capabilities ContextProviderCommandBuffer::ContextCapabilities()
{
    DCHECK(lost_context_callback_proxy_); // Is bound to thread.
    DCHECK(context_thread_checker_.CalledOnValidThread());

    return capabilities_;
}

void ContextProviderCommandBuffer::DeleteCachedResources()
{
    DCHECK(context_thread_checker_.CalledOnValidThread());

    if (gr_context_)
        gr_context_->FreeGpuResources();
}

void ContextProviderCommandBuffer::OnLostContext()
{
    DCHECK(context_thread_checker_.CalledOnValidThread());

    if (!lost_context_callback_.is_null())
        base::ResetAndReturn(&lost_context_callback_).Run();
    if (gr_context_)
        gr_context_->OnLostContext();
}

void ContextProviderCommandBuffer::InitializeCapabilities()
{
    Capabilities caps;
    caps.gpu = WebContext3DNoChecks()->GetImplementation()->capabilities();

    size_t mapped_memory_limit = WebContext3DNoChecks()->GetMappedMemoryLimit();
    caps.max_transfer_buffer_usage_bytes = mapped_memory_limit == gpu_blink::WebGraphicsContext3DInProcessCommandBufferImpl::kNoLimit
        ? std::numeric_limits<size_t>::max()
        : mapped_memory_limit;

    caps.gpu.image = true; // TODO weolar
    capabilities_ = caps;
}

void ContextProviderCommandBuffer::SetLostContextCallback(
    const LostContextCallback& lost_context_callback)
{
    DCHECK(context_thread_checker_.CalledOnValidThread());
    DCHECK(lost_context_callback_.is_null() || lost_context_callback.is_null());
    lost_context_callback_ = lost_context_callback;
}

void ContextProviderCommandBuffer::VerifyContexts()
{
    DebugBreak();
}

bool ContextProviderCommandBuffer::DestroyedOnMainThread()
{
    DebugBreak();
    return false;
}

void ContextProviderCommandBuffer::SetMemoryPolicyChangedCallback(const cc::ContextProvider::MemoryPolicyChangedCallback& memory_policy_changed_callback)
{
}

} // namespace content
