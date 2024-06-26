// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/latency_info_swap_promise_monitor.h"

#include "base/threading/platform_thread.h"
#include "cc/output/latency_info_swap_promise.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"

namespace {

bool AddRenderingScheduledComponent(ui::LatencyInfo* latency_info,
    bool on_main)
{
    ui::LatencyComponentType type = on_main ? ui::INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_MAIN_COMPONENT
                                            : ui::INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_IMPL_COMPONENT;
    if (latency_info->FindLatency(type, 0, nullptr))
        return false;
    latency_info->AddLatencyNumber(type, 0, 0);
    return true;
}

bool AddForwardingScrollUpdateToMainComponent(ui::LatencyInfo* latency_info)
{
    if (latency_info->FindLatency(
            ui::INPUT_EVENT_LATENCY_FORWARD_SCROLL_UPDATE_TO_MAIN_COMPONENT, 0,
            nullptr))
        return false;
    latency_info->AddLatencyNumber(
        ui::INPUT_EVENT_LATENCY_FORWARD_SCROLL_UPDATE_TO_MAIN_COMPONENT, 0,
        latency_info->trace_id());
    return true;
}

} // namespace

namespace cc {

LatencyInfoSwapPromiseMonitor::LatencyInfoSwapPromiseMonitor(
    ui::LatencyInfo* latency,
    LayerTreeHost* layer_tree_host,
    LayerTreeHostImpl* layer_tree_host_impl)
    : SwapPromiseMonitor(layer_tree_host, layer_tree_host_impl)
    , latency_(latency)
{
}

LatencyInfoSwapPromiseMonitor::~LatencyInfoSwapPromiseMonitor()
{
}

void LatencyInfoSwapPromiseMonitor::OnSetNeedsCommitOnMain()
{
    if (AddRenderingScheduledComponent(latency_, true /* on_main */)) {
        scoped_ptr<SwapPromise> swap_promise(new LatencyInfoSwapPromise(*latency_));
        layer_tree_host_->QueueSwapPromise(swap_promise.Pass());
    }
}

void LatencyInfoSwapPromiseMonitor::OnSetNeedsRedrawOnImpl()
{
    if (AddRenderingScheduledComponent(latency_, false /* on_main */)) {
        scoped_ptr<SwapPromise> swap_promise(new LatencyInfoSwapPromise(*latency_));
        // Queue a pinned swap promise on the active tree. This will allow
        // measurement of the time to the next SwapBuffers(). The swap
        // promise is pinned so that it is not interrupted by new incoming
        // activations (which would otherwise break the swap promise).
        layer_tree_host_impl_->active_tree()->QueuePinnedSwapPromise(
            swap_promise.Pass());
    }
}

void LatencyInfoSwapPromiseMonitor::OnForwardScrollUpdateToMainThreadOnImpl()
{
    if (AddForwardingScrollUpdateToMainComponent(latency_)) {
        DebugBreak();

        //     int64 new_sequence_number = 0;
        //     for (ui::LatencyInfo::LatencyMap::const_iterator it =
        //              latency_->latency_components().begin();
        //          it != latency_->latency_components().end(); ++it) {
        //       if (it->first.first == ui::INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT) {
        //         new_sequence_number =
        //             ((static_cast<int64>(base::PlatformThread::CurrentId()) << 32) ^
        //              (reinterpret_cast<uint64>(this) << 32)) |
        //             (it->second.sequence_number & 0xffffffff);
        //         if (new_sequence_number == it->second.sequence_number)
        //           return;
        //         break;
        //       }
        //     }
        //     if (!new_sequence_number)
        //       return;
        //     ui::LatencyInfo new_latency;
        //     new_latency.AddLatencyNumberWithTraceName(
        //         ui::LATENCY_BEGIN_SCROLL_LISTENER_UPDATE_MAIN_COMPONENT, 0,
        //         new_sequence_number, "ScrollUpdate");
        //     new_latency.CopyLatencyFrom(
        //         *latency_,
        //         ui::INPUT_EVENT_LATENCY_FORWARD_SCROLL_UPDATE_TO_MAIN_COMPONENT);
        //     scoped_ptr<SwapPromise> swap_promise(
        //         new LatencyInfoSwapPromise(new_latency));
        //     layer_tree_host_impl_->QueueSwapPromiseForMainThreadScrollUpdate(
        //         swap_promise.Pass());
    }
}

} // namespace cc
