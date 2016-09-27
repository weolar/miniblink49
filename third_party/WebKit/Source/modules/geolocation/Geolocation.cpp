/*
 * Copyright (C) 2008, 2009, 2010, 2011 Apple Inc. All Rights Reserved.
 * Copyright (C) 2009 Torch Mobile, Inc.
 * Copyright 2010, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/geolocation/Geolocation.h"

#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "modules/geolocation/Coordinates.h"
#include "modules/geolocation/GeolocationController.h"
#include "modules/geolocation/GeolocationError.h"
#include "modules/geolocation/GeolocationPosition.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/CurrentTime.h"

namespace blink {

static const char permissionDeniedErrorMessage[] = "User denied Geolocation";
static const char failedToStartServiceErrorMessage[] = "Failed to start Geolocation service";
static const char framelessDocumentErrorMessage[] = "Geolocation cannot be used in frameless documents";

static Geoposition* createGeoposition(GeolocationPosition* position)
{
    if (!position)
        return nullptr;

    Coordinates* coordinates = Coordinates::create(
        position->latitude(),
        position->longitude(),
        position->canProvideAltitude(),
        position->altitude(),
        position->accuracy(),
        position->canProvideAltitudeAccuracy(),
        position->altitudeAccuracy(),
        position->canProvideHeading(),
        position->heading(),
        position->canProvideSpeed(),
        position->speed());
    return Geoposition::create(coordinates, convertSecondsToDOMTimeStamp(position->timestamp()));
}

static PositionError* createPositionError(GeolocationError* error)
{
    PositionError::ErrorCode code = PositionError::POSITION_UNAVAILABLE;
    switch (error->code()) {
    case GeolocationError::PermissionDenied:
        code = PositionError::PERMISSION_DENIED;
        break;
    case GeolocationError::PositionUnavailable:
        code = PositionError::POSITION_UNAVAILABLE;
        break;
    }

    return PositionError::create(code, error->message());
}

Geolocation* Geolocation::create(ExecutionContext* context)
{
    Geolocation* geolocation = new Geolocation(context);
    geolocation->suspendIfNeeded();
    return geolocation;
}

Geolocation::Geolocation(ExecutionContext* context)
    : ActiveDOMObject(context)
    , m_geolocationPermission(PermissionUnknown)
{
}

Geolocation::~Geolocation()
{
    ASSERT(m_geolocationPermission != PermissionRequested);
}

DEFINE_TRACE(Geolocation)
{
    visitor->trace(m_oneShots);
    visitor->trace(m_watchers);
    visitor->trace(m_pendingForPermissionNotifiers);
    visitor->trace(m_lastPosition);
    visitor->trace(m_requestsAwaitingCachedPosition);
    ActiveDOMObject::trace(visitor);
}

Document* Geolocation::document() const
{
    return toDocument(executionContext());
}

LocalFrame* Geolocation::frame() const
{
    return document() ? document()->frame() : 0;
}

void Geolocation::stop()
{
    LocalFrame* frame = this->frame();
    if (frame && m_geolocationPermission == PermissionRequested)
        GeolocationController::from(frame)->cancelPermissionRequest(this);

    // The frame may be moving to a new page and we want to get the permissions from the new page's client.
    m_geolocationPermission = PermissionUnknown;
    cancelAllRequests();
    stopUpdating();
    m_pendingForPermissionNotifiers.clear();
}

Geoposition* Geolocation::lastPosition()
{
    LocalFrame* frame = this->frame();
    if (!frame)
        return 0;

    m_lastPosition = createGeoposition(GeolocationController::from(frame)->lastPosition());

    return m_lastPosition.get();
}

void Geolocation::recordOriginTypeAccess() const
{
    ASSERT(frame());

    Document* document = this->document();
    ASSERT(document);

    // It is required by isPrivilegedContext() but isn't
    // actually used. This could be used later if a warning is shown in the
    // developer console.
    String insecureOriginMsg;
    if (document->isPrivilegedContext(insecureOriginMsg))
        UseCounter::count(document, UseCounter::GeolocationSecureOrigin);
    else
        UseCounter::countDeprecation(document, UseCounter::GeolocationInsecureOrigin);
}

void Geolocation::getCurrentPosition(PositionCallback* successCallback, PositionErrorCallback* errorCallback, const PositionOptions& options)
{
    if (!frame())
        return;

    recordOriginTypeAccess();

    GeoNotifier* notifier = GeoNotifier::create(this, successCallback, errorCallback, options);
    startRequest(notifier);

    m_oneShots.add(notifier);
}

int Geolocation::watchPosition(PositionCallback* successCallback, PositionErrorCallback* errorCallback, const PositionOptions& options)
{
    if (!frame())
        return 0;

    recordOriginTypeAccess();

    GeoNotifier* notifier = GeoNotifier::create(this, successCallback, errorCallback, options);
    startRequest(notifier);

    int watchID;
    // Keep asking for the next id until we're given one that we don't already have.
    do {
        watchID = executionContext()->circularSequentialID();
    } while (!m_watchers.add(watchID, notifier));
    return watchID;
}

void Geolocation::startRequest(GeoNotifier *notifier)
{
    if (frame()->settings()->strictPowerfulFeatureRestrictions()) {
        String errorMessage;
        if (!executionContext()->isPrivilegedContext(errorMessage)) {
            notifier->setFatalError(PositionError::create(PositionError::POSITION_UNAVAILABLE, errorMessage));
            return;
        }
    }

    // Check whether permissions have already been denied. Note that if this is the case,
    // the permission state can not change again in the lifetime of this page.
    if (isDenied())
        notifier->setFatalError(PositionError::create(PositionError::PERMISSION_DENIED, permissionDeniedErrorMessage));
    else if (haveSuitableCachedPosition(notifier->options()))
        notifier->setUseCachedPosition();
    else if (!notifier->options().timeout())
        notifier->startTimer();
    else if (!isAllowed()) {
        // if we don't yet have permission, request for permission before calling startUpdating()
        m_pendingForPermissionNotifiers.add(notifier);
        requestPermission();
    } else if (startUpdating(notifier))
        notifier->startTimer();
    else
        notifier->setFatalError(PositionError::create(PositionError::POSITION_UNAVAILABLE, failedToStartServiceErrorMessage));
}

void Geolocation::fatalErrorOccurred(GeoNotifier* notifier)
{
    // This request has failed fatally. Remove it from our lists.
    m_oneShots.remove(notifier);
    m_watchers.remove(notifier);

    if (!hasListeners())
        stopUpdating();
}

void Geolocation::requestUsesCachedPosition(GeoNotifier* notifier)
{
    // This is called asynchronously, so the permissions could have been denied
    // since we last checked in startRequest.
    if (isDenied()) {
        notifier->setFatalError(PositionError::create(PositionError::PERMISSION_DENIED, permissionDeniedErrorMessage));
        return;
    }

    m_requestsAwaitingCachedPosition.add(notifier);

    // If permissions are allowed, make the callback
    if (isAllowed()) {
        makeCachedPositionCallbacks();
        return;
    }

    // Request permissions, which may be synchronous or asynchronous.
    requestPermission();
}

void Geolocation::makeCachedPositionCallbacks()
{
    // All modifications to m_requestsAwaitingCachedPosition are done
    // asynchronously, so we don't need to worry about it being modified from
    // the callbacks.
    for (GeoNotifier* notifier : m_requestsAwaitingCachedPosition) {
        notifier->runSuccessCallback(lastPosition());

        // If this is a one-shot request, stop it. Otherwise, if the watch still
        // exists, start the service to get updates.
        if (m_oneShots.contains(notifier))
            m_oneShots.remove(notifier);
        else if (m_watchers.contains(notifier)) {
            if (!notifier->options().timeout() || startUpdating(notifier))
                notifier->startTimer();
            else
                notifier->setFatalError(PositionError::create(PositionError::POSITION_UNAVAILABLE, failedToStartServiceErrorMessage));
        }
    }

    m_requestsAwaitingCachedPosition.clear();

    if (!hasListeners())
        stopUpdating();
}

void Geolocation::requestTimedOut(GeoNotifier* notifier)
{
    // If this is a one-shot request, stop it.
    m_oneShots.remove(notifier);

    if (!hasListeners())
        stopUpdating();
}

bool Geolocation::haveSuitableCachedPosition(const PositionOptions& options)
{
    Geoposition* cachedPosition = lastPosition();
    if (!cachedPosition)
        return false;
    if (!options.maximumAge())
        return false;
    DOMTimeStamp currentTimeMillis = convertSecondsToDOMTimeStamp(currentTime());
    return cachedPosition->timestamp() > currentTimeMillis - options.maximumAge();
}

void Geolocation::clearWatch(int watchID)
{
    if (watchID <= 0)
        return;

    if (GeoNotifier* notifier = m_watchers.find(watchID))
        m_pendingForPermissionNotifiers.remove(notifier);
    m_watchers.remove(watchID);

    if (!hasListeners())
        stopUpdating();
}

void Geolocation::setIsAllowed(bool allowed)
{
    // This may be due to either a new position from the service, or a cached position.
    m_geolocationPermission = allowed ? PermissionAllowed : PermissionDenied;

    // Permission request was made during the startRequest process
    if (!m_pendingForPermissionNotifiers.isEmpty()) {
        handlePendingPermissionNotifiers();
        m_pendingForPermissionNotifiers.clear();
        return;
    }

    if (!isAllowed()) {
        PositionError* error = PositionError::create(PositionError::PERMISSION_DENIED, permissionDeniedErrorMessage);
        error->setIsFatal(true);
        handleError(error);
        m_requestsAwaitingCachedPosition.clear();
        return;
    }

    // If the service has a last position, use it to call back for all requests.
    // If any of the requests are waiting for permission for a cached position,
    // the position from the service will be at least as fresh.
    if (lastPosition())
        makeSuccessCallbacks();
    else
        makeCachedPositionCallbacks();
}

void Geolocation::sendError(GeoNotifierVector& notifiers, PositionError* error)
{
    for (GeoNotifier* notifier : notifiers)
        notifier->runErrorCallback(error);
}

void Geolocation::sendPosition(GeoNotifierVector& notifiers, Geoposition* position)
{
    for (GeoNotifier* notifier : notifiers)
        notifier->runSuccessCallback(position);
}

void Geolocation::stopTimer(GeoNotifierVector& notifiers)
{
    for (GeoNotifier* notifier : notifiers)
        notifier->stopTimer();
}

void Geolocation::stopTimersForOneShots()
{
    GeoNotifierVector copy;
    copyToVector(m_oneShots, copy);

    stopTimer(copy);
}

void Geolocation::stopTimersForWatchers()
{
    GeoNotifierVector copy;
    m_watchers.getNotifiersVector(copy);

    stopTimer(copy);
}

void Geolocation::stopTimers()
{
    stopTimersForOneShots();
    stopTimersForWatchers();
}

void Geolocation::cancelRequests(GeoNotifierVector& notifiers)
{
    for (GeoNotifier* notifier : notifiers)
        notifier->setFatalError(PositionError::create(PositionError::POSITION_UNAVAILABLE, framelessDocumentErrorMessage));
}

void Geolocation::cancelAllRequests()
{
    GeoNotifierVector copy;
    copyToVector(m_oneShots, copy);
    cancelRequests(copy);
    m_watchers.getNotifiersVector(copy);
    cancelRequests(copy);
}

void Geolocation::extractNotifiersWithCachedPosition(GeoNotifierVector& notifiers, GeoNotifierVector* cached)
{
    GeoNotifierVector nonCached;
    for (GeoNotifier* notifier : notifiers) {
        if (notifier->useCachedPosition()) {
            if (cached)
                cached->append(notifier);
        } else
            nonCached.append(notifier);
    }
    notifiers.swap(nonCached);
}

void Geolocation::copyToSet(const GeoNotifierVector& src, GeoNotifierSet& dest)
{
    for (GeoNotifier* notifier : src)
         dest.add(notifier);
}

void Geolocation::handleError(PositionError* error)
{
    ASSERT(error);

    GeoNotifierVector oneShotsCopy;
    copyToVector(m_oneShots, oneShotsCopy);

    GeoNotifierVector watchersCopy;
    m_watchers.getNotifiersVector(watchersCopy);

    // Clear the lists before we make the callbacks, to avoid clearing notifiers
    // added by calls to Geolocation methods from the callbacks, and to prevent
    // further callbacks to these notifiers.
    GeoNotifierVector oneShotsWithCachedPosition;
    m_oneShots.clear();
    if (error->isFatal())
        m_watchers.clear();
    else {
        // Don't send non-fatal errors to notifiers due to receive a cached position.
        extractNotifiersWithCachedPosition(oneShotsCopy, &oneShotsWithCachedPosition);
        extractNotifiersWithCachedPosition(watchersCopy, 0);
    }

    sendError(oneShotsCopy, error);
    sendError(watchersCopy, error);

    // hasListeners() doesn't distinguish between notifiers due to receive a
    // cached position and those requiring a fresh position. Perform the check
    // before restoring the notifiers below.
    if (!hasListeners())
        stopUpdating();

    // Maintain a reference to the cached notifiers until their timer fires.
    copyToSet(oneShotsWithCachedPosition, m_oneShots);
}

void Geolocation::requestPermission()
{
    if (m_geolocationPermission != PermissionUnknown)
        return;

    LocalFrame* frame = this->frame();
    if (!frame)
        return;

    m_geolocationPermission = PermissionRequested;

    // Ask the embedder: it maintains the geolocation challenge policy itself.
    GeolocationController::from(frame)->requestPermission(this);
}

void Geolocation::makeSuccessCallbacks()
{
    ASSERT(lastPosition());
    ASSERT(isAllowed());

    GeoNotifierVector oneShotsCopy;
    copyToVector(m_oneShots, oneShotsCopy);

    GeoNotifierVector watchersCopy;
    m_watchers.getNotifiersVector(watchersCopy);

    // Clear the lists before we make the callbacks, to avoid clearing notifiers
    // added by calls to Geolocation methods from the callbacks, and to prevent
    // further callbacks to these notifiers.
    m_oneShots.clear();

    // Also clear the set of notifiers waiting for a cached position. All the
    // oneshots and watchers will receive a position now, and if they happen to
    // be lingering in that set, avoid this bug: http://crbug.com/311876 .
    m_requestsAwaitingCachedPosition.clear();

    sendPosition(oneShotsCopy, lastPosition());
    sendPosition(watchersCopy, lastPosition());

    if (!hasListeners())
        stopUpdating();
}

void Geolocation::positionChanged()
{
    ASSERT(isAllowed());

    // Stop all currently running timers.
    stopTimers();

    makeSuccessCallbacks();
}

void Geolocation::setError(GeolocationError* error)
{
    handleError(createPositionError(error));
}

bool Geolocation::startUpdating(GeoNotifier* notifier)
{
    LocalFrame* frame = this->frame();
    if (!frame)
        return false;

    GeolocationController::from(frame)->addObserver(this, notifier->options().enableHighAccuracy());
    return true;
}

void Geolocation::stopUpdating()
{
    LocalFrame* frame = this->frame();
    if (!frame)
        return;

    GeolocationController::from(frame)->removeObserver(this);
}

void Geolocation::handlePendingPermissionNotifiers()
{
    // While we iterate through the list, we need not worry about list being modified as the permission
    // is already set to Yes/No and no new listeners will be added to the pending list.
    for (GeoNotifier* notifier : m_pendingForPermissionNotifiers) {
        if (isAllowed()) {
            // start all pending notification requests as permission granted.
            // The notifier is always ref'ed by m_oneShots or m_watchers.
            if (startUpdating(notifier))
                notifier->startTimer();
            else
                notifier->setFatalError(PositionError::create(PositionError::POSITION_UNAVAILABLE, failedToStartServiceErrorMessage));
        } else {
            notifier->setFatalError(PositionError::create(PositionError::PERMISSION_DENIED, permissionDeniedErrorMessage));
        }
    }
}

} // namespace blink
