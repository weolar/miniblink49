// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StorageClientImpl_h
#define StorageClientImpl_h

#include "modules/storage/StorageClient.h"

namespace blink {

class WebViewImpl;

class StorageClientImpl : public StorageClient {
public:
    explicit StorageClientImpl(WebViewImpl*);

    PassOwnPtr<StorageNamespace> createSessionStorageNamespace() override;
    bool canAccessStorage(LocalFrame*, StorageType) const override;

private:
    WebViewImpl* m_webView;
};

} // namespace blink

#endif // StorageClientImpl_h
