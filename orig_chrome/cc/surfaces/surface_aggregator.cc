// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface_aggregator.h"

#include <map>

#include "base/bind.h"
#include "base/containers/hash_tables.h"
#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "cc/base/math_util.h"
#include "cc/output/compositor_frame.h"
#include "cc/output/delegated_frame_data.h"
#include "cc/quads/draw_quad.h"
#include "cc/quads/render_pass_draw_quad.h"
#include "cc/quads/shared_quad_state.h"
#include "cc/quads/surface_draw_quad.h"
#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_manager.h"
#include "cc/trees/blocking_task_runner.h"

namespace cc {
namespace {

    void MoveMatchingRequests(
        RenderPassId id,
        std::multimap<RenderPassId, CopyOutputRequest*>* copy_requests,
        ScopedPtrVector<CopyOutputRequest>* output_requests)
    {
        auto request_range = copy_requests->equal_range(id);
        for (auto it = request_range.first; it != request_range.second; ++it) {
            DCHECK(it->second);
            output_requests->push_back(scoped_ptr<CopyOutputRequest>(it->second));
            it->second = nullptr;
        }
        copy_requests->erase(request_range.first, request_range.second);
    }

} // namespace

SurfaceAggregator::SurfaceAggregator(SurfaceManager* manager,
    ResourceProvider* provider,
    bool aggregate_only_damaged)
    : manager_(manager)
    , provider_(provider)
    , next_render_pass_id_(1)
    , aggregate_only_damaged_(aggregate_only_damaged)
{
    DCHECK(manager_);
}

SurfaceAggregator::~SurfaceAggregator() { }

// Create a clip rect for an aggregated quad from the original clip rect and
// the clip rect from the surface it's on.
SurfaceAggregator::ClipData SurfaceAggregator::CalculateClipRect(
    const ClipData& surface_clip,
    const ClipData& quad_clip,
    const gfx::Transform& target_transform)
{
    ClipData out_clip;
    if (surface_clip.is_clipped)
        out_clip = surface_clip;

    if (quad_clip.is_clipped) {
        // TODO(jamesr): This only works if target_transform maps integer
        // rects to integer rects.
        gfx::Rect final_clip = MathUtil::MapEnclosingClippedRect(target_transform, quad_clip.rect);
        if (out_clip.is_clipped)
            out_clip.rect.Intersect(final_clip);
        else
            out_clip.rect = final_clip;
        out_clip.is_clipped = true;
    }

    return out_clip;
}

class SurfaceAggregator::RenderPassIdAllocator {
public:
    explicit RenderPassIdAllocator(int* next_index)
        : next_index_(next_index)
    {
    }
    ~RenderPassIdAllocator() { }

    void AddKnownPass(RenderPassId id)
    {
        if (id_to_index_map_.find(id) != id_to_index_map_.end())
            return;
        id_to_index_map_[id] = (*next_index_)++;
    }

    RenderPassId Remap(RenderPassId id)
    {
        DCHECK(id_to_index_map_.find(id) != id_to_index_map_.end());
        return RenderPassId(1, id_to_index_map_[id]);
    }

private:
    base::hash_map<RenderPassId, int> id_to_index_map_;
    int* next_index_;

    DISALLOW_COPY_AND_ASSIGN(RenderPassIdAllocator);
};

static void UnrefHelper(base::WeakPtr<SurfaceFactory> surface_factory,
    const ReturnedResourceArray& resources,
    BlockingTaskRunner* main_thread_task_runner)
{
    if (surface_factory)
        surface_factory->UnrefResources(resources);
}

RenderPassId SurfaceAggregator::RemapPassId(RenderPassId surface_local_pass_id,
    SurfaceId surface_id)
{
    RenderPassIdAllocator* allocator = render_pass_allocator_map_.get(surface_id);
    if (!allocator) {
        allocator = new RenderPassIdAllocator(&next_render_pass_id_);
        render_pass_allocator_map_.set(surface_id, make_scoped_ptr(allocator));
    }
    allocator->AddKnownPass(surface_local_pass_id);
    return allocator->Remap(surface_local_pass_id);
}

int SurfaceAggregator::ChildIdForSurface(Surface* surface)
{
    SurfaceToResourceChildIdMap::iterator it = surface_id_to_resource_child_id_.find(surface->surface_id());
    if (it == surface_id_to_resource_child_id_.end()) {
        int child_id = provider_->CreateChild(base::Bind(&UnrefHelper, surface->factory()));
        if (surface->factory()) {
            provider_->SetChildNeedsSyncPoints(
                child_id, surface->factory()->needs_sync_points());
        }
        surface_id_to_resource_child_id_[surface->surface_id()] = child_id;
        return child_id;
    } else {
        return it->second;
    }
}

gfx::Rect SurfaceAggregator::DamageRectForSurface(const Surface* surface,
    const RenderPass& source,
    const gfx::Rect& full_rect)
{
    int previous_index = previous_contained_surfaces_[surface->surface_id()];
    if (previous_index == surface->frame_index())
        return gfx::Rect();
    else if (previous_index == surface->frame_index() - 1)
        return source.damage_rect;
    return full_rect;
}

void SurfaceAggregator::HandleSurfaceQuad(
    const SurfaceDrawQuad* surface_quad,
    const gfx::Transform& target_transform,
    const ClipData& clip_rect,
    RenderPass* dest_pass)
{
    SurfaceId surface_id = surface_quad->surface_id;
    // If this surface's id is already in our referenced set then it creates
    // a cycle in the graph and should be dropped.
    if (referenced_surfaces_.count(surface_id))
        return;
    Surface* surface = manager_->GetSurfaceForId(surface_id);
    if (!surface)
        return;
    const CompositorFrame* frame = surface->GetEligibleFrame();
    if (!frame)
        return;
    const DelegatedFrameData* frame_data = frame->delegated_frame_data.get();
    if (!frame_data)
        return;

    std::multimap<RenderPassId, CopyOutputRequest*> copy_requests;
    surface->TakeCopyOutputRequests(&copy_requests);

    const RenderPassList& render_pass_list = frame_data->render_pass_list;
    if (!valid_surfaces_.count(surface->surface_id())) {
        for (auto& request : copy_requests) {
            request.second->SendEmptyResult();
            delete request.second;
        }
        return;
    }

    SurfaceSet::iterator it = referenced_surfaces_.insert(surface_id).first;
    // TODO(vmpstr): provider check is a hack for unittests that don't set up a
    // resource provider.
    ResourceProvider::ResourceIdMap empty_map;
    const ResourceProvider::ResourceIdMap& child_to_parent_map = provider_ ? provider_->GetChildToParentMap(ChildIdForSurface(surface))
                                                                           : empty_map;
    bool merge_pass = surface_quad->shared_quad_state->opacity == 1.f && copy_requests.empty();

    const RenderPassList& referenced_passes = render_pass_list;
    size_t passes_to_copy = merge_pass ? referenced_passes.size() - 1 : referenced_passes.size();
    for (size_t j = 0; j < passes_to_copy; ++j) {
        const RenderPass& source = *referenced_passes[j];

        size_t sqs_size = source.shared_quad_state_list.size();
        size_t dq_size = source.quad_list.size();
        scoped_ptr<RenderPass> copy_pass(RenderPass::Create(sqs_size, dq_size));

        RenderPassId remapped_pass_id = RemapPassId(source.id, surface_id);

        copy_pass->SetAll(remapped_pass_id, source.output_rect, gfx::Rect(),
            source.transform_to_root_target,
            source.has_transparent_background);

        MoveMatchingRequests(source.id, &copy_requests, &copy_pass->copy_requests);

        // Contributing passes aggregated in to the pass list need to take the
        // transform of the surface quad into account to update their transform to
        // the root surface.
        copy_pass->transform_to_root_target.ConcatTransform(
            surface_quad->shared_quad_state->quad_to_target_transform);
        copy_pass->transform_to_root_target.ConcatTransform(target_transform);
        copy_pass->transform_to_root_target.ConcatTransform(
            dest_pass->transform_to_root_target);

        CopyQuadsToPass(source.quad_list, source.shared_quad_state_list,
            child_to_parent_map, gfx::Transform(), ClipData(),
            copy_pass.get(), surface_id);

        dest_pass_list_->push_back(copy_pass.Pass());
    }

    gfx::Transform surface_transform = surface_quad->shared_quad_state->quad_to_target_transform;
    surface_transform.ConcatTransform(target_transform);

    const RenderPass& last_pass = *render_pass_list.back();
    if (merge_pass) {
        // TODO(jamesr): Clean up last pass special casing.
        const QuadList& quads = last_pass.quad_list;

        // Intersect the transformed visible rect and the clip rect to create a
        // smaller cliprect for the quad.
        ClipData surface_quad_clip_rect(
            true, MathUtil::MapEnclosingClippedRect(surface_quad->shared_quad_state->quad_to_target_transform, surface_quad->visible_rect));
        if (surface_quad->shared_quad_state->is_clipped) {
            surface_quad_clip_rect.rect.Intersect(
                surface_quad->shared_quad_state->clip_rect);
        }

        ClipData quads_clip = CalculateClipRect(clip_rect, surface_quad_clip_rect, target_transform);

        CopyQuadsToPass(quads, last_pass.shared_quad_state_list,
            child_to_parent_map, surface_transform, quads_clip,
            dest_pass, surface_id);
    } else {
        RenderPassId remapped_pass_id = RemapPassId(last_pass.id, surface_id);

        SharedQuadState* shared_quad_state = CopySharedQuadState(surface_quad->shared_quad_state, target_transform,
            clip_rect, dest_pass);

        RenderPassDrawQuad* quad = dest_pass->CreateAndAppendDrawQuad<RenderPassDrawQuad>();
        quad->SetNew(shared_quad_state,
            surface_quad->rect,
            surface_quad->visible_rect,
            remapped_pass_id,
            0,
            gfx::Vector2dF(),
            gfx::Size(),
            FilterOperations(),
            gfx::Vector2dF(),
            FilterOperations());
    }

    referenced_surfaces_.erase(it);
}

SharedQuadState* SurfaceAggregator::CopySharedQuadState(
    const SharedQuadState* source_sqs,
    const gfx::Transform& target_transform,
    const ClipData& clip_rect,
    RenderPass* dest_render_pass)
{
    SharedQuadState* copy_shared_quad_state = dest_render_pass->CreateAndAppendSharedQuadState();
    copy_shared_quad_state->CopyFrom(source_sqs);
    // target_transform contains any transformation that may exist
    // between the context that these quads are being copied from (i.e. the
    // surface's draw transform when aggregated from within a surface) to the
    // target space of the pass. This will be identity except when copying the
    // root draw pass from a surface into a pass when the surface draw quad's
    // transform is not identity.
    copy_shared_quad_state->quad_to_target_transform.ConcatTransform(
        target_transform);

    ClipData new_clip_rect = CalculateClipRect(
        clip_rect, ClipData(source_sqs->is_clipped, source_sqs->clip_rect),
        target_transform);
    copy_shared_quad_state->is_clipped = new_clip_rect.is_clipped;
    copy_shared_quad_state->clip_rect = new_clip_rect.rect;
    return copy_shared_quad_state;
}

static gfx::Rect CalculateQuadSpaceDamageRect(
    const gfx::Transform& quad_to_target_transform,
    const gfx::Transform& target_to_root_transform,
    const gfx::Rect& root_damage_rect)
{
    gfx::Transform quad_to_root_transform(target_to_root_transform,
        quad_to_target_transform);
    gfx::Transform inverse_transform(gfx::Transform::kSkipInitialization);
    bool inverse_valid = quad_to_root_transform.GetInverse(&inverse_transform);
    DCHECK(inverse_valid);

    return MathUtil::ProjectEnclosingClippedRect(inverse_transform,
        root_damage_rect);
}

void SurfaceAggregator::CopyQuadsToPass(
    const QuadList& source_quad_list,
    const SharedQuadStateList& source_shared_quad_state_list,
    const ResourceProvider::ResourceIdMap& child_to_parent_map,
    const gfx::Transform& target_transform,
    const ClipData& clip_rect,
    RenderPass* dest_pass,
    SurfaceId surface_id)
{
    const SharedQuadState* last_copied_source_shared_quad_state = nullptr;
    const SharedQuadState* dest_shared_quad_state = nullptr;
    // If the current frame has copy requests then aggregate the entire
    // thing, as otherwise parts of the copy requests may be ignored.
    const bool ignore_undamaged = aggregate_only_damaged_ && !has_copy_requests_;
    // Damage rect in the quad space of the current shared quad state.
    // TODO(jbauman): This rect may contain unnecessary area if
    // transform isn't axis-aligned.
    gfx::Rect damage_rect_in_quad_space;

#if DCHECK_IS_ON()
    // If quads have come in with SharedQuadState out of order, or when quads have
    // invalid SharedQuadState pointer, it should DCHECK.
    SharedQuadStateList::ConstIterator sqs_iter = source_shared_quad_state_list.begin();
    for (const auto& quad : source_quad_list) {
        while (sqs_iter != source_shared_quad_state_list.end() && quad->shared_quad_state != *sqs_iter) {
            ++sqs_iter;
        }
        DCHECK(sqs_iter != source_shared_quad_state_list.end());
    }
#endif

    for (const auto& quad : source_quad_list) {
        if (quad->material == DrawQuad::SURFACE_CONTENT) {
            const SurfaceDrawQuad* surface_quad = SurfaceDrawQuad::MaterialCast(quad);
            // HandleSurfaceQuad may add other shared quad state, so reset the
            // current data.
            last_copied_source_shared_quad_state = nullptr;

            if (ignore_undamaged) {
                gfx::Transform quad_to_target_transform(
                    target_transform,
                    quad->shared_quad_state->quad_to_target_transform);
                damage_rect_in_quad_space = CalculateQuadSpaceDamageRect(
                    quad_to_target_transform, dest_pass->transform_to_root_target,
                    root_damage_rect_);
                if (!damage_rect_in_quad_space.Intersects(quad->visible_rect))
                    continue;
            }
            HandleSurfaceQuad(surface_quad, target_transform, clip_rect, dest_pass);
        } else {
            if (quad->shared_quad_state != last_copied_source_shared_quad_state) {
                dest_shared_quad_state = CopySharedQuadState(
                    quad->shared_quad_state, target_transform, clip_rect, dest_pass);
                last_copied_source_shared_quad_state = quad->shared_quad_state;
                if (aggregate_only_damaged_ && !has_copy_requests_) {
                    damage_rect_in_quad_space = CalculateQuadSpaceDamageRect(
                        dest_shared_quad_state->quad_to_target_transform,
                        dest_pass->transform_to_root_target, root_damage_rect_);
                }
            }

            if (ignore_undamaged) {
                if (!damage_rect_in_quad_space.Intersects(quad->visible_rect))
                    continue;
            }

            DrawQuad* dest_quad;
            if (quad->material == DrawQuad::RENDER_PASS) {
                const RenderPassDrawQuad* pass_quad = RenderPassDrawQuad::MaterialCast(quad);
                RenderPassId original_pass_id = pass_quad->render_pass_id;
                RenderPassId remapped_pass_id = RemapPassId(original_pass_id, surface_id);

                dest_quad = dest_pass->CopyFromAndAppendRenderPassDrawQuad(
                    pass_quad, dest_shared_quad_state, remapped_pass_id);
            } else {
                dest_quad = dest_pass->CopyFromAndAppendDrawQuad(quad, dest_shared_quad_state);
            }
            if (!child_to_parent_map.empty()) {
                for (ResourceId& resource_id : dest_quad->resources) {
                    ResourceProvider::ResourceIdMap::const_iterator it = child_to_parent_map.find(resource_id);
                    DCHECK(it != child_to_parent_map.end());

                    DCHECK_EQ(it->first, resource_id);
                    ResourceId remapped_id = it->second;
                    resource_id = remapped_id;
                }
            }
        }
    }
}

void SurfaceAggregator::CopyPasses(const DelegatedFrameData* frame_data,
    Surface* surface)
{
    // The root surface is allowed to have copy output requests, so grab them
    // off its render passes.
    std::multimap<RenderPassId, CopyOutputRequest*> copy_requests;
    surface->TakeCopyOutputRequests(&copy_requests);

    const RenderPassList& source_pass_list = frame_data->render_pass_list;
    DCHECK(valid_surfaces_.count(surface->surface_id()));
    if (!valid_surfaces_.count(surface->surface_id()))
        return;

    // TODO(vmpstr): provider check is a hack for unittests that don't set up a
    // resource provider.
    ResourceProvider::ResourceIdMap empty_map;
    const ResourceProvider::ResourceIdMap& child_to_parent_map = provider_ ? provider_->GetChildToParentMap(ChildIdForSurface(surface))
                                                                           : empty_map;
    for (size_t i = 0; i < source_pass_list.size(); ++i) {
        const RenderPass& source = *source_pass_list[i];

        size_t sqs_size = source.shared_quad_state_list.size();
        size_t dq_size = source.quad_list.size();
        scoped_ptr<RenderPass> copy_pass(RenderPass::Create(sqs_size, dq_size));

        MoveMatchingRequests(source.id, &copy_requests, &copy_pass->copy_requests);

        RenderPassId remapped_pass_id = RemapPassId(source.id, surface->surface_id());

        copy_pass->SetAll(remapped_pass_id, source.output_rect, gfx::Rect(),
            source.transform_to_root_target,
            source.has_transparent_background);

        CopyQuadsToPass(source.quad_list, source.shared_quad_state_list,
            child_to_parent_map, gfx::Transform(), ClipData(),
            copy_pass.get(), surface->surface_id());

        dest_pass_list_->push_back(copy_pass.Pass());
    }
}

void SurfaceAggregator::RemoveUnreferencedChildren()
{
    for (const auto& surface : previous_contained_surfaces_) {
        if (!contained_surfaces_.count(surface.first)) {
            SurfaceToResourceChildIdMap::iterator it = surface_id_to_resource_child_id_.find(surface.first);
            if (it != surface_id_to_resource_child_id_.end()) {
                provider_->DestroyChild(it->second);
                surface_id_to_resource_child_id_.erase(it);
            }

            Surface* surface_ptr = manager_->GetSurfaceForId(surface.first);
            if (surface_ptr)
                surface_ptr->RunDrawCallbacks(SurfaceDrawStatus::DRAW_SKIPPED);
        }
    }
}

// Walk the Surface tree from surface_id. Validate the resources of the current
// surface and its descendants, check if there are any copy requests, and
// return the combined damage rect.
gfx::Rect SurfaceAggregator::PrewalkTree(SurfaceId surface_id)
{
    if (referenced_surfaces_.count(surface_id))
        return gfx::Rect();
    Surface* surface = manager_->GetSurfaceForId(surface_id);
    if (!surface) {
        contained_surfaces_[surface_id] = 0;
        return gfx::Rect();
    }
    contained_surfaces_[surface_id] = surface->frame_index();
    const CompositorFrame* surface_frame = surface->GetEligibleFrame();
    if (!surface_frame)
        return gfx::Rect();
    const DelegatedFrameData* frame_data = surface_frame->delegated_frame_data.get();
    if (!frame_data)
        return gfx::Rect();
    int child_id = 0;
    // TODO(jbauman): hack for unit tests that don't set up rp
    if (provider_) {
        child_id = ChildIdForSurface(surface);
        if (surface->factory())
            surface->factory()->RefResources(frame_data->resource_list);
        provider_->ReceiveFromChild(child_id, frame_data->resource_list);
    }

    ResourceProvider::ResourceIdSet referenced_resources;
    size_t reserve_size = frame_data->resource_list.size();

#if USING_VC6RT = 1

#else

#if defined(COMPILER_MSVC)
    referenced_resources.reserve(reserve_size);
#elif defined(COMPILER_GCC)
    // Pre-standard hash-tables only implement resize, which behaves similarly
    // to reserve for these keys. Resizing to 0 may also be broken (particularly
    // on stlport).
    // TODO(jbauman): Replace with reserve when C++11 is supported everywhere.
    if (reserve_size)
        referenced_resources.resize(reserve_size);
#endif

#endif // USING_VC6RT

    bool invalid_frame = false;
    ResourceProvider::ResourceIdMap empty_map;
    const ResourceProvider::ResourceIdMap& child_to_parent_map = provider_ ? provider_->GetChildToParentMap(child_id) : empty_map;

    // Each pair in the vector is a child surface and the transform from its
    // target to the root target of this surface.
    std::vector<std::pair<SurfaceId, gfx::Transform>> child_surfaces;
    for (const auto& render_pass : frame_data->render_pass_list) {
        for (const auto& quad : render_pass->quad_list) {
            if (quad->material == DrawQuad::SURFACE_CONTENT) {
                const SurfaceDrawQuad* surface_quad = SurfaceDrawQuad::MaterialCast(quad);
                gfx::Transform target_to_surface_transform(
                    render_pass->transform_to_root_target,
                    surface_quad->shared_quad_state->quad_to_target_transform);
                child_surfaces.push_back(std::make_pair(surface_quad->surface_id,
                    target_to_surface_transform));
            }

            if (!provider_)
                continue;
            for (ResourceId resource_id : quad->resources) {
                if (!child_to_parent_map.count(resource_id)) {
                    invalid_frame = true;
                    break;
                }
                referenced_resources.insert(resource_id);
            }
        }
    }

    if (invalid_frame)
        return gfx::Rect();
    valid_surfaces_.insert(surface->surface_id());

    if (provider_)
        provider_->DeclareUsedResourcesFromChild(child_id, referenced_resources);

    for (const auto& render_pass : frame_data->render_pass_list)
        has_copy_requests_ |= !render_pass->copy_requests.empty();

    gfx::Rect damage_rect;
    if (!frame_data->render_pass_list.empty()) {
        RenderPass* last_pass = frame_data->render_pass_list.back();
        damage_rect = DamageRectForSurface(surface, *last_pass, last_pass->output_rect);
    }

    // Avoid infinite recursion by adding current surface to
    // referenced_surfaces_.
    SurfaceSet::iterator it = referenced_surfaces_.insert(surface->surface_id()).first;
    for (const auto& surface_info : child_surfaces) {
        gfx::Rect surface_damage = PrewalkTree(surface_info.first);
        damage_rect.Union(
            MathUtil::MapEnclosingClippedRect(surface_info.second, surface_damage));
    }
    referenced_surfaces_.erase(it);
    return damage_rect;
}

scoped_ptr<CompositorFrame> SurfaceAggregator::Aggregate(SurfaceId surface_id)
{
    Surface* surface = manager_->GetSurfaceForId(surface_id);
    DCHECK(surface);
    contained_surfaces_[surface_id] = surface->frame_index();
    const CompositorFrame* root_surface_frame = surface->GetEligibleFrame();
    if (!root_surface_frame)
        return nullptr;
    TRACE_EVENT0("cc", "SurfaceAggregator::Aggregate");

    scoped_ptr<CompositorFrame> frame(new CompositorFrame);
    frame->delegated_frame_data = make_scoped_ptr(new DelegatedFrameData);

    DCHECK(root_surface_frame->delegated_frame_data);

    dest_resource_list_ = &frame->delegated_frame_data->resource_list;
    dest_pass_list_ = &frame->delegated_frame_data->render_pass_list;

    valid_surfaces_.clear();
    has_copy_requests_ = false;
    root_damage_rect_ = PrewalkTree(surface_id);

    SurfaceSet::iterator it = referenced_surfaces_.insert(surface_id).first;
    CopyPasses(root_surface_frame->delegated_frame_data.get(), surface);
    referenced_surfaces_.erase(it);

    DCHECK(referenced_surfaces_.empty());

    if (dest_pass_list_->empty())
        return nullptr;
    dest_pass_list_->back()->damage_rect = root_damage_rect_;

    dest_pass_list_ = NULL;
    RemoveUnreferencedChildren();
    contained_surfaces_.swap(previous_contained_surfaces_);
    contained_surfaces_.clear();

    for (SurfaceIndexMap::iterator it = previous_contained_surfaces_.begin();
         it != previous_contained_surfaces_.end();
         ++it) {
        Surface* surface = manager_->GetSurfaceForId(it->first);
        if (surface)
            surface->TakeLatencyInfo(&frame->metadata.latency_info);
    }

    // TODO(jamesr): Aggregate all resource references into the returned frame's
    // resource list.

    return frame.Pass();
}

void SurfaceAggregator::ReleaseResources(SurfaceId surface_id)
{
    SurfaceToResourceChildIdMap::iterator it = surface_id_to_resource_child_id_.find(surface_id);
    if (it != surface_id_to_resource_child_id_.end()) {
        provider_->DestroyChild(it->second);
        surface_id_to_resource_child_id_.erase(it);
    }
}

void SurfaceAggregator::SetFullDamageForSurface(SurfaceId surface_id)
{
    auto it = previous_contained_surfaces_.find(surface_id);
    if (it == previous_contained_surfaces_.end())
        return;
    // Set the last drawn index as 0 to ensure full damage next time it's drawn.
    it->second = 0;
}

} // namespace cc
