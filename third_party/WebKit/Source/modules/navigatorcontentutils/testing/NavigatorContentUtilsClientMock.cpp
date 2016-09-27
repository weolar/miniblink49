// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "NavigatorContentUtilsClientMock.h"

#include "modules/navigatorcontentutils/NavigatorContentUtilsClient.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/StringHash.h"

namespace blink {

void NavigatorContentUtilsClientMock::registerProtocolHandler(const String& scheme,
    const KURL& url, const String& title)
{
    ProtocolInfo info;
    info.scheme = scheme;
    info.url = url;
    info.title = title;

    m_protocolMap.set(scheme, info);
}

NavigatorContentUtilsClient::CustomHandlersState NavigatorContentUtilsClientMock::isProtocolHandlerRegistered(const String& scheme,
    const KURL& url)
{
    // "declined" state is checked by NavigatorContentUtils::isProtocolHandlerRegistered() before calling this function.
    if (m_protocolMap.contains(scheme))
        return NavigatorContentUtilsClient::CustomHandlersRegistered;

    return NavigatorContentUtilsClient::CustomHandlersNew;
}

void NavigatorContentUtilsClientMock::unregisterProtocolHandler(const String& scheme,
    const KURL& url)
{
    m_protocolMap.remove(scheme);
}

} // namespace blink
