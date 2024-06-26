// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// AudioRendererAlgorithm buffers and transforms audio data. The owner of
// this object provides audio data to the object through EnqueueBuffer() and
// requests data from the buffer via FillBuffer().
//
// This class is *not* thread-safe. Calls to enqueue and retrieve data must be
// locked if called from multiple threads.
//
// AudioRendererAlgorithm uses the Waveform Similarity Overlap and Add (WSOLA)
// algorithm to stretch or compress audio data to meet playback speeds less than
// or greater than the natural playback of the audio stream. The algorithm
// preserves local properties of the audio, therefore, pitch and harmonics are
// are preserved. See audio_renderer_algorith.cc for a more elaborate
// description of the algorithm.
//
// Audio at very low or very high playback rates are muted to preserve quality.

#ifndef MEDIA_FILTERS_AUDIO_RENDERER_ALGORITHM_H_
#define MEDIA_FILTERS_AUDIO_RENDERER_ALGORITHM_H_

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "media/audio/audio_parameters.h"
#include "media/base/audio_buffer.h"
#include "media/base/audio_buffer_queue.h"

namespace media {

class AudioBus;

class MEDIA_EXPORT AudioRendererAlgorithm {
public:
    AudioRendererAlgorithm();
    ~AudioRendererAlgorithm();

    // Initializes this object with information about the audio stream.
    void Initialize(const AudioParameters& params);

    // Tries to fill |requested_frames| frames into |dest| with possibly scaled
    // data from our |audio_buffer_|. Data is scaled based on |playback_rate|,
    // using a variation of the Overlap-Add method to combine sample windows.
    //
    // Data from |audio_buffer_| is consumed in proportion to the playback rate.
    //
    // |dest_offset| is the offset in frames for writing into |dest|.
    //
    // Returns the number of frames copied into |dest|.
    int FillBuffer(AudioBus* dest,
        int dest_offset,
        int requested_frames,
        double playback_rate);

    // Clears |audio_buffer_|.
    void FlushBuffers();

    // Enqueues a buffer. It is called from the owner of the algorithm after a
    // read completes.
    void EnqueueBuffer(const scoped_refptr<AudioBuffer>& buffer_in);

    // Returns true if |audio_buffer_| is at or exceeds capacity.
    bool IsQueueFull();

    // Returns the capacity of |audio_buffer_| in frames.
    int QueueCapacity() const { return capacity_; }

    // Increase the capacity of |audio_buffer_| if possible.
    void IncreaseQueueCapacity();

    // Returns an estimate of the amount of memory (in bytes) used for frames.
    int64_t GetMemoryUsage() const;

    // Returns the number of frames left in |audio_buffer_|, which may be larger
    // than QueueCapacity() in the event that EnqueueBuffer() delivered more data
    // than |audio_buffer_| was intending to hold.
    int frames_buffered() { return audio_buffer_.frames(); }

    // Returns the samples per second for this audio stream.
    int samples_per_second() { return samples_per_second_; }

private:
    // Within |search_block_|, find the block of data that is most similar to
    // |target_block_|, and write it in |optimal_block_|. This method assumes that
    // there is enough data to perform a search, i.e. |search_block_| and
    // |target_block_| can be extracted from the available frames.
    void GetOptimalBlock();

    // Read a maximum of |requested_frames| frames from |wsola_output_|. Returns
    // number of frames actually read.
    int WriteCompletedFramesTo(
        int requested_frames, int output_offset, AudioBus* dest);

    // Fill |dest| with frames from |audio_buffer_| starting from frame
    // |read_offset_frames|. |dest| is expected to have the same number of
    // channels as |audio_buffer_|. A negative offset, i.e.
    // |read_offset_frames| < 0, is accepted assuming that |audio_buffer| is zero
    // for negative indices. This might happen for few first frames. This method
    // assumes there is enough frames to fill |dest|, i.e. |read_offset_frames| +
    // |dest->frames()| does not extend to future.
    void PeekAudioWithZeroPrepend(int read_offset_frames, AudioBus* dest);

    // Run one iteration of WSOLA, if there are sufficient frames. This will
    // overlap-and-add one block to |wsola_output_|, hence, |num_complete_frames_|
    // is incremented by |ola_hop_size_|.
    bool RunOneWsolaIteration(double playback_rate);

    // Seek |audio_buffer_| forward to remove frames from input that are not used
    // any more. State of the WSOLA will be updated accordingly.
    void RemoveOldInputFrames(double playback_rate);

    // Update |output_time_| by |time_change|. In turn |search_block_index_| is
    // updated.
    void UpdateOutputTime(double playback_rate, double time_change);

    // Is |target_block_| fully within |search_block_|? If so, we don't need to
    // perform the search.
    bool TargetIsWithinSearchRegion() const;

    // Do we have enough data to perform one round of WSOLA?
    bool CanPerformWsola() const;

    // Converts a time in milliseconds to frames using |samples_per_second_|.
    int ConvertMillisecondsToFrames(int ms) const;

    // Number of channels in audio stream.
    int channels_;

    // Sample rate of audio stream.
    int samples_per_second_;

    // Buffered audio data.
    AudioBufferQueue audio_buffer_;

    // If muted, keep track of partial frames that should have been skipped over.
    double muted_partial_frame_;

    // How many frames to have in the queue before we report the queue is full.
    int capacity_;

    // Book keeping of the current time of generated audio, in frames. This
    // should be appropriately updated when out samples are generated, regardless
    // of whether we push samples out when FillBuffer() is called or we store
    // audio in |wsola_output_| for the subsequent calls to FillBuffer().
    // Furthermore, if samples from |audio_buffer_| are evicted then this
    // member variable should be updated based on |playback_rate_|.
    // Note that this member should be updated ONLY by calling UpdateOutputTime(),
    // so that |search_block_index_| is update accordingly.
    double output_time_;

    // The offset of the center frame of |search_block_| w.r.t. its first frame.
    int search_block_center_offset_;

    // Index of the beginning of the |search_block_|, in frames.
    int search_block_index_;

    // Number of Blocks to search to find the most similar one to the target
    // frame.
    int num_candidate_blocks_;

    // Index of the beginning of the target block, counted in frames.
    int target_block_index_;

    // Overlap-and-add window size in frames.
    int ola_window_size_;

    // The hop size of overlap-and-add in frames. This implementation assumes 50%
    // overlap-and-add.
    int ola_hop_size_;

    // Number of frames in |wsola_output_| that overlap-and-add is completed for
    // them and can be copied to output if FillBuffer() is called. It also
    // specifies the index where the next WSOLA window has to overlap-and-add.
    int num_complete_frames_;

    // This stores a part of the output that is created but couldn't be rendered.
    // Output is generated frame-by-frame which at some point might exceed the
    // number of requested samples. Furthermore, due to overlap-and-add,
    // the last half-window of the output is incomplete, which is stored in this
    // buffer.
    scoped_ptr<AudioBus> wsola_output_;

    // Overlap-and-add window.
    scoped_ptr<float[]> ola_window_;

    // Transition window, used to update |optimal_block_| by a weighted sum of
    // |optimal_block_| and |target_block_|.
    scoped_ptr<float[]> transition_window_;

    // Auxiliary variables to avoid allocation in every iteration.

    // Stores the optimal block in every iteration. This is the most
    // similar block to |target_block_| within |search_block_| and it is
    // overlap-and-added to |wsola_output_|.
    scoped_ptr<AudioBus> optimal_block_;

    // A block of data that search is performed over to find the |optimal_block_|.
    scoped_ptr<AudioBus> search_block_;

    // Stores the target block, denoted as |target| above. |search_block_| is
    // searched for a block (|optimal_block_|) that is most similar to
    // |target_block_|.
    scoped_ptr<AudioBus> target_block_;

    DISALLOW_COPY_AND_ASSIGN(AudioRendererAlgorithm);
};

} // namespace media

#endif // MEDIA_FILTERS_AUDIO_RENDERER_ALGORITHM_H_
