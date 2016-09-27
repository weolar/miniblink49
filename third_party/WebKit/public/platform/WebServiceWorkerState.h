// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebServiceWorkerState_h
#define WebServiceWorkerState_h

namespace blink {

enum WebServiceWorkerState {
    WebServiceWorkerStateUnknown,
    WebServiceWorkerStateInstalling,
    WebServiceWorkerStateInstalled,
    WebServiceWorkerStateActivating,
    WebServiceWorkerStateActivated,
    WebServiceWorkerStateRedundant,
    WebServiceWorkerStateLast = WebServiceWorkerStateRedundant
};

} // namespace blink

#endif // WebServiceWorkerState_h
