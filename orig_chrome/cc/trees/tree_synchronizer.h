// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_TREE_SYNCHRONIZER_H_
#define CC_TREES_TREE_SYNCHRONIZER_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"

namespace cc {

class LayerImpl;
class LayerTreeImpl;
class Layer;

class CC_EXPORT TreeSynchronizer {
public:
    // Accepts a Layer tree and returns a reference to a LayerImpl tree that
    // duplicates the structure of the Layer tree, reusing the LayerImpls in the
    // tree provided by old_layer_impl_root if possible.
    static scoped_ptr<LayerImpl> SynchronizeTrees(
        Layer* layer_root,
        scoped_ptr<LayerImpl> old_layer_impl_root,
        LayerTreeImpl* tree_impl);
    static scoped_ptr<LayerImpl> SynchronizeTrees(
        LayerImpl* layer_root,
        scoped_ptr<LayerImpl> old_layer_impl_root,
        LayerTreeImpl* tree_impl);

    // Pushes properties from a Layer or LayerImpl tree to a structurally
    // equivalent LayerImpl tree.
    static void PushProperties(Layer* layer_root,
        LayerImpl* layer_impl_root);
    static void PushProperties(LayerImpl* layer_root, LayerImpl* layer_impl_root);

private:
    TreeSynchronizer(); // Not instantiable.

    template <typename LayerType>
    static void PushPropertiesInternal(
        LayerType* layer,
        LayerImpl* layer_impl,
        int* num_dependents_need_push_properties_for_parent);

    DISALLOW_COPY_AND_ASSIGN(TreeSynchronizer);
};

} // namespace cc

#endif // CC_TREES_TREE_SYNCHRONIZER_H_
