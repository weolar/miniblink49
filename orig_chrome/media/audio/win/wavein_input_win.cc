// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/win/wavein_input_win.h"

#pragma comment(lib, "winmm.lib")

#include "base/logging.h"
#include "media/audio/audio_io.h"
#include "media/audio/win/audio_manager_win.h"
#include "media/audio/win/device_enumeration_win.h"
#include "media/base/audio_bus.h"

namespace media {

// Our sound buffers are allocated once and kept in a linked list using the
// the WAVEHDR::dwUser variable. The last buffer points to the first buffer.
static WAVEHDR* GetNextBuffer(WAVEHDR* current)
{
    return reinterpret_cast<WAVEHDR*>(current->dwUser);
}

PCMWaveInAudioInputStream::PCMWaveInAudioInputStream(
    AudioManagerWin* manager,
    const AudioParameters& params,
    int num_buffers,
    const std::string& device_id)
    : state_(kStateEmpty)
    , manager_(manager)
    , callback_(NULL)
    , num_buffers_(num_buffers)
    , channels_(params.channels())
    , device_id_(device_id)
    , wavein_(NULL)
    , buffer_(NULL)
    , audio_bus_(media::AudioBus::Create(params))
{
    DCHECK_GT(num_buffers_, 0);
    format_.wFormatTag = WAVE_FORMAT_PCM;
    format_.nChannels = params.channels() > 2 ? 2 : params.channels();
    format_.nSamplesPerSec = params.sample_rate();
    format_.wBitsPerSample = params.bits_per_sample();
    format_.cbSize = 0;
    format_.nBlockAlign = (format_.nChannels * format_.wBitsPerSample) / 8;
    format_.nAvgBytesPerSec = format_.nBlockAlign * format_.nSamplesPerSec;
    buffer_size_ = params.frames_per_buffer() * format_.nBlockAlign;
    // If we don't have a packet size we use 100ms.
    if (!buffer_size_)
        buffer_size_ = format_.nAvgBytesPerSec / 10;
    // The event is auto-reset.
    stopped_event_.Set(::CreateEventW(NULL, FALSE, FALSE, NULL));
}

PCMWaveInAudioInputStream::~PCMWaveInAudioInputStream()
{
    DCHECK(NULL == wavein_);
}

bool PCMWaveInAudioInputStream::Open()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    if (state_ != kStateEmpty)
        return false;
    if (num_buffers_ < 2 || num_buffers_ > 10)
        return false;

    // Convert the stored device id string into an unsigned integer
    // corresponding to the selected device.
    UINT device_id = WAVE_MAPPER;
    if (!GetDeviceId(&device_id)) {
        return false;
    }

    // Open the specified input device for recording.
    MMRESULT result = MMSYSERR_NOERROR;
    result = ::waveInOpen(&wavein_, device_id, &format_,
        reinterpret_cast<DWORD_PTR>(WaveCallback),
        reinterpret_cast<DWORD_PTR>(this),
        CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR)
        return false;

    SetupBuffers();
    state_ = kStateReady;
    return true;
}

void PCMWaveInAudioInputStream::SetupBuffers()
{
    WAVEHDR* last = NULL;
    WAVEHDR* first = NULL;
    for (int ix = 0; ix != num_buffers_; ++ix) {
        uint32 sz = sizeof(WAVEHDR) + buffer_size_;
        buffer_ = reinterpret_cast<WAVEHDR*>(new char[sz]);
        buffer_->lpData = reinterpret_cast<char*>(buffer_) + sizeof(WAVEHDR);
        buffer_->dwBufferLength = buffer_size_;
        buffer_->dwBytesRecorded = 0;
        buffer_->dwUser = reinterpret_cast<DWORD_PTR>(last);
        buffer_->dwFlags = WHDR_DONE;
        buffer_->dwLoops = 0;
        if (ix == 0)
            first = buffer_;
        last = buffer_;
        ::waveInPrepareHeader(wavein_, buffer_, sizeof(WAVEHDR));
    }
    // Fix the first buffer to point to the last one.
    first->dwUser = reinterpret_cast<DWORD_PTR>(last);
}

void PCMWaveInAudioInputStream::FreeBuffers()
{
    WAVEHDR* current = buffer_;
    for (int ix = 0; ix != num_buffers_; ++ix) {
        WAVEHDR* next = GetNextBuffer(current);
        if (current->dwFlags & WHDR_PREPARED)
            ::waveInUnprepareHeader(wavein_, current, sizeof(WAVEHDR));
        delete[] reinterpret_cast<char*>(current);
        current = next;
    }
    buffer_ = NULL;
}

void PCMWaveInAudioInputStream::Start(AudioInputCallback* callback)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    if (state_ != kStateReady)
        return;

    DCHECK(!callback_);
    callback_ = callback;
    state_ = kStateRecording;

    WAVEHDR* buffer = buffer_;
    for (int ix = 0; ix != num_buffers_; ++ix) {
        QueueNextPacket(buffer);
        buffer = GetNextBuffer(buffer);
    }
    buffer = buffer_;

    MMRESULT result = ::waveInStart(wavein_);
    if (result != MMSYSERR_NOERROR) {
        HandleError(result);
        state_ = kStateReady;
        callback_ = NULL;
    }
}

// Stopping is tricky. First, no buffer should be locked by the audio driver
// or else the waveInReset() will deadlock and secondly, the callback should
// not be inside the AudioInputCallback's OnData because waveInReset()
// forcefully kills the callback thread.
void PCMWaveInAudioInputStream::Stop()
{
    DVLOG(1) << "PCMWaveInAudioInputStream::Stop()";
    DCHECK(thread_checker_.CalledOnValidThread());
    if (state_ != kStateRecording)
        return;

    bool already_stopped = false;
    {
        // Tell the callback that we're stopping.
        // As a result, |stopped_event_| will be signaled in callback method.
        base::AutoLock auto_lock(lock_);
        already_stopped = (callback_ == NULL);
        callback_ = NULL;
    }

    if (already_stopped)
        return;

    // Wait for the callback to finish, it will signal us when ready to be reset.
    DWORD wait = ::WaitForSingleObject(stopped_event_.Get(), INFINITE);
    DCHECK_EQ(wait, WAIT_OBJECT_0);

    // Stop input and reset the current position to zero for |wavein_|.
    // All pending buffers are marked as done and returned to the application.
    MMRESULT res = ::waveInReset(wavein_);
    DCHECK_EQ(res, static_cast<MMRESULT>(MMSYSERR_NOERROR));

    state_ = kStateReady;
}

void PCMWaveInAudioInputStream::Close()
{
    DVLOG(1) << "PCMWaveInAudioInputStream::Close()";
    DCHECK(thread_checker_.CalledOnValidThread());

    // We should not call Close() while recording. Catch it with DCHECK and
    // implement auto-stop just in case.
    DCHECK_NE(state_, kStateRecording);
    Stop();

    if (wavein_) {
        FreeBuffers();

        // waveInClose() generates a WIM_CLOSE callback.  In case Start() was never
        // called, force a reset to ensure close succeeds.
        MMRESULT res = ::waveInReset(wavein_);
        DCHECK_EQ(res, static_cast<MMRESULT>(MMSYSERR_NOERROR));
        res = ::waveInClose(wavein_);
        DCHECK_EQ(res, static_cast<MMRESULT>(MMSYSERR_NOERROR));
        state_ = kStateClosed;
        wavein_ = NULL;
    }

    // Tell the audio manager that we have been released. This can result in
    // the manager destroying us in-place so this needs to be the last thing
    // we do on this function.
    manager_->ReleaseInputStream(this);
}

double PCMWaveInAudioInputStream::GetMaxVolume()
{
    // TODO(henrika): Add volume support using the Audio Mixer API.
    return 0.0;
}

void PCMWaveInAudioInputStream::SetVolume(double volume)
{
    // TODO(henrika): Add volume support using the Audio Mixer API.
}

double PCMWaveInAudioInputStream::GetVolume()
{
    // TODO(henrika): Add volume support using the Audio Mixer API.
    return 0.0;
}

bool PCMWaveInAudioInputStream::SetAutomaticGainControl(bool enabled)
{
    // TODO(henrika): Add AGC support when volume control has been added.
    NOTIMPLEMENTED();
    return false;
}

bool PCMWaveInAudioInputStream::GetAutomaticGainControl()
{
    // TODO(henrika): Add AGC support when volume control has been added.
    NOTIMPLEMENTED();
    return false;
}

bool PCMWaveInAudioInputStream::IsMuted()
{
    NOTIMPLEMENTED();
    return false;
}

void PCMWaveInAudioInputStream::HandleError(MMRESULT error)
{
    DLOG(WARNING) << "PCMWaveInAudio error " << error;
    if (callback_)
        callback_->OnError(this);
}

void PCMWaveInAudioInputStream::QueueNextPacket(WAVEHDR* buffer)
{
    MMRESULT res = ::waveInAddBuffer(wavein_, buffer, sizeof(WAVEHDR));
    if (res != MMSYSERR_NOERROR)
        HandleError(res);
}

bool PCMWaveInAudioInputStream::GetDeviceId(UINT* device_index)
{
    // Deliver the default input device id (WAVE_MAPPER) if the default
    // device has been selected.
    if (device_id_ == AudioManagerBase::kDefaultDeviceId) {
        *device_index = WAVE_MAPPER;
        return true;
    }

    // Get list of all available and active devices.
    AudioDeviceNames device_names;
    if (!media::GetInputDeviceNamesWinXP(&device_names))
        return false;

    if (device_names.empty())
        return false;

    // Search the full list of devices and compare with the specified
    // device id which was specified in the constructor. Stop comparing
    // when a match is found and return the corresponding index.
    UINT index = 0;
    bool found_device = false;
    AudioDeviceNames::const_iterator it = device_names.begin();
    while (it != device_names.end()) {
        if (it->unique_id.compare(device_id_) == 0) {
            *device_index = index;
            found_device = true;
            break;
        }
        ++index;
        ++it;
    }

    return found_device;
}

// Windows calls us back in this function when some events happen. Most notably
// when it has an audio buffer with recorded data.
void PCMWaveInAudioInputStream::WaveCallback(HWAVEIN hwi, UINT msg,
    DWORD_PTR instance,
    DWORD_PTR param1, DWORD_PTR)
{
    PCMWaveInAudioInputStream* obj = reinterpret_cast<PCMWaveInAudioInputStream*>(instance);

    // The lock ensures that Stop() can't be called during a callback.
    base::AutoLock auto_lock(obj->lock_);

    if (msg == WIM_DATA) {
        // The WIM_DATA message is sent when waveform-audio data is present in
        // the input buffer and the buffer is being returned to the application.
        // The message can be sent when the buffer is full or after the
        // waveInReset function is called.
        if (obj->callback_) {
            // TODO(henrika): the |volume| parameter is always set to zero since
            // there is currently no support for controlling the microphone volume
            // level.
            WAVEHDR* buffer = reinterpret_cast<WAVEHDR*>(param1);
            obj->audio_bus_->FromInterleaved(reinterpret_cast<uint8*>(buffer->lpData),
                obj->audio_bus_->frames(),
                obj->format_.wBitsPerSample / 8);
            obj->callback_->OnData(
                obj, obj->audio_bus_.get(), buffer->dwBytesRecorded, 0.0);

            // Queue the finished buffer back with the audio driver. Since we are
            // reusing the same buffers we can get away without calling
            // waveInPrepareHeader.
            obj->QueueNextPacket(buffer);
        } else {
            // Main thread has called Stop() and set |callback_| to NULL and is
            // now waiting to issue waveInReset which will kill this thread.
            // We should not call AudioSourceCallback code anymore.
            ::SetEvent(obj->stopped_event_.Get());
        }
    } else if (msg == WIM_CLOSE) {
        // Intentionaly no-op for now.
    } else if (msg == WIM_OPEN) {
        // Intentionaly no-op for now.
    }
}

} // namespace media
