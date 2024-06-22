// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLTokenizerNames_h
#define HTMLTokenizerNames_h

#include "wtf/text/AtomicString.h"
#include "platform/PlatformExport.h"

// Generated from:
// - html/parser/HTMLTokenizerNames.in

namespace blink {
namespace HTMLTokenizerNames {

extern const WTF::AtomicString& doctype;
extern const WTF::AtomicString& system;
extern const WTF::AtomicString& cdata;
extern const WTF::AtomicString& dashDash;
extern const WTF::AtomicString& publicString;

void init();

} // HTMLTokenizerNames
} // namespace blink

#endif
