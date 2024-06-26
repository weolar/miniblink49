// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gl/angle_platform_impl.h"

#include "base/metrics/histogram.h"
#include "base/metrics/sparse_histogram.h"
#include "base/trace_event/trace_event.h"

namespace gfx {

ANGLEPlatformImpl::ANGLEPlatformImpl()
{
}

ANGLEPlatformImpl::~ANGLEPlatformImpl()
{
}

double ANGLEPlatformImpl::currentTime()
{
    return base::Time::Now().ToDoubleT();
}

double ANGLEPlatformImpl::monotonicallyIncreasingTime()
{
    return (base::TimeTicks::Now() - base::TimeTicks()).InSecondsF();
}

const unsigned char* ANGLEPlatformImpl::getTraceCategoryEnabledFlag(
    const char* category_group)
{
    //return TRACE_EVENT_API_GET_CATEGORY_GROUP_ENABLED(category_group);
    return (const unsigned char*)category_group;
}

void ANGLEPlatformImpl::logError(const char* errorMessage)
{
    LOG(ERROR) << errorMessage;
}

void ANGLEPlatformImpl::logWarning(const char* warningMessage)
{
    LOG(WARNING) << warningMessage;
}

angle::Platform::TraceEventHandle ANGLEPlatformImpl::addTraceEvent(
    char phase,
    const unsigned char* category_group_enabled,
    const char* name,
    unsigned long long id,
    double timestamp,
    int num_args,
    const char** arg_names,
    const unsigned char* arg_types,
    const unsigned long long* arg_values,
    unsigned char flags)
{
    //   base::TimeTicks timestamp_tt =
    //       base::TimeTicks() + base::TimeDelta::FromSecondsD(timestamp);
    //   base::trace_event::TraceEventHandle handle =
    //       TRACE_EVENT_API_ADD_TRACE_EVENT_WITH_THREAD_ID_AND_TIMESTAMP(
    //           phase, category_group_enabled, name, id, trace_event_internal::kNoId,
    //           base::PlatformThread::CurrentId(), timestamp_tt, num_args, arg_names,
    //           arg_types, arg_values, nullptr, flags);
    //   angle::Platform::TraceEventHandle result;
    //   memcpy(&result, &handle, sizeof(result));
    //   return result;

    angle::Platform::TraceEventHandle result = 1;
    return result;
}

void ANGLEPlatformImpl::updateTraceEventDuration(
    const unsigned char* category_group_enabled,
    const char* name,
    TraceEventHandle handle)
{
    //   base::trace_event::TraceEventHandle trace_event_handle;
    //   memcpy(&trace_event_handle, &handle, sizeof(handle));
    //   TRACE_EVENT_API_UPDATE_TRACE_EVENT_DURATION(category_group_enabled, name,
    //                                               trace_event_handle);
}

void ANGLEPlatformImpl::histogramCustomCounts(const char* name,
    int sample,
    int min,
    int max,
    int bucket_count)
{
    // Copied from histogram macro, but without the static variable caching
    // the histogram because name is dynamic.
    //   base::HistogramBase* counter = base::Histogram::FactoryGet(
    //       name, min, max, bucket_count,
    //       base::HistogramBase::kUmaTargetedHistogramFlag);
    //   DCHECK_EQ(name, counter->histogram_name());
    //   counter->Add(sample);
}

void ANGLEPlatformImpl::histogramEnumeration(const char* name,
    int sample,
    int boundary_value)
{
    // Copied from histogram macro, but without the static variable caching
    // the histogram because name is dynamic.
    //   base::HistogramBase* counter = base::LinearHistogram::FactoryGet(
    //       name, 1, boundary_value, boundary_value + 1,
    //       base::HistogramBase::kUmaTargetedHistogramFlag);
    //   DCHECK_EQ(name, counter->histogram_name());
    //   counter->Add(sample);
}

void ANGLEPlatformImpl::histogramSparse(const char* name, int sample)
{
    // For sparse histograms, we can use the macro, as it does not incorporate a
    // static.
    //UMA_HISTOGRAM_SPARSE_SLOWLY(name, sample);
}

void ANGLEPlatformImpl::histogramBoolean(const char* name, bool sample)
{
    histogramEnumeration(name, sample ? 1 : 0, 2);
}

} // namespace gfx
