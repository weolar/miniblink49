// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebMediaKeySystemMediaCapability_h
#define WebMediaKeySystemMediaCapability_h

#include "public/platform/WebString.h"

namespace blink {

struct WebMediaKeySystemMediaCapability {
    WebMediaKeySystemMediaCapability() { }

    WebString contentType;
    WebString mimeType;
    WebString codecs;
    WebString robustness;
};

} // namespace blink

#endif // WebMediaKeySystemMediaCapability_h
