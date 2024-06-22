// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLElementLookupTrie_h
#define HTMLElementLookupTrie_h

#include "wtf/text/StringImpl.h"

namespace blink {

StringImpl* lookupHTMLTag(const UChar* data, unsigned length);

} // namespace blink

#endif
