// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaKeysController_h
#define MediaKeysController_h

#include "core/page/Page.h"
#include "modules/ModulesExport.h"

namespace blink {

class ExecutionContext;
class MediaKeysClient;
class WebEncryptedMediaClient;

class MODULES_EXPORT MediaKeysController final : public NoBaseWillBeGarbageCollected<MediaKeysController>, public WillBeHeapSupplement<Page> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(MediaKeysController);
public:
    WebEncryptedMediaClient* encryptedMediaClient(ExecutionContext*);

    static void provideMediaKeysTo(Page&, MediaKeysClient*);
    static MediaKeysController* from(Page* page) { return static_cast<MediaKeysController*>(WillBeHeapSupplement<Page>::from(page, supplementName())); }

    DEFINE_INLINE_VIRTUAL_TRACE() { WillBeHeapSupplement<Page>::trace(visitor); }

private:
    explicit MediaKeysController(MediaKeysClient*);
    static const char* supplementName();

    // Raw reference to the client implementation, which is currently owned
    // by the WebView. Its lifetime extends past any m_client accesses.
    // It is not on the Oilpan heap.
    MediaKeysClient* m_client;
};

} // namespace blink

#endif // MediaKeysController_h

