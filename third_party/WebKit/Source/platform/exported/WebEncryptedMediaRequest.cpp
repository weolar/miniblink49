// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "public/platform/WebEncryptedMediaRequest.h"

#include "platform/EncryptedMediaRequest.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/WebMediaKeySystemConfiguration.h"
#include "public/platform/WebSecurityOrigin.h"
#include "public/platform/WebString.h"
#include "public/platform/WebVector.h"

namespace blink {

WebEncryptedMediaRequest::WebEncryptedMediaRequest(const WebEncryptedMediaRequest& request)
{
    assign(request);
}

WebEncryptedMediaRequest::WebEncryptedMediaRequest(EncryptedMediaRequest* request)
    : m_private(request)
{
}

WebEncryptedMediaRequest::~WebEncryptedMediaRequest()
{
    reset();
}

WebString WebEncryptedMediaRequest::keySystem() const
{
    return m_private->keySystem();
}

const WebVector<WebMediaKeySystemConfiguration>& WebEncryptedMediaRequest::supportedConfigurations() const
{
    return m_private->supportedConfigurations();
}

WebSecurityOrigin WebEncryptedMediaRequest::securityOrigin() const
{
    return WebSecurityOrigin(m_private->securityOrigin());
}

void WebEncryptedMediaRequest::requestSucceeded(WebContentDecryptionModuleAccess* access)
{
    m_private->requestSucceeded(access);
}

void WebEncryptedMediaRequest::requestNotSupported(const WebString& errorMessage)
{
    m_private->requestNotSupported(errorMessage);
}

void WebEncryptedMediaRequest::assign(const WebEncryptedMediaRequest& other)
{
    m_private = other.m_private;
}

void WebEncryptedMediaRequest::reset()
{
    m_private.reset();
}

} // namespace blink
