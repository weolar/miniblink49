// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServicePortCallbacks_h
#define WebServicePortCallbacks_h

#include "public/platform/WebCallbacks.h"
#include "public/platform/modules/navigator_services/WebServicePort.h"

namespace blink {

using WebServicePortConnectCallbacks = WebCallbacks<WebServicePortID, void>;

// Ownership of the ports passed to onSuccess is kept with the caller, and its
// lifetime does not outlive past the end of the onSuccess call.
using WebServicePortConnectEventCallbacks = WebCallbacks<WebServicePort, void>;

} // namespace blink

#endif // WebServicePortCallbacks_h
