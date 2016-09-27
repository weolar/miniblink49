// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DisplayItemClient_h
#define DisplayItemClient_h

#include "platform/PlatformExport.h"
#include "platform/heap/Heap.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DisplayItemClientInternalVoid;
using DisplayItemClient = const DisplayItemClientInternalVoid*;

inline DisplayItemClient toDisplayItemClient(const void* object) { return static_cast<DisplayItemClient>(object); }

// Used to pass DisplayItemClient and debugName() (called only when needed) from
// core/layout module etc. to platform/paint module.
// The instance must not out-live the object. Long-time reference to a client must
// use DisplayItemClient.
class PLATFORM_EXPORT DisplayItemClientWrapper {
    DISALLOW_ALLOCATION(); // Allow allocated in stack or in another object only.
public:
    template <typename T>
    DisplayItemClientWrapper(const T& object)
        : m_displayItemClient(object.displayItemClient())
        , m_object(reinterpret_cast<const GenericClass&>(object))
        , m_debugNameInvoker(&invokeDebugName<T>)
    { }

    DisplayItemClientWrapper(const DisplayItemClientWrapper& other)
        : m_displayItemClient(other.m_displayItemClient)
        , m_object(other.m_object)
        , m_debugNameInvoker(other.m_debugNameInvoker)
    { }

    DisplayItemClient displayItemClient() const { return m_displayItemClient; }
    String debugName() const { return m_debugNameInvoker(m_object); }

private:
    DisplayItemClientWrapper& operator=(const DisplayItemClientWrapper&) = delete;

    class GenericClass;
    template <typename T>
    static String invokeDebugName(const GenericClass& object) { return reinterpret_cast<const T&>(object).debugName(); }

    DisplayItemClient m_displayItemClient;
    const GenericClass& m_object;
    using DebugNameInvoker = String(*)(const GenericClass&);
    DebugNameInvoker m_debugNameInvoker;
};

}

#endif // DisplayItemClient_h
