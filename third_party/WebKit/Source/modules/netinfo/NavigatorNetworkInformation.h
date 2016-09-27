// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NavigatorNetworkInformation_h
#define NavigatorNetworkInformation_h

#include "core/frame/DOMWindowProperty.h"
#include "platform/Supplementable.h"

namespace blink {

class Navigator;
class NetworkInformation;

class NavigatorNetworkInformation final : public GarbageCollectedFinalized<NavigatorNetworkInformation>, public HeapSupplement<Navigator>, public DOMWindowProperty {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorNetworkInformation);
public:
    static NavigatorNetworkInformation& from(Navigator&);
    static NavigatorNetworkInformation* toNavigatorNetworkInformation(Navigator&);

    virtual ~NavigatorNetworkInformation();

    static NetworkInformation* connection(Navigator&);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit NavigatorNetworkInformation(Navigator&);
    NetworkInformation* connection();

    static const char* supplementName();

    Member<NetworkInformation> m_connection;
};

} // namespace blink

#endif // NavigatorNetworkInformation_h
