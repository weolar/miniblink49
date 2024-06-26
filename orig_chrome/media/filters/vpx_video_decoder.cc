// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/vpx_video_decoder.h"

#include <algorithm>
#include <string>
#include <vector>

#include "base/bind.h"
#include "base/callback_helpers.h"
#include "base/command_line.h"
#include "base/location.h"
#include "base/logging.h"
#include "base/metrics/histogram_macros.h"
#include "base/single_thread_task_runner.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/sys_byteorder.h"
#include "base/sys_info.h"
#include "base/trace_event/memory_allocator_dump.h"
//#include "base/trace_event/memory_dump_manager.h"
#include "base/trace_event/memory_dump_provider.h"
//#include "base/trace_event/process_memory_dump.h"
#include "base/trace_event/trace_event.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/decoder_buffer.h"
#include "media/base/limits.h"
#include "media/base/media_switches.h"
#include "media/base/pipeline.h"
#include "media/base/timestamp_constants.h"
#include "media/base/video_util.h"

// Include libvpx header files.
// VPX_CODEC_DISABLE_COMPAT excludes parts of the libvpx API that provide
// backwards compatibility for legacy applications using the library.
#define VPX_CODEC_DISABLE_COMPAT 1
extern "C" {
#include "third_party/libvpx_new/source/libvpx/vpx/vp8dx.h"
#include "third_party/libvpx_new/source/libvpx/vpx/vpx_decoder.h"
#include "third_party/libvpx_new/source/libvpx/vpx/vpx_frame_buffer.h"
}

namespace media {

// Always try to use three threads for video decoding.  There is little reason
// not to since current day CPUs tend to be multi-core and we measured
// performance benefits on older machines such as P4s with hyperthreading.
static const int kDecodeThreads = 2;
static const int kMaxDecodeThreads = 16;

// Returns the number of threads.
static int GetThreadCount(const VideoDecoderConfig& config)
{
    // Refer to http://crbug.com/93932 for tsan suppressions on decoding.
    int decode_threads = kDecodeThreads;

    const base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
    std::string threads(cmd_line->GetSwitchValueASCII(switches::kVideoThreads));
    if (threads.empty() || !base::StringToInt(threads, &decode_threads)) {
        if (config.codec() == kCodecVP9) {
            // For VP9 decode when using the default thread count, increase the number
            // of decode threads to equal the maximum number of tiles possible for
            // higher resolution streams.
            if (config.coded_size().width() >= 2048)
                decode_threads = 8;
            else if (config.coded_size().width() >= 1024)
                decode_threads = 4;
        }

        decode_threads = std::min(decode_threads,
            base::SysInfo::NumberOfProcessors());
        return decode_threads;
    }

    decode_threads = std::max(decode_threads, 0);
    decode_threads = std::min(decode_threads, kMaxDecodeThreads);
    return decode_threads;
}

class VpxVideoDecoder::MemoryPool
    : public base::RefCountedThreadSafe<VpxVideoDecoder::MemoryPool>,
      public base::trace_event::MemoryDumpProvider {
public:
    MemoryPool();

    // Callback that will be called by libvpx when it needs a frame buffer.
    // Parameters:
    // |user_priv|  Private data passed to libvpx (pointer to memory pool).
    // |min_size|   Minimum size needed by libvpx to decompress the next frame.
    // |fb|         Pointer to the frame buffer to update.
    // Returns 0 on success. Returns < 0 on failure.
    static int32 GetVP9FrameBuffer(void* user_priv, size_t min_size,
        vpx_codec_frame_buffer* fb);

    // Callback that will be called by libvpx when the frame buffer is no longer
    // being used by libvpx. Parameters:
    // |user_priv|  Private data passed to libvpx (pointer to memory pool).
    // |fb|         Pointer to the frame buffer that's being released.
    static int32 ReleaseVP9FrameBuffer(void* user_priv,
        vpx_codec_frame_buffer* fb);

    // Generates a "no_longer_needed" closure that holds a reference
    // to this pool.
    base::Closure CreateFrameCallback(void* fb_priv_data);

    bool OnMemoryDump(const base::trace_event::MemoryDumpArgs& args,
        base::trace_event::ProcessMemoryDump* pmd) override;

    int NumberOfFrameBuffersInUseByDecoder() const;
    int NumberOfFrameBuffersInUseByDecoderAndVideoFrame() const;

private:
    friend class base::RefCountedThreadSafe<VpxVideoDecoder::MemoryPool>;
    ~MemoryPool() override;

    // Reference counted frame buffers used for VP9 decoding. Reference counting
    // is done manually because both chromium and libvpx has to release this
    // before a buffer can be re-used.
    struct VP9FrameBuffer {
        VP9FrameBuffer()
            : ref_cnt(0)
        {
        }
        std::vector<uint8> data;
        uint32 ref_cnt;
    };

    // Gets the next available frame buffer for use by libvpx.
    VP9FrameBuffer* GetFreeFrameBuffer(size_t min_size);

    // Method that gets called when a VideoFrame that references this pool gets
    // destroyed.
    void OnVideoFrameDestroyed(VP9FrameBuffer* frame_buffer);

    // Frame buffers to be used by libvpx for VP9 Decoding.
    std::vector<VP9FrameBuffer*> frame_buffers_;

    // Number of VP9FrameBuffer currently in use by the decoder.
    int in_use_by_decoder_ = 0;
    // Number of VP9FrameBuffer currently in use by the decoder and a video frame.
    int in_use_by_decoder_and_video_frame_ = 0;
    DISALLOW_COPY_AND_ASSIGN(MemoryPool);
};

VpxVideoDecoder::MemoryPool::MemoryPool() { }

VpxVideoDecoder::MemoryPool::~MemoryPool()
{
    STLDeleteElements(&frame_buffers_);
}

VpxVideoDecoder::MemoryPool::VP9FrameBuffer*
VpxVideoDecoder::MemoryPool::GetFreeFrameBuffer(size_t min_size)
{
    // Check if a free frame buffer exists.
    size_t i = 0;
    for (; i < frame_buffers_.size(); ++i) {
        if (frame_buffers_[i]->ref_cnt == 0)
            break;
    }

    if (i == frame_buffers_.size()) {
        // Create a new frame buffer.
        frame_buffers_.push_back(new VP9FrameBuffer());
    }

    // Resize the frame buffer if necessary.
    if (frame_buffers_[i]->data.size() < min_size)
        frame_buffers_[i]->data.resize(min_size);
    return frame_buffers_[i];
}

int32 VpxVideoDecoder::MemoryPool::GetVP9FrameBuffer(
    void* user_priv, size_t min_size, vpx_codec_frame_buffer* fb)
{
    DCHECK(user_priv);
    DCHECK(fb);

    VpxVideoDecoder::MemoryPool* memory_pool = static_cast<VpxVideoDecoder::MemoryPool*>(user_priv);

    VP9FrameBuffer* fb_to_use = memory_pool->GetFreeFrameBuffer(min_size);
    if (fb_to_use == NULL)
        return -1;

    fb->data = &fb_to_use->data[0];
    fb->size = fb_to_use->data.size();
    ++fb_to_use->ref_cnt;
    ++memory_pool->in_use_by_decoder_;

    // Set the frame buffer's private data to point at the external frame buffer.
    fb->priv = static_cast<void*>(fb_to_use);
    return 0;
}

int32 VpxVideoDecoder::MemoryPool::ReleaseVP9FrameBuffer(
    void* user_priv, vpx_codec_frame_buffer* fb)
{
    DCHECK(user_priv);
    DCHECK(fb);
    VP9FrameBuffer* frame_buffer = static_cast<VP9FrameBuffer*>(fb->priv);
    --frame_buffer->ref_cnt;

    VpxVideoDecoder::MemoryPool* memory_pool = static_cast<VpxVideoDecoder::MemoryPool*>(user_priv);
    --memory_pool->in_use_by_decoder_;
    if (frame_buffer->ref_cnt)
        --memory_pool->in_use_by_decoder_and_video_frame_;
    return 0;
}

base::Closure VpxVideoDecoder::MemoryPool::CreateFrameCallback(
    void* fb_priv_data)
{
    VP9FrameBuffer* frame_buffer = static_cast<VP9FrameBuffer*>(fb_priv_data);
    ++frame_buffer->ref_cnt;
    if (frame_buffer->ref_cnt > 1)
        ++in_use_by_decoder_and_video_frame_;
    return BindToCurrentLoop(
        base::Bind(&MemoryPool::OnVideoFrameDestroyed, this,
            frame_buffer));
}

bool VpxVideoDecoder::MemoryPool::OnMemoryDump(
    const base::trace_event::MemoryDumpArgs& args,
    base::trace_event::ProcessMemoryDump* pmd)
{
    base::trace_event::MemoryAllocatorDump* memory_dump = pmd->CreateAllocatorDump("media/vpx/memory_pool");
    base::trace_event::MemoryAllocatorDump* used_memory_dump = pmd->CreateAllocatorDump("media/vpx/memory_pool/used");

    pmd->AddSuballocation(memory_dump->guid(),
        base::trace_event::MemoryDumpManager::GetInstance()
            ->system_allocator_pool_name());
    size_t bytes_used = 0;
    size_t bytes_reserved = 0;
    for (const VP9FrameBuffer* frame_buffer : frame_buffers_) {
        if (frame_buffer->ref_cnt) {
            bytes_used += frame_buffer->data.size();
        }
        bytes_reserved += frame_buffer->data.size();
    }

    memory_dump->AddScalar(base::trace_event::MemoryAllocatorDump::kNameSize,
        base::trace_event::MemoryAllocatorDump::kUnitsBytes,
        bytes_reserved);
    used_memory_dump->AddScalar(
        base::trace_event::MemoryAllocatorDump::kNameSize,
        base::trace_event::MemoryAllocatorDump::kUnitsBytes, bytes_used);

    return true;
}

int VpxVideoDecoder::MemoryPool::NumberOfFrameBuffersInUseByDecoder() const
{
    return in_use_by_decoder_;
}

int VpxVideoDecoder::MemoryPool::
    NumberOfFrameBuffersInUseByDecoderAndVideoFrame() const
{
    return in_use_by_decoder_and_video_frame_;
}

void VpxVideoDecoder::MemoryPool::OnVideoFrameDestroyed(
    VP9FrameBuffer* frame_buffer)
{
    --frame_buffer->ref_cnt;
    if (frame_buffer->ref_cnt)
        --in_use_by_decoder_and_video_frame_;
}

VpxVideoDecoder::VpxVideoDecoder(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner)
    : task_runner_(task_runner)
    , state_(kUninitialized)
    , vpx_codec_(NULL)
    , vpx_codec_alpha_(NULL)
{
}

VpxVideoDecoder::~VpxVideoDecoder()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    CloseDecoder();
}

std::string VpxVideoDecoder::GetDisplayName() const
{
    return "VpxVideoDecoder";
}

void VpxVideoDecoder::Initialize(const VideoDecoderConfig& config,
    bool low_delay,
    const InitCB& init_cb,
    const OutputCB& output_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(config.IsValidConfig());
    DCHECK(decode_cb_.is_null());

    InitCB bound_init_cb = BindToCurrentLoop(init_cb);

    if (config.is_encrypted() || !ConfigureDecoder(config)) {
        bound_init_cb.Run(false);
        return;
    }

    // Success!
    config_ = config;
    state_ = kNormal;
    output_cb_ = BindToCurrentLoop(output_cb);
    bound_init_cb.Run(true);
}

static vpx_codec_ctx* InitializeVpxContext(vpx_codec_ctx* context,
    const VideoDecoderConfig& config)
{
    context = new vpx_codec_ctx();
    vpx_codec_dec_cfg_t vpx_config = { 0 };
    vpx_config.w = config.coded_size().width();
    vpx_config.h = config.coded_size().height();
    vpx_config.threads = GetThreadCount(config);

    vpx_codec_err_t status = vpx_codec_dec_init(context,
        config.codec() == kCodecVP9 ? vpx_codec_vp9_dx() : vpx_codec_vp8_dx(),
        &vpx_config,
        0);
    if (status != VPX_CODEC_OK) {
        LOG(ERROR) << "vpx_codec_dec_init failed, status=" << status;
        delete context;
        return NULL;
    }
    return context;
}

bool VpxVideoDecoder::ConfigureDecoder(const VideoDecoderConfig& config)
{
    if (config.codec() != kCodecVP8 && config.codec() != kCodecVP9)
        return false;

#if !defined(DISABLE_FFMPEG_VIDEO_DECODERS)
    // When FFmpegVideoDecoder is available it handles VP8 that doesn't have
    // alpha, and VpxVideoDecoder will handle VP8 with alpha.
    if (config.codec() == kCodecVP8 && config.format() != PIXEL_FORMAT_YV12A)
        return false;
#endif

    CloseDecoder();

    vpx_codec_ = InitializeVpxContext(vpx_codec_, config);
    if (!vpx_codec_)
        return false;

    // We use our own buffers for VP9 so that there is no need to copy data after
    // decoding.
    if (config.codec() == kCodecVP9) {
        memory_pool_ = new MemoryPool();
        base::trace_event::MemoryDumpManager::GetInstance()->RegisterDumpProvider(
            memory_pool_.get(), "VpxVideoDecoder", task_runner_);
        if (vpx_codec_set_frame_buffer_functions(vpx_codec_,
                &MemoryPool::GetVP9FrameBuffer,
                &MemoryPool::ReleaseVP9FrameBuffer,
                memory_pool_.get())) {
            LOG(ERROR) << "Failed to configure external buffers.";
            return false;
        }
    }

    if (config.format() == PIXEL_FORMAT_YV12A) {
        vpx_codec_alpha_ = InitializeVpxContext(vpx_codec_alpha_, config);
        if (!vpx_codec_alpha_)
            return false;
    }

    return true;
}

void VpxVideoDecoder::CloseDecoder()
{
    if (vpx_codec_) {
        vpx_codec_destroy(vpx_codec_);
        delete vpx_codec_;
        vpx_codec_ = NULL;
        base::trace_event::MemoryDumpManager::GetInstance()->UnregisterDumpProvider(
            memory_pool_.get());
        memory_pool_ = NULL;
    }
    if (vpx_codec_alpha_) {
        vpx_codec_destroy(vpx_codec_alpha_);
        delete vpx_codec_alpha_;
        vpx_codec_alpha_ = NULL;
    }
}

void VpxVideoDecoder::Decode(const scoped_refptr<DecoderBuffer>& buffer,
    const DecodeCB& decode_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!decode_cb.is_null());
    CHECK_NE(state_, kUninitialized);
    CHECK(decode_cb_.is_null()) << "Overlapping decodes are not supported.";

    decode_cb_ = BindToCurrentLoop(decode_cb);

    if (state_ == kError) {
        base::ResetAndReturn(&decode_cb_).Run(kDecodeError);
        return;
    }

    // Return empty frames if decoding has finished.
    if (state_ == kDecodeFinished) {
        base::ResetAndReturn(&decode_cb_).Run(kOk);
        return;
    }

    DecodeBuffer(buffer);
}

void VpxVideoDecoder::Reset(const base::Closure& closure)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(decode_cb_.is_null());

    state_ = kNormal;
    task_runner_->PostTask(FROM_HERE, closure);
}

void VpxVideoDecoder::DecodeBuffer(const scoped_refptr<DecoderBuffer>& buffer)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_NE(state_, kUninitialized);
    DCHECK_NE(state_, kDecodeFinished);
    DCHECK_NE(state_, kError);
    DCHECK(!decode_cb_.is_null());
    DCHECK(buffer.get());

    // Transition to kDecodeFinished on the first end of stream buffer.
    if (state_ == kNormal && buffer->end_of_stream()) {
        state_ = kDecodeFinished;
        base::ResetAndReturn(&decode_cb_).Run(kOk);
        return;
    }

    scoped_refptr<VideoFrame> video_frame;
    if (!VpxDecode(buffer, &video_frame)) {
        state_ = kError;
        base::ResetAndReturn(&decode_cb_).Run(kDecodeError);
        return;
    }

    if (video_frame.get())
        output_cb_.Run(video_frame);

    // VideoDecoderShim expects that |decode_cb| is called only after
    // |output_cb_|.
    base::ResetAndReturn(&decode_cb_).Run(kOk);
}

bool VpxVideoDecoder::VpxDecode(const scoped_refptr<DecoderBuffer>& buffer,
    scoped_refptr<VideoFrame>* video_frame)
{
    DCHECK(video_frame);
    DCHECK(!buffer->end_of_stream());

    // Pass |buffer| to libvpx.
    int64 timestamp = buffer->timestamp().InMicroseconds();
    void* user_priv = reinterpret_cast<void*>(&timestamp);

    {
        TRACE_EVENT1("video", "vpx_codec_decode", "timestamp", timestamp);
        vpx_codec_err_t status = vpx_codec_decode(vpx_codec_,
            buffer->data(),
            buffer->data_size(),
            user_priv,
            0);
        if (status != VPX_CODEC_OK) {
            LOG(ERROR) << "vpx_codec_decode() failed, status=" << status;
            return false;
        }
    }

    // Gets pointer to decoded data.
    vpx_codec_iter_t iter = NULL;
    const vpx_image_t* vpx_image = vpx_codec_get_frame(vpx_codec_, &iter);
    if (!vpx_image) {
        *video_frame = NULL;
        return true;
    }

    if (vpx_image->user_priv != reinterpret_cast<void*>(&timestamp)) {
        LOG(ERROR) << "Invalid output timestamp.";
        return false;
    }

    const vpx_image_t* vpx_image_alpha = NULL;
    if (vpx_codec_alpha_ && buffer->side_data_size() >= 8) {
        // Pass alpha data to libvpx.
        int64 timestamp_alpha = buffer->timestamp().InMicroseconds();
        void* user_priv_alpha = reinterpret_cast<void*>(&timestamp_alpha);

        // First 8 bytes of side data is side_data_id in big endian.
        const uint64 side_data_id = base::NetToHost64(
            *(reinterpret_cast<const uint64*>(buffer->side_data())));
        if (side_data_id == 1) {
            {
                TRACE_EVENT1("video", "vpx_codec_decode_alpha",
                    "timestamp_alpha", timestamp_alpha);
                vpx_codec_err_t status = vpx_codec_decode(vpx_codec_alpha_,
                    buffer->side_data() + 8,
                    buffer->side_data_size() - 8,
                    user_priv_alpha,
                    0);
                if (status != VPX_CODEC_OK) {
                    LOG(ERROR) << "vpx_codec_decode() failed on alpha, status=" << status;
                    return false;
                }
            }

            // Gets pointer to decoded data.
            vpx_codec_iter_t iter_alpha = NULL;
            vpx_image_alpha = vpx_codec_get_frame(vpx_codec_alpha_, &iter_alpha);
            if (!vpx_image_alpha) {
                *video_frame = NULL;
                return true;
            }

            if (vpx_image_alpha->user_priv != reinterpret_cast<void*>(&timestamp_alpha)) {
                LOG(ERROR) << "Invalid output timestamp on alpha.";
                return false;
            }

            if (vpx_image_alpha->d_h != vpx_image->d_h || vpx_image_alpha->d_w != vpx_image->d_w) {
                LOG(ERROR) << "The alpha plane dimensions are not the same as the "
                              "image dimensions.";
                return false;
            }
        }
    }

    CopyVpxImageTo(vpx_image, vpx_image_alpha, video_frame);
    (*video_frame)->set_timestamp(base::TimeDelta::FromMicroseconds(timestamp));
    return true;
}

void VpxVideoDecoder::CopyVpxImageTo(const vpx_image* vpx_image,
    const struct vpx_image* vpx_image_alpha,
    scoped_refptr<VideoFrame>* video_frame)
{
    CHECK(vpx_image);
    CHECK(vpx_image->fmt == VPX_IMG_FMT_I420 || vpx_image->fmt == VPX_IMG_FMT_YV12 || vpx_image->fmt == VPX_IMG_FMT_I444);

    VideoPixelFormat codec_format = PIXEL_FORMAT_YV12;
    int uv_rows = (vpx_image->d_h + 1) / 2;

    if (vpx_image->fmt == VPX_IMG_FMT_I444) {
        CHECK(!vpx_codec_alpha_);
        codec_format = PIXEL_FORMAT_YV24;
        uv_rows = vpx_image->d_h;
    } else if (vpx_codec_alpha_) {
        codec_format = PIXEL_FORMAT_YV12A;
    }

    // Default to the color space from the config, but if the bistream specifies
    // one, prefer that instead.
    ColorSpace color_space = config_.color_space();
    if (vpx_image->cs == VPX_CS_BT_709)
        color_space = COLOR_SPACE_HD_REC709;
    else if (vpx_image->cs == VPX_CS_BT_601)
        color_space = COLOR_SPACE_SD_REC601;

    // The mixed |w|/|d_h| in |coded_size| is intentional. Setting the correct
    // coded width is necessary to allow coalesced memory access, which may avoid
    // frame copies. Setting the correct coded height however does not have any
    // benefit, and only risk copying too much data.
    const gfx::Size coded_size(vpx_image->w, vpx_image->d_h);
    const gfx::Size visible_size(vpx_image->d_w, vpx_image->d_h);

    if (!vpx_codec_alpha_ && memory_pool_.get()) {
        *video_frame = VideoFrame::WrapExternalYuvData(
            codec_format,
            coded_size, gfx::Rect(visible_size), config_.natural_size(),
            vpx_image->stride[VPX_PLANE_Y],
            vpx_image->stride[VPX_PLANE_U],
            vpx_image->stride[VPX_PLANE_V],
            vpx_image->planes[VPX_PLANE_Y],
            vpx_image->planes[VPX_PLANE_U],
            vpx_image->planes[VPX_PLANE_V],
            kNoTimestamp());
        video_frame->get()->AddDestructionObserver(
            memory_pool_->CreateFrameCallback(vpx_image->fb_priv));
        video_frame->get()->metadata()->SetInteger(VideoFrameMetadata::COLOR_SPACE,
            color_space);

        UMA_HISTOGRAM_COUNTS("Media.Vpx.VideoDecoderBuffersInUseByDecoder",
            memory_pool_->NumberOfFrameBuffersInUseByDecoder());
        UMA_HISTOGRAM_COUNTS(
            "Media.Vpx.VideoDecoderBuffersInUseByDecoderAndVideoFrame",
            memory_pool_->NumberOfFrameBuffersInUseByDecoderAndVideoFrame());

        return;
    }

    *video_frame = frame_pool_.CreateFrame(
        codec_format,
        visible_size,
        gfx::Rect(visible_size),
        config_.natural_size(),
        kNoTimestamp());
    video_frame->get()->metadata()->SetInteger(VideoFrameMetadata::COLOR_SPACE,
        color_space);

    CopyYPlane(vpx_image->planes[VPX_PLANE_Y],
        vpx_image->stride[VPX_PLANE_Y],
        vpx_image->d_h,
        video_frame->get());
    CopyUPlane(vpx_image->planes[VPX_PLANE_U],
        vpx_image->stride[VPX_PLANE_U],
        uv_rows,
        video_frame->get());
    CopyVPlane(vpx_image->planes[VPX_PLANE_V],
        vpx_image->stride[VPX_PLANE_V],
        uv_rows,
        video_frame->get());
    if (!vpx_codec_alpha_)
        return;
    if (!vpx_image_alpha) {
        MakeOpaqueAPlane(
            vpx_image->stride[VPX_PLANE_Y], vpx_image->d_h, video_frame->get());
        return;
    }
    CopyAPlane(vpx_image_alpha->planes[VPX_PLANE_Y],
        vpx_image_alpha->stride[VPX_PLANE_Y],
        vpx_image_alpha->d_h,
        video_frame->get());
}

} // namespace media
