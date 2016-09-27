// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "PlatformResourceLoader.h"

#include "public/platform/Platform.h"
#include "public/platform/WebData.h"

namespace blink {

String loadResourceAsASCIIString(const char* resource)
{
    const WebData& resourceData = Platform::current()->loadResource(resource);
    String dataString(resourceData.data(), resourceData.size());
    ASSERT(!dataString.isEmpty() && dataString.containsOnlyASCII());
    return dataString;
}

} // namespace blink
