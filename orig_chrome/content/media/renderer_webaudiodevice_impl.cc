// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/media/renderer_webaudiodevice_impl.h"

#include <string>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "base/time/time.h"
#include "content/media/audio_device_factory.h"
//#include "content/renderer/render_frame_impl.h"
#include "media/audio/audio_output_device.h"
#include "media/audio/null_audio_sink.h"
#include "media/base/media_switches.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebView.h"

using blink::WebAudioDevice;
using blink::WebLocalFrame;
using blink::WebVector;
using blink::WebView;

namespace content {

#if defined(OS_ANDROID)
static const int kSilenceInSecondsToEnterIdleMode = 30;
#endif

RendererWebAudioDeviceImpl::RendererWebAudioDeviceImpl(const media::AudioParameters& params, WebAudioDevice::RenderCallback* callback, int session_id)
    : params_(params)
    , client_callback_(callback)
    , session_id_(session_id)
    , task_runner_(base::ThreadTaskRunnerHandle::Get())
    , null_audio_sink_(new media::NullAudioSink(task_runner_))
    , is_using_null_audio_sink_(false)
    , first_buffer_after_silence_(media::AudioBus::Create(params_))
    , is_first_buffer_after_silence_(false)
{
    DCHECK(client_callback_);
    null_audio_sink_->Initialize(params_, this);
}

RendererWebAudioDeviceImpl::~RendererWebAudioDeviceImpl()
{
    DCHECK(!output_device_);
}

void RendererWebAudioDeviceImpl::start()
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (output_device_)
        return;  // Already started.

    // Assumption: This method is being invoked within a V8 call stack.  CHECKs
    // will fail in the call to frameForCurrentContext() otherwise.
    //
    // Therefore, we can perform look-ups to determine which RenderView is
    // starting the audio device.  The reason for all this is because the creator
    // of the WebAudio objects might not be the actual source of the audio (e.g.,
    // an extension creates a object that is passed and used within a page).
    WebLocalFrame* const web_frame = WebLocalFrame::frameForCurrentContext();
    //RenderFrame* const render_frame = web_frame ? RenderFrame::FromWebFrame(web_frame) : NULL;

    output_device_ = AudioDeviceFactory::NewOutputDevice(/*render_frame ? render_frame->GetRoutingID() : MSG_ROUTING_NONE*/-1, session_id_, std::string(), std::string());
    output_device_->Initialize(params_, this);
    output_device_->Start();
    start_null_audio_sink_callback_.Reset(base::Bind(&media::NullAudioSink::Play, null_audio_sink_));
    // Note: Default behavior is to auto-play on start.
}

void RendererWebAudioDeviceImpl::stop()
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (output_device_) {
        output_device_->Stop();
        output_device_ = NULL;
    }
    null_audio_sink_->Stop();
    is_using_null_audio_sink_ = false;
    is_first_buffer_after_silence_ = false;
    start_null_audio_sink_callback_.Cancel();
}

double RendererWebAudioDeviceImpl::sampleRate()
{
    return params_.sample_rate();
}

int RendererWebAudioDeviceImpl::Render(media::AudioBus* dest, int audio_delay_milliseconds)
{
#if defined(OS_ANDROID)
    if (is_first_buffer_after_silence_) {
        DCHECK(!is_using_null_audio_sink_);
        first_buffer_after_silence_->CopyTo(dest);
        is_first_buffer_after_silence_ = false;
        return dest->frames();
    }
#endif
    // Wrap the output pointers using WebVector.
    WebVector<float*> web_audio_dest_data(static_cast<size_t>(dest->channels()));
    for (int i = 0; i < dest->channels(); ++i)
        web_audio_dest_data[i] = dest->channel(i);

    // TODO(xians): Remove the following |web_audio_source_data| after
    // changing the blink interface.
    WebVector<float*> web_audio_source_data(static_cast<size_t>(0));
    client_callback_->render(web_audio_source_data, web_audio_dest_data, dest->frames());

#if defined(OS_ANDROID)
    const bool is_zero = dest->AreFramesZero();
    if (!is_zero) {
        first_silence_time_ = base::TimeTicks();
        if (is_using_null_audio_sink_) {
            // This is called on the main render thread when audio is detected.
            output_device_->Play();
            is_using_null_audio_sink_ = false;
            is_first_buffer_after_silence_ = true;
            dest->CopyTo(first_buffer_after_silence_.get());
            task_runner_->PostTask(FROM_HERE, base::Bind(&media::NullAudioSink::Stop, null_audio_sink_));
        }
    } else if (!is_using_null_audio_sink_) {
        // Called on the audio device thread.
        const base::TimeTicks now = base::TimeTicks::Now();
        if (first_silence_time_.is_null())
            first_silence_time_ = now;

        if (now - first_silence_time_ > base::TimeDelta::FromSeconds(kSilenceInSecondsToEnterIdleMode)) {
            output_device_->Pause();
            is_using_null_audio_sink_ = true;
            // If Stop() is called right after the task is posted, need to cancel this task.
            task_runner_->PostDelayedTask(FROM_HERE, start_null_audio_sink_callback_.callback(), params_.GetBufferDuration());
        }
    }
#endif
    return dest->frames();
}

void RendererWebAudioDeviceImpl::OnRenderError()
{
    // TODO(crogers): implement error handling.
}

}  // namespace content
