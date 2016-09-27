/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GeolocationController_h
#define GeolocationController_h

#include "core/frame/LocalFrame.h"
#include "core/page/PageLifecycleObserver.h"
#include "modules/ModulesExport.h"
#include "modules/geolocation/Geolocation.h"
#include "platform/heap/Handle.h"
#include "wtf/HashSet.h"
#include "wtf/Noncopyable.h"

namespace blink {

class GeolocationClient;
class GeolocationError;
class GeolocationPosition;

class MODULES_EXPORT GeolocationController final : public NoBaseWillBeGarbageCollectedFinalized<GeolocationController>, public WillBeHeapSupplement<LocalFrame>, public PageLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(GeolocationController);
    WTF_MAKE_NONCOPYABLE(GeolocationController);
public:
    virtual ~GeolocationController();

    static PassOwnPtrWillBeRawPtr<GeolocationController> create(LocalFrame&, GeolocationClient*);

    void addObserver(Geolocation*, bool enableHighAccuracy);
    void removeObserver(Geolocation*);

    void requestPermission(Geolocation*);
    void cancelPermissionRequest(Geolocation*);

    void positionChanged(GeolocationPosition*);
    void errorOccurred(GeolocationError*);

    GeolocationPosition* lastPosition();

    void setClientForTest(GeolocationClient*);
    bool hasClientForTest() { return m_hasClientForTest; }
    GeolocationClient* client() { return m_client; }

    // Inherited from PageLifecycleObserver.
    void pageVisibilityChanged() override;

    static const char* supplementName();
    static GeolocationController* from(LocalFrame* frame) { return static_cast<GeolocationController*>(WillBeHeapSupplement<LocalFrame>::from(frame, supplementName())); }

    // Inherited from Supplement.
    DECLARE_VIRTUAL_TRACE();

private:
    GeolocationController(LocalFrame&, GeolocationClient*);

    void startUpdatingIfNeeded();
    void stopUpdatingIfNeeded();

    RawPtrWillBeMember<GeolocationClient> m_client;
    bool m_hasClientForTest;

    PersistentWillBeMember<GeolocationPosition> m_lastPosition;
    typedef PersistentHeapHashSetWillBeHeapHashSet<Member<Geolocation>> ObserversSet;
    // All observers; both those requesting high accuracy and those not.
    ObserversSet m_observers;
    ObserversSet m_highAccuracyObservers;
    bool m_isClientUpdating;
};

} // namespace blink

#endif // GeolocationController_h
