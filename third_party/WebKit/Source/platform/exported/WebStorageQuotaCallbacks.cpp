// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebStorageQuotaCallbacks.h"

#include "platform/StorageQuotaCallbacks.h"

namespace blink {

WebStorageQuotaCallbacks::WebStorageQuotaCallbacks(StorageQuotaCallbacks* callbacks)
    : m_private(callbacks)
{
}

void WebStorageQuotaCallbacks::reset()
{
    m_private.reset();
}

void WebStorageQuotaCallbacks::assign(const WebStorageQuotaCallbacks& other)
{
    m_private = other.m_private;
}

void WebStorageQuotaCallbacks::didQueryStorageUsageAndQuota(unsigned long long usageInBytes, unsigned long long quotaInBytes)
{
    ASSERT(!m_private.isNull());
    m_private->didQueryStorageUsageAndQuota(usageInBytes, quotaInBytes);
    m_private.reset();
}

void WebStorageQuotaCallbacks::didGrantStorageQuota(unsigned long long usageInBytes, unsigned long long grantedQuotaInBytes)
{
    ASSERT(!m_private.isNull());
    m_private->didGrantStorageQuota(usageInBytes, grantedQuotaInBytes);
    m_private.reset();
}

void WebStorageQuotaCallbacks::didFail(WebStorageQuotaError error)
{
    ASSERT(!m_private.isNull());
    m_private->didFail(error);
    m_private.reset();
}

} // namespace blink
