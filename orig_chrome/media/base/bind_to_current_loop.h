// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_BIND_TO_CURRENT_LOOP_H_
#define MEDIA_BASE_BIND_TO_CURRENT_LOOP_H_

#include "base/bind.h"
#include "base/location.h"
#include "base/memory/scoped_vector.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"

// This is a helper utility for base::Bind()ing callbacks to the current
// MessageLoop. The typical use is when |a| (of class |A|) wants to hand a
// callback such as base::Bind(&A::AMethod, a) to |b|, but needs to ensure that
// when |b| executes the callback, it does so on |a|'s current MessageLoop.
//
// Typical usage: request to be called back on the current thread:
// other->StartAsyncProcessAndCallMeBack(
//    media::BindToCurrentLoop(base::Bind(&MyClass::MyMethod, this)));
//
// Note that like base::Bind(), BindToCurrentLoop() can't bind non-constant
// references, and that *unlike* base::Bind(), BindToCurrentLoop() makes copies
// of its arguments, and thus can't be used with arrays.

namespace media {

// Mimic base::internal::CallbackForward, replacing p.Pass() with
// base::Passed(&p) to account for the extra layer of indirection.
namespace internal {
    template <typename T>
    T& TrampolineForward(T& t) { return t; }

    template <typename T, typename R>
    base::internal::PassedWrapper<scoped_ptr<T, R>> TrampolineForward(
        scoped_ptr<T, R>& p) { return base::Passed(&p); }

    template <typename T>
    base::internal::PassedWrapper<ScopedVector<T>> TrampolineForward(
        ScopedVector<T>& p) { return base::Passed(&p); }

    // First, tell the compiler TrampolineHelper is a struct template with one
    // type parameter.  Then define specializations where the type is a function
    // returning void and taking zero or more arguments.
    template <typename Sig>
    struct TrampolineHelper;

    template <typename... Args>
    struct TrampolineHelper<void(Args...)> {
        static void Run(
            const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
            const base::Callback<void(Args...)>& cb,
            Args... args)
        {
            task_runner->PostTask(FROM_HERE,
                base::Bind(cb, TrampolineForward(args)...));
        }
    };

} // namespace internal

template <typename T>
static base::Callback<T> BindToCurrentLoop(
    const base::Callback<T>& cb)
{
    return base::Bind(&internal::TrampolineHelper<T>::Run,
        base::ThreadTaskRunnerHandle::Get(), cb);
}

} // namespace media

#endif // MEDIA_BASE_BIND_TO_CURRENT_LOOP_H_
