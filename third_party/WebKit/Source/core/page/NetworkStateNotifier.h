/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NetworkStateNotifier_h
#define NetworkStateNotifier_h

#include "core/CoreExport.h"
#include "public/platform/WebConnectionType.h"
#include "wtf/FastAllocBase.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/ThreadingPrimitives.h"
#include "wtf/Vector.h"

namespace blink {

class ExecutionContext;

class CORE_EXPORT NetworkStateNotifier {
    WTF_MAKE_NONCOPYABLE(NetworkStateNotifier); WTF_MAKE_FAST_ALLOCATED(NetworkStateNotifier);
public:
    class NetworkStateObserver {
    public:
        // Will be called on the thread of the context passed in addObserver.
        virtual void connectionTypeChange(WebConnectionType) = 0;
    };

    NetworkStateNotifier()
        : m_isOnLine(true)
        , m_type(ConnectionTypeOther)
        , m_testUpdatesOnly(false)
    {
    }

    bool onLine() const
    {
        MutexLocker locker(m_mutex);
        return m_isOnLine;
    }

    void setOnLine(bool);

    WebConnectionType connectionType() const
    {
        MutexLocker locker(m_mutex);
        return m_type;
    }

    void setWebConnectionType(WebConnectionType);

    // Must be called on the context's thread. An added observer must be removed
    // before its ExecutionContext is deleted. It's possible for an observer to
    // be called twice for the same event if it is first removed and then added
    // during notification.
    void addObserver(NetworkStateObserver*, ExecutionContext*);
    void removeObserver(NetworkStateObserver*, ExecutionContext*);

    // The following functions are for testing purposes.

    // When true, setWebConnectionType calls are ignored and only setWebConnectionTypeForTest
    // can update the connection type. This is used for layout tests (see crbug.com/377736).
    void setTestUpdatesOnly(bool);
    // Tests should call this as it will change the type regardless of the value of m_testUpdatesOnly.
    void setWebConnectionTypeForTest(WebConnectionType);

private:
    struct ObserverList {
        ObserverList()
            : iterating(false)
        {
        }
        bool iterating;
        Vector<NetworkStateObserver*> observers;
        Vector<size_t> zeroedObservers; // Indices in observers that are 0.
    };

    void setWebConnectionTypeImpl(WebConnectionType);

    using ObserverListMap = HashMap<ExecutionContext*, OwnPtr<ObserverList>>;

    void notifyObserversOnContext(WebConnectionType, ExecutionContext*);

    ObserverList* lockAndFindObserverList(ExecutionContext*);

    // Removed observers are nulled out in the list in case the list is being
    // iterated over. Once done iterating, call this to clean up nulled
    // observers.
    void collectZeroedObservers(ObserverList*, ExecutionContext*);

    mutable Mutex m_mutex;
    bool m_isOnLine;
    WebConnectionType m_type;
    ObserverListMap m_observers;
    bool m_testUpdatesOnly;
};

CORE_EXPORT NetworkStateNotifier& networkStateNotifier();

} // namespace blink

#endif // NetworkStateNotifier_h
