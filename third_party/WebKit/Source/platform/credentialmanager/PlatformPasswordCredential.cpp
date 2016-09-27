// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/credentialmanager/PlatformPasswordCredential.h"

namespace blink {

PlatformPasswordCredential* PlatformPasswordCredential::create(const String& id, const String& password, const String& name, const KURL& iconURL)
{
    return new PlatformPasswordCredential(id, password, name, iconURL);
}

PlatformPasswordCredential::PlatformPasswordCredential(const String& id, const String& password, const String& name, const KURL& iconURL)
    : PlatformCredential(id, name, iconURL)
    , m_password(password)
{
    setType("password");
}

PlatformPasswordCredential::~PlatformPasswordCredential()
{
}

} // namespace blink

