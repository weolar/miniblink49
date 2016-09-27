// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/NavigatorContentUtilsClientImpl.h"

#include "public/web/WebFrameClient.h"
#include "web/WebLocalFrameImpl.h"

namespace blink {

PassOwnPtr<NavigatorContentUtilsClientImpl> NavigatorContentUtilsClientImpl::create(WebLocalFrameImpl* webFrame)
{
    return adoptPtr(new NavigatorContentUtilsClientImpl(webFrame));
}

NavigatorContentUtilsClientImpl::NavigatorContentUtilsClientImpl(WebLocalFrameImpl* webFrame)
    : m_webFrame(webFrame)
{
}

void NavigatorContentUtilsClientImpl::registerProtocolHandler(const String& scheme, const KURL& url, const String& title)
{
    m_webFrame->client()->registerProtocolHandler(scheme, url, title);
}

NavigatorContentUtilsClient::CustomHandlersState NavigatorContentUtilsClientImpl::isProtocolHandlerRegistered(const String& scheme, const KURL& url)
{
    return static_cast<NavigatorContentUtilsClient::CustomHandlersState>(m_webFrame->client()->isProtocolHandlerRegistered(scheme, url));
}

void NavigatorContentUtilsClientImpl::unregisterProtocolHandler(const String& scheme, const KURL& url)
{
    m_webFrame->client()->unregisterProtocolHandler(scheme, url);
}

} // namespace blink

