// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GeoNotifier_h
#define GeoNotifier_h

#include "modules/geolocation/PositionCallback.h"
#include "modules/geolocation/PositionErrorCallback.h"
#include "modules/geolocation/PositionOptions.h"
#include "platform/Timer.h"
#include "platform/heap/Handle.h"

namespace blink {

class Geolocation;
class Geoposition;
class PositionError;

class GeoNotifier : public GarbageCollectedFinalized<GeoNotifier> {
public:
    static GeoNotifier* create(Geolocation* geolocation, PositionCallback* positionCallback, PositionErrorCallback* positionErrorCallback, const PositionOptions& options)
    {
        return new GeoNotifier(geolocation, positionCallback, positionErrorCallback, options);
    }
    DECLARE_TRACE();

    const PositionOptions& options() const { return m_options; }

    // Sets the given error as the fatal error if there isn't one yet.
    // Starts the timer with an interval of 0.
    void setFatalError(PositionError*);

    bool useCachedPosition() const { return m_useCachedPosition; }

    // Tells the notifier to use a cached position and starts its timer with
    // an interval of 0.
    void setUseCachedPosition();

    void runSuccessCallback(Geoposition*);
    void runErrorCallback(PositionError*);

    void startTimer();
    void stopTimer();

    // Runs the error callback if there is a fatal error. Otherwise, if a
    // cached position must be used, registers itself for receiving one.
    // Otherwise, the notifier has expired, and its error callback is run.
    void timerFired(Timer<GeoNotifier>*);

private:
    GeoNotifier(Geolocation*, PositionCallback*, PositionErrorCallback*, const PositionOptions&);

    Member<Geolocation> m_geolocation;
    Member<PositionCallback> m_successCallback;
    Member<PositionErrorCallback> m_errorCallback;
    const PositionOptions m_options;
    Timer<GeoNotifier> m_timer;
    Member<PositionError> m_fatalError;
    bool m_useCachedPosition;
};

} // namespace blink

#endif // GeoNotifier_h
