// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IndexedDBNames_h
#define IndexedDBNames_h

#include "wtf/text/AtomicString.h"
#include "platform/PlatformExport.h"

// Generated from:
// - indexeddb/IndexedDBNames.in

namespace blink {
namespace IndexedDBNames {

extern const WTF::AtomicString& done;
extern const WTF::AtomicString& next;
extern const WTF::AtomicString& nextunique;
extern const WTF::AtomicString& none;
extern const WTF::AtomicString& pending;
extern const WTF::AtomicString& prev;
extern const WTF::AtomicString& prevunique;
extern const WTF::AtomicString& readonly;
extern const WTF::AtomicString& readwrite;
extern const WTF::AtomicString& total;
extern const WTF::AtomicString& versionchange;

void init();

} // IndexedDBNames
} // namespace blink

#endif
