// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FetchInitiatorTypeNames_h
#define FetchInitiatorTypeNames_h

#include "wtf/text/AtomicString.h"
#include "core/CoreExport.h"

// Generated from:
// - fetch/FetchInitiatorTypeNames.in

namespace blink {
namespace FetchInitiatorTypeNames {

CORE_EXPORT extern const WTF::AtomicString& beacon;
CORE_EXPORT extern const WTF::AtomicString& css;
CORE_EXPORT extern const WTF::AtomicString& document;
CORE_EXPORT extern const WTF::AtomicString& icon;
CORE_EXPORT extern const WTF::AtomicString& internal;
CORE_EXPORT extern const WTF::AtomicString& link;
CORE_EXPORT extern const WTF::AtomicString& ping;
CORE_EXPORT extern const WTF::AtomicString& processinginstruction;
CORE_EXPORT extern const WTF::AtomicString& texttrack;
CORE_EXPORT extern const WTF::AtomicString& violationreport;
CORE_EXPORT extern const WTF::AtomicString& xml;
CORE_EXPORT extern const WTF::AtomicString& xmlhttprequest;

CORE_EXPORT void init();

} // FetchInitiatorTypeNames
} // namespace blink

#endif
