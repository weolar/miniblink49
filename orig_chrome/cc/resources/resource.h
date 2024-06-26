// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_RESOURCE_H_
#define CC_RESOURCES_RESOURCE_H_

#include "cc/base/cc_export.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/resource_util.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT Resource {
public:
    Resource()
        : id_(0)
        , format_(RGBA_8888)
    {
    }
    Resource(unsigned id, const gfx::Size& size, ResourceFormat format)
        : id_(id)
        , size_(size)
        , format_(format)
    {
    }

    ResourceId id() const { return id_; }
    gfx::Size size() const { return size_; }
    ResourceFormat format() const { return format_; }

protected:
    void set_id(ResourceId id) { id_ = id; }
    void set_dimensions(const gfx::Size& size, ResourceFormat format)
    {
        size_ = size;
        format_ = format;
    }

private:
    ResourceId id_;
    gfx::Size size_;
    ResourceFormat format_;

    DISALLOW_COPY_AND_ASSIGN(Resource);
};

} // namespace cc

#endif // CC_RESOURCES_RESOURCE_H_
