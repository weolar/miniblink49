// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/geofencing/CircularGeofencingRegion.h"

#include "modules/geofencing/CircularGeofencingRegionInit.h"
#include "public/platform/WebString.h"

namespace blink {

CircularGeofencingRegion* CircularGeofencingRegion::create(const CircularGeofencingRegionInit& init)
{
    WebCircularGeofencingRegion region;
    if (init.hasLatitude())
        region.latitude = init.latitude();
    if (init.hasLongitude())
        region.longitude = init.longitude();
    if (init.hasRadius())
        region.radius = init.radius();
    return new CircularGeofencingRegion(init.id(), region);
}

CircularGeofencingRegion* CircularGeofencingRegion::create(const WebString& id, const WebCircularGeofencingRegion& region)
{
    return new CircularGeofencingRegion(id, region);
}

CircularGeofencingRegion::CircularGeofencingRegion(const String& id, const WebCircularGeofencingRegion& region)
    : GeofencingRegion(id)
    , m_webRegion(region)
{
}

WebCircularGeofencingRegion CircularGeofencingRegion::webRegion() const
{
    return m_webRegion;
}

} // namespace blink
