// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_LAYER_LISTS_H_
#define CC_LAYERS_LAYER_LISTS_H_

#include <vector>

#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"

namespace cc {
class Layer;
class LayerImpl;

typedef std::vector<scoped_refptr<Layer>> LayerList;
typedef ScopedPtrVector<LayerImpl> OwnedLayerImplList;
typedef std::vector<LayerImpl*> LayerImplList;

} // namespace cc

#endif // CC_LAYERS_LAYER_LISTS_H_
