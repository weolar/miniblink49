// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_LAYER_ITERATOR_H_
#define CC_LAYERS_LAYER_ITERATOR_H_

#include "cc/base/cc_export.h"
#include "cc/layers/layer_impl.h"
#include "cc/trees/layer_tree_host_common.h"

namespace cc {

// These classes provide means to iterate over the
// RenderSurfaceImpl-LayerImpl tree.

// Example code follows, for a tree of LayerImpl/RenderSurfaceImpl objects.
// See below for details.
//
// void DoStuffOnLayers(
//     const LayerImplList& render_surface_layer_list) {
//
//   LayerIterator end =
//       LayerIterator::End(&render_surface_layer_list);
//   for (LayerIterator
//            it = LayerIterator::Begin(&render_surface_layer_list);
//        it != end;
//        ++it) {
//     // Only one of these will be true
//     if (it.represents_target_render_surface())
//       foo(*it);  // *it is a layer representing a target RenderSurface
//     if (it.represents_contributing_render_surface())
//       bar(*it);  // *it is a layer representing a RenderSurface that
//                  // contributes to the layer's target RenderSurface
//     if (it.represents_itself())
//       baz(*it);  // *it is a layer representing itself,
//                  // as it contributes to its own target RenderSurface
//   }
// }

// A RenderSurface R may be referred to in one of two different contexts.
// One RenderSurface is "current" at any time, for whatever operation
// is being performed. This current surface is referred to as a target surface.
// For example, when R is being painted it would be the target surface.
// Once R has been painted, its contents may be included into another
// surface S. While S is considered the target surface when it is being
// painted, R is called a contributing surface in this context as it
// contributes to the content of the target surface S.
//
// The iterator's current position in the tree always points to some layer.
// The state of the iterator indicates the role of the layer,
// and will be one of the following three states.
// A single layer L will appear in the iteration process in at least one,
// and possibly all, of these states.
// 1. Representing the target surface: The iterator in this state,
// pointing at layer L, indicates that the target RenderSurface
// is now the surface owned by L. This will occur exactly once for each
// RenderSurface in the tree.
// 2. Representing a contributing surface: The iterator in this state,
// pointing at layer L, refers to the RenderSurface owned
// by L as a contributing surface, without changing the current
// target RenderSurface.
// 3. Representing itself: The iterator in this state, pointing at layer L,
// refers to the layer itself, as a child of the
// current target RenderSurface.
//
// The FrontToBack iterator will iterate over children layers of a surface
// before the layer representing the surface as a target surface.
//
// To use the iterators:
//
// Create a stepping iterator and end iterator by calling
// LayerIterator::Begin() and LayerIterator::End() and passing in the
// list of layers owning target RenderSurfaces. Step through the tree
// by incrementing the stepping iterator while it is != to
// the end iterator. At each step the iterator knows what the layer
// is representing, and you can query the iterator to decide
// what actions to perform with the layer given what it represents.

////////////////////////////////////////////////////////////////////////////////

struct LayerIteratorValue {
    static const int kInvalidTargetRenderSurfaceLayerIndex = -1;
    // This must be (size_t)-1 since the iterator action code assumes that this
    // value can be reached by subtracting one from the position of the first
    // layer in the current target surface's child layer list, which is 0.
    static const size_t kLayerIndexRepresentingTargetRenderSurface = static_cast<size_t>(-1);
};

// The position of a layer iterator that is independent
// of its many template types.
struct LayerIteratorPosition {
    bool represents_target_render_surface;
    bool represents_contributing_render_surface;
    bool represents_itself;
    LayerImpl* target_render_surface_layer;
    LayerImpl* current_layer;
};

// An iterator class for walking over layers in the
// RenderSurface-Layer tree.
// TODO(enne): This class probably shouldn't be entirely inline and
// should get moved to a .cc file where it makes sense.
class LayerIterator {
public:
    LayerIterator()
        : render_surface_layer_list_(nullptr)
    {
    }

    static LayerIterator Begin(const LayerImplList* render_surface_layer_list)
    {
        return LayerIterator(render_surface_layer_list, true);
    }
    static LayerIterator End(const LayerImplList* render_surface_layer_list)
    {
        return LayerIterator(render_surface_layer_list, false);
    }

    LayerIterator& operator++()
    {
        MoveToNext();
        return *this;
    }
    bool operator==(const LayerIterator& other) const
    {
        return target_render_surface_layer_index_ == other.target_render_surface_layer_index_ && current_layer_index_ == other.current_layer_index_;
    }
    bool operator!=(const LayerIterator& other) const
    {
        return !(*this == other);
    }

    LayerImpl* operator->() const { return current_layer(); }
    LayerImpl* operator*() const { return current_layer(); }

    bool represents_target_render_surface() const
    {
        return current_layer_represents_target_render_surface();
    }
    bool represents_contributing_render_surface() const
    {
        return !represents_target_render_surface() && current_layer_represents_contributing_render_surface();
    }
    bool represents_itself() const
    {
        return !represents_target_render_surface() && !represents_contributing_render_surface();
    }

    LayerImpl* target_render_surface_layer() const
    {
        return render_surface_layer_list_->at(target_render_surface_layer_index_);
    }

    operator const LayerIteratorPosition() const
    {
        LayerIteratorPosition position;
        position.represents_target_render_surface = represents_target_render_surface();
        position.represents_contributing_render_surface = represents_contributing_render_surface();
        position.represents_itself = represents_itself();
        position.target_render_surface_layer = target_render_surface_layer();
        position.current_layer = current_layer();
        return position;
    }

private:
    LayerIterator(const LayerImplList* render_surface_layer_list, bool start)
        : render_surface_layer_list_(render_surface_layer_list)
        , target_render_surface_layer_index_(0)
    {
        for (size_t i = 0; i < render_surface_layer_list->size(); ++i) {
            if (!render_surface_layer_list->at(i)->render_surface()) {
                NOTREACHED();
                MoveToEnd();
                return;
            }
        }

        if (start && !render_surface_layer_list->empty())
            MoveToBegin();
        else
            MoveToEnd();
    }

    void MoveToBegin()
    {
        target_render_surface_layer_index_ = 0;
        current_layer_index_ = target_render_surface_children().size() - 1;
        MoveToHighestInSubtree();
    }

    void MoveToEnd()
    {
        target_render_surface_layer_index_ = LayerIteratorValue::kInvalidTargetRenderSurfaceLayerIndex;
        current_layer_index_ = 0;
    }

    void MoveToNext()
    {
        // Moves to the previous layer in the current RS layer list.
        // Then we check if the new current layer has its own RS,
        // in which case there are things in that RS layer list that are higher,
        // so we find the highest layer in that subtree.
        // If we move back past the front of the list,
        // we jump up to the previous RS layer list, picking up again where we
        // had previously recursed into the current RS layer list.

        if (!current_layer_represents_target_render_surface()) {
            // Subtracting one here will eventually cause the current layer
            // to become that layer representing the target render surface.
            --current_layer_index_;
            MoveToHighestInSubtree();
        } else {
            while (current_layer_represents_target_render_surface()) {
                if (!target_render_surface_layer_index_) {
                    // End of the list.
                    target_render_surface_layer_index_ = LayerIteratorValue::kInvalidTargetRenderSurfaceLayerIndex;
                    current_layer_index_ = 0;
                    return;
                }
                target_render_surface_layer_index_ = target_render_surface()->target_render_surface_layer_index_history_;
                current_layer_index_ = target_render_surface()->current_layer_index_history_;
            }
        }
    }

    void MoveToHighestInSubtree()
    {
        if (current_layer_represents_target_render_surface())
            return;
        while (current_layer_represents_contributing_render_surface()) {
            // Save where we were in the current target surface, move to the next one,
            // and save the target surface that we came from there
            // so we can go back to it.
            target_render_surface()->current_layer_index_history_ = current_layer_index_;
            int previous_target_render_surface_layer = target_render_surface_layer_index_;

            for (LayerImpl* layer = current_layer();
                 target_render_surface_layer() != layer;
                 ++target_render_surface_layer_index_) {
            }
            current_layer_index_ = target_render_surface_children().size() - 1;

            target_render_surface()->target_render_surface_layer_index_history_ = previous_target_render_surface_layer;
        }
    }

    inline LayerImpl* current_layer() const
    {
        return current_layer_represents_target_render_surface()
            ? target_render_surface_layer()
            : LayerTreeHostCommon::get_layer_as_raw_ptr(
                target_render_surface_children(), current_layer_index_);
    }

    inline bool current_layer_represents_contributing_render_surface() const
    {
        return LayerTreeHostCommon::RenderSurfaceContributesToTarget<LayerImpl>(
            current_layer(), target_render_surface_layer()->id());
    }
    inline bool current_layer_represents_target_render_surface() const
    {
        return current_layer_index_ == LayerIteratorValue::kLayerIndexRepresentingTargetRenderSurface;
    }

    inline RenderSurfaceImpl* target_render_surface() const
    {
        return target_render_surface_layer()->render_surface();
    }
    inline const LayerImplList& target_render_surface_children() const
    {
        return target_render_surface()->layer_list();
    }

    const LayerImplList* render_surface_layer_list_;

    // The iterator's current position.

    // A position in the render_surface_layer_list. This points to a layer which
    // owns the current target surface. This is a value from 0 to n-1
    // (n = size of render_surface_layer_list = number of surfaces).
    // A value outside of this range
    // (for example, LayerIteratorValue::kInvalidTargetRenderSurfaceLayerIndex)
    // is used to indicate a position outside the bounds of the tree.
    int target_render_surface_layer_index_;
    // A position in the list of layers that are children of the
    // current target surface. When pointing to one of these layers,
    // this is a value from 0 to n-1 (n = number of children).
    // Since the iterator must also stop at the layers representing
    // the target surface, this is done by setting the current_layer_index
    // to a value of
    // LayerIteratorValue::kLayerIndexRepresentingTargetRenderSurface.
    size_t current_layer_index_;
};

} // namespace cc

#endif // CC_LAYERS_LAYER_ITERATOR_H_
