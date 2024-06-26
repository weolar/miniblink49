#ifndef CONTENT_BROWSER_COMPOSITOR_BROWSER_CONTENT_PROVIDER_H_
#define CONTENT_BROWSER_COMPOSITOR_BROWSER_CONTENT_PROVIDER_H_

#include "cc/output/context_provider.h"

namespace content {

class BrowserContextProvider : public cc::ContextProvider {
public:
    static scoped_refptr<BrowserContextProvider> Create();

    // cc::ContextProvider implementation.
    virtual bool BindToCurrentThread() override;
    virtual void DetachFromThread();

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

private:
    void InitializeCapabilities();

    cc::ContextProvider::Capabilities capabilities_;
    //scoped_ptr<gpu::gles2::GLES2Implementation> real_gl_;
};

}
#endif // CONTENT_BROWSER_COMPOSITOR_BROWSER_CONTENT_PROVIDER_H_