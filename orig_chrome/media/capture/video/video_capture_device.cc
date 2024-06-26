// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/video_capture_device.h"

#include "base/command_line.h"
#include "base/i18n/timezone.h"
#include "base/strings/string_util.h"
#include "media/base/media_switches.h"

namespace media {

// TODO(msu.koo): http://crbug.com/532272, remove checking the switch in favour
// of deferring GetModel() call to the actual VideoCaptureDevice object.
const std::string VideoCaptureDevice::Name::GetNameAndModel() const
{
    const std::string model_id = GetModel();
    if (model_id.empty())
        return device_name_;
    const std::string suffix = " (" + model_id + ")";
    if (base::EndsWith(device_name_, suffix, base::CompareCase::SENSITIVE) || base::CommandLine::ForCurrentProcess()->HasSwitch(switches::kUseFakeDeviceForMediaStream))
        // Ignore |model_id| if |kUseFakeDeviceForMediaStream| flag is present.
        return device_name_;
    return device_name_ + suffix;
}

VideoCaptureDevice::Name::Name()
{
}

VideoCaptureDevice::Name::Name(const std::string& name, const std::string& id)
    : device_name_(name)
    , unique_id_(id)
{
}

#if defined(OS_LINUX)
VideoCaptureDevice::Name::Name(const std::string& name,
    const std::string& id,
    const CaptureApiType api_type)
    : device_name_(name)
    , unique_id_(id)
    , capture_api_class_(api_type)
{
}
#elif defined(OS_WIN)
VideoCaptureDevice::Name::Name(const std::string& name,
    const std::string& id,
    const CaptureApiType api_type)
    : device_name_(name)
    , unique_id_(id)
    , capture_api_class_(api_type)
    , capabilities_id_(id)
{
}
#elif defined(OS_MACOSX)
VideoCaptureDevice::Name::Name(const std::string& name,
    const std::string& id,
    const CaptureApiType api_type)
    : device_name_(name)
    , unique_id_(id)
    , capture_api_class_(api_type)
    , transport_type_(OTHER_TRANSPORT)
    , is_blacklisted_(false)
{
}

VideoCaptureDevice::Name::Name(const std::string& name,
    const std::string& id,
    const CaptureApiType api_type,
    const TransportType transport_type)
    : device_name_(name)
    , unique_id_(id)
    , capture_api_class_(api_type)
    , transport_type_(transport_type)
    , is_blacklisted_(false)
{
}
#elif defined(ANDROID)
VideoCaptureDevice::Name::Name(const std::string& name,
    const std::string& id,
    const CaptureApiType api_type)
    : device_name_(name)
    , unique_id_(id)
    , capture_api_class_(api_type)
{
}
#endif

VideoCaptureDevice::Name::~Name()
{
}

#if defined(OS_LINUX)
const char* VideoCaptureDevice::Name::GetCaptureApiTypeString() const
{
    switch (capture_api_type()) {
    case V4L2_SINGLE_PLANE:
        return "V4L2 SPLANE";
    case V4L2_MULTI_PLANE:
        return "V4L2 MPLANE";
    default:
        NOTREACHED() << "Unknown Video Capture API type!";
        return "Unknown API";
    }
}
#elif defined(OS_WIN)
const char* VideoCaptureDevice::Name::GetCaptureApiTypeString() const
{
    switch (capture_api_type()) {
    case MEDIA_FOUNDATION:
        return "Media Foundation";
    case DIRECT_SHOW:
        return "Direct Show";
    default:
        NOTREACHED() << "Unknown Video Capture API type!";
        return "Unknown API";
    }
}
#elif defined(OS_MACOSX)
const char* VideoCaptureDevice::Name::GetCaptureApiTypeString() const
{
    switch (capture_api_type()) {
    case AVFOUNDATION:
        return "AV Foundation";
    case QTKIT:
        return "QTKit";
    case DECKLINK:
        return "DeckLink";
    default:
        NOTREACHED() << "Unknown Video Capture API type!";
        return "Unknown API";
    }
}
#elif defined(OS_ANDROID)
const char* VideoCaptureDevice::Name::GetCaptureApiTypeString() const
{
    switch (capture_api_type()) {
    case API1:
        return "Camera API1";
    case API2_LEGACY:
        return "Camera API2 Legacy";
    case API2_FULL:
        return "Camera API2 Full";
    case API2_LIMITED:
        return "Camera API2 Limited";
    case TANGO:
        return "Tango API";
    case API_TYPE_UNKNOWN:
    default:
        NOTREACHED() << "Unknown Video Capture API type!";
        return "Unknown API";
    }
}
#endif

VideoCaptureDevice::Client::Buffer::~Buffer()
{
}

VideoCaptureDevice::~VideoCaptureDevice()
{
}

PowerLineFrequency VideoCaptureDevice::GetPowerLineFrequencyForLocation()
    const
{
    const std::string current_country = base::CountryCodeForCurrentTimezone();
    if (current_country.empty())
        return PowerLineFrequency::FREQUENCY_DEFAULT;
    // Sorted out list of countries with 60Hz power line frequency, from
    // http://en.wikipedia.org/wiki/Mains_electricity_by_country
    const char* countries_using_60Hz[] = {
        "AI", "AO", "AS", "AW", "AZ", "BM", "BR", "BS", "BZ", "CA", "CO",
        "CR", "CU", "DO", "EC", "FM", "GT", "GU", "GY", "HN", "HT", "JP",
        "KN", "KR", "KY", "MS", "MX", "NI", "PA", "PE", "PF", "PH", "PR",
        "PW", "SA", "SR", "SV", "TT", "TW", "UM", "US", "VG", "VI", "VE"
    };
    const char** countries_using_60Hz_end = countries_using_60Hz + arraysize(countries_using_60Hz);
    if (std::find(countries_using_60Hz, countries_using_60Hz_end,
            current_country)
        == countries_using_60Hz_end) {
        return media::PowerLineFrequency::FREQUENCY_50HZ;
    }
    return media::PowerLineFrequency::FREQUENCY_60HZ;
}

PowerLineFrequency VideoCaptureDevice::GetPowerLineFrequency(
    const VideoCaptureParams& params) const
{
    switch (params.power_line_frequency) {
    case media::PowerLineFrequency::FREQUENCY_50HZ: // fall through
    case media::PowerLineFrequency::FREQUENCY_60HZ:
        return params.power_line_frequency;
    default:
        return GetPowerLineFrequencyForLocation();
    }
}

} // namespace media
