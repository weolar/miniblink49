// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FontFamilyNames_h
#define FontFamilyNames_h

#include "wtf/text/AtomicString.h"
#include "platform/PlatformExport.h"

// Generated from:
// - fonts/FontFamilyNames.in

namespace blink {
namespace FontFamilyNames {

PLATFORM_EXPORT extern const WTF::AtomicString& webkit_cursive;
PLATFORM_EXPORT extern const WTF::AtomicString& webkit_fantasy;
PLATFORM_EXPORT extern const WTF::AtomicString& webkit_monospace;
PLATFORM_EXPORT extern const WTF::AtomicString& webkit_pictograph;
PLATFORM_EXPORT extern const WTF::AtomicString& webkit_sans_serif;
PLATFORM_EXPORT extern const WTF::AtomicString& webkit_serif;
PLATFORM_EXPORT extern const WTF::AtomicString& webkit_standard;

PLATFORM_EXPORT void init();

} // FontFamilyNames
} // namespace blink

#endif
