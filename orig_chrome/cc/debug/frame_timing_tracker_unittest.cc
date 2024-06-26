// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>
#include <string>

#include "base/time/time.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/debug/frame_timing_tracker.h"
#include "cc/test/fake_impl_proxy.h"
#include "cc/test/fake_layer_tree_host_impl.h"
#include "cc/test/test_shared_bitmap_manager.h"
#include "cc/test/test_task_graph_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    std::string CompositeToString(
        scoped_ptr<FrameTimingTracker::CompositeTimingSet> timingset)
    {
        scoped_refptr<base::trace_event::TracedValue> value = new base::trace_event::TracedValue();
        value->BeginArray("values");
        std::set<int> rect_ids;
        for (const auto& pair : *timingset)
            rect_ids.insert(pair.first);

        for (const auto& rect_id : rect_ids) {
            auto& events = (*timingset)[rect_id];
            value->BeginDictionary();
            value->SetInteger("rect_id", rect_id);
            value->BeginArray("events");
            for (const auto& event : events) {
                value->BeginDictionary();
                value->SetInteger("frame_id", event.frame_id);
                value->SetInteger("timestamp", event.timestamp.ToInternalValue());
                value->EndDictionary();
            }
            value->EndArray();
            value->EndDictionary();
        }
        value->EndArray();
        return value->ToString();
    }

    std::string MainFrameToString(
        scoped_ptr<FrameTimingTracker::MainFrameTimingSet> timingset)
    {
        scoped_refptr<base::trace_event::TracedValue> value = new base::trace_event::TracedValue();
        value->BeginArray("values");
        std::set<int> rect_ids;
        for (const auto& pair : *timingset)
            rect_ids.insert(pair.first);

        for (const auto& rect_id : rect_ids) {
            auto& events = (*timingset)[rect_id];
            value->BeginDictionary();
            value->SetInteger("rect_id", rect_id);
            value->BeginArray("events");
            for (const auto& event : events) {
                value->BeginDictionary();
                value->SetInteger("end_time", event.end_time.ToInternalValue());
                value->SetInteger("frame_id", event.frame_id);
                value->SetInteger("timestamp", event.timestamp.ToInternalValue());
                value->EndDictionary();
            }
            value->EndArray();
            value->EndDictionary();
        }
        value->EndArray();
        return value->ToString();
    }

    TEST(FrameTimingTrackerTest, DefaultTrackerIsEmpty)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        EXPECT_EQ("{\"values\":[]}",
            CompositeToString(tracker->GroupCompositeCountsByRectId()));
        EXPECT_EQ("{\"values\":[]}",
            MainFrameToString(tracker->GroupMainFrameCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, NoFrameIdsIsEmpty)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        std::vector<std::pair<int, int64_t>> ids;
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(100), ids);
        EXPECT_EQ("{\"values\":[]}",
            CompositeToString(tracker->GroupCompositeCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, NoRectIdsYieldsNoMainFrameEvents)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        tracker->SaveMainFrameTimeStamps(std::vector<int64_t>(),
            base::TimeTicks::FromInternalValue(100),
            base::TimeTicks::FromInternalValue(110), 1);
        EXPECT_EQ("{\"values\":[]}",
            MainFrameToString(tracker->GroupMainFrameCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, OneFrameId)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        std::vector<std::pair<int, int64_t>> ids;
        ids.push_back(std::make_pair(1, 2));
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(100), ids);
        EXPECT_EQ(
            "{\"values\":[{\"events\":["
            "{\"frame_id\":1,\"timestamp\":100}],\"rect_id\":2}]}",
            CompositeToString(tracker->GroupCompositeCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, OneMainFrameRect)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        std::vector<int64_t> rect_ids;
        rect_ids.push_back(1);
        tracker->SaveMainFrameTimeStamps(rect_ids,
            base::TimeTicks::FromInternalValue(100),
            base::TimeTicks::FromInternalValue(110), 2);
        EXPECT_EQ(
            "{\"values\":[{\"events\":["
            "{\"end_time\":110,\"frame_id\":2,\"timestamp\":100}],\"rect_id\":1}]}",
            MainFrameToString(tracker->GroupMainFrameCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, UnsortedTimestampsIds)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        std::vector<std::pair<int, int64_t>> ids;
        ids.push_back(std::make_pair(1, 2));
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(200), ids);
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(400), ids);
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(100), ids);
        EXPECT_EQ(
            "{\"values\":[{\"events\":["
            "{\"frame_id\":1,\"timestamp\":100},"
            "{\"frame_id\":1,\"timestamp\":200},"
            "{\"frame_id\":1,\"timestamp\":400}],\"rect_id\":2}]}",
            CompositeToString(tracker->GroupCompositeCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, MainFrameUnsortedTimestamps)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));
        std::vector<int64_t> rect_ids;
        rect_ids.push_back(2);
        tracker->SaveMainFrameTimeStamps(rect_ids,
            base::TimeTicks::FromInternalValue(200),
            base::TimeTicks::FromInternalValue(280), 1);
        tracker->SaveMainFrameTimeStamps(rect_ids,
            base::TimeTicks::FromInternalValue(400),
            base::TimeTicks::FromInternalValue(470), 1);
        tracker->SaveMainFrameTimeStamps(rect_ids,
            base::TimeTicks::FromInternalValue(100),
            base::TimeTicks::FromInternalValue(160), 1);
        EXPECT_EQ(
            "{\"values\":[{\"events\":["
            "{\"end_time\":160,\"frame_id\":1,\"timestamp\":100},"
            "{\"end_time\":280,\"frame_id\":1,\"timestamp\":200},"
            "{\"end_time\":470,\"frame_id\":1,\"timestamp\":400}],\"rect_id\":2}]}",
            MainFrameToString(tracker->GroupMainFrameCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, MultipleFrameIds)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));

        std::vector<std::pair<int, int64_t>> ids200;
        ids200.push_back(std::make_pair(1, 2));
        ids200.push_back(std::make_pair(1, 3));
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(200), ids200);

        std::vector<std::pair<int, int64_t>> ids400;
        ids400.push_back(std::make_pair(2, 2));
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(400), ids400);

        std::vector<std::pair<int, int64_t>> ids100;
        ids100.push_back(std::make_pair(3, 2));
        ids100.push_back(std::make_pair(2, 3));
        ids100.push_back(std::make_pair(3, 4));
        tracker->SaveTimeStamps(base::TimeTicks::FromInternalValue(100), ids100);

        EXPECT_EQ(
            "{\"values\":[{\"events\":["
            "{\"frame_id\":3,\"timestamp\":100},"
            "{\"frame_id\":1,\"timestamp\":200},"
            "{\"frame_id\":2,\"timestamp\":400}],\"rect_id\":2},"
            "{\"events\":["
            "{\"frame_id\":2,\"timestamp\":100},"
            "{\"frame_id\":1,\"timestamp\":200}],\"rect_id\":3},"
            "{\"events\":["
            "{\"frame_id\":3,\"timestamp\":100}],\"rect_id\":4}"
            "]}",
            CompositeToString(tracker->GroupCompositeCountsByRectId()));
    }

    TEST(FrameTimingTrackerTest, MultipleMainFrameEvents)
    {
        FakeImplProxy proxy;
        TestSharedBitmapManager shared_bitmap_manager;
        TestTaskGraphRunner task_graph_runner;
        FakeLayerTreeHostImpl host_impl(&proxy, &shared_bitmap_manager,
            &task_graph_runner);

        scoped_ptr<FrameTimingTracker> tracker(
            FrameTimingTracker::Create(&host_impl));

        std::vector<int64_t> rect_ids200;
        rect_ids200.push_back(2);
        rect_ids200.push_back(3);
        tracker->SaveMainFrameTimeStamps(rect_ids200,
            base::TimeTicks::FromInternalValue(200),
            base::TimeTicks::FromInternalValue(220), 1);

        std::vector<int64_t> rect_ids400;
        rect_ids400.push_back(2);
        tracker->SaveMainFrameTimeStamps(rect_ids400,
            base::TimeTicks::FromInternalValue(400),
            base::TimeTicks::FromInternalValue(440), 2);

        std::vector<int64_t> rect_ids100;
        rect_ids100.push_back(2);
        rect_ids100.push_back(3);
        rect_ids100.push_back(4);
        tracker->SaveMainFrameTimeStamps(rect_ids100,
            base::TimeTicks::FromInternalValue(100),
            base::TimeTicks::FromInternalValue(110), 3);

        EXPECT_EQ(
            "{\"values\":[{\"events\":["
            "{\"end_time\":110,\"frame_id\":3,\"timestamp\":100},"
            "{\"end_time\":220,\"frame_id\":1,\"timestamp\":200},"
            "{\"end_time\":440,\"frame_id\":2,\"timestamp\":400}],\"rect_id\":2},"
            "{\"events\":["
            "{\"end_time\":110,\"frame_id\":3,\"timestamp\":100},"
            "{\"end_time\":220,\"frame_id\":1,\"timestamp\":200}],\"rect_id\":3},"
            "{\"events\":["
            "{\"end_time\":110,\"frame_id\":3,\"timestamp\":100}],\"rect_id\":4}"
            "]}",
            MainFrameToString(tracker->GroupMainFrameCountsByRectId()));
    }

} // namespace
} // namespace cc
