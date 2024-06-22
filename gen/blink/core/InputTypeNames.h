// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InputTypeNames_h
#define InputTypeNames_h

#include "wtf/text/AtomicString.h"
#include "core/CoreExport.h"

// Generated from:
// - html/forms/InputTypeNames.in

namespace blink {
namespace InputTypeNames {

CORE_EXPORT extern const WTF::AtomicString& button;
CORE_EXPORT extern const WTF::AtomicString& checkbox;
CORE_EXPORT extern const WTF::AtomicString& color;
CORE_EXPORT extern const WTF::AtomicString& date;
CORE_EXPORT extern const WTF::AtomicString& datetime;
CORE_EXPORT extern const WTF::AtomicString& datetime_local;
CORE_EXPORT extern const WTF::AtomicString& email;
CORE_EXPORT extern const WTF::AtomicString& file;
CORE_EXPORT extern const WTF::AtomicString& hidden;
CORE_EXPORT extern const WTF::AtomicString& image;
CORE_EXPORT extern const WTF::AtomicString& month;
CORE_EXPORT extern const WTF::AtomicString& number;
CORE_EXPORT extern const WTF::AtomicString& password;
CORE_EXPORT extern const WTF::AtomicString& radio;
CORE_EXPORT extern const WTF::AtomicString& range;
CORE_EXPORT extern const WTF::AtomicString& reset;
CORE_EXPORT extern const WTF::AtomicString& search;
CORE_EXPORT extern const WTF::AtomicString& submit;
CORE_EXPORT extern const WTF::AtomicString& tel;
CORE_EXPORT extern const WTF::AtomicString& text;
CORE_EXPORT extern const WTF::AtomicString& time;
CORE_EXPORT extern const WTF::AtomicString& url;
CORE_EXPORT extern const WTF::AtomicString& week;

CORE_EXPORT void init();

} // InputTypeNames
} // namespace blink

#endif
