// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/geolocation/GeoNotifier.h"

#include "modules/geolocation/Geolocation.h"
#include "modules/geolocation/PositionError.h"
#include "modules/geolocation/PositionOptions.h"

namespace blink {

GeoNotifier::GeoNotifier(Geolocation* geolocation, PositionCallback* successCallback, PositionErrorCallback* errorCallback, const PositionOptions& options)
    // FIXME : m_geolocation should be removed, it makes circular dependancy.
    : m_geolocation(geolocation)
    , m_successCallback(successCallback)
    , m_errorCallback(errorCallback)
    , m_options(options)
    , m_timer(this, &GeoNotifier::timerFired)
    , m_useCachedPosition(false)
{
    ASSERT(m_geolocation);
    ASSERT(m_successCallback);
}

DEFINE_TRACE(GeoNotifier)
{
    visitor->trace(m_geolocation);
    visitor->trace(m_successCallback);
    visitor->trace(m_errorCallback);
    visitor->trace(m_fatalError);
}

void GeoNotifier::setFatalError(PositionError* error)
{
    // If a fatal error has already been set, stick with it. This makes sure that
    // when permission is denied, this is the error reported, as required by the
    // spec.
    if (m_fatalError)
        return;

    m_fatalError = error;
    // An existing timer may not have a zero timeout.
    m_timer.stop();
    m_timer.startOneShot(0, FROM_HERE);
}

void GeoNotifier::setUseCachedPosition()
{
    m_useCachedPosition = true;
    m_timer.startOneShot(0, FROM_HERE);
}

void GeoNotifier::runSuccessCallback(Geoposition* position)
{
    m_successCallback->handleEvent(position);
}

void GeoNotifier::runErrorCallback(PositionError* error)
{
    if (m_errorCallback)
        m_errorCallback->handleEvent(error);
}

void GeoNotifier::startTimer()
{
    m_timer.startOneShot(m_options.timeout() / 1000.0, FROM_HERE);
}

void GeoNotifier::stopTimer()
{
    m_timer.stop();
}

void GeoNotifier::timerFired(Timer<GeoNotifier>*)
{
    m_timer.stop();

    // Test for fatal error first. This is required for the case where the LocalFrame is
    // disconnected and requests are cancelled.
    if (m_fatalError) {
        runErrorCallback(m_fatalError.get());
        // This will cause this notifier to be deleted.
        m_geolocation->fatalErrorOccurred(this);
        return;
    }

    if (m_useCachedPosition) {
        // Clear the cached position flag in case this is a watch request, which
        // will continue to run.
        m_useCachedPosition = false;
        m_geolocation->requestUsesCachedPosition(this);
        return;
    }

    if (m_errorCallback)
        m_errorCallback->handleEvent(PositionError::create(PositionError::TIMEOUT, "Timeout expired"));
    m_geolocation->requestTimedOut(this);
}

} // namespace blink
