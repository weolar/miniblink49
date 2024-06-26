// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/audio_renderer_algorithm.h"

#include <algorithm>
#include <cmath>

#include "base/logging.h"
#include "media/base/audio_bus.h"
#include "media/base/limits.h"
#include "media/filters/wsola_internals.h"

namespace media {

// Waveform Similarity Overlap-and-add (WSOLA).
//
// One WSOLA iteration
//
// 1) Extract |target_block_| as input frames at indices
//    [|target_block_index_|, |target_block_index_| + |ola_window_size_|).
//    Note that |target_block_| is the "natural" continuation of the output.
//
// 2) Extract |search_block_| as input frames at indices
//    [|search_block_index_|,
//     |search_block_index_| + |num_candidate_blocks_| + |ola_window_size_|).
//
// 3) Find a block within the |search_block_| that is most similar
//    to |target_block_|. Let |optimal_index| be the index of such block and
//    write it to |optimal_block_|.
//
// 4) Update:
//    |optimal_block_| = |transition_window_| * |target_block_| +
//    (1 - |transition_window_|) * |optimal_block_|.
//
// 5) Overlap-and-add |optimal_block_| to the |wsola_output_|.
//
// 6) Update:
//    |target_block_| = |optimal_index| + |ola_window_size_| / 2.
//    |output_index_| = |output_index_| + |ola_window_size_| / 2,
//    |search_block_center_offset_| = |output_index_| * |playback_rate|, and
//    |search_block_index_| = |search_block_center_offset_| -
//        |search_block_center_offset_|.

// Max/min supported playback rates for fast/slow audio. Audio outside of these
// ranges are muted.
// Audio at these speeds would sound better under a frequency domain algorithm.
static const double kMinPlaybackRate = 0.5;
static const double kMaxPlaybackRate = 4.0;

// Overlap-and-add window size in milliseconds.
static const int kOlaWindowSizeMs = 20;

// Size of search interval in milliseconds. The search interval is
// [-delta delta] around |output_index_| * |playback_rate|. So the search
// interval is 2 * delta.
static const int kWsolaSearchIntervalMs = 30;

// The maximum size in seconds for the |audio_buffer_|. Arbitrarily determined.
static const int kMaxCapacityInSeconds = 3;

// The minimum size in ms for the |audio_buffer_|. Arbitrarily determined.
static const int kStartingCapacityInMs = 200;

AudioRendererAlgorithm::AudioRendererAlgorithm()
    : channels_(0)
    , samples_per_second_(0)
    , muted_partial_frame_(0)
    , capacity_(0)
    , output_time_(0.0)
    , search_block_center_offset_(0)
    , search_block_index_(0)
    , num_candidate_blocks_(0)
    , target_block_index_(0)
    , ola_window_size_(0)
    , ola_hop_size_(0)
    , num_complete_frames_(0)
{
}

AudioRendererAlgorithm::~AudioRendererAlgorithm() { }

void AudioRendererAlgorithm::Initialize(const AudioParameters& params)
{
    CHECK(params.IsValid());

    channels_ = params.channels();
    samples_per_second_ = params.sample_rate();
    capacity_ = ConvertMillisecondsToFrames(kStartingCapacityInMs);
    num_candidate_blocks_ = ConvertMillisecondsToFrames(kWsolaSearchIntervalMs);
    ola_window_size_ = ConvertMillisecondsToFrames(kOlaWindowSizeMs);

    // Make sure window size in an even number.
    ola_window_size_ += ola_window_size_ & 1;
    ola_hop_size_ = ola_window_size_ / 2;

    // |num_candidate_blocks_| / 2 is the offset of the center of the search
    // block to the center of the first (left most) candidate block. The offset
    // of the center of a candidate block to its left most point is
    // |ola_window_size_| / 2 - 1. Note that |ola_window_size_| is even and in
    // our convention the center belongs to the left half, so we need to subtract
    // one frame to get the correct offset.
    //
    //                             Search Block
    //              <------------------------------------------->
    //
    //   |ola_window_size_| / 2 - 1
    //              <----
    //
    //             |num_candidate_blocks_| / 2
    //                   <----------------
    //                                 center
    //              X----X----------------X---------------X-----X
    //              <---------->                     <---------->
    //                Candidate      ...               Candidate
    //                   1,          ...         |num_candidate_blocks_|
    search_block_center_offset_ = num_candidate_blocks_ / 2 + (ola_window_size_ / 2 - 1);

    ola_window_.reset(new float[ola_window_size_]);
    internal::GetSymmetricHanningWindow(ola_window_size_, ola_window_.get());

    transition_window_.reset(new float[ola_window_size_ * 2]);
    internal::GetSymmetricHanningWindow(2 * ola_window_size_,
        transition_window_.get());

    wsola_output_ = AudioBus::Create(channels_, ola_window_size_ + ola_hop_size_);
    wsola_output_->Zero(); // Initialize for overlap-and-add of the first block.

    // Auxiliary containers.
    optimal_block_ = AudioBus::Create(channels_, ola_window_size_);
    search_block_ = AudioBus::Create(
        channels_, num_candidate_blocks_ + (ola_window_size_ - 1));
    target_block_ = AudioBus::Create(channels_, ola_window_size_);
}

int AudioRendererAlgorithm::FillBuffer(AudioBus* dest,
    int dest_offset,
    int requested_frames,
    double playback_rate)
{
    if (playback_rate == 0)
        return 0;

    DCHECK_EQ(channels_, dest->channels());

    // Optimize the muted case to issue a single clear instead of performing
    // the full crossfade and clearing each crossfaded frame.
    if (playback_rate < kMinPlaybackRate || playback_rate > kMaxPlaybackRate) {
        int frames_to_render = std::min(static_cast<int>(audio_buffer_.frames() / playback_rate),
            requested_frames);

        // Compute accurate number of frames to actually skip in the source data.
        // Includes the leftover partial frame from last request. However, we can
        // only skip over complete frames, so a partial frame may remain for next
        // time.
        muted_partial_frame_ += frames_to_render * playback_rate;
        int seek_frames = static_cast<int>(muted_partial_frame_);
        dest->ZeroFramesPartial(dest_offset, frames_to_render);
        audio_buffer_.SeekFrames(seek_frames);

        // Determine the partial frame that remains to be skipped for next call. If
        // the user switches back to playing, it may be off time by this partial
        // frame, which would be undetectable. If they subsequently switch to
        // another playback rate that mutes, the code will attempt to line up the
        // frames again.
        muted_partial_frame_ -= seek_frames;
        return frames_to_render;
    }

    int slower_step = ceil(ola_window_size_ * playback_rate);
    int faster_step = ceil(ola_window_size_ / playback_rate);

    // Optimize the most common |playback_rate| ~= 1 case to use a single copy
    // instead of copying frame by frame.
    if (ola_window_size_ <= faster_step && slower_step >= ola_window_size_) {
        const int frames_to_copy = std::min(audio_buffer_.frames(), requested_frames);
        const int frames_read = audio_buffer_.ReadFrames(frames_to_copy, dest_offset, dest);
        DCHECK_EQ(frames_read, frames_to_copy);
        return frames_read;
    }

    int rendered_frames = 0;
    do {
        rendered_frames += WriteCompletedFramesTo(requested_frames - rendered_frames,
            dest_offset + rendered_frames, dest);
    } while (rendered_frames < requested_frames && RunOneWsolaIteration(playback_rate));
    return rendered_frames;
}

void AudioRendererAlgorithm::FlushBuffers()
{
    // Clear the queue of decoded packets (releasing the buffers).
    audio_buffer_.Clear();
    output_time_ = 0.0;
    search_block_index_ = 0;
    target_block_index_ = 0;
    wsola_output_->Zero();
    num_complete_frames_ = 0;

    // Reset |capacity_| so growth triggered by underflows doesn't penalize seek
    // time.
    capacity_ = ConvertMillisecondsToFrames(kStartingCapacityInMs);
}

void AudioRendererAlgorithm::EnqueueBuffer(
    const scoped_refptr<AudioBuffer>& buffer_in)
{
    DCHECK(!buffer_in->end_of_stream());
    audio_buffer_.Append(buffer_in);
}

bool AudioRendererAlgorithm::IsQueueFull()
{
    return audio_buffer_.frames() >= capacity_;
}

void AudioRendererAlgorithm::IncreaseQueueCapacity()
{
    int max_capacity = kMaxCapacityInSeconds * samples_per_second_;
    DCHECK_LE(capacity_, max_capacity);

    capacity_ = std::min(2 * capacity_, max_capacity);
}

int64_t AudioRendererAlgorithm::GetMemoryUsage() const
{
    return audio_buffer_.frames() * channels_ * sizeof(float);
}

bool AudioRendererAlgorithm::CanPerformWsola() const
{
    const int search_block_size = num_candidate_blocks_ + (ola_window_size_ - 1);
    const int frames = audio_buffer_.frames();
    return target_block_index_ + ola_window_size_ <= frames && search_block_index_ + search_block_size <= frames;
}

int AudioRendererAlgorithm::ConvertMillisecondsToFrames(int ms) const
{
    return ms * (samples_per_second_ / static_cast<double>(base::Time::kMillisecondsPerSecond));
}

bool AudioRendererAlgorithm::RunOneWsolaIteration(double playback_rate)
{
    if (!CanPerformWsola())
        return false;

    GetOptimalBlock();

    // Overlap-and-add.
    for (int k = 0; k < channels_; ++k) {
        const float* const ch_opt_frame = optimal_block_->channel(k);
        float* ch_output = wsola_output_->channel(k) + num_complete_frames_;
        for (int n = 0; n < ola_hop_size_; ++n) {
            ch_output[n] = ch_output[n] * ola_window_[ola_hop_size_ + n] + ch_opt_frame[n] * ola_window_[n];
        }

        // Copy the second half to the output.
        memcpy(&ch_output[ola_hop_size_], &ch_opt_frame[ola_hop_size_],
            sizeof(*ch_opt_frame) * ola_hop_size_);
    }

    num_complete_frames_ += ola_hop_size_;
    UpdateOutputTime(playback_rate, ola_hop_size_);
    RemoveOldInputFrames(playback_rate);
    return true;
}

void AudioRendererAlgorithm::UpdateOutputTime(double playback_rate,
    double time_change)
{
    output_time_ += time_change;
    // Center of the search region, in frames.
    const int search_block_center_index = static_cast<int>(
        output_time_ * playback_rate + 0.5);
    search_block_index_ = search_block_center_index - search_block_center_offset_;
}

void AudioRendererAlgorithm::RemoveOldInputFrames(double playback_rate)
{
    const int earliest_used_index = std::min(target_block_index_,
        search_block_index_);
    if (earliest_used_index <= 0)
        return; // Nothing to remove.

    // Remove frames from input and adjust indices accordingly.
    audio_buffer_.SeekFrames(earliest_used_index);
    target_block_index_ -= earliest_used_index;

    // Adjust output index.
    double output_time_change = static_cast<double>(earliest_used_index) / playback_rate;
    CHECK_GE(output_time_, output_time_change);
    UpdateOutputTime(playback_rate, -output_time_change);
}

int AudioRendererAlgorithm::WriteCompletedFramesTo(
    int requested_frames, int dest_offset, AudioBus* dest)
{
    int rendered_frames = std::min(num_complete_frames_, requested_frames);

    if (rendered_frames == 0)
        return 0; // There is nothing to read from |wsola_output_|, return.

    wsola_output_->CopyPartialFramesTo(0, rendered_frames, dest_offset, dest);

    // Remove the frames which are read.
    int frames_to_move = wsola_output_->frames() - rendered_frames;
    for (int k = 0; k < channels_; ++k) {
        float* ch = wsola_output_->channel(k);
        memmove(ch, &ch[rendered_frames], sizeof(*ch) * frames_to_move);
    }
    num_complete_frames_ -= rendered_frames;
    return rendered_frames;
}

bool AudioRendererAlgorithm::TargetIsWithinSearchRegion() const
{
    const int search_block_size = num_candidate_blocks_ + (ola_window_size_ - 1);

    return target_block_index_ >= search_block_index_ && target_block_index_ + ola_window_size_ <= search_block_index_ + search_block_size;
}

void AudioRendererAlgorithm::GetOptimalBlock()
{
    int optimal_index = 0;

    // An interval around last optimal block which is excluded from the search.
    // This is to reduce the buzzy sound. The number 160 is rather arbitrary and
    // derived heuristically.
    const int kExcludeIntervalLengthFrames = 160;
    if (TargetIsWithinSearchRegion()) {
        optimal_index = target_block_index_;
        PeekAudioWithZeroPrepend(optimal_index, optimal_block_.get());
    } else {
        PeekAudioWithZeroPrepend(target_block_index_, target_block_.get());
        PeekAudioWithZeroPrepend(search_block_index_, search_block_.get());
        int last_optimal = target_block_index_ - ola_hop_size_ - search_block_index_;
        internal::Interval exclude_iterval = std::make_pair(
            last_optimal - kExcludeIntervalLengthFrames / 2,
            last_optimal + kExcludeIntervalLengthFrames / 2);

        // |optimal_index| is in frames and it is relative to the beginning of the
        // |search_block_|.
        optimal_index = internal::OptimalIndex(
            search_block_.get(), target_block_.get(), exclude_iterval);

        // Translate |index| w.r.t. the beginning of |audio_buffer_| and extract the
        // optimal block.
        optimal_index += search_block_index_;
        PeekAudioWithZeroPrepend(optimal_index, optimal_block_.get());

        // Make a transition from target block to the optimal block if different.
        // Target block has the best continuation to the current output.
        // Optimal block is the most similar block to the target, however, it might
        // introduce some discontinuity when over-lap-added. Therefore, we combine
        // them for a smoother transition. The length of transition window is twice
        // as that of the optimal-block which makes it like a weighting function
        // where target-block has higher weight close to zero (weight of 1 at index
        // 0) and lower weight close the end.
        for (int k = 0; k < channels_; ++k) {
            float* ch_opt = optimal_block_->channel(k);
            const float* const ch_target = target_block_->channel(k);
            for (int n = 0; n < ola_window_size_; ++n) {
                ch_opt[n] = ch_opt[n] * transition_window_[n] + ch_target[n] * transition_window_[ola_window_size_ + n];
            }
        }
    }

    // Next target is one hop ahead of the current optimal.
    target_block_index_ = optimal_index + ola_hop_size_;
}

void AudioRendererAlgorithm::PeekAudioWithZeroPrepend(
    int read_offset_frames, AudioBus* dest)
{
    CHECK_LE(read_offset_frames + dest->frames(), audio_buffer_.frames());

    int write_offset = 0;
    int num_frames_to_read = dest->frames();
    if (read_offset_frames < 0) {
        int num_zero_frames_appended = std::min(-read_offset_frames,
            num_frames_to_read);
        read_offset_frames = 0;
        num_frames_to_read -= num_zero_frames_appended;
        write_offset = num_zero_frames_appended;
        dest->ZeroFrames(num_zero_frames_appended);
    }
    audio_buffer_.PeekFrames(num_frames_to_read, read_offset_frames,
        write_offset, dest);
}

} // namespace media
