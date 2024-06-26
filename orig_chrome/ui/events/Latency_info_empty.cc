#include "ui/events/latency_info.h"

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
    DebugBreak();
}

LatencyInfo::LatencyInfo(int64_t trace_id, bool terminated)
    : input_coordinates_size_(0)
    , coalesced_events_size_(0)
    , trace_id_(trace_id)
    , terminated_(terminated)
{
    DebugBreak();
}

LatencyInfo::~LatencyInfo(void)
{
}

void LatencyInfo::AddLatencyNumber(LatencyComponentType component,
    int64_t id,
    int64_t component_sequence_number)
{
    DebugBreak();
}

bool LatencyInfo::FindLatency(LatencyComponentType type,
    int64_t id,
    LatencyComponent* output) const
{
    DebugBreak();
    return false;
}

} // ui