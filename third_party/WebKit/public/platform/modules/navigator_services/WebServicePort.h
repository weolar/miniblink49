// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServicePort_h
#define WebServicePort_h

#include "public/platform/WebURL.h"

namespace blink {

using WebServicePortID = int;

// Struct containing the data representing a ServicePort.
struct WebServicePort {
    // Members corresponding to ServicePort.idl attributes.
    WebURL targetUrl;
    WebString name;
    WebString data;

    // Unique ID to identify this port.
    WebServicePortID id = -1;
};

} // namespace blink

#endif // WebServicePort_h
