// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventModulesFactory_h
#define EventModulesFactory_h

#include "core/events/EventFactory.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class Event;

class EventModulesFactory final : public EventFactoryBase {
public:
    static PassOwnPtr<EventModulesFactory> create()
    {
        return adoptPtr(new EventModulesFactory());
    }

    PassRefPtrWillBeRawPtr<Event> create(const String& eventType) override;
};

}

#endif
