/*
 * Copyright (C) 2008 Collin Jackson  <collinj@webkit.org>
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/network/NetworkHints.h"

#include "public/platform/Platform.h"
#include "public/platform/WebPrescientNetworking.h"

namespace blink {

void prefetchDNS(const String& hostname)
{
    if (WebPrescientNetworking* prescientNetworking = Platform::current()->prescientNetworking())
        prescientNetworking->prefetchDNS(hostname);
}

void preconnect(const KURL& url, const CrossOriginAttributeValue crossOrigin)
{
    if (WebPrescientNetworking* prescientNetworking = Platform::current()->prescientNetworking()) {
        bool allowCredentials = (crossOrigin != CrossOriginAttributeAnonymous);
        // TODO(yoav): Call only the crossorigin interface once everything is hooked up.
        if (crossOrigin == CrossOriginAttributeNotSet)
            prescientNetworking->preconnect(url);
        else
            prescientNetworking->preconnect(url, allowCredentials);
    }
}

} // namespace blink
