// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebEncryptedMediaKeyInformation.h"

namespace blink {

WebEncryptedMediaKeyInformation::WebEncryptedMediaKeyInformation()
{
}

WebEncryptedMediaKeyInformation::~WebEncryptedMediaKeyInformation()
{
}

WebData WebEncryptedMediaKeyInformation::id() const
{
    return m_id;
}

void WebEncryptedMediaKeyInformation::setId(const WebData& id)
{
    m_id.assign(id);
}

WebEncryptedMediaKeyInformation::KeyStatus WebEncryptedMediaKeyInformation::status() const
{
    return m_status;
}

void WebEncryptedMediaKeyInformation::setStatus(KeyStatus status)
{
    m_status = status;
}

uint32_t WebEncryptedMediaKeyInformation::systemCode() const
{
    return m_systemCode;
}

void WebEncryptedMediaKeyInformation::setSystemCode(uint32_t systemCode)
{
    m_systemCode = systemCode;
}

} // namespace blink
