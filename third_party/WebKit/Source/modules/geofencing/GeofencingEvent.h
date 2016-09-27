// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeofencingEvent_h
#define GeofencingEvent_h

#include "modules/EventModules.h"
#include "modules/ModulesExport.h"
#include "modules/geofencing/GeofencingRegion.h"
#include "platform/heap/Handle.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {

class GeofencingRegion;

// FIXME: This should derive from ExtendableEvent.
class MODULES_EXPORT GeofencingEvent final : public Event {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<GeofencingEvent> create()
    {
        return adoptRefWillBeNoop(new GeofencingEvent);
    }

    static PassRefPtrWillBeRawPtr<GeofencingEvent> create(const AtomicString& type, const String& id, GeofencingRegion* region)
    {
        return adoptRefWillBeNoop(new GeofencingEvent(type, id, region));
    }

    ~GeofencingEvent() override;
    DECLARE_VIRTUAL_TRACE();

    const AtomicString& interfaceName() const override;

    String id() const { return m_id; }

    GeofencingRegion* region() const { return m_region; }

private:
    GeofencingEvent();
    GeofencingEvent(const AtomicString& type, const String& id, GeofencingRegion*);
    String m_id;
    PersistentWillBeMember<GeofencingRegion> m_region;
};

} // namespace blink

#endif // GeofencingEvent_h
