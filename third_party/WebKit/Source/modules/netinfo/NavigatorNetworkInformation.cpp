// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/netinfo/NavigatorNetworkInformation.h"

#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Navigator.h"
#include "modules/netinfo/NetworkInformation.h"

namespace blink {

NavigatorNetworkInformation::NavigatorNetworkInformation(Navigator& navigator)
    : DOMWindowProperty(navigator.frame())
{
}

NavigatorNetworkInformation::~NavigatorNetworkInformation()
{
}

NavigatorNetworkInformation& NavigatorNetworkInformation::from(Navigator& navigator)
{
    NavigatorNetworkInformation* supplement = toNavigatorNetworkInformation(navigator);
    if (!supplement) {
        supplement = new NavigatorNetworkInformation(navigator);
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

NavigatorNetworkInformation* NavigatorNetworkInformation::toNavigatorNetworkInformation(Navigator& navigator)
{
    return static_cast<NavigatorNetworkInformation*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
}

const char* NavigatorNetworkInformation::supplementName()
{
    return "NavigatorNetworkInformation";
}

NetworkInformation* NavigatorNetworkInformation::connection(Navigator& navigator)
{
    return NavigatorNetworkInformation::from(navigator).connection();
}

NetworkInformation* NavigatorNetworkInformation::connection()
{
    if (!m_connection && frame()) {
        ASSERT(frame()->domWindow());
        m_connection = NetworkInformation::create(frame()->domWindow()->executionContext());
    }
    return m_connection.get();
}

DEFINE_TRACE(NavigatorNetworkInformation)
{
    visitor->trace(m_connection);
    HeapSupplement<Navigator>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
