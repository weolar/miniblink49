// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorBeacon_h
#define NavigatorBeacon_h

#include "core/frame/LocalFrameLifecycleObserver.h"
#include "core/frame/Navigator.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Blob;
class ExceptionState;
class ExecutionContext;
class KURL;
class ArrayBufferViewOrBlobOrStringOrFormData;

class NavigatorBeacon final : public GarbageCollectedFinalized<NavigatorBeacon>, public LocalFrameLifecycleObserver, public HeapSupplement<Navigator> {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorBeacon);
public:
    static NavigatorBeacon& from(Navigator&);
    virtual ~NavigatorBeacon();

    static bool sendBeacon(ExecutionContext*, Navigator&, const String&, const ArrayBufferViewOrBlobOrStringOrFormData&, ExceptionState&);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit NavigatorBeacon(Navigator&);

    static const char* supplementName();

    bool canSendBeacon(ExecutionContext*, const KURL&, ExceptionState&);
    int maxAllowance() const;
    bool beaconResult(ExecutionContext*, bool allowed, int sentBytes);

    int m_transmittedBytes;
};

} // namespace blink

#endif // NavigatorBeacon_h
