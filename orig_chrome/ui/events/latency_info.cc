// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/latency_info.h"

#include <stddef.h>

#include <algorithm>
#include <string>
#include <utility>

#include "base/json/json_writer.h"
#include "base/lazy_instance.h"
#include "base/macros.h"
#include "base/strings/stringprintf.h"

namespace {

const size_t kMaxLatencyInfoNumber = 100;

const char* GetComponentName(ui::LatencyComponentType type)
{
#define CASE_TYPE(t) \
    case ui::t:      \
        return #t
    switch (type) {
        CASE_TYPE(INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT);
        CASE_TYPE(LATENCY_BEGIN_SCROLL_LISTENER_UPDATE_MAIN_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_SCROLL_UPDATE_ORIGINAL_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_FIRST_SCROLL_UPDATE_ORIGINAL_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_UI_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_MAIN_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_IMPL_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_FORWARD_SCROLL_UPDATE_TO_MAIN_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_ACK_RWH_COMPONENT);
        CASE_TYPE(WINDOW_SNAPSHOT_FRAME_NUMBER_COMPONENT);
        CASE_TYPE(TAB_SHOW_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_RENDERER_SWAP_COMPONENT);
        CASE_TYPE(INPUT_EVENT_BROWSER_RECEIVED_RENDERER_SWAP_COMPONENT);
        CASE_TYPE(INPUT_EVENT_GPU_SWAP_BUFFER_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_MOUSE_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_MOUSE_WHEEL_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_KEYBOARD_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_TOUCH_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_GESTURE_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_FRAME_SWAP_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_COMMIT_FAILED_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_COMMIT_NO_UPDATE_COMPONENT);
        CASE_TYPE(INPUT_EVENT_LATENCY_TERMINATED_SWAP_FAILED_COMPONENT);
    default:
        DLOG(WARNING) << "Unhandled LatencyComponentType.\n";
        break;
    }
#undef CASE_TYPE
    return "unknown";
}

bool IsTerminalComponent(ui::LatencyComponentType type)
{
    switch (type) {
    case ui::INPUT_EVENT_LATENCY_TERMINATED_MOUSE_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_MOUSE_WHEEL_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_KEYBOARD_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_TOUCH_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_GESTURE_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_FRAME_SWAP_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_COMMIT_FAILED_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_COMMIT_NO_UPDATE_COMPONENT:
    case ui::INPUT_EVENT_LATENCY_TERMINATED_SWAP_FAILED_COMPONENT:
        return true;
    default:
        return false;
    }
}

bool IsBeginComponent(ui::LatencyComponentType type)
{
    return (type == ui::INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT || type == ui::LATENCY_BEGIN_SCROLL_LISTENER_UPDATE_MAIN_COMPONENT);
}

bool IsInputLatencyBeginComponent(ui::LatencyComponentType type)
{
    return type == ui::INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT;
}

// This class is for converting latency info to trace buffer friendly format.
class LatencyInfoTracedValue
    : public base::trace_event::ConvertableToTraceFormat {
public:
    static scoped_refptr<ConvertableToTraceFormat> FromValue(
        scoped_ptr<base::Value> value);

    void AppendAsTraceFormat(std::string* out) const override;

private:
    explicit LatencyInfoTracedValue(base::Value* value);
    ~LatencyInfoTracedValue() override;

    scoped_ptr<base::Value> value_;

    DISALLOW_COPY_AND_ASSIGN(LatencyInfoTracedValue);
};

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
LatencyInfoTracedValue::FromValue(scoped_ptr<base::Value> value)
{
    return scoped_refptr<base::trace_event::ConvertableToTraceFormat>(
        new LatencyInfoTracedValue(value.release()));
}

LatencyInfoTracedValue::~LatencyInfoTracedValue()
{
}

void LatencyInfoTracedValue::AppendAsTraceFormat(std::string* out) const
{
    std::string tmp;
    base::JSONWriter::Write(*value_, &tmp);
    *out += tmp;
}

LatencyInfoTracedValue::LatencyInfoTracedValue(base::Value* value)
    : value_(value)
{
}

const char kTraceCategoriesForAsyncEvents[] = "benchmark,latencyInfo";

struct LatencyInfoEnabledInitializer {
    LatencyInfoEnabledInitializer()
        : latency_info_enabled(TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED(
            kTraceCategoriesForAsyncEvents))
    {
    }

    const unsigned char* latency_info_enabled;
};

static base::LazyInstance<LatencyInfoEnabledInitializer>::Leaky
    g_latency_info_enabled
    = LAZY_INSTANCE_INITIALIZER;

} // namespace

namespace ui {

LatencyInfo::InputCoordinate::InputCoordinate()
    : x(0)
    , y(0)
{
}

LatencyInfo::InputCoordinate::InputCoordinate(float x, float y)
    : x(x)
    , y(y)
{
}

LatencyInfo::LatencyInfo()
    : input_coordinates_size_(0)
    , coalesced_events_size_(0)
    , trace_id_(-1)
    , terminated_(false)
{
}

LatencyInfo::~LatencyInfo()
{
}

LatencyInfo::LatencyInfo(int64_t trace_id, bool terminated)
    : input_coordinates_size_(0)
    , coalesced_events_size_(0)
    , trace_id_(trace_id)
    , terminated_(terminated)
{
}

bool LatencyInfo::Verify(const std::vector<LatencyInfo>& latency_info,
    const char* referring_msg)
{
    if (latency_info.size() > kMaxLatencyInfoNumber) {
        LOG(ERROR) << referring_msg << ", LatencyInfo vector size "
                   << latency_info.size() << " is too big.";
        TRACE_EVENT_INSTANT1("input,benchmark", "LatencyInfo::Verify Fails",
            TRACE_EVENT_SCOPE_GLOBAL,
            "size", latency_info.size());
        return false;
    }
    return true;
}

void LatencyInfo::CopyLatencyFrom(const LatencyInfo& other,
    LatencyComponentType type)
{
    for (const auto& lc : other.latency_components()) {
        if (lc.first.first == type) {
            AddLatencyNumberWithTimestamp(lc.first.first,
                lc.first.second,
                lc.second.sequence_number,
                lc.second.event_time,
                lc.second.event_count);
        }
    }
}

void LatencyInfo::AddNewLatencyFrom(const LatencyInfo& other)
{
    for (const auto& lc : other.latency_components()) {
        if (!FindLatency(lc.first.first, lc.first.second, NULL)) {
            AddLatencyNumberWithTimestamp(lc.first.first,
                lc.first.second,
                lc.second.sequence_number,
                lc.second.event_time,
                lc.second.event_count);
        }
    }
}

void LatencyInfo::AddLatencyNumber(LatencyComponentType component,
    int64_t id,
    int64_t component_sequence_number)
{
    AddLatencyNumberWithTimestampImpl(component, id, component_sequence_number,
        base::TimeTicks::Now(), 1, nullptr);
}

void LatencyInfo::AddLatencyNumberWithTraceName(
    LatencyComponentType component,
    int64_t id,
    int64_t component_sequence_number,
    const char* trace_name_str)
{
    AddLatencyNumberWithTimestampImpl(component, id, component_sequence_number,
        base::TimeTicks::Now(), 1, trace_name_str);
}

void LatencyInfo::AddLatencyNumberWithTimestamp(
    LatencyComponentType component,
    int64_t id,
    int64_t component_sequence_number,
    base::TimeTicks time,
    uint32_t event_count)
{
    AddLatencyNumberWithTimestampImpl(component, id, component_sequence_number,
        time, event_count, nullptr);
}

void LatencyInfo::AddLatencyNumberWithTimestampImpl(
    LatencyComponentType component,
    int64_t id,
    int64_t component_sequence_number,
    base::TimeTicks time,
    uint32_t event_count,
    const char* trace_name_str)
{
    const unsigned char* latency_info_enabled = g_latency_info_enabled.Get().latency_info_enabled;

    if (IsBeginComponent(component)) {
        // Should only ever add begin component once.
        CHECK_EQ(-1, trace_id_);
        trace_id_ = component_sequence_number;

        if (*latency_info_enabled) {
            // The timestamp for ASYNC_BEGIN trace event is used for drawing the
            // beginning of the trace event in trace viewer. For better visualization,
            // for an input event, we want to draw the beginning as when the event is
            // originally created, e.g. the timestamp of its ORIGINAL/UI_COMPONENT,
            // not when we actually issue the ASYNC_BEGIN trace event.
            LatencyComponent begin_component;
            int64_t ts = 0;
            if (FindLatency(INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT,
                    0,
                    &begin_component)
                || FindLatency(INPUT_EVENT_LATENCY_UI_COMPONENT,
                    0,
                    &begin_component)) {
                ts = begin_component.event_time.ToInternalValue();
            } else {
                ts = base::TimeTicks::Now().ToInternalValue();
            }

            if (trace_name_str) {
                if (IsInputLatencyBeginComponent(component))
                    trace_name_ = std::string("InputLatency::") + trace_name_str;
                else
                    trace_name_ = std::string("Latency::") + trace_name_str;
            }

            TRACE_EVENT_COPY_ASYNC_BEGIN_WITH_TIMESTAMP0(
                kTraceCategoriesForAsyncEvents,
                trace_name_.c_str(),
                TRACE_ID_DONT_MANGLE(trace_id_),
                ts);
        }

        TRACE_EVENT_WITH_FLOW1("input,benchmark",
            "LatencyInfo.Flow",
            TRACE_ID_DONT_MANGLE(trace_id_),
            TRACE_EVENT_FLAG_FLOW_OUT,
            "trace_id", trace_id_);
    }

    LatencyMap::key_type key = std::make_pair(component, id);
    LatencyMap::iterator it = latency_components_.find(key);
    if (it == latency_components_.end()) {
        LatencyComponent info = { component_sequence_number, time, event_count };
        latency_components_[key] = info;
    } else {
        it->second.sequence_number = std::max(component_sequence_number,
            it->second.sequence_number);
        uint32_t new_count = event_count + it->second.event_count;
        if (event_count > 0 && new_count != 0) {
            // Do a weighted average, so that the new event_time is the average of
            // the times of events currently in this structure with the time passed
            // into this method.
            it->second.event_time += (time - it->second.event_time) * event_count / new_count;
            it->second.event_count = new_count;
        }
    }

    if (IsTerminalComponent(component) && trace_id_ != -1) {
        // Should only ever add terminal component once.
        CHECK(!terminated_);
        terminated_ = true;

        if (*latency_info_enabled) {
            TRACE_EVENT_COPY_ASYNC_END2(kTraceCategoriesForAsyncEvents,
                trace_name_.c_str(),
                TRACE_ID_DONT_MANGLE(trace_id_),
                "data", AsTraceableData(),
                "coordinates", CoordinatesAsTraceableData());
        }

        TRACE_EVENT_WITH_FLOW0("input,benchmark",
            "LatencyInfo.Flow",
            TRACE_ID_DONT_MANGLE(trace_id_),
            TRACE_EVENT_FLAG_FLOW_IN);
    }
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
LatencyInfo::AsTraceableData()
{
    scoped_ptr<base::DictionaryValue> record_data(new base::DictionaryValue());
    for (const auto& lc : latency_components_) {
        scoped_ptr<base::DictionaryValue>
            component_info(new base::DictionaryValue());
        component_info->SetDouble("comp_id", static_cast<double>(lc.first.second));
        component_info->SetDouble(
            "time",
            static_cast<double>(lc.second.event_time.ToInternalValue()));
        component_info->SetDouble("count", lc.second.event_count);
        component_info->SetDouble("sequence_number",
            lc.second.sequence_number);
        record_data->Set(GetComponentName(lc.first.first),
            std::move(component_info));
    }
    record_data->SetDouble("trace_id", static_cast<double>(trace_id_));
    return LatencyInfoTracedValue::FromValue(std::move(record_data));
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
LatencyInfo::CoordinatesAsTraceableData()
{
    scoped_ptr<base::ListValue> coordinates(new base::ListValue());
    for (size_t i = 0; i < input_coordinates_size_; i++) {
        scoped_ptr<base::DictionaryValue> coordinate_pair(
            new base::DictionaryValue());
        coordinate_pair->SetDouble("x", input_coordinates_[i].x);
        coordinate_pair->SetDouble("y", input_coordinates_[i].y);
        coordinates->Append(coordinate_pair.release());
    }
    return LatencyInfoTracedValue::FromValue(std::move(coordinates));
}

bool LatencyInfo::FindLatency(LatencyComponentType type,
    int64_t id,
    LatencyComponent* output) const
{
    LatencyMap::const_iterator it = latency_components_.find(
        std::make_pair(type, id));
    if (it == latency_components_.end())
        return false;
    if (output)
        *output = it->second;
    return true;
}

void LatencyInfo::RemoveLatency(LatencyComponentType type)
{
    LatencyMap::iterator it = latency_components_.begin();
    while (it != latency_components_.end()) {
        if (it->first.first == type) {
            LatencyMap::iterator tmp = it;
            ++it;
            latency_components_.erase(tmp);
        } else {
            it++;
        }
    }
}

bool LatencyInfo::AddInputCoordinate(const InputCoordinate& input_coordinate)
{
    if (input_coordinates_size_ >= kMaxInputCoordinates)
        return false;
    input_coordinates_[input_coordinates_size_++] = input_coordinate;
    return true;
}

bool LatencyInfo::AddCoalescedEventTimestamp(double timestamp)
{
    if (coalesced_events_size_ >= kMaxCoalescedEventTimestamps)
        return false;
    timestamps_of_coalesced_events_[coalesced_events_size_++] = timestamp;
    return true;
}

} // namespace ui
