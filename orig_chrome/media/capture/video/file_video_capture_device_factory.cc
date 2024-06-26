// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/file_video_capture_device_factory.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/strings/sys_string_conversions.h"
#include "media/base/media_switches.h"
#include "media/capture/video/file_video_capture_device.h"

namespace media {

const char kFileVideoCaptureDeviceName[] = "/dev/placeholder-for-file-backed-fake-capture-device";

// Inspects the command line and retrieves the file path parameter.
base::FilePath GetFilePathFromCommandLine()
{
    base::FilePath command_line_file_path = base::CommandLine::ForCurrentProcess()->GetSwitchValuePath(
        switches::kUseFileForFakeVideoCapture);
    CHECK(!command_line_file_path.empty());
    return command_line_file_path;
}

scoped_ptr<VideoCaptureDevice> FileVideoCaptureDeviceFactory::Create(
    const VideoCaptureDevice::Name& device_name)
{
    DCHECK(thread_checker_.CalledOnValidThread());
#if defined(OS_WIN)
    return scoped_ptr<VideoCaptureDevice>(new FileVideoCaptureDevice(
        base::FilePath(base::SysUTF8ToWide(device_name.name()))));
#else
    return scoped_ptr<VideoCaptureDevice>(
        new FileVideoCaptureDevice(base::FilePath(device_name.name())));
#endif
}

void FileVideoCaptureDeviceFactory::GetDeviceNames(
    VideoCaptureDevice::Names* const device_names)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(device_names->empty());
    const base::FilePath command_line_file_path = GetFilePathFromCommandLine();
#if defined(OS_WIN)
    device_names->push_back(VideoCaptureDevice::Name(
        base::SysWideToUTF8(command_line_file_path.value()),
        kFileVideoCaptureDeviceName, VideoCaptureDevice::Name::DIRECT_SHOW));
#elif defined(OS_MACOSX)
    device_names->push_back(VideoCaptureDevice::Name(
        command_line_file_path.value(), kFileVideoCaptureDeviceName,
        VideoCaptureDevice::Name::AVFOUNDATION));
#elif defined(OS_LINUX)
    device_names->push_back(VideoCaptureDevice::Name(
        command_line_file_path.value(), kFileVideoCaptureDeviceName,
        VideoCaptureDevice::Name::V4L2_SINGLE_PLANE));
#else
    device_names->push_back(VideoCaptureDevice::Name(
        command_line_file_path.value(), kFileVideoCaptureDeviceName));
#endif
}

void FileVideoCaptureDeviceFactory::GetDeviceSupportedFormats(
    const VideoCaptureDevice::Name& device,
    VideoCaptureFormats* supported_formats)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    VideoCaptureFormat capture_format;
    if (!FileVideoCaptureDevice::GetVideoCaptureFormat(
            GetFilePathFromCommandLine(), &capture_format)) {
        return;
    }

    supported_formats->push_back(capture_format);
}

} // namespace media
