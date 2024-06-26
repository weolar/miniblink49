// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/tree_synchronizer.h"

#include <set>

#include "base/containers/hash_tables.h"
#include "base/containers/scoped_ptr_hash_map.h"
#include "base/logging.h"
#include "base/trace_event/trace_event.h"
#include "cc/animation/scrollbar_animation_controller.h"
#include "cc/input/scrollbar.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/scrollbar_layer_impl_base.h"
#include "cc/layers/scrollbar_layer_interface.h"

namespace cc {

typedef base::ScopedPtrHashMap<int, scoped_ptr<LayerImpl>>
    ScopedPtrLayerImplMap;
typedef base::hash_map<int, LayerImpl*> RawPtrLayerImplMap;

void CollectExistingLayerImplRecursive(ScopedPtrLayerImplMap* old_layers,
    scoped_ptr<LayerImpl> layer_impl)
{
    if (!layer_impl)
        return;

    layer_impl->ClearScrollbars();
    if (ScrollbarLayerImplBase* scrollbar_layer = layer_impl->ToScrollbarLayer()) {
        scrollbar_layer->ClearClipLayer();
        scrollbar_layer->ClearScrollLayer();
    }

    OwnedLayerImplList& children = layer_impl->children();
    for (OwnedLayerImplList::iterator it = children.begin();
         it != children.end();
         ++it)
        CollectExistingLayerImplRecursive(old_layers, children.take(it));

    CollectExistingLayerImplRecursive(old_layers, layer_impl->TakeMaskLayer());
    CollectExistingLayerImplRecursive(old_layers, layer_impl->TakeReplicaLayer());

    int id = layer_impl->id();
    old_layers->set(id, layer_impl.Pass());
}

template <typename LayerType>
scoped_ptr<LayerImpl> SynchronizeTreesInternal(
    LayerType* layer_root,
    scoped_ptr<LayerImpl> old_layer_impl_root,
    LayerTreeImpl* tree_impl)
{
    DCHECK(tree_impl);

    TRACE_EVENT0("cc", "TreeSynchronizer::SynchronizeTrees");
    ScopedPtrLayerImplMap old_layers;
    RawPtrLayerImplMap new_layers;

    CollectExistingLayerImplRecursive(&old_layers, old_layer_impl_root.Pass());

    scoped_ptr<LayerImpl> new_tree = SynchronizeTreesRecursive(
        &new_layers, &old_layers, layer_root, tree_impl);

    UpdateScrollbarLayerPointersRecursive(&new_layers, layer_root);

    return new_tree.Pass();
}

scoped_ptr<LayerImpl> TreeSynchronizer::SynchronizeTrees(
    Layer* layer_root,
    scoped_ptr<LayerImpl> old_layer_impl_root,
    LayerTreeImpl* tree_impl)
{
    return SynchronizeTreesInternal(
        layer_root, old_layer_impl_root.Pass(), tree_impl);
}

scoped_ptr<LayerImpl> TreeSynchronizer::SynchronizeTrees(
    LayerImpl* layer_root,
    scoped_ptr<LayerImpl> old_layer_impl_root,
    LayerTreeImpl* tree_impl)
{
    return SynchronizeTreesInternal(
        layer_root, old_layer_impl_root.Pass(), tree_impl);
}

template <typename LayerType>
scoped_ptr<LayerImpl> ReuseOrCreateLayerImpl(RawPtrLayerImplMap* new_layers,
    ScopedPtrLayerImplMap* old_layers,
    LayerType* layer,
    LayerTreeImpl* tree_impl)
{
    scoped_ptr<LayerImpl> layer_impl = old_layers->take(layer->id());

    if (!layer_impl)
        layer_impl = layer->CreateLayerImpl(tree_impl);

    (*new_layers)[layer->id()] = layer_impl.get();
    return layer_impl.Pass();
}

template <typename LayerType>
scoped_ptr<LayerImpl> SynchronizeTreesRecursiveInternal(
    RawPtrLayerImplMap* new_layers,
    ScopedPtrLayerImplMap* old_layers,
    LayerType* layer,
    LayerTreeImpl* tree_impl)
{
    if (!layer)
        return nullptr;

    scoped_ptr<LayerImpl> layer_impl = ReuseOrCreateLayerImpl(new_layers, old_layers, layer, tree_impl);

    layer_impl->ClearChildList();
    for (size_t i = 0; i < layer->children().size(); ++i) {
        layer_impl->AddChild(SynchronizeTreesRecursiveInternal(
            new_layers, old_layers, layer->child_at(i), tree_impl));
    }

    layer_impl->SetMaskLayer(SynchronizeTreesRecursiveInternal(
        new_layers, old_layers, layer->mask_layer(), tree_impl));
    layer_impl->SetReplicaLayer(SynchronizeTreesRecursiveInternal(
        new_layers, old_layers, layer->replica_layer(), tree_impl));

    return layer_impl.Pass();
}

scoped_ptr<LayerImpl> SynchronizeTreesRecursive(
    RawPtrLayerImplMap* new_layers,
    ScopedPtrLayerImplMap* old_layers,
    Layer* layer,
    LayerTreeImpl* tree_impl)
{
    return SynchronizeTreesRecursiveInternal(
        new_layers, old_layers, layer, tree_impl);
}

scoped_ptr<LayerImpl> SynchronizeTreesRecursive(
    RawPtrLayerImplMap* new_layers,
    ScopedPtrLayerImplMap* old_layers,
    LayerImpl* layer,
    LayerTreeImpl* tree_impl)
{
    return SynchronizeTreesRecursiveInternal(
        new_layers, old_layers, layer, tree_impl);
}

template <typename LayerType, typename ScrollbarLayerType>
void UpdateScrollbarLayerPointersRecursiveInternal(
    const RawPtrLayerImplMap* new_layers,
    LayerType* layer)
{
    if (!layer)
        return;

    for (size_t i = 0; i < layer->children().size(); ++i) {
        UpdateScrollbarLayerPointersRecursiveInternal<
            LayerType, ScrollbarLayerType>(new_layers, layer->child_at(i));
    }

    ScrollbarLayerType* scrollbar_layer = layer->ToScrollbarLayer();
    if (!scrollbar_layer)
        return;

    RawPtrLayerImplMap::const_iterator iter = new_layers->find(layer->id());
    ScrollbarLayerImplBase* scrollbar_layer_impl = iter != new_layers->end()
        ? static_cast<ScrollbarLayerImplBase*>(iter->second)
        : NULL;
    DCHECK(scrollbar_layer_impl);

    scrollbar_layer->PushScrollClipPropertiesTo(scrollbar_layer_impl);
}

void UpdateScrollbarLayerPointersRecursive(const RawPtrLayerImplMap* new_layers,
    Layer* layer)
{
    UpdateScrollbarLayerPointersRecursiveInternal<Layer, ScrollbarLayerInterface>(
        new_layers, layer);
}

void UpdateScrollbarLayerPointersRecursive(const RawPtrLayerImplMap* new_layers,
    LayerImpl* layer)
{
    UpdateScrollbarLayerPointersRecursiveInternal<
        LayerImpl,
        ScrollbarLayerImplBase>(new_layers, layer);
}

// static
template <typename LayerType>
void TreeSynchronizer::PushPropertiesInternal(
    LayerType* layer,
    LayerImpl* layer_impl,
    int* num_dependents_need_push_properties_for_parent)
{
    if (!layer) {
        DCHECK(!layer_impl);
        return;
    }

    DCHECK_EQ(layer->id(), layer_impl->id());

    bool push_layer = layer->needs_push_properties();
    bool recurse_on_children_and_dependents = layer->descendant_needs_push_properties();

    if (push_layer)
        layer->PushPropertiesTo(layer_impl);
    else if (layer->ToScrollbarLayer())
        layer->ToScrollbarLayer()->PushScrollClipPropertiesTo(layer_impl);

    int num_dependents_need_push_properties = 0;
    if (recurse_on_children_and_dependents) {
        PushPropertiesInternal(layer->mask_layer(),
            layer_impl->mask_layer(),
            &num_dependents_need_push_properties);
        PushPropertiesInternal(layer->replica_layer(),
            layer_impl->replica_layer(),
            &num_dependents_need_push_properties);

        const OwnedLayerImplList& impl_children = layer_impl->children();
        DCHECK_EQ(layer->children().size(), impl_children.size());

        for (size_t i = 0; i < layer->children().size(); ++i) {
            PushPropertiesInternal(layer->child_at(i),
                impl_children[i],
                &num_dependents_need_push_properties);
        }

        // When PushPropertiesTo completes for a layer, it may still keep
        // its needs_push_properties() state if the layer must push itself
        // every PushProperties tree walk. Here we keep track of those layers, and
        // ensure that their ancestors know about them for the next PushProperties
        // tree walk.
        layer->num_dependents_need_push_properties_ = num_dependents_need_push_properties;
    }

    bool add_self_to_parent = num_dependents_need_push_properties > 0 || layer->needs_push_properties();
    *num_dependents_need_push_properties_for_parent += add_self_to_parent ? 1 : 0;
}

static void CheckScrollAndClipPointersRecursive(Layer* layer,
    LayerImpl* layer_impl)
{
    DCHECK_EQ(!!layer, !!layer_impl);
    if (!layer)
        return;

    // Having a scroll parent on the impl thread implies having one the main
    // thread, too. The main thread may have a scroll parent that is not in the
    // tree because it's been removed but not deleted. In this case, the layer
    // impl will have no scroll parent. Same argument applies for clip parents and
    // scroll/clip children.
    DCHECK(!layer_impl->scroll_parent() || !!layer->scroll_parent());
    DCHECK(!layer_impl->clip_parent() || !!layer->clip_parent());
    DCHECK(!layer_impl->scroll_children() || !!layer->scroll_children());
    DCHECK(!layer_impl->clip_children() || !!layer->clip_children());

    if (layer_impl->scroll_parent())
        DCHECK_EQ(layer->scroll_parent()->id(), layer_impl->scroll_parent()->id());

    if (layer_impl->clip_parent())
        DCHECK_EQ(layer->clip_parent()->id(), layer_impl->clip_parent()->id());

    if (layer_impl->scroll_children()) {
        for (std::set<Layer*>::iterator it = layer->scroll_children()->begin();
             it != layer->scroll_children()->end();
             ++it) {
            DCHECK_EQ((*it)->scroll_parent(), layer);
        }
        for (std::set<LayerImpl*>::iterator it = layer_impl->scroll_children()->begin();
             it != layer_impl->scroll_children()->end();
             ++it) {
            DCHECK_EQ((*it)->scroll_parent(), layer_impl);
        }
    }

    if (layer_impl->clip_children()) {
        for (std::set<Layer*>::iterator it = layer->clip_children()->begin();
             it != layer->clip_children()->end();
             ++it) {
            DCHECK_EQ((*it)->clip_parent(), layer);
        }
        for (std::set<LayerImpl*>::iterator it = layer_impl->clip_children()->begin();
             it != layer_impl->clip_children()->end();
             ++it) {
            DCHECK_EQ((*it)->clip_parent(), layer_impl);
        }
    }

    for (size_t i = 0u; i < layer->children().size(); ++i) {
        CheckScrollAndClipPointersRecursive(layer->child_at(i),
            layer_impl->child_at(i));
    }
}

void TreeSynchronizer::PushProperties(Layer* layer,
    LayerImpl* layer_impl)
{
    int num_dependents_need_push_properties = 0;
    PushPropertiesInternal(
        layer, layer_impl, &num_dependents_need_push_properties);
#if DCHECK_IS_ON()
    CheckScrollAndClipPointersRecursive(layer, layer_impl);
#endif
}

void TreeSynchronizer::PushProperties(LayerImpl* layer, LayerImpl* layer_impl)
{
    int num_dependents_need_push_properties = 0;
    PushPropertiesInternal(
        layer, layer_impl, &num_dependents_need_push_properties);
}

} // namespace cc
