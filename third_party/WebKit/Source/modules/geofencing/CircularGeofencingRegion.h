// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CircularGeofencingRegion_h
#define CircularGeofencingRegion_h

#include "modules/ModulesExport.h"
#include "modules/geofencing/GeofencingRegion.h"
#include "public/platform/WebCircularGeofencingRegion.h"

namespace blink {

class CircularGeofencingRegionInit;

class MODULES_EXPORT CircularGeofencingRegion final : public GeofencingRegion {
    DEFINE_WRAPPERTYPEINFO();
    WTF_MAKE_NONCOPYABLE(CircularGeofencingRegion);
public:
    static CircularGeofencingRegion* create(const CircularGeofencingRegionInit&);
    static CircularGeofencingRegion* create(const WebString& id, const WebCircularGeofencingRegion&);
    ~CircularGeofencingRegion() override { }

    double latitude() const { return m_webRegion.latitude; }
    double longitude() const { return m_webRegion.longitude; }
    double radius() const { return m_webRegion.radius; }

    WebCircularGeofencingRegion webRegion() const;

    DEFINE_INLINE_VIRTUAL_TRACE() { GeofencingRegion::trace(visitor); }

    bool isCircularGeofencingRegion() const override { return true; }

private:
    explicit CircularGeofencingRegion(const String& id, const WebCircularGeofencingRegion&);

    WebCircularGeofencingRegion m_webRegion;
};

DEFINE_TYPE_CASTS(CircularGeofencingRegion, GeofencingRegion, region, region->isCircularGeofencingRegion(), region.isCircularGeofencingRegion());

} // namespace blink

#endif // CircularGeofencingRegion_h
