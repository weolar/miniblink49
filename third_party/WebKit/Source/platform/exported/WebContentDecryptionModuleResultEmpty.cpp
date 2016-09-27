// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebContentDecryptionModuleResult.h"

#include "platform/ContentDecryptionModuleResult.h"

namespace blink {

void WebContentDecryptionModuleResult::completeWithError(WebContentDecryptionModuleException exception, unsigned long systemCode, const WebString& errorMessage)
{
}

void WebContentDecryptionModuleResult::reset()
{
}

} // blink