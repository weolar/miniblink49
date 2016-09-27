/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "public/web/WebMediaDevicesRequest.h"

#include "core/dom/Document.h"
#include "modules/mediastream/MediaDevicesRequest.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/WebMediaDeviceInfo.h"
#include "public/platform/WebSecurityOrigin.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"
#include "public/web/WebDocument.h"
#include "wtf/Vector.h"

namespace blink {

WebMediaDevicesRequest::WebMediaDevicesRequest(MediaDevicesRequest* request)
    : m_private(request)
{
}

void WebMediaDevicesRequest::reset()
{
    m_private.reset();
}

WebSecurityOrigin WebMediaDevicesRequest::securityOrigin() const
{
    ASSERT(!isNull() && m_private->executionContext());
    return WebSecurityOrigin(m_private->executionContext()->securityOrigin());
}

WebDocument WebMediaDevicesRequest::ownerDocument() const
{
    ASSERT(!isNull());
    return WebDocument(m_private->ownerDocument());
}

void WebMediaDevicesRequest::requestSucceeded(WebVector<WebMediaDeviceInfo> webDevices)
{
    ASSERT(!isNull());

    MediaDeviceInfoVector devices(webDevices.size());
    for (size_t i = 0; i < webDevices.size(); ++i)
        devices[i] = MediaDeviceInfo::create(webDevices[i]);

    m_private->succeed(devices);
}

bool WebMediaDevicesRequest::equals(const WebMediaDevicesRequest& other) const
{
    if (isNull() || other.isNull())
        return false;
    return m_private.get() == other.m_private.get();
}

void WebMediaDevicesRequest::assign(const WebMediaDevicesRequest& other)
{
    m_private = other.m_private;
}

WebMediaDevicesRequest::operator MediaDevicesRequest*() const
{
    return m_private.get();
}

} // namespace blink
