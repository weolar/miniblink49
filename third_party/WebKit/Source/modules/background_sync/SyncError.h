// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SyncError_h
#define SyncError_h

#include "core/dom/DOMException.h"
#include "platform/heap/Handle.h"
#include "public/platform/modules/background_sync/WebSyncError.h"

namespace blink {

class ScriptPromiseResolver;

class SyncError {
    WTF_MAKE_NONCOPYABLE(SyncError);
public:
    // For CallbackPromiseAdapter.
    using WebType = WebSyncError;
    static DOMException* take(ScriptPromiseResolver*, WebType*);
    static void dispose(WebType*);

private:
    SyncError() = delete;
};

} // namespace blink

#endif // SyncError_h
