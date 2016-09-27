// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/encryptedmedia/SimpleContentDecryptionModuleResultPromise.h"

namespace blink {

SimpleContentDecryptionModuleResultPromise::SimpleContentDecryptionModuleResultPromise(ScriptState* scriptState)
    : ContentDecryptionModuleResultPromise(scriptState)
{
}

SimpleContentDecryptionModuleResultPromise::~SimpleContentDecryptionModuleResultPromise()
{
}

void SimpleContentDecryptionModuleResultPromise::complete()
{
    resolve();
}

} // namespace blink
