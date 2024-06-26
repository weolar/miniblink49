// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface.h"

#include <algorithm>

#include "cc/output/compositor_frame.h"
#include "cc/output/copy_output_request.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_id_allocator.h"
#include "cc/surfaces/surface_manager.h"

namespace cc {

// The frame index starts at 2 so that empty frames will be treated as
// completely damaged the first time they're drawn from.
static const int kFrameIndexStart = 2;

Surface::Surface(SurfaceId id, SurfaceFactory* factory)
    : surface_id_(id)
    , factory_(factory->AsWeakPtr())
    , frame_index_(kFrameIndexStart)
    , destroyed_(false)
{
}

Surface::~Surface()
{
    ClearCopyRequests();
    if (current_frame_ && factory_) {
        ReturnedResourceArray current_resources;
        TransferableResource::ReturnResources(
            current_frame_->delegated_frame_data->resource_list,
            &current_resources);
        factory_->UnrefResources(current_resources);
    }
    if (!draw_callback_.is_null())
        draw_callback_.Run(SurfaceDrawStatus::DRAW_SKIPPED);
}

void Surface::QueueFrame(scoped_ptr<CompositorFrame> frame,
    const DrawCallback& callback)
{
    DCHECK(factory_);
    ClearCopyRequests();

    if (frame) {
        TakeLatencyInfo(&frame->metadata.latency_info);
    }

    scoped_ptr<CompositorFrame> previous_frame = current_frame_.Pass();
    current_frame_ = frame.Pass();

    if (current_frame_) {
        factory_->ReceiveFromChild(
            current_frame_->delegated_frame_data->resource_list);
    }

    // Empty frames shouldn't be drawn and shouldn't contribute damage, so don't
    // increment frame index for them.
    if (current_frame_ && !current_frame_->delegated_frame_data->render_pass_list.empty())
        ++frame_index_;

    std::vector<SurfaceId> new_referenced_surfaces;
    if (current_frame_) {
        for (auto& render_pass :
            current_frame_->delegated_frame_data->render_pass_list) {
            new_referenced_surfaces.insert(new_referenced_surfaces.end(),
                render_pass->referenced_surfaces.begin(),
                render_pass->referenced_surfaces.end());
        }
    }

    if (previous_frame) {
        ReturnedResourceArray previous_resources;
        TransferableResource::ReturnResources(
            previous_frame->delegated_frame_data->resource_list,
            &previous_resources);
        factory_->UnrefResources(previous_resources);
    }
    if (!draw_callback_.is_null())
        draw_callback_.Run(SurfaceDrawStatus::DRAW_SKIPPED);
    draw_callback_ = callback;

    bool referenced_surfaces_changed = (referenced_surfaces_ != new_referenced_surfaces);
    referenced_surfaces_ = new_referenced_surfaces;
    std::vector<uint32_t> satisfies_sequences;
    if (current_frame_)
        current_frame_->metadata.satisfies_sequences.swap(satisfies_sequences);
    if (referenced_surfaces_changed || !satisfies_sequences.empty()) {
        // Notify the manager that sequences were satisfied either if some new
        // sequences were satisfied, or if the set of referenced surfaces changed
        // to force a GC to happen.
        factory_->manager()->DidSatisfySequences(
            SurfaceIdAllocator::NamespaceForId(surface_id_), &satisfies_sequences);
    }
}

void Surface::RequestCopyOfOutput(scoped_ptr<CopyOutputRequest> copy_request)
{
    if (current_frame_ && !current_frame_->delegated_frame_data->render_pass_list.empty())
        current_frame_->delegated_frame_data->render_pass_list.back()
            ->copy_requests.push_back(copy_request.Pass());
    else
        copy_request->SendEmptyResult();
}

void Surface::TakeCopyOutputRequests(
    std::multimap<RenderPassId, CopyOutputRequest*>* copy_requests)
{
    DCHECK(copy_requests->empty());
    if (current_frame_) {
        for (const auto& render_pass :
            current_frame_->delegated_frame_data->render_pass_list) {
            while (!render_pass->copy_requests.empty()) {
                scoped_ptr<CopyOutputRequest> request = render_pass->copy_requests.take_back();
                render_pass->copy_requests.pop_back();
                copy_requests->insert(
                    std::make_pair(render_pass->id, request.release()));
            }
        }
    }
}

const CompositorFrame* Surface::GetEligibleFrame()
{
    return current_frame_.get();
}

void Surface::TakeLatencyInfo(std::vector<ui::LatencyInfo>* latency_info)
{
    if (!current_frame_)
        return;
    if (latency_info->empty()) {
        current_frame_->metadata.latency_info.swap(*latency_info);
        return;
    }
    std::copy(current_frame_->metadata.latency_info.begin(),
        current_frame_->metadata.latency_info.end(),
        std::back_inserter(*latency_info));
    current_frame_->metadata.latency_info.clear();
}

void Surface::RunDrawCallbacks(SurfaceDrawStatus drawn)
{
    if (!draw_callback_.is_null()) {
        DrawCallback callback = draw_callback_;
        draw_callback_ = DrawCallback();
        callback.Run(drawn);
    }
}

void Surface::AddDestructionDependency(SurfaceSequence sequence)
{
    destruction_dependencies_.push_back(sequence);
}

void Surface::SatisfyDestructionDependencies(
    base::hash_set<SurfaceSequence>* sequences,
    base::hash_set<uint32_t>* valid_id_namespaces)
{
    destruction_dependencies_.erase(
        std::remove_if(destruction_dependencies_.begin(),
            destruction_dependencies_.end(),
            [sequences, valid_id_namespaces](SurfaceSequence seq) {
                return (!!sequences->erase(seq) || !valid_id_namespaces->count(seq.id_namespace));
            }),
        destruction_dependencies_.end());
}

void Surface::ClearCopyRequests()
{
    if (current_frame_) {
        for (const auto& render_pass :
            current_frame_->delegated_frame_data->render_pass_list) {
            for (const auto& copy_request : render_pass->copy_requests)
                copy_request->SendEmptyResult();
        }
    }
}

} // namespace cc
