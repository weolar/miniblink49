// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/CrossOriginAttribute.h"

namespace blink {

CrossOriginAttributeValue crossOriginAttributeValue(const String& value)
{
    if (value.isNull())
        return CrossOriginAttributeNotSet;
    if (equalIgnoringCase(value, "use-credentials"))
        return CrossOriginAttributeUseCredentials;
    return CrossOriginAttributeAnonymous;
}

} // namespace blink

