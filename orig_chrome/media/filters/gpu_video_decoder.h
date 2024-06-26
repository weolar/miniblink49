// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FILTERS_GPU_VIDEO_DECODER_H_
#define MEDIA_FILTERS_GPU_VIDEO_DECODER_H_

#include <list>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "base/memory/weak_ptr.h"
#include "media/base/pipeline_status.h"
#include "media/base/video_decoder.h"
#include "media/video/video_decode_accelerator.h"

template <class T>
class scoped_refptr;

namespace base {
class SharedMemory;
class SingleThreadTaskRunner;
}

namespace gpu {
struct SyncToken;
}

namespace media {

class DecoderBuffer;
class GpuVideoAcceleratorFactories;
class MediaLog;

// GPU-accelerated video decoder implementation.  Relies on
// AcceleratedVideoDecoderMsg_Decode and friends.  Can be created on any thread
// but must be accessed and destroyed on GpuVideoAcceleratorFactories's
// GetMessageLoop().
class MEDIA_EXPORT GpuVideoDecoder
    : public VideoDecoder,
      public VideoDecodeAccelerator::Client {
public:
    explicit GpuVideoDecoder(GpuVideoAcceleratorFactories* factories);

    // VideoDecoder implementation.
    std::string GetDisplayName() const override;
    void Initialize(const VideoDecoderConfig& config,
        bool low_delay,
        const InitCB& init_cb,
        const OutputCB& output_cb) override;
    void Decode(const scoped_refptr<DecoderBuffer>& buffer,
        const DecodeCB& decode_cb) override;
    void Reset(const base::Closure& closure) override;
    bool NeedsBitstreamConversion() const override;
    bool CanReadWithoutStalling() const override;
    int GetMaxDecodeRequests() const override;

    // VideoDecodeAccelerator::Client implementation.
    void ProvidePictureBuffers(uint32 count,
        const gfx::Size& size,
        uint32 texture_target) override;
    void DismissPictureBuffer(int32 id) override;
    void PictureReady(const media::Picture& picture) override;
    void NotifyEndOfBitstreamBuffer(int32 id) override;
    void NotifyFlushDone() override;
    void NotifyResetDone() override;
    void NotifyError(media::VideoDecodeAccelerator::Error error) override;

    static const char kDecoderName[];

protected:
    ~GpuVideoDecoder() override;

private:
    enum State {
        kNormal,
        kDrainingDecoder,
        kDecoderDrained,
        kError
    };

    // A shared memory segment and its allocated size.
    struct SHMBuffer {
        SHMBuffer(scoped_ptr<base::SharedMemory> m, size_t s);
        ~SHMBuffer();
        scoped_ptr<base::SharedMemory> shm;
        size_t size;
    };

    // A SHMBuffer and the DecoderBuffer its data came from.
    struct PendingDecoderBuffer {
        PendingDecoderBuffer(SHMBuffer* s,
            const scoped_refptr<DecoderBuffer>& b,
            const DecodeCB& done_cb);
        ~PendingDecoderBuffer();
        SHMBuffer* shm_buffer;
        scoped_refptr<DecoderBuffer> buffer;
        DecodeCB done_cb;
    };

    typedef std::map<int32, PictureBuffer> PictureBufferMap;

    void DeliverFrame(const scoped_refptr<VideoFrame>& frame);

    // Static method is to allow it to run even after GVD is deleted.
    static void ReleaseMailbox(base::WeakPtr<GpuVideoDecoder> decoder,
        media::GpuVideoAcceleratorFactories* factories,
        int64 picture_buffer_id,
        uint32 texture_id,
        const gpu::SyncToken& release_sync_token);
    // Indicate the picture buffer can be reused by the decoder.
    void ReusePictureBuffer(int64 picture_buffer_id);

    void RecordBufferData(
        const BitstreamBuffer& bitstream_buffer, const DecoderBuffer& buffer);
    void GetBufferData(int32 id, base::TimeDelta* timetamp,
        gfx::Rect* visible_rect, gfx::Size* natural_size);

    void DestroyVDA();

    // Request a shared-memory segment of at least |min_size| bytes.  Will
    // allocate as necessary.
    scoped_ptr<SHMBuffer> GetSHM(size_t min_size);

    // Return a shared-memory segment to the available pool.
    void PutSHM(scoped_ptr<SHMBuffer> shm_buffer);

    // Destroy all PictureBuffers in |buffers|, and delete their textures.
    void DestroyPictureBuffers(PictureBufferMap* buffers);

    // Returns true if the video decoder can support |profile| and |coded_size|.
    bool IsProfileSupported(VideoCodecProfile profile,
        const gfx::Size& coded_size);

    // Assert the contract that this class is operated on the right thread.
    void DCheckGpuVideoAcceleratorFactoriesTaskRunnerIsCurrent() const;

    bool needs_bitstream_conversion_;

    GpuVideoAcceleratorFactories* factories_;

    // Populated during Initialize() (on success) and unchanged until an error
    // occurs.
    scoped_ptr<VideoDecodeAccelerator> vda_;

    OutputCB output_cb_;

    DecodeCB eos_decode_cb_;

    // Not null only during reset.
    base::Closure pending_reset_cb_;

    State state_;

    VideoDecoderConfig config_;

    // Shared-memory buffer pool.  Since allocating SHM segments requires a
    // round-trip to the browser process, we keep allocation out of the
    // steady-state of the decoder.
    std::vector<SHMBuffer*> available_shm_segments_;

    std::map<int32, PendingDecoderBuffer> bitstream_buffers_in_decoder_;
    PictureBufferMap assigned_picture_buffers_;
    // PictureBuffers given to us by VDA via PictureReady, which we sent forward
    // as VideoFrames to be rendered via decode_cb_, and which will be returned
    // to us via ReusePictureBuffer.
    typedef std::map<int32 /* picture_buffer_id */, uint32 /* texture_id */>
        PictureBufferTextureMap;
    PictureBufferTextureMap picture_buffers_at_display_;

    // The texture target used for decoded pictures.
    uint32 decoder_texture_target_;

    struct BufferData {
        BufferData(int32 bbid, base::TimeDelta ts, const gfx::Rect& visible_rect,
            const gfx::Size& natural_size);
        ~BufferData();
        int32 bitstream_buffer_id;
        base::TimeDelta timestamp;
        gfx::Rect visible_rect;
        gfx::Size natural_size;
    };
    std::list<BufferData> input_buffer_data_;

    // picture_buffer_id and the frame wrapping the corresponding Picture, for
    // frames that have been decoded but haven't been requested by a Decode() yet.
    int32 next_picture_buffer_id_;
    int32 next_bitstream_buffer_id_;

    // Set during ProvidePictureBuffers(), used for checking and implementing
    // HasAvailableOutputFrames().
    int available_pictures_;

    // Bound to factories_->GetMessageLoop().
    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<GpuVideoDecoder> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(GpuVideoDecoder);
};

} // namespace media

#endif // MEDIA_FILTERS_GPU_VIDEO_DECODER_H_
