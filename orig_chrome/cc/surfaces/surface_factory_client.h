// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_FACTORY_CLIENT_H_
#define CC_SURFACES_SURFACE_FACTORY_CLIENT_H_

#include "cc/resources/returned_resource.h"
#include "cc/surfaces/surfaces_export.h"

namespace cc {

class CC_SURFACES_EXPORT SurfaceFactoryClient {
public:
    virtual ~SurfaceFactoryClient() { }

    virtual void ReturnResources(const ReturnedResourceArray& resources) = 0;
};

} // namespace cc

#endif // CC_SURFACES_SURFACE_FACTORY_CLIENT_H_
