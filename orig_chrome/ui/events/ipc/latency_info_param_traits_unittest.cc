// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <stdint.h>

#include "ipc/ipc_message_macros.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/ipc/latency_info_param_traits.h"
#include "ui/events/ipc/latency_info_param_traits_macros.h"

namespace ui {

TEST(LatencyInfoParamTraitsTest, Basic)
{
    LatencyInfo latency;
    ASSERT_FALSE(latency.terminated());
    ASSERT_EQ(0u, latency.input_coordinates_size());
    ASSERT_EQ(0u, latency.coalesced_events_size());
    latency.AddLatencyNumber(INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT, 1234, 0);
    latency.AddLatencyNumber(INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT, 1234, 100);
    latency.AddLatencyNumber(INPUT_EVENT_LATENCY_TERMINATED_FRAME_SWAP_COMPONENT,
        1234, 0);
    EXPECT_TRUE(latency.AddInputCoordinate(
        LatencyInfo::InputCoordinate(100, 200)));
    EXPECT_TRUE(latency.AddInputCoordinate(
        LatencyInfo::InputCoordinate(101, 201)));
    // Up to 2 InputCoordinate is allowed.
    EXPECT_FALSE(latency.AddInputCoordinate(
        LatencyInfo::InputCoordinate(102, 202)));

    EXPECT_TRUE(latency.AddCoalescedEventTimestamp(10.0));
    EXPECT_TRUE(latency.AddCoalescedEventTimestamp(20.0));
    // Up to 2 coalesced events is allowed.
    EXPECT_FALSE(latency.AddCoalescedEventTimestamp(30.0));

    EXPECT_EQ(100, latency.trace_id());
    EXPECT_TRUE(latency.terminated());
    EXPECT_EQ(2u, latency.input_coordinates_size());
    EXPECT_EQ(2u, latency.coalesced_events_size());

    IPC::Message msg(1, 2, IPC::Message::PRIORITY_NORMAL);
    IPC::WriteParam(&msg, latency);
    base::PickleIterator iter(msg);
    LatencyInfo output;
    EXPECT_TRUE(IPC::ReadParam(&msg, &iter, &output));

    EXPECT_EQ(latency.trace_id(), output.trace_id());
    EXPECT_EQ(latency.terminated(), output.terminated());
    EXPECT_EQ(latency.input_coordinates_size(), output.input_coordinates_size());
    for (size_t i = 0; i < latency.input_coordinates_size(); i++) {
        EXPECT_EQ(latency.input_coordinates()[i].x,
            output.input_coordinates()[i].x);
        EXPECT_EQ(latency.input_coordinates()[i].y,
            output.input_coordinates()[i].y);
    }
    EXPECT_EQ(latency.coalesced_events_size(), output.coalesced_events_size());
    for (size_t i = 0; i < latency.coalesced_events_size(); i++) {
        EXPECT_EQ(latency.timestamps_of_coalesced_events()[i],
            output.timestamps_of_coalesced_events()[i]);
    }

    EXPECT_TRUE(output.FindLatency(INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT,
        1234,
        nullptr));

    LatencyInfo::LatencyComponent rwh_comp;
    EXPECT_TRUE(output.FindLatency(INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT,
        1234,
        &rwh_comp));
    EXPECT_EQ(100, rwh_comp.sequence_number);
    EXPECT_EQ(1u, rwh_comp.event_count);

    EXPECT_TRUE(output.FindLatency(
        INPUT_EVENT_LATENCY_TERMINATED_FRAME_SWAP_COMPONENT, 1234, nullptr));
}

TEST(LatencyInfoParamTraitsTest, InvalidData)
{
    IPC::Message msg(1, 2, IPC::Message::PRIORITY_NORMAL);
    IPC::WriteParam(&msg, std::string());
    ui::LatencyInfo::LatencyMap components;
    IPC::WriteParam(&msg, components);
    IPC::WriteParam(&msg, static_cast<uint32_t>(2));
    IPC::WriteParam(&msg, ui::LatencyInfo::InputCoordinate());
    IPC::WriteParam(&msg, ui::LatencyInfo::InputCoordinate());
    // coalesced_events_size is 2 but only one event timestamp is written.
    IPC::WriteParam(&msg, static_cast<uint32_t>(2));
    IPC::WriteParam(&msg, static_cast<double>(10.0));
    IPC::WriteParam(&msg, static_cast<int64_t>(1234));
    IPC::WriteParam(&msg, true);

    base::PickleIterator iter(msg);
    LatencyInfo output;
    EXPECT_FALSE(IPC::ReadParam(&msg, &iter, &output));
}

} // namespace ui
