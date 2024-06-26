// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/unique_notifier.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/location.h"
#include "base/sequenced_task_runner.h"

namespace cc {

UniqueNotifier::UniqueNotifier(base::SequencedTaskRunner* task_runner,
    const base::Closure& closure)
    : task_runner_(task_runner)
    , closure_(closure)
    , notification_pending_(false)
    , weak_ptr_factory_(this)
{
}

UniqueNotifier::~UniqueNotifier()
{
}

void UniqueNotifier::Cancel()
{
    notification_pending_ = false;
}

void UniqueNotifier::Schedule()
{
    if (notification_pending_)
        return;

    task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&UniqueNotifier::Notify, weak_ptr_factory_.GetWeakPtr()));
    notification_pending_ = true;
}

void UniqueNotifier::Notify()
{
    if (!notification_pending_)
        return;

    // Note that the order here is important in case closure schedules another
    // run.
    notification_pending_ = false;
    closure_.Run();
}

} // namespace cc
