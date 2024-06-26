// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_output_surface.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "cc/output/compositor_frame_ack.h"
#include "cc/output/output_surface_client.h"
#include "cc/resources/returned_resource.h"
#include "cc/test/begin_frame_args_test.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

FakeOutputSurface::FakeOutputSurface(
    scoped_refptr<ContextProvider> context_provider,
    scoped_refptr<ContextProvider> worker_context_provider,
    bool delegated_rendering)
    : OutputSurface(context_provider, worker_context_provider)
    , client_(NULL)
    , num_sent_frames_(0)
    , has_external_stencil_test_(false)
    , suspended_for_recycle_(false)
    , framebuffer_(0)
{
    if (delegated_rendering) {
        capabilities_.delegated_rendering = true;
        capabilities_.max_frames_pending = 1;
    }
}

FakeOutputSurface::FakeOutputSurface(
    scoped_refptr<ContextProvider> context_provider,
    bool delegated_rendering)
    : OutputSurface(context_provider)
    , client_(NULL)
    , num_sent_frames_(0)
    , has_external_stencil_test_(false)
    , suspended_for_recycle_(false)
    , framebuffer_(0)
{
    if (delegated_rendering) {
        capabilities_.delegated_rendering = true;
        capabilities_.max_frames_pending = 1;
    }
}

FakeOutputSurface::FakeOutputSurface(
    scoped_ptr<SoftwareOutputDevice> software_device,
    bool delegated_rendering)
    : OutputSurface(software_device.Pass())
    , client_(NULL)
    , num_sent_frames_(0)
    , has_external_stencil_test_(false)
    , suspended_for_recycle_(false)
    , framebuffer_(0)
{
    if (delegated_rendering) {
        capabilities_.delegated_rendering = true;
        capabilities_.max_frames_pending = 1;
    }
}

FakeOutputSurface::FakeOutputSurface(
    scoped_refptr<ContextProvider> context_provider,
    scoped_ptr<SoftwareOutputDevice> software_device,
    bool delegated_rendering)
    : OutputSurface(context_provider, software_device.Pass())
    , client_(NULL)
    , num_sent_frames_(0)
    , has_external_stencil_test_(false)
    , suspended_for_recycle_(false)
    , framebuffer_(0)
{
    if (delegated_rendering) {
        capabilities_.delegated_rendering = true;
        capabilities_.max_frames_pending = 1;
    }
}

FakeOutputSurface::~FakeOutputSurface() { }

void FakeOutputSurface::SwapBuffers(CompositorFrame* frame)
{
    if (frame->delegated_frame_data || !context_provider()) {
        frame->AssignTo(&last_sent_frame_);

        if (last_sent_frame_.delegated_frame_data) {
            resources_held_by_parent_.insert(
                resources_held_by_parent_.end(),
                last_sent_frame_.delegated_frame_data->resource_list.begin(),
                last_sent_frame_.delegated_frame_data->resource_list.end());
        }

        ++num_sent_frames_;
    } else {
        last_swap_rect_ = frame->gl_frame_data->sub_buffer_rect;
        frame->AssignTo(&last_sent_frame_);
        ++num_sent_frames_;
    }
    PostSwapBuffersComplete();
    client_->DidSwapBuffers();
}

void FakeOutputSurface::BindFramebuffer()
{
    if (framebuffer_)
        context_provider_->ContextGL()->BindFramebuffer(GL_FRAMEBUFFER,
            framebuffer_);
    else
        OutputSurface::BindFramebuffer();
}

bool FakeOutputSurface::BindToClient(OutputSurfaceClient* client)
{
    if (OutputSurface::BindToClient(client)) {
        client_ = client;
        if (memory_policy_to_set_at_bind_) {
            client_->SetMemoryPolicy(*memory_policy_to_set_at_bind_.get());
            memory_policy_to_set_at_bind_ = nullptr;
        }
        return true;
    } else {
        return false;
    }
}

void FakeOutputSurface::SetTreeActivationCallback(
    const base::Closure& callback)
{
    DCHECK(client_);
    client_->SetTreeActivationCallback(callback);
}

void FakeOutputSurface::ReturnResource(unsigned id, CompositorFrameAck* ack)
{
    TransferableResourceArray::iterator it;
    for (it = resources_held_by_parent_.begin();
         it != resources_held_by_parent_.end();
         ++it) {
        if (it->id == id)
            break;
    }
    DCHECK(it != resources_held_by_parent_.end());
    ack->resources.push_back(it->ToReturnedResource());
    resources_held_by_parent_.erase(it);
}

bool FakeOutputSurface::HasExternalStencilTest() const
{
    return has_external_stencil_test_;
}

bool FakeOutputSurface::SurfaceIsSuspendForRecycle() const
{
    return suspended_for_recycle_;
}

void FakeOutputSurface::SetMemoryPolicyToSetAtBind(
    scoped_ptr<ManagedMemoryPolicy> memory_policy_to_set_at_bind)
{
    memory_policy_to_set_at_bind_.swap(memory_policy_to_set_at_bind);
}

} // namespace cc
