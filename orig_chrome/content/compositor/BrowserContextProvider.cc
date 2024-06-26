
#include "orig_chrome/content/compositor/BrowserContextProvider.h"

namespace content {

scoped_refptr<BrowserContextProvider> BrowserContextProvider::Create()
{
    return new BrowserContextProvider();
}

bool BrowserContextProvider::BindToCurrentThread()
{
    InitializeCapabilities();
    return true;
}

void BrowserContextProvider::DetachFromThread()
{
}

gpu::gles2::GLES2Interface* BrowserContextProvider::ContextGL()
{
    //   real_gl_.reset(new gpu::gles2::GLES2Implementation(gles2_helper_.get(),
    //     gles2_share_group,
    //     transfer_buffer_.get(),
    //     bind_generates_resources,
    //     lose_context_when_out_of_memory_,
    //     command_buffer_.get()));
    return nullptr;
}

gpu::ContextSupport* BrowserContextProvider::ContextSupport()
{
    return nullptr;
}

class GrContext* BrowserContextProvider::GrContext()
{
    return nullptr;
}

void BrowserContextProvider::InvalidateGrContext(uint32_t state)
{
}

void BrowserContextProvider::SetupLock()
{
}

base::Lock* BrowserContextProvider::GetLock()
{
    return nullptr;
}

cc::ContextProvider::Capabilities BrowserContextProvider::ContextCapabilities()
{
    return capabilities_;
}

void BrowserContextProvider::DeleteCachedResources()
{
}

void BrowserContextProvider::SetLostContextCallback(const LostContextCallback& lost_context_callback)
{
}

void BrowserContextProvider::InitializeCapabilities()
{
    Capabilities caps;
    //caps.gpu = context3d_->GetImplementation()->capabilities();

    size_t mapped_memory_limit = std::numeric_limits<size_t>::max();
    caps.max_transfer_buffer_usage_bytes = std::numeric_limits<size_t>::max();

    capabilities_ = caps;
}

void BrowserContextProvider::VerifyContexts()
{
}

bool BrowserContextProvider::DestroyedOnMainThread()
{
    DebugBreak();
    return true;
}

void BrowserContextProvider::SetMemoryPolicyChangedCallback(const cc::ContextProvider::MemoryPolicyChangedCallback& memory_policy_changed_callback)
{
}

} // content
