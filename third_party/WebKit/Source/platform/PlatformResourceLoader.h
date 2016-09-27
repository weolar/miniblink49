// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PlatformResourceLoader_h
#define PlatformResourceLoader_h

#include "platform/PlatformExport.h"
#include "wtf/text/WTFString.h"

namespace blink {

PLATFORM_EXPORT String loadResourceAsASCIIString(const char* resource);

} // namespace blink

#endif // PlatformResourceLoader_h
