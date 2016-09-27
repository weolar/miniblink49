// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/credentialmanager/PlatformCredential.h"

namespace blink {

PlatformCredential* PlatformCredential::create(const String& id, const String& name, const KURL& iconURL)
{
    return new PlatformCredential(id, name, iconURL);
}

PlatformCredential::PlatformCredential(const String& id, const String& name, const KURL& iconURL)
    : m_id(id)
    , m_name(name)
    , m_iconURL(iconURL)
    , m_type("credential")
{
}

PlatformCredential::~PlatformCredential()
{
}

} // namespace blink
