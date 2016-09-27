// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MediaKeysClient_h
#define MediaKeysClient_h

namespace blink {

class ExecutionContext;
class WebEncryptedMediaClient;

class MediaKeysClient {
public:
    virtual WebEncryptedMediaClient* encryptedMediaClient(ExecutionContext*) = 0;

protected:
    virtual ~MediaKeysClient() { }
};

} // namespace blink

#endif // MediaKeysClient_h
