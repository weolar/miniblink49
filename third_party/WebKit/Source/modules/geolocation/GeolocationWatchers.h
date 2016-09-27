// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeolocationWatchers_h
#define GeolocationWatchers_h

#include "platform/heap/Handle.h"

namespace blink {

class GeoNotifier;

class GeolocationWatchers {
    DISALLOW_ALLOCATION();

public:
    GeolocationWatchers() { }
    DECLARE_TRACE();

    bool add(int id, GeoNotifier*);
    GeoNotifier* find(int id);
    void remove(int id);
    void remove(GeoNotifier*);
    bool contains(GeoNotifier*) const;
    void clear();
    bool isEmpty() const;

    void getNotifiersVector(HeapVector<Member<GeoNotifier>>&) const;

private:
    typedef HeapHashMap<int, Member<GeoNotifier>> IdToNotifierMap;
    typedef HeapHashMap<Member<GeoNotifier>, int> NotifierToIdMap;

    IdToNotifierMap m_idToNotifierMap;
    NotifierToIdMap m_notifierToIdMap;
};

} // namespace blink

#endif // GeolocationWatchers_h
