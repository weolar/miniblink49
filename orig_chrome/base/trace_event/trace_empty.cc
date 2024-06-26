
#include "base/profiler/scoped_profile.h"
#include "base/profiler/scoped_tracker.h"
#include "base/profiler/tracked_time.h"
#include "base/tracked_objects.h"
#include "base/tracking_info.h"

namespace tracked_objects {

// TrackedTime::TrackedTime(class base::TimeTicks const &)
// {
//   DebugBreak();
// }
//
// Duration TrackedTime::operator-(TrackedTime const &)const
// {
//   DebugBreak();
//   return Duration();
// }

// void ThreadData::TallyRunOnNamedThreadIfTracking(base::TrackingInfo const &, TaskStopwatch const &)
// {
//
// }

// TaskStopwatch::TaskStopwatch(void)
// {
//
// }
//
// TaskStopwatch::~TaskStopwatch(void)
// {
//
// }
//
// void TaskStopwatch::Start(void)
// {
//
// }
//
// void TaskStopwatch::Stop(void)
// {
//
// }
//
// TrackedTime TaskStopwatch::StartTime(void) const
// {
//     return TrackedTime();
// }
//
// void ThreadData::InitializeThreadContext(class std::basic_string<char, struct std::char_traits<char>, class std::allocator<char> > const &)
// {
//     //DebugBreak();
// }

ScopedTracker::ScopedTracker(const Location& loc)
    : scoped_profile_(loc, ScopedProfile::DISABLED)
{
}

ScopedProfile::ScopedProfile(const Location& loc, enum ScopedProfile::Mode)
{
}

ScopedProfile::~ScopedProfile(void)
{
}

// Duration::Duration(void)
// {
//
// }
//
// TrackedTime::TrackedTime(void)
// {
//
// }

} // tracked_objects