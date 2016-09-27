// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebContentDecryptionModuleAccess_h
#define WebContentDecryptionModuleAccess_h

#include "WebCommon.h"

namespace blink {

class WebContentDecryptionModuleResult;
struct WebMediaKeySystemConfiguration;

class BLINK_PLATFORM_EXPORT WebContentDecryptionModuleAccess {
public:
    virtual ~WebContentDecryptionModuleAccess();
    virtual void createContentDecryptionModule(WebContentDecryptionModuleResult) = 0;
    virtual WebMediaKeySystemConfiguration getConfiguration() = 0;
};

} // namespace blink

#endif // WebContentDecryptionModuleAccess_h
