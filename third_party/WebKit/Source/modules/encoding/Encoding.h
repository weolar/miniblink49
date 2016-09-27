// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Encoding_h
#define Encoding_h

#include "wtf/text/Unicode.h"

namespace blink {

namespace Encoding {

// The Encoding Standard has a definition of whitespace that differs from
// WTF::isWhiteSpace() (it excludes vertical tab).
bool isASCIIWhiteSpace(UChar);

} // namespace Encoding

} // namespace blink

#endif // Encoding_h
