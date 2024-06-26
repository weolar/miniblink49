// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_SURFACES_SURFACE_DAMAGE_OBSERVER_H_
#define CC_SURFACES_SURFACE_DAMAGE_OBSERVER_H_

#include "cc/surfaces/surface_id.h"

namespace cc {

class SurfaceDamageObserver {
public:
    // Runs when a Surface is damaged. *changed should be set to true if this
    // causes a Display to be damaged.
    virtual void OnSurfaceDamaged(SurfaceId surface_id, bool* changed) = 0;
};

} // namespace cc

#endif // CC_SURFACES_SURFACE_DAMAGE_OBSERVER_H_
