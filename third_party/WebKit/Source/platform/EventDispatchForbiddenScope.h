// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventDispatchForbiddenScope_h
#define EventDispatchForbiddenScope_h

#include "platform/PlatformExport.h"
#include "wtf/Assertions.h"
#include "wtf/MainThread.h"
#include "wtf/TemporaryChange.h"

namespace blink {

#if ENABLE(ASSERT)

class EventDispatchForbiddenScope {
public:
    EventDispatchForbiddenScope()
    {
        ASSERT(isMainThread());
        ++s_count;
    }

    ~EventDispatchForbiddenScope()
    {
        ASSERT(isMainThread());
        ASSERT(s_count);
        --s_count;
    }

    static bool isEventDispatchForbidden()
    {
        if (!isMainThread())
            return false;
        return s_count;
    }

    class AllowUserAgentEvents {
    public:
        AllowUserAgentEvents()
            : m_change(s_count, 0)
        {
            ASSERT(isMainThread());
        }

        ~AllowUserAgentEvents()
        {
            ASSERT(!s_count);
        }

        TemporaryChange<unsigned> m_change;
    };

private:
    PLATFORM_EXPORT static unsigned s_count;
};

#else

class EventDispatchForbiddenScope {
public:
    EventDispatchForbiddenScope() { }

    class AllowUserAgentEvents {
    public:
        AllowUserAgentEvents() { }
    };
};

#endif // ENABLE(ASSERT)

} // namespace blink

#endif // EventDispatchForbiddenScope_h
