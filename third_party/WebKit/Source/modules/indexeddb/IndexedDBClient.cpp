// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/indexeddb/IndexedDBClient.h"

namespace blink {

static CreateIndexedDBClient* idbClientCreateFunction = nullptr;

void setIndexedDBClientCreateFunction(CreateIndexedDBClient createFunction)
{
    idbClientCreateFunction = createFunction;
}

IndexedDBClient* IndexedDBClient::create()
{
    ASSERT(idbClientCreateFunction);
    // There's no reason why we need to allocate a new proxy each time, but
    // there's also no strong reason not to.
    return idbClientCreateFunction();
}

} // namespace blink
