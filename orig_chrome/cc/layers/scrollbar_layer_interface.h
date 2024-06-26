// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_SCROLLBAR_LAYER_INTERFACE_H_
#define CC_LAYERS_SCROLLBAR_LAYER_INTERFACE_H_

#include "cc/base/cc_export.h"
#include "cc/input/scrollbar.h"

namespace cc {

class Layer;
class LayerImpl;

class CC_EXPORT ScrollbarLayerInterface {
public:
    virtual int ScrollLayerId() const = 0;
    virtual void SetScrollLayer(int layer_id) = 0;
    virtual void SetClipLayer(int layer_id) = 0;
    virtual void PushScrollClipPropertiesTo(LayerImpl* layer) = 0;

    virtual ScrollbarOrientation orientation() const = 0;

protected:
    ScrollbarLayerInterface() { }
    virtual ~ScrollbarLayerInterface() { }

private:
    DISALLOW_COPY_AND_ASSIGN(ScrollbarLayerInterface);
};

} // namespace cc

#endif // CC_LAYERS_SCROLLBAR_LAYER_INTERFACE_H_
