// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_OUTPUT_CONTEXT_PROVIDER_H_
#define CC_OUTPUT_CONTEXT_PROVIDER_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/synchronization/lock.h"
#include "cc/base/cc_export.h"
#include "gpu/command_buffer/common/capabilities.h"

class GrContext;

namespace base {
class Lock;
}

namespace gpu {
class ContextSupport;
namespace gles2 {
    class GLES2Interface;
}
}

namespace cc {
struct ManagedMemoryPolicy;

class ContextProvider : public base::RefCountedThreadSafe<ContextProvider> {
public:
    class ScopedContextLock {
    public:
        explicit ScopedContextLock(ContextProvider* context_provider)
            : context_provider_(context_provider)
            , context_lock_(*context_provider_->GetLock())
        {
            // Allow current thread to use |context_provider_|.
            context_provider_->DetachFromThread();
        }
        ~ScopedContextLock()
        {
            // Allow usage by thread for which |context_provider_| is bound to.
            context_provider_->DetachFromThread();
        }

        gpu::gles2::GLES2Interface* ContextGL()
        {
            return context_provider_->ContextGL();
        }

    private:
        ContextProvider* const context_provider_;
        base::AutoLock context_lock_;
    };
    // Bind the 3d context to the current thread. This should be called before
    // accessing the contexts. Calling it more than once should have no effect.
    // Once this function has been called, the class should only be accessed
    // from the same thread unless the function has some explicitly specified
    // rules for access on a different thread. See SetupLockOnMainThread(), which
    // can be used to provide access from multiple threads.
    virtual bool BindToCurrentThread() = 0;
    virtual void DetachFromThread() { }

    virtual gpu::gles2::GLES2Interface* ContextGL() = 0;
    virtual gpu::ContextSupport* ContextSupport() = 0;
    virtual class GrContext* GrContext() = 0;

    struct Capabilities {
        gpu::Capabilities gpu;
        size_t max_transfer_buffer_usage_bytes;

        CC_EXPORT Capabilities();
    };

    // Invalidates the cached OpenGL state in GrContext.
    // See skia GrContext::resetContext for details.
    virtual void InvalidateGrContext(uint32_t state) = 0;

    // Sets up a lock so this context can be used from multiple threads. After
    // calling this, all functions without explicit thread usage constraints can
    // be used on any thread while the lock returned by GetLock() is acquired.
    virtual void SetupLock() = 0;

    // Returns the lock that should be held if using this context from multiple
    // threads. This can be called on any thread.
    virtual base::Lock* GetLock() = 0;

    // Returns the capabilities of the currently bound 3d context.
    virtual Capabilities ContextCapabilities() = 0;

    // Ask the provider to check if the contexts are valid or lost. If they are,
    // this should invalidate the provider so that it can be replaced with a new
    // one.
    virtual void VerifyContexts() = 0;

    // Delete all cached gpu resources.
    virtual void DeleteCachedResources() = 0;

    // A method to be called from the main thread that should return true if
    // the context inside the provider is no longer valid.
    virtual bool DestroyedOnMainThread() = 0;

    // Sets a callback to be called when the context is lost. This should be
    // called from the same thread that the context is bound to. To avoid races,
    // it should be called before BindToCurrentThread(), or VerifyContexts()
    // should be called after setting the callback.
    typedef base::Closure LostContextCallback;
    virtual void SetLostContextCallback(
        const LostContextCallback& lost_context_callback)
        = 0;

    // Sets a callback to be called when the memory policy changes. This should be
    // called from the same thread that the context is bound to.
    typedef base::Callback<void(const ManagedMemoryPolicy& policy)>
        MemoryPolicyChangedCallback;
    virtual void SetMemoryPolicyChangedCallback(
        const MemoryPolicyChangedCallback& memory_policy_changed_callback)
        = 0;

protected:
    friend class base::RefCountedThreadSafe<ContextProvider>;
    virtual ~ContextProvider() { }
};

} // namespace cc

#endif // CC_OUTPUT_CONTEXT_PROVIDER_H_
