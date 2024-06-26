// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/object_watcher.h"

#include "base/bind.h"
#include "base/logging.h"

namespace base {
namespace win {

    //-----------------------------------------------------------------------------

    ObjectWatcher::ObjectWatcher()
        : object_(NULL)
        , wait_object_(NULL)
        , origin_loop_(NULL)
        , run_once_(true)
        , weak_factory_(this)
    {
    }

    ObjectWatcher::~ObjectWatcher()
    {
        StopWatching();
    }

    bool ObjectWatcher::StartWatchingOnce(HANDLE object, Delegate* delegate)
    {
        return StartWatchingInternal(object, delegate, true);
    }

    bool ObjectWatcher::StartWatchingMultipleTimes(HANDLE object,
        Delegate* delegate)
    {
        return StartWatchingInternal(object, delegate, false);
    }

    bool ObjectWatcher::StopWatching()
    {
        //   if (!wait_object_)
        //     return false;
        //
        //   // Make sure ObjectWatcher is used in a single-threaded fashion.
        //   DCHECK_EQ(origin_loop_, MessageLoop::current());
        //
        //   // Blocking call to cancel the wait. Any callbacks already in progress will
        //   // finish before we return from this call.
        //   if (!UnregisterWaitEx(wait_object_, INVALID_HANDLE_VALUE)) {
        //     DPLOG(FATAL) << "UnregisterWaitEx failed";
        //     return false;
        //   }
        //
        //   weak_factory_.InvalidateWeakPtrs();
        //   object_ = NULL;
        //   wait_object_ = NULL;
        //
        //   MessageLoop::current()->RemoveDestructionObserver(this);
        //   return true;
        DebugBreak();
        return false;
    }

    bool ObjectWatcher::IsWatching() const
    {
        return object_ != NULL;
    }

    HANDLE ObjectWatcher::GetWatchedObject() const
    {
        return object_;
    }

    // static
    void CALLBACK ObjectWatcher::DoneWaiting(void* param, BOOLEAN timed_out)
    {
        DCHECK(!timed_out);

        // The destructor blocks on any callbacks that are in flight, so we know that
        // that is always a pointer to a valid ObjectWater.
        ObjectWatcher* that = static_cast<ObjectWatcher*>(param);
        that->origin_loop_->task_runner()->PostTask(FROM_HERE, that->callback_);
        if (that->run_once_)
            that->callback_.Reset();
    }

    bool ObjectWatcher::StartWatchingInternal(HANDLE object, Delegate* delegate,
        bool execute_only_once)
    {
        //   CHECK(delegate);
        //   if (wait_object_) {
        //     NOTREACHED() << "Already watching an object";
        //     return false;
        //   }
        //   run_once_ = execute_only_once;
        //
        //   // Since our job is to just notice when an object is signaled and report the
        //   // result back to this thread, we can just run on a Windows wait thread.
        //   DWORD wait_flags = WT_EXECUTEINWAITTHREAD;
        //   if (run_once_)
        //     wait_flags |= WT_EXECUTEONLYONCE;
        //
        //   // DoneWaiting can be synchronously called from RegisterWaitForSingleObject,
        //   // so set up all state now.
        //   callback_ = base::Bind(&ObjectWatcher::Signal, weak_factory_.GetWeakPtr(),
        //                          delegate);
        //   object_ = object;
        //   origin_loop_ = MessageLoop::current();
        //
        //   if (!RegisterWaitForSingleObject(&wait_object_, object, DoneWaiting,
        //                                    this, INFINITE, wait_flags)) {
        //     DPLOG(FATAL) << "RegisterWaitForSingleObject failed";
        //     object_ = NULL;
        //     wait_object_ = NULL;
        //     return false;
        //   }
        //
        //   // We need to know if the current message loop is going away so we can
        //   // prevent the wait thread from trying to access a dead message loop.
        //   MessageLoop::current()->AddDestructionObserver(this);
        //   return true;
        DebugBreak();
        return false;
    }

    void ObjectWatcher::Signal(Delegate* delegate)
    {
        // Signaling the delegate may result in our destruction or a nested call to
        // StartWatching(). As a result, we save any state we need and clear previous
        // watcher state before signaling the delegate.
        HANDLE object = object_;
        if (run_once_)
            StopWatching();
        delegate->OnObjectSignaled(object);
    }

    void ObjectWatcher::WillDestroyCurrentMessageLoop()
    {
        // Need to shutdown the watch so that we don't try to access the MessageLoop
        // after this point.
        StopWatching();
    }

} // namespace win
} // namespace base
