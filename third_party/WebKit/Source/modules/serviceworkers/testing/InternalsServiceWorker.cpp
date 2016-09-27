// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/testing/InternalsServiceWorker.h"

#include "modules/serviceworkers/ServiceWorker.h"

namespace blink {

void InternalsServiceWorker::terminateServiceWorker(Internals& internals, ServiceWorker* worker)
{
    worker->internalsTerminate();
}

} // namespace blink
