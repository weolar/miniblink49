// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/layer_lists.h"

#include "cc/layers/layer.h"

namespace cc {

RenderSurfaceLayerList::RenderSurfaceLayerList() { }

RenderSurfaceLayerList::~RenderSurfaceLayerList()
{
    for (size_t i = 0; i < size(); ++i)
        at(size() - 1 - i)->ClearRenderSurface();
}

Layer* RenderSurfaceLayerList::at(size_t i) const
{
    return list_.at(i).get();
}

void RenderSurfaceLayerList::pop_back()
{
    list_.pop_back();
}

void RenderSurfaceLayerList::push_back(const scoped_refptr<Layer>& layer)
{
    list_.push_back(layer);
}

Layer* RenderSurfaceLayerList::back()
{
    return list_.back().get();
}

size_t RenderSurfaceLayerList::size() const
{
    return list_.size();
}

scoped_refptr<Layer>& RenderSurfaceLayerList::operator[](size_t i)
{
    return list_[i];
}
const scoped_refptr<Layer>& RenderSurfaceLayerList::operator[](size_t i) const
{
    return list_[i];
}

LayerList::iterator RenderSurfaceLayerList::begin()
{
    return list_.begin();
}

LayerList::iterator RenderSurfaceLayerList::end()
{
    return list_.end();
}

LayerList::const_iterator RenderSurfaceLayerList::begin() const
{
    return list_.begin();
}

LayerList::const_iterator RenderSurfaceLayerList::end() const
{
    return list_.end();
}

void RenderSurfaceLayerList::clear()
{
    for (size_t i = 0; i < list_.size(); ++i)
        DCHECK(!list_[i]->render_surface());
    list_.clear();
}

} // namespace cc
