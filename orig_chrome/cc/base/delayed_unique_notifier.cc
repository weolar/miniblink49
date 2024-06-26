// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/delayed_unique_notifier.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/location.h"
#include "base/sequenced_task_runner.h"

namespace cc {

DelayedUniqueNotifier::DelayedUniqueNotifier(
    base::SequencedTaskRunner* task_runner,
    const base::Closure& closure,
    const base::TimeDelta& delay)
    : task_runner_(task_runner)
    , closure_(closure)
    , delay_(delay)
    , notification_pending_(false)
    , weak_ptr_factory_(this)
{
}

DelayedUniqueNotifier::~DelayedUniqueNotifier()
{
}

void DelayedUniqueNotifier::Schedule()
{
    if (notification_pending_) {
        next_notification_time_ = Now() + delay_;
        return;
    }

    next_notification_time_ = Now() + delay_;
    task_runner_->PostDelayedTask(FROM_HERE,
        base::Bind(&DelayedUniqueNotifier::NotifyIfTime,
            weak_ptr_factory_.GetWeakPtr()),
        delay_);
    notification_pending_ = true;
}

void DelayedUniqueNotifier::Cancel()
{
    next_notification_time_ = base::TimeTicks();
}

void DelayedUniqueNotifier::Shutdown()
{
    // This function must destroy any weak ptrs since after being cancelled, this
    // class may be destroyed on another thread during compositor shutdown.
    weak_ptr_factory_.InvalidateWeakPtrs();
    // Deliberately leaves notification_pending_ = true forever so new tasks with
    // weak ptrs can not be created.
    notification_pending_ = true;
}

bool DelayedUniqueNotifier::HasPendingNotification() const
{
    return notification_pending_ && !next_notification_time_.is_null();
}

base::TimeTicks DelayedUniqueNotifier::Now() const
{
    return base::TimeTicks::Now();
}

void DelayedUniqueNotifier::NotifyIfTime()
{
    // If next notifiaction time is not valid, then this schedule was canceled.
    if (next_notification_time_.is_null()) {
        notification_pending_ = false;
        return;
    }

    // If the notification was rescheduled or arrived too early for any other
    // reason, then post another task instead of running the callback.
    base::TimeTicks now = Now();
    if (next_notification_time_ > now) {
        task_runner_->PostDelayedTask(
            FROM_HERE,
            base::Bind(&DelayedUniqueNotifier::NotifyIfTime,
                weak_ptr_factory_.GetWeakPtr()),
            next_notification_time_ - now);
        return;
    }

    // Note the order here is important since closure might schedule another run.
    notification_pending_ = false;
    closure_.Run();
}

} // namespace cc
