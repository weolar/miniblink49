// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/fake_video_capture_device_factory.h"

#include "base/command_line.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_tokenizer.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "media/base/media_switches.h"

namespace media {

// Cap the frame rate command line input to reasonable values.
static const float kFakeCaptureMinFrameRate = 5.0f;
static const float kFakeCaptureMaxFrameRate = 60.0f;
// Default rate if none is specified as part of the command line.
static const float kFakeCaptureDefaultFrameRate = 20.0f;

FakeVideoCaptureDeviceFactory::FakeVideoCaptureDeviceFactory()
    : number_of_devices_(1)
    , fake_vcd_ownership_(FakeVideoCaptureDevice::BufferOwnership::OWN_BUFFERS)
    , fake_vcd_planarity_(FakeVideoCaptureDevice::BufferPlanarity::PACKED)
    , frame_rate_(kFakeCaptureDefaultFrameRate)
{
}

scoped_ptr<VideoCaptureDevice> FakeVideoCaptureDeviceFactory::Create(
    const VideoCaptureDevice::Name& device_name)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    parse_command_line();

    for (int n = 0; n < number_of_devices_; ++n) {
        std::string possible_id = base::StringPrintf("/dev/video%d", n);
        if (device_name.id().compare(possible_id) == 0) {
            return scoped_ptr<VideoCaptureDevice>(new FakeVideoCaptureDevice(
                fake_vcd_ownership_, fake_vcd_planarity_, frame_rate_));
        }
    }
    return scoped_ptr<VideoCaptureDevice>();
}

void FakeVideoCaptureDeviceFactory::GetDeviceNames(
    VideoCaptureDevice::Names* const device_names)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(device_names->empty());
    for (int n = 0; n < number_of_devices_; ++n) {
        VideoCaptureDevice::Name name(base::StringPrintf("fake_device_%d", n),
            base::StringPrintf("/dev/video%d", n)
#if defined(OS_LINUX)
                ,
            VideoCaptureDevice::Name::V4L2_SINGLE_PLANE
#elif defined(OS_MACOSX)
                ,
            VideoCaptureDevice::Name::AVFOUNDATION
#elif defined(OS_WIN)
                ,
            VideoCaptureDevice::Name::DIRECT_SHOW
#elif defined(OS_ANDROID)
                ,
            VideoCaptureDevice::Name::API2_LEGACY
#endif
        );
        device_names->push_back(name);
    }
}

void FakeVideoCaptureDeviceFactory::GetDeviceSupportedFormats(
    const VideoCaptureDevice::Name& device,
    VideoCaptureFormats* supported_formats)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    const gfx::Size supported_sizes[] = { gfx::Size(320, 240),
        gfx::Size(640, 480),
        gfx::Size(1280, 720),
        gfx::Size(1920, 1080) };
    supported_formats->clear();
    for (const auto& size : supported_sizes) {
        supported_formats->push_back(
            VideoCaptureFormat(size, frame_rate_, media::PIXEL_FORMAT_I420));
    }
}

// Optional comma delimited parameters to the command line can specify buffer
// ownership, buffer planarity, and the fake video device FPS.
// Examples: "ownership=client, planarity=triplanar, fps=60" "fps=30"
void FakeVideoCaptureDeviceFactory::parse_command_line()
{
    const std::string option = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
        switches::kUseFakeDeviceForMediaStream);
    base::StringTokenizer option_tokenizer(option, ", ");
    option_tokenizer.set_quote_chars("\"");

    while (option_tokenizer.GetNext()) {
        std::vector<std::string> param = base::SplitString(option_tokenizer.token(), "=", base::TRIM_WHITESPACE,
            base::SPLIT_WANT_NONEMPTY);

        if (param.size() != 2u) {
            LOG(WARNING) << "Forget a value '" << option << "'? Use name=value for "
                         << switches::kUseFakeDeviceForMediaStream << ".";
            return;
        }

        if (base::EqualsCaseInsensitiveASCII(param.front(), "ownership") && base::EqualsCaseInsensitiveASCII(param.back(), "client")) {
            fake_vcd_ownership_ = FakeVideoCaptureDevice::BufferOwnership::CLIENT_BUFFERS;
        } else if (base::EqualsCaseInsensitiveASCII(param.front(), "planarity") && base::EqualsCaseInsensitiveASCII(param.back(), "triplanar")) {
            fake_vcd_planarity_ = FakeVideoCaptureDevice::BufferPlanarity::TRIPLANAR;
        } else if (base::EqualsCaseInsensitiveASCII(param.front(), "fps")) {
            double fps = 0;
            if (base::StringToDouble(param.back(), &fps)) {
                frame_rate_ = std::max(kFakeCaptureMinFrameRate, static_cast<float>(fps));
                frame_rate_ = std::min(kFakeCaptureMaxFrameRate, frame_rate_);
            }
        }
    }
}

} // namespace media
