// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_QUADS_RENDER_PASS_H_
#define CC_QUADS_RENDER_PASS_H_

#include <utility>

#include "base/basictypes.h"
#include "base/callback.h"
#include "base/containers/hash_tables.h"
#include "cc/base/cc_export.h"
#include "cc/base/list_container.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/quads/render_pass_id.h"
#include "cc/surfaces/surface_id.h"
#include "skia/ext/refptr.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/transform.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
class Value;
}

namespace cc {

class DrawQuad;
class CopyOutputRequest;
class RenderPassDrawQuad;
class SharedQuadState;

// A list of DrawQuad objects, sorted internally in front-to-back order.
class QuadList : public ListContainer<DrawQuad> {
public:
    explicit QuadList(size_t default_size_to_reserve);

    typedef QuadList::ReverseIterator BackToFrontIterator;
    typedef QuadList::ConstReverseIterator ConstBackToFrontIterator;

    inline BackToFrontIterator BackToFrontBegin() { return rbegin(); }
    inline BackToFrontIterator BackToFrontEnd() { return rend(); }
    inline ConstBackToFrontIterator BackToFrontBegin() const { return rbegin(); }
    inline ConstBackToFrontIterator BackToFrontEnd() const { return rend(); }
};

typedef ListContainer<SharedQuadState> SharedQuadStateList;

class CC_EXPORT RenderPass {
public:
    ~RenderPass();

    static scoped_ptr<RenderPass> Create();
    static scoped_ptr<RenderPass> Create(size_t num_layers);
    static scoped_ptr<RenderPass> Create(size_t shared_quad_state_list_size,
        size_t quad_list_size);

    // A shallow copy of the render pass, which does not include its quads or copy
    // requests.
    scoped_ptr<RenderPass> Copy(RenderPassId new_id) const;

    // A deep copy of the render passes in the list including the quads.
    static void CopyAll(const ScopedPtrVector<RenderPass>& in,
        ScopedPtrVector<RenderPass>* out);

    void SetNew(RenderPassId id,
        const gfx::Rect& output_rect,
        const gfx::Rect& damage_rect,
        const gfx::Transform& transform_to_root_target);

    void SetAll(RenderPassId id,
        const gfx::Rect& output_rect,
        const gfx::Rect& damage_rect,
        const gfx::Transform& transform_to_root_target,
        bool has_transparent_background);

    void AsValueInto(base::trace_event::TracedValue* dict) const;

    SharedQuadState* CreateAndAppendSharedQuadState();

    template <typename DrawQuadType>
    DrawQuadType* CreateAndAppendDrawQuad()
    {
        return quad_list.AllocateAndConstruct<DrawQuadType>();
    }

    RenderPassDrawQuad* CopyFromAndAppendRenderPassDrawQuad(
        const RenderPassDrawQuad* quad,
        const SharedQuadState* shared_quad_state,
        RenderPassId render_pass_id);
    DrawQuad* CopyFromAndAppendDrawQuad(const DrawQuad* quad,
        const SharedQuadState* shared_quad_state);

    // Uniquely identifies the render pass in the compositor's current frame.
    RenderPassId id;

    // These are in the space of the render pass' physical pixels.
    gfx::Rect output_rect;
    gfx::Rect damage_rect;

    // Transforms from the origin of the |output_rect| to the origin of the root
    // render pass' |output_rect|.
    gfx::Transform transform_to_root_target;

    // If false, the pixels in the render pass' texture are all opaque.
    bool has_transparent_background;

    // If non-empty, the renderer should produce a copy of the render pass'
    // contents as a bitmap, and give a copy of the bitmap to each callback in
    // this list. This property should not be serialized between compositors, as
    // it only makes sense in the root compositor.
    ScopedPtrVector<CopyOutputRequest> copy_requests;

    QuadList quad_list;
    SharedQuadStateList shared_quad_state_list;

    // This vector contains the complete set of SurfaceIds referenced by
    // DrawQuads in quad_list.
    std::vector<SurfaceId> referenced_surfaces;

protected:
    explicit RenderPass(size_t num_layers);
    RenderPass();
    RenderPass(size_t shared_quad_state_list_size, size_t quad_list_size);

private:
    template <typename DrawQuadType>
    DrawQuadType* CopyFromAndAppendTypedDrawQuad(const DrawQuad* quad)
    {
        return quad_list.AllocateAndCopyFrom(DrawQuadType::MaterialCast(quad));
    }

    DISALLOW_COPY_AND_ASSIGN(RenderPass);
};

} // namespace cc

namespace BASE_HASH_NAMESPACE {

#if USING_VC6RT != 1

template <>
struct hash<cc::RenderPassId> {
    size_t operator()(cc::RenderPassId key) const
    {
        return base::HashPair(key.layer_id, static_cast<int>(key.index));
    }
};

#endif

} // namespace BASE_HASH_NAMESPACE

namespace cc {
typedef ScopedPtrVector<RenderPass> RenderPassList;
typedef base::hash_map<RenderPassId, RenderPass*> RenderPassIdHashMap;
} // namespace cc

#endif // CC_QUADS_RENDER_PASS_H_
