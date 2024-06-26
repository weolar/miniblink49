// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// MSVC++ requires this to be set before any other includes to get M_PI.
#define _USE_MATH_DEFINES
#include <cmath>

#include "media/audio/simple_sources.h"

#include <algorithm>

#include "base/files/file.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "media/audio/sounds/wav_audio_handler.h"
#include "media/base/audio_bus.h"

namespace media {

// Opens |wav_filename|, reads it and loads it as a wav file. This function will
// return a null pointer if we can't read the file or if it's malformed. The
// caller takes ownership of the returned data. The size of the data is stored
// in |read_length|.
static scoped_ptr<uint8[]> ReadWavFile(const base::FilePath& wav_filename,
    size_t* file_length)
{
    //     base::File wav_file(
    //         wav_filename, base::File::FLAG_OPEN | base::File::FLAG_READ);
    //     if (!wav_file.IsValid()) {
    //         LOG(ERROR) << "Failed to read " << wav_filename.value()
    //                    << " as input to the fake device.";
    //         return nullptr;
    //     }
    //
    //     size_t wav_file_length = wav_file.GetLength();
    //     if (wav_file_length == 0u) {
    //         LOG(ERROR) << "Input file to fake device is empty: "
    //                    << wav_filename.value();
    //         return nullptr;
    //     }
    //
    //     uint8* wav_file_data = new uint8[wav_file_length];
    //     size_t read_bytes = wav_file.Read(0, reinterpret_cast<char*>(wav_file_data),
    //         wav_file_length);
    //     if (read_bytes != wav_file_length) {
    //         LOG(ERROR) << "Failed to read all bytes of " << wav_filename.value();
    //         return nullptr;
    //     }
    //     *file_length = wav_file_length;
    //     return scoped_ptr<uint8[]>(wav_file_data);
    DebugBreak();
    return nullptr;
}

// Opens |wav_filename|, reads it and loads it as a wav file. This function will
// bluntly trigger CHECKs if we can't read the file or if it's malformed.
static scoped_ptr<WavAudioHandler> CreateWavAudioHandler(
    const base::FilePath& wav_filename, const uint8* wav_file_data,
    size_t wav_file_length, const AudioParameters& expected_params)
{
    base::StringPiece wav_data(reinterpret_cast<const char*>(wav_file_data),
        wav_file_length);
    scoped_ptr<WavAudioHandler> wav_audio_handler(new WavAudioHandler(wav_data));
    return wav_audio_handler.Pass();
}

// These values are based on experiments for local-to-local
// PeerConnection to demonstrate audio/video synchronization.
static const int kBeepDurationMilliseconds = 20;
static const int kBeepFrequency = 400;

// Intervals between two automatic beeps.
static const int kAutomaticBeepIntervalInMs = 500;

// Automatic beep will be triggered every |kAutomaticBeepIntervalInMs| unless
// users explicitly call BeepOnce(), which will disable the automatic beep.
class BeepContext {
public:
    BeepContext()
        : beep_once_(false)
        , automatic_beep_(true)
    {
    }

    void SetBeepOnce(bool enable)
    {
        base::AutoLock auto_lock(lock_);
        beep_once_ = enable;

        // Disable the automatic beep if users explicit set |beep_once_| to true.
        if (enable)
            automatic_beep_ = false;
    }

    bool beep_once() const
    {
        base::AutoLock auto_lock(lock_);
        return beep_once_;
    }

    bool automatic_beep() const
    {
        base::AutoLock auto_lock(lock_);
        return automatic_beep_;
    }

private:
    mutable base::Lock lock_;
    bool beep_once_;
    bool automatic_beep_;
};

static base::LazyInstance<BeepContext>::Leaky g_beep_context = LAZY_INSTANCE_INITIALIZER;

//////////////////////////////////////////////////////////////////////////////
// SineWaveAudioSource implementation.

SineWaveAudioSource::SineWaveAudioSource(int channels,
    double freq, double sample_freq)
    : channels_(channels)
    , f_(freq / sample_freq)
    , time_state_(0)
    , cap_(0)
    , callbacks_(0)
    , errors_(0)
{
}

SineWaveAudioSource::~SineWaveAudioSource()
{
}

// The implementation could be more efficient if a lookup table is constructed
// but it is efficient enough for our simple needs.
int SineWaveAudioSource::OnMoreData(AudioBus* audio_bus,
    uint32 total_bytes_delay)
{
    base::AutoLock auto_lock(time_lock_);
    callbacks_++;

    // The table is filled with s(t) = kint16max*sin(Theta*t),
    // where Theta = 2*PI*fs.
    // We store the discrete time value |t| in a member to ensure that the
    // next pass starts at a correct state.
    int max_frames = cap_ > 0 ? std::min(audio_bus->frames(), cap_ - time_state_) : audio_bus->frames();
    for (int i = 0; i < max_frames; ++i)
        audio_bus->channel(0)[i] = sin(2.0 * M_PI * f_ * time_state_++);
    for (int i = 1; i < audio_bus->channels(); ++i) {
        memcpy(audio_bus->channel(i), audio_bus->channel(0),
            max_frames * sizeof(*audio_bus->channel(i)));
    }
    return max_frames;
}

void SineWaveAudioSource::OnError(AudioOutputStream* stream)
{
    errors_++;
}

void SineWaveAudioSource::CapSamples(int cap)
{
    base::AutoLock auto_lock(time_lock_);
    DCHECK_GT(cap, 0);
    cap_ = cap;
}

void SineWaveAudioSource::Reset()
{
    base::AutoLock auto_lock(time_lock_);
    time_state_ = 0;
}

FileSource::FileSource(const AudioParameters& params,
    const base::FilePath& path_to_wav_file)
    : params_(params)
    , path_to_wav_file_(path_to_wav_file)
    , wav_file_read_pos_(0)
    , load_failed_(false)
{
}

FileSource::~FileSource()
{
}

void FileSource::LoadWavFile(const base::FilePath& path_to_wav_file)
{
    // Don't try again if we already failed.
    if (load_failed_)
        return;

    // Read the file, and put its data in a scoped_ptr so it gets deleted later.
    size_t file_length = 0;
    wav_file_data_ = ReadWavFile(path_to_wav_file, &file_length);
    if (!wav_file_data_) {
        load_failed_ = true;
        return;
    }

    wav_audio_handler_ = CreateWavAudioHandler(
        path_to_wav_file, wav_file_data_.get(), file_length, params_);

    // Hook us up so we pull in data from the file into the converter. We need to
    // modify the wav file's audio parameters since we'll be reading small slices
    // of it at a time and not the whole thing (like 10 ms at a time).
    AudioParameters file_audio_slice(
        AudioParameters::AUDIO_PCM_LOW_LATENCY,
        GuessChannelLayout(wav_audio_handler_->num_channels()),
        wav_audio_handler_->sample_rate(), wav_audio_handler_->bits_per_sample(),
        params_.frames_per_buffer());

    file_audio_converter_.reset(
        new AudioConverter(file_audio_slice, params_, false));
    file_audio_converter_->AddInput(this);
}

int FileSource::OnMoreData(AudioBus* audio_bus, uint32 total_bytes_delay)
{
    // Load the file if we haven't already. This load needs to happen on the
    // audio thread, otherwise we'll run on the UI thread on Mac for instance.
    // This will massively delay the first OnMoreData, but we'll catch up.
    if (!wav_audio_handler_)
        LoadWavFile(path_to_wav_file_);
    if (load_failed_)
        return 0;

    DCHECK(wav_audio_handler_.get());

    // Stop playing if we've played out the whole file.
    if (wav_audio_handler_->AtEnd(wav_file_read_pos_))
        return 0;

    // This pulls data from ProvideInput.
    file_audio_converter_->Convert(audio_bus);
    return audio_bus->frames();
}

double FileSource::ProvideInput(AudioBus* audio_bus_into_converter,
    base::TimeDelta buffer_delay)
{
    // Unfilled frames will be zeroed by CopyTo.
    size_t bytes_written;
    wav_audio_handler_->CopyTo(audio_bus_into_converter, wav_file_read_pos_,
        &bytes_written);
    wav_file_read_pos_ += bytes_written;
    return 1.0;
}

void FileSource::OnError(AudioOutputStream* stream)
{
}

BeepingSource::BeepingSource(const AudioParameters& params)
    : buffer_size_(params.GetBytesPerBuffer())
    , buffer_(new uint8[buffer_size_])
    , params_(params)
    , last_callback_time_(base::TimeTicks::Now())
    , beep_duration_in_buffers_(kBeepDurationMilliseconds * params.sample_rate() / params.frames_per_buffer() / 1000)
    , beep_generated_in_buffers_(0)
    , beep_period_in_frames_(params.sample_rate() / kBeepFrequency)
{
}

BeepingSource::~BeepingSource()
{
}

int BeepingSource::OnMoreData(AudioBus* audio_bus, uint32 total_bytes_delay)
{
    // Accumulate the time from the last beep.
    interval_from_last_beep_ += base::TimeTicks::Now() - last_callback_time_;

    memset(buffer_.get(), 0, buffer_size_);
    bool should_beep = false;
    BeepContext* beep_context = g_beep_context.Pointer();
    if (beep_context->automatic_beep()) {
        base::TimeDelta delta = interval_from_last_beep_ - base::TimeDelta::FromMilliseconds(kAutomaticBeepIntervalInMs);
        if (delta > base::TimeDelta()) {
            should_beep = true;
            interval_from_last_beep_ = delta;
        }
    } else {
        should_beep = beep_context->beep_once();
        beep_context->SetBeepOnce(false);
    }

    // If this object was instructed to generate a beep or has started to
    // generate a beep sound.
    if (should_beep || beep_generated_in_buffers_) {
        // Compute the number of frames to output high value. Then compute the
        // number of bytes based on channels and bits per channel.
        int high_frames = beep_period_in_frames_ / 2;
        int high_bytes = high_frames * params_.bits_per_sample() * params_.channels() / 8;

        // Separate high and low with the same number of bytes to generate a
        // square wave.
        int position = 0;
        while (position + high_bytes <= buffer_size_) {
            // Write high values first.
            memset(buffer_.get() + position, 128, high_bytes);
            // Then leave low values in the buffer with |high_bytes|.
            position += high_bytes * 2;
        }

        ++beep_generated_in_buffers_;
        if (beep_generated_in_buffers_ >= beep_duration_in_buffers_)
            beep_generated_in_buffers_ = 0;
    }

    last_callback_time_ = base::TimeTicks::Now();
    audio_bus->FromInterleaved(
        buffer_.get(), audio_bus->frames(), params_.bits_per_sample() / 8);
    return audio_bus->frames();
}

void BeepingSource::OnError(AudioOutputStream* stream)
{
}

void BeepingSource::BeepOnce()
{
    g_beep_context.Pointer()->SetBeepOnce(true);
}

} // namespace media
