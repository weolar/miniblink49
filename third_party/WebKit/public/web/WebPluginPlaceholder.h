// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebPluginPlaceholder_h
#define WebPluginPlaceholder_h

#include "public/platform/WebString.h"

namespace blink {

// Provides content and behavior for a plugin placeholder rendered by Blink.
class WebPluginPlaceholder {
public:
    virtual ~WebPluginPlaceholder() { }

    // Returns a plaintext message to be displayed.
    virtual WebString message() const { return WebString(); }

    // If set, the placeholder will present UI to permit dismissing it.
    virtual bool isCloseable() const { return false; }
};

} // namespace blink

#endif // WebPluginPlaceholder_h
