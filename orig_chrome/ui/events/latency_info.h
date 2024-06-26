// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_LATENCY_INFO_H_
#define UI_EVENTS_LATENCY_INFO_H_

#include <stdint.h>

#include <string>
#include <utility>
#include <vector>

//#include "base/containers/small_map.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
//#include "ipc/ipc_param_traits.h"
#include "ui/events/events_base_export.h"

namespace ui {

// When adding new components, or new metrics based on LatencyInfo,
// please update latency_info.dot.
enum LatencyComponentType {
    // ---------------------------BEGIN COMPONENT-------------------------------
    // BEGIN COMPONENT is when we show the latency begin in chrome://tracing.
    // Timestamp when the input event is sent from RenderWidgetHost to renderer.
    INPUT_EVENT_LATENCY_BEGIN_RWH_COMPONENT,
    // In threaded scrolling, main thread scroll listener update is async to
    // scroll processing in impl thread. This is the timestamp when we consider
    // the main thread scroll listener update is begun.
    LATENCY_BEGIN_SCROLL_LISTENER_UPDATE_MAIN_COMPONENT,
    // ---------------------------NORMAL COMPONENT-------------------------------
    // The original timestamp of the touch event which converts to scroll update.
    INPUT_EVENT_LATENCY_SCROLL_UPDATE_ORIGINAL_COMPONENT,
    // The original timestamp of the touch event which converts to the *first*
    // scroll update in a scroll gesture sequence.
    INPUT_EVENT_LATENCY_FIRST_SCROLL_UPDATE_ORIGINAL_COMPONENT,
    // Original timestamp for input event (e.g. timestamp from kernel).
    INPUT_EVENT_LATENCY_ORIGINAL_COMPONENT,
    // Timestamp when the UI event is created.
    INPUT_EVENT_LATENCY_UI_COMPONENT,
    // This is special component indicating there is rendering scheduled for
    // the event associated with this LatencyInfo on main thread.
    INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_MAIN_COMPONENT,
    // This is special component indicating there is rendering scheduled for
    // the event associated with this LatencyInfo on impl thread.
    INPUT_EVENT_LATENCY_RENDERING_SCHEDULED_IMPL_COMPONENT,
    // Timestamp when a scroll update is forwarded to the main thread.
    INPUT_EVENT_LATENCY_FORWARD_SCROLL_UPDATE_TO_MAIN_COMPONENT,
    // Timestamp when the event's ack is received by the RWH.
    INPUT_EVENT_LATENCY_ACK_RWH_COMPONENT,
    // Frame number when a window snapshot was requested. The snapshot
    // is taken when the rendering results actually reach the screen.
    WINDOW_SNAPSHOT_FRAME_NUMBER_COMPONENT,
    // Timestamp when a tab is requested to be shown.
    TAB_SHOW_COMPONENT,
    // Timestamp when the frame is swapped in renderer.
    INPUT_EVENT_LATENCY_RENDERER_SWAP_COMPONENT,
    // Timestamp of when the browser process receives a buffer swap notification
    // from the renderer.
    INPUT_EVENT_BROWSER_RECEIVED_RENDERER_SWAP_COMPONENT,
    // Timestamp of when the gpu service began swap buffers, unlike
    // INPUT_EVENT_LATENCY_TERMINATED_FRAME_SWAP_COMPONENT which measures after.
    INPUT_EVENT_GPU_SWAP_BUFFER_COMPONENT,
    // ---------------------------TERMINAL COMPONENT-----------------------------
    // TERMINAL COMPONENT is when we show the latency end in chrome://tracing.
    // Timestamp when the mouse event is acked from renderer and it does not
    // cause any rendering scheduled.
    INPUT_EVENT_LATENCY_TERMINATED_MOUSE_COMPONENT,
    // Timestamp when the mouse wheel event is acked from renderer and it does not
    // cause any rendering scheduled.
    INPUT_EVENT_LATENCY_TERMINATED_MOUSE_WHEEL_COMPONENT,
    // Timestamp when the keyboard event is acked from renderer and it does not
    // cause any rendering scheduled.
    INPUT_EVENT_LATENCY_TERMINATED_KEYBOARD_COMPONENT,
    // Timestamp when the touch event is acked from renderer and it does not
    // cause any rendering schedueld and does not generate any gesture event.
    INPUT_EVENT_LATENCY_TERMINATED_TOUCH_COMPONENT,
    // Timestamp when the gesture event is acked from renderer, and it does not
    // cause any rendering schedueld.
    INPUT_EVENT_LATENCY_TERMINATED_GESTURE_COMPONENT,
    // Timestamp when the frame is swapped (i.e. when the rendering caused by
    // input event actually takes effect).
    INPUT_EVENT_LATENCY_TERMINATED_FRAME_SWAP_COMPONENT,
    // This component indicates that the input causes a commit to be scheduled
    // but the commit failed.
    INPUT_EVENT_LATENCY_TERMINATED_COMMIT_FAILED_COMPONENT,
    // This component indicates that the input causes a commit to be scheduled
    // but the commit was aborted since it carried no new information.
    INPUT_EVENT_LATENCY_TERMINATED_COMMIT_NO_UPDATE_COMPONENT,
    // This component indicates that the input causes a swap to be scheduled
    // but the swap failed.
    INPUT_EVENT_LATENCY_TERMINATED_SWAP_FAILED_COMPONENT,
    LATENCY_COMPONENT_TYPE_LAST = INPUT_EVENT_LATENCY_TERMINATED_SWAP_FAILED_COMPONENT,
};

class EVENTS_BASE_EXPORT LatencyInfo {
public:
    struct LatencyComponent {
        // Nondecreasing number that can be used to determine what events happened
        // in the component at the time this struct was sent on to the next
        // component.
        int64_t sequence_number;
        // Average time of events that happened in this component.
        base::TimeTicks event_time;
        // Count of events that happened in this component
        uint32_t event_count;
    };

    struct EVENTS_BASE_EXPORT InputCoordinate {
        InputCoordinate();
        InputCoordinate(float x, float y);

        float x;
        float y;
    };

    // Empirically determined constant based on a typical scroll sequence.
    enum { kTypicalMaxComponentsPerLatencyInfo = 10 };

    enum { kMaxCoalescedEventTimestamps = 2 };
    enum { kMaxInputCoordinates = 2 };

    // Map a Latency Component (with a component-specific int64_t id) to a
    // component info.
    //   typedef base::SmallMap<
    //       std::map<std::pair<LatencyComponentType, int64_t>, LatencyComponent>,
    //       kTypicalMaxComponentsPerLatencyInfo> LatencyMap;

    LatencyInfo();
    ~LatencyInfo();

    // For test only.
    LatencyInfo(int64_t trace_id, bool terminated);

    // Returns true if the vector |latency_info| is valid. Returns false
    // if it is not valid and log the |referring_msg|.
    // This function is mainly used to check the latency_info vector that
    // is passed between processes using IPC message has reasonable size
    // so that we are confident the IPC message is not corrupted/compromised.
    // This check will go away once the IPC system has better built-in scheme
    // for corruption/compromise detection.
    static bool Verify(const std::vector<LatencyInfo>& latency_info,
        const char* referring_msg);

    // Copy LatencyComponents with type |type| from |other| into |this|.
    void CopyLatencyFrom(const LatencyInfo& other, LatencyComponentType type);

    // Add LatencyComponents that are in |other| but not in |this|.
    void AddNewLatencyFrom(const LatencyInfo& other);

    // Modifies the current sequence number for a component, and adds a new
    // sequence number with the current timestamp.
    void AddLatencyNumber(LatencyComponentType component,
        int64_t id,
        int64_t component_sequence_number);

    // Similar to |AddLatencyNumber|, and also appends |trace_name_str| to
    // the trace event's name.
    // This function should only be called when adding a BEGIN component.
    void AddLatencyNumberWithTraceName(LatencyComponentType component,
        int64_t id,
        int64_t component_sequence_number,
        const char* trace_name_str);

    // Modifies the current sequence number and adds a certain number of events
    // for a specific component.
    void AddLatencyNumberWithTimestamp(LatencyComponentType component,
        int64_t id,
        int64_t component_sequence_number,
        base::TimeTicks time,
        uint32_t event_count);

    // Returns true if the a component with |type| and |id| is found in
    // the latency_components and the component is stored to |output| if
    // |output| is not NULL. Returns false if no such component is found.
    bool FindLatency(LatencyComponentType type,
        int64_t id,
        LatencyComponent* output) const;

    void RemoveLatency(LatencyComponentType type);

    // Returns true if there is still room for keeping the |input_coordinate|,
    // false otherwise.
    bool AddInputCoordinate(const InputCoordinate& input_coordinate);

    uint32_t input_coordinates_size() const { return input_coordinates_size_; }
    const InputCoordinate* input_coordinates() const
    {
        return input_coordinates_;
    }

    // Returns true if there is still room for keeping the |timestamp|,
    // false otherwise.
    bool AddCoalescedEventTimestamp(double timestamp);

    uint32_t coalesced_events_size() const { return coalesced_events_size_; }
    const double* timestamps_of_coalesced_events() const
    {
        return timestamps_of_coalesced_events_;
    }

    //const LatencyMap& latency_components() const { return latency_components_; }

    bool terminated() const { return terminated_; }
    int64_t trace_id() const { return trace_id_; }

private:
    //   void AddLatencyNumberWithTimestampImpl(LatencyComponentType component,
    //                                          int64_t id,
    //                                          int64_t component_sequence_number,
    //                                          base::TimeTicks time,
    //                                          uint32_t event_count,
    //                                          const char* trace_name_str);
    //
    //   // Converts latencyinfo into format that can be dumped into trace buffer.
    //   scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsTraceableData();
    //   scoped_refptr<base::trace_event::ConvertableToTraceFormat>
    //     CoordinatesAsTraceableData();

    // Shown as part of the name of the trace event for this LatencyInfo.
    // String is empty if no tracing is enabled.
    std::string trace_name_;

    //LatencyMap latency_components_;

    // These coordinates represent window coordinates of the original input event.
    uint32_t input_coordinates_size_;
    InputCoordinate input_coordinates_[kMaxInputCoordinates];

    uint32_t coalesced_events_size_;
    double timestamps_of_coalesced_events_[kMaxCoalescedEventTimestamps];

    // The unique id for matching the ASYNC_BEGIN/END trace event.
    int64_t trace_id_;
    // Whether a terminal component has been added.
    bool terminated_;

    //friend struct IPC::ParamTraits<ui::LatencyInfo>;
};

} // namespace ui

#endif // UI_EVENTS_LATENCY_INFO_H_
