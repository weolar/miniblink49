// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PluginPlaceholder_h
#define PluginPlaceholder_h

#include "platform/heap/Handle.h"

namespace blink {

class ContainerNode;

class PluginPlaceholder : public WillBeGarbageCollectedMixin {
public:
#if !ENABLE(OILPAN)
    virtual ~PluginPlaceholder() { }
#endif

    // Loads the plugin placeholder into a container, such as a shadow root.
    // The placeholder may replace/remove any existing children.
    virtual void loadIntoContainer(ContainerNode&) = 0;
};

} // namespace blink

#endif // PluginPlaceholder_h
