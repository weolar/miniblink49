// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebSyncRegistration_h
#define WebSyncRegistration_h

#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebString.h"

namespace blink {

struct WebSyncRegistration {
    enum NetworkState {
        NetworkStateAny = 0,
        NetworkStateAvoidCellular,
        NetworkStateOnline,
        NetworkStateLast = NetworkStateOnline
    };

    enum PowerState {
        PowerStateAuto = 0,
        PowerStateAvoidDraining,
        PowerStateLast = PowerStateAvoidDraining
    };

    enum { UNREGISTERED_SYNC_ID = -1};

    enum Periodicity {
        PeriodicityPeriodic = 0,
        PeriodicityOneShot,
        PeriodicityLast = PeriodicityOneShot
    };

    WebSyncRegistration()
        : id(UNREGISTERED_SYNC_ID)
        , periodicity(PeriodicityOneShot)
        , tag("")
        , minPeriodMs(0)
        , networkState(NetworkState::NetworkStateOnline)
        , powerState(PowerState::PowerStateAuto)
    {
    }

    WebSyncRegistration(int64_t id, Periodicity periodicity,
        const WebString& registrationTag, unsigned long minPeriodMs,
        NetworkState networkState, PowerState powerState)
        : id(id)
        , periodicity(periodicity)
        , tag(registrationTag)
        , minPeriodMs(minPeriodMs)
        , networkState(networkState)
        , powerState(powerState)
    {
    }

    /* Internal identity; not exposed to JS API. */
    int64_t id;

    /* Internal flag; not directly exposed to JS API.
     * Instead, this determines whether this object is represented in JS as a
     * SyncRegistration or a PeriodicSyncRegistration.
     */
    Periodicity periodicity;

    WebString tag;

    /* Minimum time between periodic sync events, in milliseconds. A 0 value
     * here means that the event is a one-shot (not periodic.)
     */
    unsigned long minPeriodMs;

    NetworkState networkState;
    PowerState powerState;
};

} // namespace blink

#endif // WebSyncRegistration_h
