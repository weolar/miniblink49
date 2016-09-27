// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerSkipWaitingCallbacks_h
#define WebServiceWorkerSkipWaitingCallbacks_h

#include "public/platform/WebCallbacks.h"

namespace blink {

using WebServiceWorkerSkipWaitingCallbacks = WebCallbacks<void, void>;

} // namespace blink

#endif // WebServiceWorkerSkipWaitingCallbacks_h
