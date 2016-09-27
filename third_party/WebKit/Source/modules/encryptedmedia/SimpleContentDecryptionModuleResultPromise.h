// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SimpleContentDecryptionModuleResultPromise_h
#define SimpleContentDecryptionModuleResultPromise_h

#include "modules/encryptedmedia/ContentDecryptionModuleResultPromise.h"

namespace blink {

// This class creates a simple ContentDecryptionModuleResultPromise where the
// implementation of complete() will resolve the promise with void. All other
// complete() methods are not expected to be called (and will reject the
// promise).
class SimpleContentDecryptionModuleResultPromise : public ContentDecryptionModuleResultPromise {
public:
    explicit SimpleContentDecryptionModuleResultPromise(ScriptState*);
    ~SimpleContentDecryptionModuleResultPromise() override;

    // ContentDecryptionModuleResultPromise implementation.
    void complete() override;
};

} // namespace blink

#endif // SimpleContentDecryptionModuleResultPromise_h
