// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/output/latency_info_swap_promise.h"

#include "base/logging.h"
#include "base/trace_event/trace_event.h"

namespace {
ui::LatencyComponentType DidNotSwapReasonToLatencyComponentType(
    cc::SwapPromise::DidNotSwapReason reason)
{
    switch (reason) {
    case cc::SwapPromise::ACTIVATION_FAILS:
    case cc::SwapPromise::SWAP_FAILS:
        return ui::INPUT_EVENT_LATENCY_TERMINATED_SWAP_FAILED_COMPONENT;
    case cc::SwapPromise::COMMIT_FAILS:
        return ui::INPUT_EVENT_LATENCY_TERMINATED_COMMIT_FAILED_COMPONENT;
    case cc::SwapPromise::COMMIT_NO_UPDATE:
        return ui::INPUT_EVENT_LATENCY_TERMINATED_COMMIT_NO_UPDATE_COMPONENT;
    }
    NOTREACHED() << "Unhandled DidNotSwapReason.";
    return ui::INPUT_EVENT_LATENCY_TERMINATED_SWAP_FAILED_COMPONENT;
}
} // namespace

namespace cc {

LatencyInfoSwapPromise::LatencyInfoSwapPromise(const ui::LatencyInfo& latency)
    : latency_(latency)
{
}

LatencyInfoSwapPromise::~LatencyInfoSwapPromise()
{
}

void LatencyInfoSwapPromise::DidSwap(CompositorFrameMetadata* metadata)
{
    DCHECK(!latency_.terminated());
    metadata->latency_info.push_back(latency_);
}

void LatencyInfoSwapPromise::DidNotSwap(DidNotSwapReason reason)
{
    latency_.AddLatencyNumber(DidNotSwapReasonToLatencyComponentType(reason), 0,
        0);
    // TODO(miletus): Turn this back on once per-event LatencyInfo tracking
    // is enabled in GPU side.
    // DCHECK(latency_.terminated);
}

int64 LatencyInfoSwapPromise::TraceId() const
{
    return latency_.trace_id();
}

// Trace the original LatencyInfo of a LatencyInfoSwapPromise
void LatencyInfoSwapPromise::OnCommit()
{
    TRACE_EVENT_WITH_FLOW1("input,benchmark",
        "LatencyInfo.Flow",
        TRACE_ID_DONT_MANGLE(TraceId()),
        TRACE_EVENT_FLAG_FLOW_IN | TRACE_EVENT_FLAG_FLOW_OUT,
        "step", "HandleInputEventMainCommit");
}

} // namespace cc
