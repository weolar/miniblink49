// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_FRAME_H_
#define MEDIA_BASE_VIDEO_FRAME_H_

#include <vector>

#include "base/callback.h"
#include "base/macros.h"
#include "base/md5.h"
#include "base/memory/shared_memory.h"
#include "base/synchronization/lock.h"
#include "gpu/command_buffer/common/mailbox_holder.h"
#include "media/base/video_frame_metadata.h"
#include "media/base/video_types.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/gpu_memory_buffer.h"

#if defined(OS_MACOSX)
#include "base/mac/scoped_cftyperef.h"
#include <CoreVideo/CVPixelBuffer.h>
#endif

namespace media {

class MEDIA_EXPORT VideoFrame : public base::RefCountedThreadSafe<VideoFrame> {
public:
    enum {
        kFrameSizeAlignment = 16,
        kFrameSizePadding = 16,
        kFrameAddressAlignment = 32
    };

    enum {
        kMaxPlanes = 4,

        kYPlane = 0,
        kARGBPlane = kYPlane,
        kUPlane = 1,
        kUVPlane = kUPlane,
        kVPlane = 2,
        kAPlane = 3,
    };

    // Defines the pixel storage type. Differentiates between directly accessible
    // |data_| and pixels that are only indirectly accessible and not via mappable
    // memory.
    // Note that VideoFrames of any StorageType can also have Texture backing,
    // with "classical" GPU Driver-only textures identified as STORAGE_OPAQUE.
    enum StorageType {
        STORAGE_UNKNOWN = 0,
        STORAGE_OPAQUE = 1, // We don't know how VideoFrame's pixels are stored.
        STORAGE_UNOWNED_MEMORY = 2, // External, non owned data pointers.
        STORAGE_OWNED_MEMORY = 3, // VideoFrame has allocated its own data buffer.
        STORAGE_SHMEM = 4, // Pixels are backed by Shared Memory.
#if defined(OS_LINUX)
        // TODO(mcasas): Consider turning this type into STORAGE_NATIVE or another
        // meaningful name and handle it appropriately in all cases.
        STORAGE_DMABUFS = 5, // Each plane is stored into a DmaBuf.
#endif
#if defined(VIDEO_HOLE)
        // Indicates protected media that needs to be directly rendered to hw. It
        // is, in principle, platform independent, see http://crbug.com/323157 and
        // https://groups.google.com/a/google.com/d/topic/chrome-gpu/eIM1RwarUmk/discussion
        STORAGE_HOLE = 6,
#endif
        STORAGE_GPU_MEMORY_BUFFERS = 7,
        STORAGE_LAST = STORAGE_GPU_MEMORY_BUFFERS,
    };

    // CB to be called on the mailbox backing this frame when the frame is
    // destroyed.
    typedef base::Callback<void(const gpu::SyncToken&)> ReleaseMailboxCB;

    // Interface representing client operations on a SyncToken, i.e. insert one in
    // the GPU Command Buffer and wait for it.
    class SyncTokenClient {
    public:
        SyncTokenClient() { }
        virtual void GenerateSyncToken(gpu::SyncToken* sync_token) = 0;
        virtual void WaitSyncToken(const gpu::SyncToken& sync_token) = 0;

    protected:
        virtual ~SyncTokenClient() { }

        DISALLOW_COPY_AND_ASSIGN(SyncTokenClient);
    };

    // Call prior to CreateFrame to ensure validity of frame configuration. Called
    // automatically by VideoDecoderConfig::IsValidConfig().
    static bool IsValidConfig(VideoPixelFormat format,
        StorageType storage_type,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size);

    // Creates a new YUV frame in system memory with given parameters (|format|
    // must be YUV). Buffers for the frame are allocated but not initialized. The
    // caller most not make assumptions about the actual underlying size(s), but
    // check the returned VideoFrame instead.
    // TODO(mcasas): implement the RGB version of this factory method.
    static scoped_refptr<VideoFrame> CreateFrame(VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp);

    // Offers the same functionality as CreateFrame, and additionally zeroes out
    // the initial allocated buffers.
    static scoped_refptr<VideoFrame> CreateZeroInitializedFrame(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp);

    // Wraps a native texture of the given parameters with a VideoFrame.
    // The backing of the VideoFrame is held in the mailbox held by
    // |mailbox_holder|, and |mailbox_holder_release_cb| will be called with
    // a sync token as the argument when the VideoFrame is to be destroyed.
    static scoped_refptr<VideoFrame> WrapNativeTexture(
        VideoPixelFormat format,
        const gpu::MailboxHolder& mailbox_holder,
        const ReleaseMailboxCB& mailbox_holder_release_cb,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp);

    // Wraps a set of native textures representing YUV data with a VideoFrame.
    // |mailbox_holders_release_cb| will be called with a sync token as the
    // argument when the VideoFrame is to be destroyed.
    static scoped_refptr<VideoFrame> WrapYUV420NativeTextures(
        const gpu::MailboxHolder& y_mailbox_holder,
        const gpu::MailboxHolder& u_mailbox_holder,
        const gpu::MailboxHolder& v_mailbox_holder,
        const ReleaseMailboxCB& mailbox_holders_release_cb,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp);

    // Wraps packed image data residing in a memory buffer with a VideoFrame.
    // The image data resides in |data| and is assumed to be packed tightly in a
    // buffer of logical dimensions |coded_size| with the appropriate bit depth
    // and plane count as given by |format|. Returns NULL on failure.
    static scoped_refptr<VideoFrame> WrapExternalData(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        uint8* data,
        size_t data_size,
        base::TimeDelta timestamp);

    // Same as WrapExternalData() with SharedMemoryHandle and its offset.
    static scoped_refptr<VideoFrame> WrapExternalSharedMemory(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        uint8* data,
        size_t data_size,
        base::SharedMemoryHandle handle,
        size_t shared_memory_offset,
        base::TimeDelta timestamp);

    // Wraps external YUV data of the given parameters with a VideoFrame.
    // The returned VideoFrame does not own the data passed in.
    static scoped_refptr<VideoFrame> WrapExternalYuvData(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        int32 y_stride,
        int32 u_stride,
        int32 v_stride,
        uint8* y_data,
        uint8* u_data,
        uint8* v_data,
        base::TimeDelta timestamp);

    // Wraps external YUV data with the given parameters with a VideoFrame.
    // The returned VideoFrame does not own the GpuMemoryBuffers passed in.
    static scoped_refptr<VideoFrame> WrapExternalYuvGpuMemoryBuffers(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        int32 y_stride,
        int32 u_stride,
        int32 v_stride,
        uint8* y_data,
        uint8* u_data,
        uint8* v_data,
        const gfx::GpuMemoryBufferHandle& y_handle,
        const gfx::GpuMemoryBufferHandle& u_handle,
        const gfx::GpuMemoryBufferHandle& v_handle,
        base::TimeDelta timestamp);

#if defined(OS_LINUX)
    // Wraps provided dmabufs
    // (https://www.kernel.org/doc/Documentation/dma-buf-sharing.txt) with a
    // VideoFrame. The dmabuf fds are dup()ed on creation, so that the VideoFrame
    // retains a reference to them, and are automatically close()d on destruction,
    // dropping the reference. The caller may safely close() its reference after
    // calling WrapExternalDmabufs().
    // The image data is only accessible via dmabuf fds, which are usually passed
    // directly to a hardware device and/or to another process, or can also be
    // mapped via mmap() for CPU access.
    // Returns NULL on failure.
    static scoped_refptr<VideoFrame> WrapExternalDmabufs(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        const std::vector<int>& dmabuf_fds,
        base::TimeDelta timestamp);
#endif

#if defined(OS_MACOSX)
    // Wraps a provided CVPixelBuffer with a VideoFrame. The pixel buffer is
    // retained for the lifetime of the VideoFrame and released upon destruction.
    // The image data is only accessible via the pixel buffer, which could be
    // backed by an IOSurface from another process. All the attributes of the
    // VideoFrame are derived from the pixel buffer, with the exception of the
    // timestamp. If information is missing or is incompatible (for example, a
    // pixel format that has no VideoFrame match), NULL is returned.
    // http://crbug.com/401308
    static scoped_refptr<VideoFrame> WrapCVPixelBuffer(
        CVPixelBufferRef cv_pixel_buffer,
        base::TimeDelta timestamp);
#endif

    // Wraps |frame|. |visible_rect| must be a sub rect within
    // frame->visible_rect().
    static scoped_refptr<VideoFrame> WrapVideoFrame(
        const scoped_refptr<VideoFrame>& frame,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size);

    // Creates a frame which indicates end-of-stream.
    static scoped_refptr<VideoFrame> CreateEOSFrame();

    // Allocates YV12 frame based on |size|, and sets its data to the YUV(y,u,v).
    static scoped_refptr<VideoFrame> CreateColorFrame(
        const gfx::Size& size,
        uint8 y, uint8 u, uint8 v,
        base::TimeDelta timestamp);

    // Allocates YV12 frame based on |size|, and sets its data to the YUV
    // equivalent of RGB(0,0,0).
    static scoped_refptr<VideoFrame> CreateBlackFrame(const gfx::Size& size);

    // Allocates YV12A frame based on |size|, and sets its data to the YUVA
    // equivalent of RGBA(0,0,0,0).
    static scoped_refptr<VideoFrame> CreateTransparentFrame(
        const gfx::Size& size);

#if defined(VIDEO_HOLE)
    // Allocates a hole frame.
    static scoped_refptr<VideoFrame> CreateHoleFrame(const gfx::Size& size);
#endif // defined(VIDEO_HOLE)

    static size_t NumPlanes(VideoPixelFormat format);

    // Returns the required allocation size for a (tightly packed) frame of the
    // given coded size and format.
    static size_t AllocationSize(VideoPixelFormat format,
        const gfx::Size& coded_size);

    // Returns the plane gfx::Size (in bytes) for a plane of the given coded size
    // and format.
    static gfx::Size PlaneSize(VideoPixelFormat format,
        size_t plane,
        const gfx::Size& coded_size);

    // Returns horizontal bits per pixel for given |plane| and |format|.
    static int PlaneHorizontalBitsPerPixel(VideoPixelFormat format, size_t plane);

    // Returns bits per pixel for given |plane| and |format|.
    static int PlaneBitsPerPixel(VideoPixelFormat format, size_t plane);

    // Returns the number of bytes per row for the given plane, format, and width.
    // The width may be aligned to format requirements.
    static size_t RowBytes(size_t plane, VideoPixelFormat format, int width);

    // Returns the number of rows for the given plane, format, and height.
    // The height may be aligned to format requirements.
    static size_t Rows(size_t plane, VideoPixelFormat format, int height);

    // Returns the number of columns for the given plane, format, and width.
    // The width may be aligned to format requirements.
    static size_t Columns(size_t plane, VideoPixelFormat format, int width);

    // Used to keep a running hash of seen frames.  Expects an initialized MD5
    // context.  Calls MD5Update with the context and the contents of the frame.
    static void HashFrameForTesting(base::MD5Context* context,
        const scoped_refptr<VideoFrame>& frame);

    // Returns true if |frame| is accessible and mapped in the VideoFrame memory
    // space. If false, clients should refrain from accessing data(),
    // visible_data() etc.
    bool IsMappable() const;

    // Returns true if |frame| has textures with any StorageType and should not be
    // accessed via data(), visible_data() etc.
    bool HasTextures() const;

    VideoPixelFormat format() const { return format_; }
    StorageType storage_type() const { return storage_type_; }

    const gfx::Size& coded_size() const { return coded_size_; }
    const gfx::Rect& visible_rect() const { return visible_rect_; }
    const gfx::Size& natural_size() const { return natural_size_; }

    int stride(size_t plane) const;

    // Returns the number of bytes per row and number of rows for a given plane.
    //
    // As opposed to stride(), row_bytes() refers to the bytes representing
    // frame data scanlines (coded_size.width() pixels, without stride padding).
    int row_bytes(size_t plane) const;
    int rows(size_t plane) const;

    // Returns pointer to the buffer for a given plane, if this is an
    // IsMappable() frame type. The memory is owned by VideoFrame object and must
    // not be freed by the caller.
    const uint8* data(size_t plane) const;
    uint8* data(size_t plane);

    // Returns pointer to the data in the visible region of the frame, for
    // IsMappable() storage types. The returned pointer is offsetted into the
    // plane buffer specified by visible_rect().origin(). Memory is owned by
    // VideoFrame object and must not be freed by the caller.
    const uint8* visible_data(size_t plane) const;
    uint8* visible_data(size_t plane);

    // Returns a mailbox holder for a given texture.
    // Only valid to call if this is a NATIVE_TEXTURE frame. Before using the
    // mailbox, the caller must wait for the included sync point.
    const gpu::MailboxHolder& mailbox_holder(size_t texture_index) const;

    // Returns the shared-memory handle, if present
    base::SharedMemoryHandle shared_memory_handle() const;

    // Returns the offset into the shared memory where the frame data begins.
    size_t shared_memory_offset() const;

    // Returns the vector of GpuMemoryBuffer handles, if present.
    const std::vector<gfx::GpuMemoryBufferHandle>& gpu_memory_buffer_handles()
        const;

#if defined(OS_LINUX)
    // Returns backing DmaBuf file descriptor for given |plane|, if present, or
    // -1 if not.
    // TODO(mcasas): Rename to DmabufFd() to comply with Style Guide.
    int dmabuf_fd(size_t plane) const;

    // Duplicates internally the |fds_in|, overwriting the current ones. Returns
    // false if something goes wrong, and leaves all internal fds closed.
    bool DuplicateFileDescriptors(const std::vector<int>& fds_in);
#endif

    void AddSharedMemoryHandle(base::SharedMemoryHandle handle);

#if defined(OS_MACOSX)
    // Returns the backing CVPixelBuffer, if present.
    // TODO(mcasas): Rename to CvPixelBuffer() to comply with Style Guide.
    CVPixelBufferRef cv_pixel_buffer() const;
#endif

    // Adds a callback to be run when the VideoFrame is about to be destroyed.
    // The callback may be run from ANY THREAD, and so it is up to the client to
    // ensure thread safety.  Although read-only access to the members of this
    // VideoFrame is permitted while the callback executes (including
    // VideoFrameMetadata), clients should not assume the data pointers are
    // valid.
    void AddDestructionObserver(const base::Closure& callback);

    // Returns a dictionary of optional metadata.  This contains information
    // associated with the frame that downstream clients might use for frame-level
    // logging, quality/performance optimizations, signaling, etc.
    //
    // TODO(miu): Move some of the "extra" members of VideoFrame (below) into
    // here as a later clean-up step.
    const VideoFrameMetadata* metadata() const { return &metadata_; }
    VideoFrameMetadata* metadata() { return &metadata_; }

    // The time span between the current frame and the first frame of the stream.
    // This is the media timestamp, and not the reference time.
    // See VideoFrameMetadata::REFERENCE_TIME for details.
    base::TimeDelta timestamp() const { return timestamp_; }
    void set_timestamp(base::TimeDelta timestamp)
    {
        timestamp_ = timestamp;
    }

    // It uses |client| to insert a new sync point and potentially waits on a
    // older sync point. The final sync point will be used to release this
    // VideoFrame.
    // This method is thread safe. Both blink and compositor threads can call it.
    void UpdateReleaseSyncToken(SyncTokenClient* client);

private:
    friend class base::RefCountedThreadSafe<VideoFrame>;

    static scoped_refptr<VideoFrame> WrapExternalStorage(
        VideoPixelFormat format,
        StorageType storage_type,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        uint8* data,
        size_t data_size,
        base::TimeDelta timestamp,
        base::SharedMemoryHandle handle,
        size_t data_offset);

    // Clients must use the static factory/wrapping methods to create a new frame.
    VideoFrame(VideoPixelFormat format,
        StorageType storage_type,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp);
    VideoFrame(VideoPixelFormat format,
        StorageType storage_type,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp,
        base::SharedMemoryHandle handle,
        size_t shared_memory_offset);
    VideoFrame(VideoPixelFormat format,
        StorageType storage_type,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        const gpu::MailboxHolder (&mailbox_holders)[kMaxPlanes],
        const ReleaseMailboxCB& mailbox_holder_release_cb,
        base::TimeDelta timestamp);
    virtual ~VideoFrame();

    static scoped_refptr<VideoFrame> CreateFrameInternal(
        VideoPixelFormat format,
        const gfx::Size& coded_size,
        const gfx::Rect& visible_rect,
        const gfx::Size& natural_size,
        base::TimeDelta timestamp,
        bool zero_initialize_memory);

    void AllocateYUV(bool zero_initialize_memory);

    // Frame format.
    const VideoPixelFormat format_;

    // Storage type for the different planes.
    StorageType storage_type_; // TODO(mcasas): make const

    // Width and height of the video frame, in pixels. This must include pixel
    // data for the whole image; i.e. for YUV formats with subsampled chroma
    // planes, in the case that the visible portion of the image does not line up
    // on a sample boundary, |coded_size_| must be rounded up appropriately and
    // the pixel data provided for the odd pixels.
    const gfx::Size coded_size_;

    // Width, height, and offsets of the visible portion of the video frame. Must
    // be a subrect of |coded_size_|. Can be odd with respect to the sample
    // boundaries, e.g. for formats with subsampled chroma.
    const gfx::Rect visible_rect_;

    // Width and height of the visible portion of the video frame
    // (|visible_rect_.size()|) with aspect ratio taken into account.
    const gfx::Size natural_size_;

    // Array of strides for each plane, typically greater or equal to the width
    // of the surface divided by the horizontal sampling period.  Note that
    // strides can be negative.
    int32 strides_[kMaxPlanes];

    // Array of data pointers to each plane.
    // TODO(mcasas): we don't know on ctor if we own |data_| or not. After
    // refactoring VideoFrame, change to scoped_ptr<uint8, AlignedFreeDeleter>.
    uint8* data_[kMaxPlanes];

    // Native texture mailboxes, if this is a IsTexture() frame.
    gpu::MailboxHolder mailbox_holders_[kMaxPlanes];
    ReleaseMailboxCB mailbox_holders_release_cb_;

    // Shared memory handle and associated offset inside it, if this frame is
    // a STORAGE_SHMEM one.
    base::SharedMemoryHandle shared_memory_handle_;
    size_t shared_memory_offset_;

    // GpuMemoryBuffer handles attached to the video_frame.
    std::vector<gfx::GpuMemoryBufferHandle> gpu_memory_buffer_handles_;

#if defined(OS_LINUX)
    // Dmabufs for each plane. If set, this frame has DmaBuf backing in some way.
    base::ScopedFD dmabuf_fds_[kMaxPlanes];
#endif

#if defined(OS_MACOSX)
    // CVPixelBuffer, if this frame is wrapping one.
    base::ScopedCFTypeRef<CVPixelBufferRef> cv_pixel_buffer_;
#endif

    std::vector<base::Closure> done_callbacks_;

    base::TimeDelta timestamp_;

    base::Lock release_sync_token_lock_;
    gpu::SyncToken release_sync_token_;

    VideoFrameMetadata metadata_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(VideoFrame);
};

} // namespace media

#endif // MEDIA_BASE_VIDEO_FRAME_H_
