// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#ifndef RemoteBridgeFrameOwner_h
#define RemoteBridgeFrameOwner_h

#include "core/frame/FrameOwner.h"
#include "web/WebLocalFrameImpl.h"

namespace blink {

// Helper class to bridge communication for a frame with a remote parent.
// Currently, it serves two purposes:
// 1. Allows the local frame's loader to retrieve sandbox flags associated with
//    its owner element in another process.
// 2. Trigger a load event on its owner element once it finishes a load.
class RemoteBridgeFrameOwner final : public NoBaseWillBeGarbageCollectedFinalized<RemoteBridgeFrameOwner>, public FrameOwner {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(RemoteBridgeFrameOwner);
public:
    static PassOwnPtrWillBeRawPtr<RemoteBridgeFrameOwner> create(PassRefPtrWillBeRawPtr<WebLocalFrameImpl> frame, SandboxFlags flags)
    {
        return adoptPtrWillBeNoop(new RemoteBridgeFrameOwner(frame, flags));
    }

    bool isLocal() const override
    {
        return false;
    }

    SandboxFlags sandboxFlags() const override
    {
        return m_sandboxFlags;
    }

    void setSandboxFlags(SandboxFlags flags)
    {
        m_sandboxFlags = flags;
    }

    void setContentFrame(PassRefPtrWillBeRawPtr<WebLocalFrameImpl> frame)
    {
        m_frame = frame;
    }

    void dispatchLoad() override;

    void renderFallbackContent() override
    {
        // TODO(dcheng): Implement.
    }

    DECLARE_VIRTUAL_TRACE();

private:
    RemoteBridgeFrameOwner(PassRefPtrWillBeRawPtr<WebLocalFrameImpl>, SandboxFlags);

    RefPtrWillBeMember<WebLocalFrameImpl> m_frame;
    SandboxFlags m_sandboxFlags;
};

DEFINE_TYPE_CASTS(RemoteBridgeFrameOwner, FrameOwner, owner, !owner->isLocal(), !owner.isLocal());

} // namespace blink

#endif // RemoteBridgeFrameOwner_h
