// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BeaconLoader_h
#define BeaconLoader_h

#include "core/CoreExport.h"
#include "core/loader/PingLoader.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebURLLoaderClient.h"
#include "wtf/Forward.h"
#include "wtf/Noncopyable.h"

namespace blink {

class Blob;
class DOMArrayBufferView;
class DOMFormData;
class KURL;
class LocalFrame;
class SecurityOrigin;

// Issue asynchronous beacon transmission loads independent of LocalFrame
// staying alive. PingLoader providing the service.
class CORE_EXPORT BeaconLoader final : public PingLoader {
    WTF_MAKE_NONCOPYABLE(BeaconLoader);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(BeaconLoader);
public:
    ~BeaconLoader() override { }

    static bool sendBeacon(LocalFrame*, int, const KURL&, const String&, int&);
    static bool sendBeacon(LocalFrame*, int, const KURL&, PassRefPtr<DOMArrayBufferView>, int&);
    static bool sendBeacon(LocalFrame*, int, const KURL&, Blob*, int&);
    static bool sendBeacon(LocalFrame*, int, const KURL&, DOMFormData*, int&);

private:
    class Sender;

    BeaconLoader(LocalFrame*, ResourceRequest&, const FetchInitiatorInfo&, StoredCredentials);

    RefPtr<SecurityOrigin> m_beaconOrigin;

    // WebURLLoaderClient
    void willSendRequest(WebURLLoader*, WebURLRequest&, const WebURLResponse&) override;
};

} // namespace blink

#endif // BeaconLoader_h
