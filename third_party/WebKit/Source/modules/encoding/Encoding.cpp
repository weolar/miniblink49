// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/encoding/Encoding.h"

namespace blink {

namespace Encoding {

bool isASCIIWhiteSpace(UChar c)
{
    // From Encoding Standard:
    // "The ASCII whitespace are code points U+0009, U+000A, U+000C, U+000D, and U+0020."
    return c == 0x09 || c == 0x0A || c == 0x0C || c == 0x0D || c == 0x20;
}

} // namespace Encoding

} // namespace blink
