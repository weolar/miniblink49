// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/video_capture_device_factory.h"

#include "base/command_line.h"
#include "media/base/media_switches.h"
#include "media/capture/video/fake_video_capture_device_factory.h"
#include "media/capture/video/file_video_capture_device_factory.h"

namespace media {

// static
scoped_ptr<VideoCaptureDeviceFactory> VideoCaptureDeviceFactory::CreateFactory(
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner)
{
    const base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
    // Use a Fake or File Video Device Factory if the command line flags are
    // present, otherwise use the normal, platform-dependent, device factory.
    if (command_line->HasSwitch(switches::kUseFakeDeviceForMediaStream)) {
        if (command_line->HasSwitch(switches::kUseFileForFakeVideoCapture)) {
            return scoped_ptr<VideoCaptureDeviceFactory>(
                new media::FileVideoCaptureDeviceFactory());
        } else {
            return scoped_ptr<VideoCaptureDeviceFactory>(
                new media::FakeVideoCaptureDeviceFactory());
        }
    } else {
        // |ui_task_runner| is needed for the Linux ChromeOS factory to retrieve
        // screen rotations and for the Mac factory to run QTKit device enumeration.
        return scoped_ptr<VideoCaptureDeviceFactory>(
            CreateVideoCaptureDeviceFactory(ui_task_runner));
    }
}

VideoCaptureDeviceFactory::VideoCaptureDeviceFactory()
{
    thread_checker_.DetachFromThread();
}

VideoCaptureDeviceFactory::~VideoCaptureDeviceFactory()
{
}

void VideoCaptureDeviceFactory::EnumerateDeviceNames(const base::Callback<
    void(scoped_ptr<media::VideoCaptureDevice::Names>)>& callback)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK(!callback.is_null());
    scoped_ptr<VideoCaptureDevice::Names> device_names(
        new VideoCaptureDevice::Names());
    GetDeviceNames(device_names.get());
    callback.Run(device_names.Pass());
}

#if !defined(OS_MACOSX) && !defined(OS_LINUX) && !defined(OS_ANDROID) && !defined(OS_WIN)
// static
VideoCaptureDeviceFactory*
VideoCaptureDeviceFactory::CreateVideoCaptureDeviceFactory(
    scoped_refptr<base::SingleThreadTaskRunner> ui_task_runner)
{
    NOTIMPLEMENTED();
    return NULL;
}
#endif

} // namespace media
