// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/memory_pressure_listener.h"

#include "base/lazy_instance.h"
#include "base/observer_list_threadsafe.h"
#include "base/trace_event/trace_event.h"

namespace base {

namespace {

    // ObserverListThreadSafe is RefCountedThreadSafe, this traits is needed
    // to ensure the LazyInstance will hold a reference to it.
    struct LeakyLazyObserverListTraits : base::internal::LeakyLazyInstanceTraits<
                                             ObserverListThreadSafe<MemoryPressureListener>> {
        static ObserverListThreadSafe<MemoryPressureListener>*
        New(void* instance)
        {
            ObserverListThreadSafe<MemoryPressureListener>* ret = base::internal::LeakyLazyInstanceTraits<
                ObserverListThreadSafe<MemoryPressureListener>>::New(instance);
            // Leaky.
            ret->AddRef();
            return ret;
        }
    };

    LazyInstance<
        ObserverListThreadSafe<MemoryPressureListener>,
        LeakyLazyObserverListTraits>
        g_observers = LAZY_INSTANCE_INITIALIZER;

    // All memory pressure notifications within this process will be suppressed if
    // this variable is set to 1.
    subtle::Atomic32 g_notifications_suppressed = 0;

} // namespace

MemoryPressureListener::MemoryPressureListener(
    const MemoryPressureListener::MemoryPressureCallback& callback)
    : callback_(callback)
{
    g_observers.Get().AddObserver(this);
}

MemoryPressureListener::~MemoryPressureListener()
{
    g_observers.Get().RemoveObserver(this);
}

void MemoryPressureListener::Notify(MemoryPressureLevel memory_pressure_level)
{
    callback_.Run(memory_pressure_level);
}

// static
void MemoryPressureListener::NotifyMemoryPressure(
    MemoryPressureLevel memory_pressure_level)
{
    DCHECK_NE(memory_pressure_level, MEMORY_PRESSURE_LEVEL_NONE);
    TRACE_EVENT1("memory", "MemoryPressureListener::NotifyMemoryPressure",
        "level", memory_pressure_level);
    if (AreNotificationsSuppressed())
        return;
    DoNotifyMemoryPressure(memory_pressure_level);
}

// static
bool MemoryPressureListener::AreNotificationsSuppressed()
{
    return subtle::Acquire_Load(&g_notifications_suppressed) == 1;
}

// static
void MemoryPressureListener::SetNotificationsSuppressed(bool suppress)
{
    subtle::Release_Store(&g_notifications_suppressed, suppress ? 1 : 0);
}

// static
void MemoryPressureListener::SimulatePressureNotification(
    MemoryPressureLevel memory_pressure_level)
{
    // Notify all listeners even if regular pressure notifications are suppressed.
    DoNotifyMemoryPressure(memory_pressure_level);
}

// static
void MemoryPressureListener::DoNotifyMemoryPressure(
    MemoryPressureLevel memory_pressure_level)
{
    DCHECK_NE(memory_pressure_level, MEMORY_PRESSURE_LEVEL_NONE);
    g_observers.Get().Notify(FROM_HERE, &MemoryPressureListener::Notify,
        memory_pressure_level);
}

} // namespace base
