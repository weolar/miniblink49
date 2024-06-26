// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/scoped_resource.h"

namespace cc {

ScopedResource::ScopedResource(ResourceProvider* resource_provider)
    : resource_provider_(resource_provider)
{
    DCHECK(resource_provider_);
}

ScopedResource::~ScopedResource()
{
    Free();
}

void ScopedResource::Allocate(const gfx::Size& size,
    ResourceProvider::TextureHint hint,
    ResourceFormat format)
{
    DCHECK(!id());
    DCHECK(!size.IsEmpty());

    set_dimensions(size, format);
    set_id(resource_provider_->CreateResource(
        size, GL_CLAMP_TO_EDGE, hint, format));

#if DCHECK_IS_ON()
    allocate_thread_id_ = base::PlatformThread::CurrentId();
#endif
}

void ScopedResource::AllocateManaged(const gfx::Size& size,
    GLenum target,
    ResourceFormat format)
{
    DCHECK(!id());
    DCHECK(!size.IsEmpty());

    set_dimensions(size, format);
    set_id(resource_provider_->CreateManagedResource(
        size, target, GL_CLAMP_TO_EDGE, ResourceProvider::TEXTURE_HINT_IMMUTABLE,
        format));

#if DCHECK_IS_ON()
    allocate_thread_id_ = base::PlatformThread::CurrentId();
#endif
}

void ScopedResource::Free()
{
    if (id()) {
#if DCHECK_IS_ON()
        DCHECK(allocate_thread_id_ == base::PlatformThread::CurrentId());
#endif
        resource_provider_->DeleteResource(id());
    }
    set_id(0);
}

} // namespace cc
