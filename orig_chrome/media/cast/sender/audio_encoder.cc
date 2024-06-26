// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/sender/audio_encoder.h"

#include <algorithm>
#include <limits>
#include <string>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/location.h"
#include "base/stl_util.h"
#include "base/sys_byteorder.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "media/cast/cast_defines.h"

#if !defined(OS_IOS)
#include "third_party/opus/src/include/opus.h"
#endif

#if defined(OS_MACOSX)
#include <AudioToolbox/AudioToolbox.h>
#endif

namespace media {
namespace cast {

    namespace {

        const int kUnderrunSkipThreshold = 3;
        const int kDefaultFramesPerSecond = 100;

    } // namespace

    // Base class that handles the common problem of feeding one or more AudioBus'
    // data into a buffer and then, once the buffer is full, encoding the signal and
    // emitting a SenderEncodedFrame via the FrameEncodedCallback.
    //
    // Subclasses complete the implementation by handling the actual encoding
    // details.
    class AudioEncoder::ImplBase
        : public base::RefCountedThreadSafe<AudioEncoder::ImplBase> {
    public:
        ImplBase(const scoped_refptr<CastEnvironment>& cast_environment,
            Codec codec,
            int num_channels,
            int sampling_rate,
            int samples_per_frame,
            const FrameEncodedCallback& callback)
            : cast_environment_(cast_environment)
            , codec_(codec)
            , num_channels_(num_channels)
            , samples_per_frame_(samples_per_frame)
            , callback_(callback)
            , operational_status_(STATUS_UNINITIALIZED)
            , frame_duration_(base::TimeDelta::FromMicroseconds(
                  base::Time::kMicrosecondsPerSecond * samples_per_frame_ / sampling_rate))
            , buffer_fill_end_(0)
            , frame_id_(0)
            , frame_rtp_timestamp_(0)
            , samples_dropped_from_buffer_(0)
        {
            // Support for max sampling rate of 48KHz, 2 channels, 100 ms duration.
            const int kMaxSamplesTimesChannelsPerFrame = 48 * 2 * 100;
            if (num_channels_ <= 0 || samples_per_frame_ <= 0 || frame_duration_ == base::TimeDelta() || samples_per_frame_ * num_channels_ > kMaxSamplesTimesChannelsPerFrame) {
                operational_status_ = STATUS_INVALID_CONFIGURATION;
            }
        }

        OperationalStatus InitializationResult() const
        {
            return operational_status_;
        }

        int samples_per_frame() const
        {
            return samples_per_frame_;
        }

        base::TimeDelta frame_duration() const { return frame_duration_; }

        void EncodeAudio(scoped_ptr<AudioBus> audio_bus,
            const base::TimeTicks& recorded_time)
        {
            DCHECK_EQ(operational_status_, STATUS_INITIALIZED);
            DCHECK(!recorded_time.is_null());

            // Determine whether |recorded_time| is consistent with the amount of audio
            // data having been processed in the past.  Resolve the underrun problem by
            // dropping data from the internal buffer and skipping ahead the next
            // frame's RTP timestamp by the estimated number of frames missed.  On the
            // other hand, don't attempt to resolve overruns: A receiver should
            // gracefully deal with an excess of audio data.
            base::TimeDelta buffer_fill_duration = buffer_fill_end_ * frame_duration_ / samples_per_frame_;
            if (!frame_capture_time_.is_null()) {
                const base::TimeDelta amount_ahead_by = recorded_time - (frame_capture_time_ + buffer_fill_duration);
                const int64 num_frames_missed = amount_ahead_by / frame_duration_;
                if (num_frames_missed > kUnderrunSkipThreshold) {
                    samples_dropped_from_buffer_ += buffer_fill_end_;
                    buffer_fill_end_ = 0;
                    buffer_fill_duration = base::TimeDelta();
                    frame_rtp_timestamp_ += static_cast<uint32>(num_frames_missed * samples_per_frame_);
                    DVLOG(1) << "Skipping RTP timestamp ahead to account for "
                             << num_frames_missed * samples_per_frame_
                             << " samples' worth of underrun.";
                    TRACE_EVENT_INSTANT2("cast.stream", "Audio Skip",
                        TRACE_EVENT_SCOPE_THREAD,
                        "frames missed", num_frames_missed,
                        "samples dropped", samples_dropped_from_buffer_);
                }
            }
            frame_capture_time_ = recorded_time - buffer_fill_duration;

            // Encode all audio in |audio_bus| into zero or more frames.
            int src_pos = 0;
            while (src_pos < audio_bus->frames()) {
                // Note: This is used to compute the deadline utilization and so it uses
                // the real-world clock instead of the CastEnvironment clock, the latter
                // of which might be simulated.
                const base::TimeTicks start_time = base::TimeTicks::Now();

                const int num_samples_to_xfer = std::min(
                    samples_per_frame_ - buffer_fill_end_, audio_bus->frames() - src_pos);
                DCHECK_EQ(audio_bus->channels(), num_channels_);
                TransferSamplesIntoBuffer(
                    audio_bus.get(), src_pos, buffer_fill_end_, num_samples_to_xfer);
                src_pos += num_samples_to_xfer;
                buffer_fill_end_ += num_samples_to_xfer;

                if (buffer_fill_end_ < samples_per_frame_)
                    break;

                scoped_ptr<SenderEncodedFrame> audio_frame(
                    new SenderEncodedFrame());
                audio_frame->dependency = EncodedFrame::KEY;
                audio_frame->frame_id = frame_id_;
                audio_frame->referenced_frame_id = frame_id_;
                audio_frame->rtp_timestamp = frame_rtp_timestamp_;
                audio_frame->reference_time = frame_capture_time_;

                TRACE_EVENT_ASYNC_BEGIN2("cast.stream", "Audio Encode", audio_frame.get(),
                    "frame_id", frame_id_,
                    "rtp_timestamp", frame_rtp_timestamp_);
                if (EncodeFromFilledBuffer(&audio_frame->data)) {
                    // Compute deadline utilization as the real-world time elapsed divided
                    // by the signal duration.
                    audio_frame->deadline_utilization = (base::TimeTicks::Now() - start_time).InSecondsF() / frame_duration_.InSecondsF();

                    TRACE_EVENT_ASYNC_END1("cast.stream", "Audio Encode", audio_frame.get(),
                        "Deadline utilization",
                        audio_frame->deadline_utilization);

                    audio_frame->encode_completion_time = cast_environment_->Clock()->NowTicks();
                    cast_environment_->PostTask(
                        CastEnvironment::MAIN,
                        FROM_HERE,
                        base::Bind(callback_,
                            base::Passed(&audio_frame),
                            samples_dropped_from_buffer_));
                    samples_dropped_from_buffer_ = 0;
                }

                // Reset the internal buffer, frame ID, and timestamps for the next frame.
                buffer_fill_end_ = 0;
                ++frame_id_;
                frame_rtp_timestamp_ += samples_per_frame_;
                frame_capture_time_ += frame_duration_;
            }
        }

    protected:
        friend class base::RefCountedThreadSafe<ImplBase>;
        virtual ~ImplBase() { }

        virtual void TransferSamplesIntoBuffer(const AudioBus* audio_bus,
            int source_offset,
            int buffer_fill_offset,
            int num_samples)
            = 0;
        virtual bool EncodeFromFilledBuffer(std::string* out) = 0;

        const scoped_refptr<CastEnvironment> cast_environment_;
        const Codec codec_;
        const int num_channels_;
        const int samples_per_frame_;
        const FrameEncodedCallback callback_;

        // Subclass' ctor is expected to set this to STATUS_INITIALIZED.
        OperationalStatus operational_status_;

        // The duration of one frame of encoded audio samples. Derived from
        // |samples_per_frame_| and the sampling rate.
        const base::TimeDelta frame_duration_;

    private:
        // In the case where a call to EncodeAudio() cannot completely fill the
        // buffer, this points to the position at which to populate data in a later
        // call.
        int buffer_fill_end_;

        // A counter used to label EncodedFrames.
        uint32 frame_id_;

        // The RTP timestamp for the next frame of encoded audio.  This is defined as
        // the number of audio samples encoded so far, plus the estimated number of
        // samples that were missed due to data underruns.  A receiver uses this value
        // to detect gaps in the audio signal data being provided.  Per the spec, RTP
        // timestamp values are allowed to overflow and roll around past zero.
        uint32 frame_rtp_timestamp_;

        // The local system time associated with the start of the next frame of
        // encoded audio.  This value is passed on to a receiver as a reference clock
        // timestamp for the purposes of synchronizing audio and video.  Its
        // progression is expected to drift relative to the elapsed time implied by
        // the RTP timestamps.
        base::TimeTicks frame_capture_time_;

        // Set to non-zero to indicate the next output frame skipped over audio
        // samples in order to recover from an input underrun.
        int samples_dropped_from_buffer_;

        DISALLOW_COPY_AND_ASSIGN(ImplBase);
    };

#if !defined(OS_IOS)
    class AudioEncoder::OpusImpl : public AudioEncoder::ImplBase {
    public:
        OpusImpl(const scoped_refptr<CastEnvironment>& cast_environment,
            int num_channels,
            int sampling_rate,
            int bitrate,
            const FrameEncodedCallback& callback)
            : ImplBase(cast_environment,
                CODEC_AUDIO_OPUS,
                num_channels,
                sampling_rate,
                sampling_rate / kDefaultFramesPerSecond, /* 10 ms frames */
                callback)
            , encoder_memory_(new uint8[opus_encoder_get_size(num_channels)])
            , opus_encoder_(reinterpret_cast<OpusEncoder*>(encoder_memory_.get()))
            , buffer_(new float[num_channels * samples_per_frame_])
        {
            if (ImplBase::operational_status_ != STATUS_UNINITIALIZED || sampling_rate % samples_per_frame_ != 0 || !IsValidFrameDuration(frame_duration_)) {
                return;
            }
            if (opus_encoder_init(opus_encoder_,
                    sampling_rate,
                    num_channels,
                    OPUS_APPLICATION_AUDIO)
                != OPUS_OK) {
                ImplBase::operational_status_ = STATUS_INVALID_CONFIGURATION;
                return;
            }
            ImplBase::operational_status_ = STATUS_INITIALIZED;

            if (bitrate <= 0) {
                // Note: As of 2013-10-31, the encoder in "auto bitrate" mode would use a
                // variable bitrate up to 102kbps for 2-channel, 48 kHz audio and a 10 ms
                // frame size.  The opus library authors may, of course, adjust this in
                // later versions.
                bitrate = OPUS_AUTO;
            }
            CHECK_EQ(opus_encoder_ctl(opus_encoder_, OPUS_SET_BITRATE(bitrate)),
                OPUS_OK);
        }

    private:
        ~OpusImpl() final { }

        void TransferSamplesIntoBuffer(const AudioBus* audio_bus,
            int source_offset,
            int buffer_fill_offset,
            int num_samples) final
        {
            // Opus requires channel-interleaved samples in a single array.
            for (int ch = 0; ch < audio_bus->channels(); ++ch) {
                const float* src = audio_bus->channel(ch) + source_offset;
                const float* const src_end = src + num_samples;
                float* dest = buffer_.get() + buffer_fill_offset * num_channels_ + ch;
                for (; src < src_end; ++src, dest += num_channels_)
                    *dest = *src;
            }
        }

        bool EncodeFromFilledBuffer(std::string* out) final
        {
            out->resize(kOpusMaxPayloadSize);
            const opus_int32 result = opus_encode_float(opus_encoder_,
                buffer_.get(),
                samples_per_frame_,
                reinterpret_cast<uint8*>(string_as_array(out)),
                kOpusMaxPayloadSize);
            if (result > 1) {
                out->resize(result);
                return true;
            } else if (result < 0) {
                LOG(ERROR) << "Error code from opus_encode_float(): " << result;
                return false;
            } else {
                // Do nothing: The documentation says that a return value of zero or
                // one byte means the packet does not need to be transmitted.
                return false;
            }
        }

        static bool IsValidFrameDuration(base::TimeDelta duration)
        {
            // See https://tools.ietf.org/html/rfc6716#section-2.1.4
            return duration == base::TimeDelta::FromMicroseconds(2500) || duration == base::TimeDelta::FromMilliseconds(5) || duration == base::TimeDelta::FromMilliseconds(10) || duration == base::TimeDelta::FromMilliseconds(20) || duration == base::TimeDelta::FromMilliseconds(40) || duration == base::TimeDelta::FromMilliseconds(60);
        }

        const scoped_ptr<uint8[]> encoder_memory_;
        OpusEncoder* const opus_encoder_;
        const scoped_ptr<float[]> buffer_;

        // This is the recommended value, according to documentation in
        // third_party/opus/src/include/opus.h, so that the Opus encoder does not
        // degrade the audio due to memory constraints.
        //
        // Note: Whereas other RTP implementations do not, the cast library is
        // perfectly capable of transporting larger than MTU-sized audio frames.
        static const int kOpusMaxPayloadSize = 4000;

        DISALLOW_COPY_AND_ASSIGN(OpusImpl);
    };
#endif

#if defined(OS_MACOSX)
    class AudioEncoder::AppleAacImpl : public AudioEncoder::ImplBase {
        // AAC-LC has two access unit sizes (960 and 1024). The Apple encoder only
        // supports the latter.
        static const int kAccessUnitSamples = 1024;

        // Size of an ADTS header (w/o checksum). See
        // http://wiki.multimedia.cx/index.php?title=ADTS
        static const int kAdtsHeaderSize = 7;

    public:
        AppleAacImpl(const scoped_refptr<CastEnvironment>& cast_environment,
            int num_channels,
            int sampling_rate,
            int bitrate,
            const FrameEncodedCallback& callback)
            : ImplBase(cast_environment,
                CODEC_AUDIO_AAC,
                num_channels,
                sampling_rate,
                kAccessUnitSamples,
                callback)
            , input_buffer_(AudioBus::Create(num_channels, kAccessUnitSamples))
            , input_bus_(AudioBus::CreateWrapper(num_channels))
            , max_access_unit_size_(0)
            , output_buffer_(nullptr)
            , converter_(nullptr)
            , file_(nullptr)
            , num_access_units_(0)
            , can_resume_(true)
        {
            if (ImplBase::operational_status_ != STATUS_UNINITIALIZED) {
                return;
            }
            if (!Initialize(sampling_rate, bitrate)) {
                ImplBase::operational_status_ = STATUS_INVALID_CONFIGURATION;
                return;
            }
            ImplBase::operational_status_ = STATUS_INITIALIZED;
        }

    private:
        ~AppleAacImpl() final { Teardown(); }

        // Destroys the existing audio converter and file, if any.
        void Teardown()
        {
            if (converter_) {
                AudioConverterDispose(converter_);
                converter_ = nullptr;
            }
            if (file_) {
                AudioFileClose(file_);
                file_ = nullptr;
            }
        }

        // Initializes the audio converter and file. Calls Teardown to destroy any
        // existing state. This is so that Initialize() may be called to setup another
        // converter after a non-resumable interruption.
        bool Initialize(int sampling_rate, int bitrate)
        {
            // Teardown previous audio converter and file.
            Teardown();

            // Input data comes from AudioBus objects, which carry non-interleaved
            // packed native-endian float samples. Note that in Core Audio, a frame is
            // one sample across all channels at a given point in time. When describing
            // a non-interleaved samples format, the "per frame" fields mean "per
            // channel" or "per stream", with the exception of |mChannelsPerFrame|. For
            // uncompressed formats, one packet contains one frame.
            AudioStreamBasicDescription in_asbd;
            in_asbd.mSampleRate = sampling_rate;
            in_asbd.mFormatID = kAudioFormatLinearPCM;
            in_asbd.mFormatFlags = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved;
            in_asbd.mChannelsPerFrame = num_channels_;
            in_asbd.mBitsPerChannel = sizeof(float) * 8;
            in_asbd.mFramesPerPacket = 1;
            in_asbd.mBytesPerPacket = in_asbd.mBytesPerFrame = sizeof(float);
            in_asbd.mReserved = 0;

            // Request AAC-LC encoding, with no downmixing or downsampling.
            AudioStreamBasicDescription out_asbd;
            memset(&out_asbd, 0, sizeof(AudioStreamBasicDescription));
            out_asbd.mSampleRate = sampling_rate;
            out_asbd.mFormatID = kAudioFormatMPEG4AAC;
            out_asbd.mChannelsPerFrame = num_channels_;
            UInt32 prop_size = sizeof(out_asbd);
            if (AudioFormatGetProperty(kAudioFormatProperty_FormatInfo,
                    0,
                    nullptr,
                    &prop_size,
                    &out_asbd)
                != noErr) {
                return false;
            }

            if (AudioConverterNew(&in_asbd, &out_asbd, &converter_) != noErr) {
                return false;
            }

            // The converter will fully specify the output format and update the
            // relevant fields of the structure, which we can now query.
            prop_size = sizeof(out_asbd);
            if (AudioConverterGetProperty(converter_,
                    kAudioConverterCurrentOutputStreamDescription,
                    &prop_size,
                    &out_asbd)
                != noErr) {
                return false;
            }

            // If bitrate is <= 0, allow the encoder to pick a suitable value.
            // Otherwise, set the bitrate (which can fail if the value is not suitable
            // or compatible with the output sampling rate or channels).
            if (bitrate > 0) {
                prop_size = sizeof(int);
                if (AudioConverterSetProperty(
                        converter_, kAudioConverterEncodeBitRate, prop_size, &bitrate)
                    != noErr) {
                    return false;
                }
            }

#if defined(OS_IOS)
            // See the comment next to |can_resume_| for details on resumption. Some
            // converters can return kAudioConverterErr_PropertyNotSupported, in which
            // case resumption is implicitly supported. This is the only location where
            // the implementation modifies |can_resume_|.
            uint32_t can_resume;
            prop_size = sizeof(can_resume);
            OSStatus oserr = AudioConverterGetProperty(
                converter_,
                kAudioConverterPropertyCanResumeFromInterruption,
                &prop_size,
                &can_resume);
            if (oserr == noErr) {
                const_cast<bool&>(can_resume_) = can_resume != 0;
            }
#endif

            // Figure out the maximum size of an access unit that the encoder can
            // produce. |mBytesPerPacket| will be 0 for variable size configurations,
            // in which case we must query the value.
            uint32_t max_access_unit_size = out_asbd.mBytesPerPacket;
            if (max_access_unit_size == 0) {
                prop_size = sizeof(max_access_unit_size);
                if (AudioConverterGetProperty(
                        converter_,
                        kAudioConverterPropertyMaximumOutputPacketSize,
                        &prop_size,
                        &max_access_unit_size)
                    != noErr) {
                    return false;
                }
            }

            // This is the only location where the implementation modifies
            // |max_access_unit_size_|.
            const_cast<uint32_t&>(max_access_unit_size_) = max_access_unit_size;

            // Allocate a buffer to store one access unit. This is the only location
            // where the implementation modifies |access_unit_buffer_|.
            const_cast<scoped_ptr<uint8[]>&>(access_unit_buffer_)
                .reset(new uint8[max_access_unit_size]);

            // Initialize the converter ABL. Note that the buffer size has to be set
            // before every encode operation, since the field is modified to indicate
            // the size of the output data (on input it indicates the buffer capacity).
            converter_abl_.mNumberBuffers = 1;
            converter_abl_.mBuffers[0].mNumberChannels = num_channels_;
            converter_abl_.mBuffers[0].mData = access_unit_buffer_.get();

            // The "magic cookie" is an encoder state vector required for decoding and
            // packetization. It is queried now from |converter_| then set on |file_|
            // after initialization.
            UInt32 cookie_size;
            if (AudioConverterGetPropertyInfo(converter_,
                    kAudioConverterCompressionMagicCookie,
                    &cookie_size,
                    nullptr)
                != noErr) {
                return false;
            }
            scoped_ptr<uint8[]> cookie_data(new uint8[cookie_size]);
            if (AudioConverterGetProperty(converter_,
                    kAudioConverterCompressionMagicCookie,
                    &cookie_size,
                    cookie_data.get())
                != noErr) {
                return false;
            }

            if (AudioFileInitializeWithCallbacks(this,
                    &FileReadCallback,
                    &FileWriteCallback,
                    &FileGetSizeCallback,
                    &FileSetSizeCallback,
                    kAudioFileAAC_ADTSType,
                    &out_asbd,
                    0,
                    &file_)
                != noErr) {
                return false;
            }

            if (AudioFileSetProperty(file_,
                    kAudioFilePropertyMagicCookieData,
                    cookie_size,
                    cookie_data.get())
                != noErr) {
                return false;
            }

            // Initially the input bus points to the input buffer. See the comment on
            // |input_bus_| for more on this optimization.
            input_bus_->set_frames(kAccessUnitSamples);
            for (int ch = 0; ch < input_buffer_->channels(); ++ch) {
                input_bus_->SetChannelData(ch, input_buffer_->channel(ch));
            }

            return true;
        }

        void TransferSamplesIntoBuffer(const AudioBus* audio_bus,
            int source_offset,
            int buffer_fill_offset,
            int num_samples) final
        {
            DCHECK_EQ(audio_bus->channels(), input_buffer_->channels());

            // See the comment on |input_bus_| for more on this optimization. Note that
            // we cannot elide the copy if the source offset would result in an
            // unaligned pointer.
            if (num_samples == kAccessUnitSamples && source_offset * sizeof(float) % AudioBus::kChannelAlignment == 0) {
                DCHECK_EQ(buffer_fill_offset, 0);
                for (int ch = 0; ch < audio_bus->channels(); ++ch) {
                    auto samples = const_cast<float*>(audio_bus->channel(ch));
                    input_bus_->SetChannelData(ch, samples + source_offset);
                }
                return;
            }

            // Copy the samples into the input buffer.
            DCHECK_EQ(input_bus_->channel(0), input_buffer_->channel(0));
            audio_bus->CopyPartialFramesTo(
                source_offset, num_samples, buffer_fill_offset, input_buffer_.get());
        }

        bool EncodeFromFilledBuffer(std::string* out) final
        {
            // Reset the buffer size field to the buffer capacity.
            converter_abl_.mBuffers[0].mDataByteSize = max_access_unit_size_;

            // Encode the current input buffer. This is a sychronous call.
            OSStatus oserr;
            UInt32 io_num_packets = 1;
            AudioStreamPacketDescription packet_description;
            oserr = AudioConverterFillComplexBuffer(converter_,
                &ConverterFillDataCallback,
                this,
                &io_num_packets,
                &converter_abl_,
                &packet_description);
            if (oserr != noErr || io_num_packets == 0) {
                return false;
            }

            // Reserve space in the output buffer to write the packet.
            out->reserve(packet_description.mDataByteSize + kAdtsHeaderSize);

            // Set the current output buffer and emit an ADTS-wrapped AAC access unit.
            // This is a synchronous call. After it returns, reset the output buffer.
            output_buffer_ = out;
            oserr = AudioFileWritePackets(file_,
                false,
                converter_abl_.mBuffers[0].mDataByteSize,
                &packet_description,
                num_access_units_,
                &io_num_packets,
                converter_abl_.mBuffers[0].mData);
            output_buffer_ = nullptr;
            if (oserr != noErr || io_num_packets == 0) {
                return false;
            }
            num_access_units_ += io_num_packets;
            return true;
        }

        // The |AudioConverterFillComplexBuffer| input callback function. Configures
        // the provided |AudioBufferList| to alias |input_bus_|. The implementation
        // can only supply |kAccessUnitSamples| samples as a result of not copying
        // samples or tracking read and write positions. Note that this function is
        // called synchronously by |AudioConverterFillComplexBuffer|.
        static OSStatus ConverterFillDataCallback(
            AudioConverterRef in_converter,
            UInt32* io_num_packets,
            AudioBufferList* io_data,
            AudioStreamPacketDescription** out_packet_desc,
            void* in_encoder)
        {
            DCHECK(in_encoder);
            auto encoder = reinterpret_cast<AppleAacImpl*>(in_encoder);
            auto input_buffer = encoder->input_buffer_.get();
            auto input_bus = encoder->input_bus_.get();

            DCHECK_EQ(static_cast<int>(*io_num_packets), kAccessUnitSamples);
            DCHECK_EQ(io_data->mNumberBuffers,
                static_cast<unsigned>(input_bus->channels()));
            for (int i_buf = 0, end = io_data->mNumberBuffers; i_buf < end; ++i_buf) {
                io_data->mBuffers[i_buf].mNumberChannels = 1;
                io_data->mBuffers[i_buf].mDataByteSize = sizeof(float) * *io_num_packets;
                io_data->mBuffers[i_buf].mData = input_bus->channel(i_buf);

                // Reset the input bus back to the input buffer. See the comment on
                // |input_bus_| for more on this optimization.
                input_bus->SetChannelData(i_buf, input_buffer->channel(i_buf));
            }
            return noErr;
        }

        // The AudioFile read callback function.
        static OSStatus FileReadCallback(void* in_encoder,
            SInt64 in_position,
            UInt32 in_size,
            void* in_buffer,
            UInt32* out_size)
        {
            // This class only does writing.
            NOTREACHED();
            return kAudioFileNotOpenError;
        }

        // The AudioFile write callback function. Appends the data to the encoder's
        // current |output_buffer_|.
        static OSStatus FileWriteCallback(void* in_encoder,
            SInt64 in_position,
            UInt32 in_size,
            const void* in_buffer,
            UInt32* out_size)
        {
            DCHECK(in_encoder);
            DCHECK(in_buffer);
            auto encoder = reinterpret_cast<const AppleAacImpl*>(in_encoder);
            auto buffer = reinterpret_cast<const std::string::value_type*>(in_buffer);

            std::string* const output_buffer = encoder->output_buffer_;
            DCHECK(output_buffer);

            output_buffer->append(buffer, in_size);
            *out_size = in_size;
            return noErr;
        }

        // The AudioFile getsize callback function.
        static SInt64 FileGetSizeCallback(void* in_encoder)
        {
            // This class only does writing.
            NOTREACHED();
            return 0;
        }

        // The AudioFile setsize callback function.
        static OSStatus FileSetSizeCallback(void* in_encoder, SInt64 in_size)
        {
            return noErr;
        }

        // Buffer that holds one AAC access unit worth of samples. The input callback
        // function provides samples from this buffer via |input_bus_| to the encoder.
        const scoped_ptr<AudioBus> input_buffer_;

        // Wrapper AudioBus used by the input callback function. Normally it wraps
        // |input_buffer_|. However, as an optimization when the client submits a
        // buffer containing exactly one access unit worth of samples, the bus is
        // redirected to the client buffer temporarily. We know that the base
        // implementation will call us right after to encode the buffer and thus we
        // can eliminate the copy into |input_buffer_|.
        const scoped_ptr<AudioBus> input_bus_;

        // A buffer that holds one AAC access unit. Initialized in |Initialize| once
        // the maximum access unit size is known.
        const scoped_ptr<uint8[]> access_unit_buffer_;

        // The maximum size of an access unit that the encoder can emit.
        const uint32_t max_access_unit_size_;

        // A temporary pointer to the current output buffer. Only non-null when
        // writing an access unit. Accessed by the AudioFile write callback function.
        std::string* output_buffer_;

        // The |AudioConverter| is responsible for AAC encoding. This is a Core Audio
        // object, not to be confused with |media::AudioConverter|.
        AudioConverterRef converter_;

        // The |AudioFile| is responsible for ADTS packetization.
        AudioFileID file_;

        // An |AudioBufferList| passed to the converter to store encoded samples.
        AudioBufferList converter_abl_;

        // The number of access units emitted so far by the encoder.
        uint64_t num_access_units_;

        // On iOS, audio codecs can be interrupted by other services (such as an
        // audio alert or phone call). Depending on the underlying hardware and
        // configuration, the codec may have to be thrown away and re-initialized
        // after such an interruption. This flag tracks if we can resume or not from
        // such an interruption. It is initialized to true, which is the only possible
        // value on OS X and on most modern iOS hardware.
        // TODO(jfroy): Implement encoder re-initialization after interruption.
        //              https://crbug.com/424787
        const bool can_resume_;

        DISALLOW_COPY_AND_ASSIGN(AppleAacImpl);
    };
#endif // defined(OS_MACOSX)

    class AudioEncoder::Pcm16Impl : public AudioEncoder::ImplBase {
    public:
        Pcm16Impl(const scoped_refptr<CastEnvironment>& cast_environment,
            int num_channels,
            int sampling_rate,
            const FrameEncodedCallback& callback)
            : ImplBase(cast_environment,
                CODEC_AUDIO_PCM16,
                num_channels,
                sampling_rate,
                sampling_rate / kDefaultFramesPerSecond, /* 10 ms frames */
                callback)
            , buffer_(new int16[num_channels * samples_per_frame_])
        {
            if (ImplBase::operational_status_ != STATUS_UNINITIALIZED)
                return;
            operational_status_ = STATUS_INITIALIZED;
        }

    private:
        ~Pcm16Impl() final { }

        void TransferSamplesIntoBuffer(const AudioBus* audio_bus,
            int source_offset,
            int buffer_fill_offset,
            int num_samples) final
        {
            audio_bus->ToInterleavedPartial(
                source_offset,
                num_samples,
                sizeof(int16),
                buffer_.get() + buffer_fill_offset * num_channels_);
        }

        bool EncodeFromFilledBuffer(std::string* out) final
        {
            // Output 16-bit PCM integers in big-endian byte order.
            out->resize(num_channels_ * samples_per_frame_ * sizeof(int16));
            const int16* src = buffer_.get();
            const int16* const src_end = src + num_channels_ * samples_per_frame_;
            uint16* dest = reinterpret_cast<uint16*>(&out->at(0));
            for (; src < src_end; ++src, ++dest)
                *dest = base::HostToNet16(*src);
            return true;
        }

    private:
        const scoped_ptr<int16[]> buffer_;

        DISALLOW_COPY_AND_ASSIGN(Pcm16Impl);
    };

    AudioEncoder::AudioEncoder(
        const scoped_refptr<CastEnvironment>& cast_environment,
        int num_channels,
        int sampling_rate,
        int bitrate,
        Codec codec,
        const FrameEncodedCallback& frame_encoded_callback)
        : cast_environment_(cast_environment)
    {
        // Note: It doesn't matter which thread constructs AudioEncoder, just so long
        // as all calls to InsertAudio() are by the same thread.
        insert_thread_checker_.DetachFromThread();
        switch (codec) {
#if !defined(OS_IOS)
        case CODEC_AUDIO_OPUS:
            impl_ = new OpusImpl(cast_environment,
                num_channels,
                sampling_rate,
                bitrate,
                frame_encoded_callback);
            break;
#endif
#if defined(OS_MACOSX)
        case CODEC_AUDIO_AAC:
            impl_ = new AppleAacImpl(cast_environment,
                num_channels,
                sampling_rate,
                bitrate,
                frame_encoded_callback);
            break;
#endif // defined(OS_MACOSX)
        case CODEC_AUDIO_PCM16:
            impl_ = new Pcm16Impl(cast_environment,
                num_channels,
                sampling_rate,
                frame_encoded_callback);
            break;
        default:
            NOTREACHED() << "Unsupported or unspecified codec for audio encoder";
            break;
        }
    }

    AudioEncoder::~AudioEncoder() { }

    OperationalStatus AudioEncoder::InitializationResult() const
    {
        DCHECK(insert_thread_checker_.CalledOnValidThread());
        if (impl_.get()) {
            return impl_->InitializationResult();
        }
        return STATUS_UNSUPPORTED_CODEC;
    }

    int AudioEncoder::GetSamplesPerFrame() const
    {
        DCHECK(insert_thread_checker_.CalledOnValidThread());
        if (InitializationResult() != STATUS_INITIALIZED) {
            NOTREACHED();
            return std::numeric_limits<int>::max();
        }
        return impl_->samples_per_frame();
    }

    base::TimeDelta AudioEncoder::GetFrameDuration() const
    {
        DCHECK(insert_thread_checker_.CalledOnValidThread());
        if (InitializationResult() != STATUS_INITIALIZED) {
            NOTREACHED();
            return base::TimeDelta();
        }
        return impl_->frame_duration();
    }

    void AudioEncoder::InsertAudio(scoped_ptr<AudioBus> audio_bus,
        const base::TimeTicks& recorded_time)
    {
        DCHECK(insert_thread_checker_.CalledOnValidThread());
        DCHECK(audio_bus.get());
        if (InitializationResult() != STATUS_INITIALIZED) {
            NOTREACHED();
            return;
        }
        cast_environment_->PostTask(CastEnvironment::AUDIO,
            FROM_HERE,
            base::Bind(&AudioEncoder::ImplBase::EncodeAudio,
                impl_,
                base::Passed(&audio_bus),
                recorded_time));
    }

} // namespace cast
} // namespace media
