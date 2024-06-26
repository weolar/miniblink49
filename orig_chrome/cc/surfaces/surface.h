// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_H_
#define CC_SURFACES_SURFACE_H_

#include <map>
#include <vector>

#include "base/callback.h"
#include "base/containers/hash_tables.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/output/copy_output_request.h"
#include "cc/quads/render_pass_id.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_id.h"
#include "cc/surfaces/surface_sequence.h"
#include "cc/surfaces/surfaces_export.h"
#include "ui/gfx/geometry/size.h"

namespace ui {
class LatencyInfo;
}

namespace cc {
class CompositorFrame;
class CopyOutputRequest;
class SurfaceManager;
class SurfaceFactory;
class SurfaceResourceHolder;

class CC_SURFACES_EXPORT Surface {
public:
    using DrawCallback = SurfaceFactory::DrawCallback;

    Surface(SurfaceId id, SurfaceFactory* factory);
    ~Surface();

    SurfaceId surface_id() const { return surface_id_; }

    void QueueFrame(scoped_ptr<CompositorFrame> frame,
        const DrawCallback& draw_callback);
    void RequestCopyOfOutput(scoped_ptr<CopyOutputRequest> copy_request);
    // Adds each CopyOutputRequest in the current frame to copy_requests. The
    // caller takes ownership of them.
    void TakeCopyOutputRequests(
        std::multimap<RenderPassId, CopyOutputRequest*>* copy_requests);
    // Returns the most recent frame that is eligible to be rendered.
    const CompositorFrame* GetEligibleFrame();

    // Returns a number that increments by 1 every time a new frame is enqueued.
    int frame_index() const { return frame_index_; }

    void TakeLatencyInfo(std::vector<ui::LatencyInfo>* latency_info);
    void RunDrawCallbacks(SurfaceDrawStatus drawn);

    base::WeakPtr<SurfaceFactory> factory() { return factory_; }

    // Add a SurfaceSequence that must be satisfied before the Surface is
    // destroyed.
    void AddDestructionDependency(SurfaceSequence sequence);

    // Satisfy all destruction dependencies that are contained in sequences, and
    // remove them from sequences.
    void SatisfyDestructionDependencies(
        base::hash_set<SurfaceSequence>* sequences,
        base::hash_set<uint32_t>* valid_id_namespaces);
    size_t GetDestructionDependencyCount() const
    {
        return destruction_dependencies_.size();
    }

    const std::vector<SurfaceId>& referenced_surfaces() const
    {
        return referenced_surfaces_;
    }

    bool destroyed() const { return destroyed_; }
    void set_destroyed(bool destroyed) { destroyed_ = destroyed; }

private:
    void ClearCopyRequests();

    SurfaceId surface_id_;
    base::WeakPtr<SurfaceFactory> factory_;
    // TODO(jamesr): Support multiple frames in flight.
    scoped_ptr<CompositorFrame> current_frame_;
    int frame_index_;
    bool destroyed_;
    std::vector<SurfaceSequence> destruction_dependencies_;

    std::vector<SurfaceId> referenced_surfaces_;

    DrawCallback draw_callback_;

    DISALLOW_COPY_AND_ASSIGN(Surface);
};

} // namespace cc

#endif // CC_SURFACES_SURFACE_H_
