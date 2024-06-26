// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file contains an implementation of a VP9 bitstream parser. The main
// purpose of this parser is to support hardware decode acceleration. Some
// accelerators, e.g. libva which implements VA-API, require the caller
// (chrome) to feed them parsed VP9 frame header.
//
// See content::VP9Decoder for example usage.
//
#ifndef MEDIA_FILTERS_VP9_PARSER_H_
#define MEDIA_FILTERS_VP9_PARSER_H_

#include <stddef.h>
#include <stdint.h>

#include <deque>

#include "base/macros.h"
#include "media/base/media_export.h"
#include "media/filters/vp9_raw_bits_reader.h"

namespace media {

const int kVp9MaxProfile = 4;
const int kVp9NumRefFramesLog2 = 3;
const size_t kVp9NumRefFrames = 1 << kVp9NumRefFramesLog2;
const uint8_t kVp9MaxProb = 255;
const size_t kVp9NumRefsPerFrame = 3;

enum class Vp9ColorSpace {
    UNKNOWN = 0,
    BT_601 = 1,
    BT_709 = 2,
    SMPTE_170 = 3,
    SMPTE_240 = 4,
    BT_2020 = 5,
    RESERVED = 6,
    SRGB = 7,
};

enum Vp9InterpFilter {
    EIGHTTAP = 0,
    EIGHTTAP_SMOOTH = 1,
    EIGHTTAP_SHARP = 2,
    BILINEAR = 3,
    SWICHABLE = 4,
};

struct MEDIA_EXPORT Vp9Segmentation {
    static const size_t kNumSegments = 8;
    static const size_t kNumTreeProbs = kNumSegments - 1;
    static const size_t kNumPredictionProbs = 3;
    enum SegmentLevelFeature {
        SEG_LVL_ALT_Q = 0,
        SEG_LVL_ALT_LF = 1,
        SEG_LVL_REF_FRAME = 2,
        SEG_LVL_SKIP = 3,
        SEG_LVL_MAX
    };

    bool enabled;

    bool update_map;
    uint8_t tree_probs[kNumTreeProbs];
    bool temporal_update;
    uint8_t pred_probs[kNumPredictionProbs];

    bool update_data;
    bool abs_delta;
    bool feature_enabled[kNumSegments][SEG_LVL_MAX];
    int8_t feature_data[kNumSegments][SEG_LVL_MAX];

    int16_t y_dequant[kNumSegments][2];
    int16_t uv_dequant[kNumSegments][2];

    bool FeatureEnabled(size_t seg_id, SegmentLevelFeature feature) const
    {
        return feature_enabled[seg_id][feature];
    }

    int8_t FeatureData(size_t seg_id, SegmentLevelFeature feature) const
    {
        return feature_data[seg_id][feature];
    }
};

struct MEDIA_EXPORT Vp9LoopFilter {
    enum Vp9FrameType {
        VP9_FRAME_INTRA = 0,
        VP9_FRAME_LAST = 1,
        VP9_FRAME_GOLDEN = 2,
        VP9_FRAME_ALTREF = 3,
        VP9_FRAME_MAX = 4,
    };

    static const size_t kNumModeDeltas = 2;

    uint8_t filter_level;
    uint8_t sharpness_level;

    bool mode_ref_delta_enabled;
    bool mode_ref_delta_update;
    bool update_ref_deltas[VP9_FRAME_MAX];
    int8_t ref_deltas[VP9_FRAME_MAX];
    bool update_mode_deltas[kNumModeDeltas];
    int8_t mode_deltas[kNumModeDeltas];

    uint8_t lvl[Vp9Segmentation::kNumSegments][VP9_FRAME_MAX][kNumModeDeltas];
};

// Members of Vp9FrameHeader will be 0-initialized by Vp9Parser::ParseNextFrame.
struct MEDIA_EXPORT Vp9QuantizationParams {
    bool IsLossless() const
    {
        return base_qindex == 0 && y_dc_delta == 0 && uv_dc_delta == 0 && uv_ac_delta == 0;
    }

    uint8_t base_qindex;
    int8_t y_dc_delta;
    int8_t uv_dc_delta;
    int8_t uv_ac_delta;
};

// VP9 frame header.
struct MEDIA_EXPORT Vp9FrameHeader {
    enum FrameType {
        KEYFRAME = 0,
        INTERFRAME = 1,
    };

    bool IsKeyframe() const;
    bool RefreshFlag(size_t i) const { return !!(refresh_flags & (1u << i)); }

    uint8_t profile;

    bool show_existing_frame;
    uint8_t frame_to_show;

    FrameType frame_type;

    bool show_frame;
    bool error_resilient_mode;

    uint8_t bit_depth;
    Vp9ColorSpace color_space;
    bool yuv_range;
    uint8_t subsampling_x;
    uint8_t subsampling_y;

    // The range of width and height is 1..2^16.
    uint32_t width;
    uint32_t height;
    uint32_t display_width;
    uint32_t display_height;

    bool intra_only;
    uint8_t reset_context;
    uint8_t refresh_flags;
    uint8_t frame_refs[kVp9NumRefsPerFrame];
    bool ref_sign_biases[kVp9NumRefsPerFrame];
    bool allow_high_precision_mv;
    Vp9InterpFilter interp_filter;

    bool refresh_frame_context;
    bool frame_parallel_decoding_mode;
    uint8_t frame_context_idx;

    Vp9QuantizationParams quant_params;

    uint8_t log2_tile_cols;
    uint8_t log2_tile_rows;

    // Pointer to the beginning of frame data. It is a responsibility of the
    // client of the Vp9Parser to maintain validity of this data while it is
    // being used outside of that class.
    const uint8_t* data;

    // Size of |data| in bytes.
    size_t frame_size;

    // Size of compressed header in bytes.
    size_t first_partition_size;

    // Size of uncompressed header in bytes.
    size_t uncompressed_header_size;
};

// A parser for VP9 bitstream.
class MEDIA_EXPORT Vp9Parser {
public:
    // ParseNextFrame() return values. See documentation for ParseNextFrame().
    enum Result {
        kOk,
        kInvalidStream,
        kEOStream,
    };

    Vp9Parser();
    ~Vp9Parser();

    // Set a new stream buffer to read from, starting at |stream| and of size
    // |stream_size| in bytes. |stream| must point to the beginning of a single
    // frame or a single superframe, is owned by caller and must remain valid
    // until the next call to SetStream().
    void SetStream(const uint8_t* stream, off_t stream_size);

    // Parse the next frame in the current stream buffer, filling |fhdr| with
    // the parsed frame header and updating current segmentation and loop filter
    // state. Return kOk if a frame has successfully been parsed, kEOStream if
    // there is no more data in the current stream buffer, or kInvalidStream
    // on error.
    Result ParseNextFrame(Vp9FrameHeader* fhdr);

    // Return current segmentation state.
    const Vp9Segmentation& GetSegmentation() const { return segmentation_; }

    // Return current loop filter state.
    const Vp9LoopFilter& GetLoopFilter() const { return loop_filter_; }

    // Clear parser state and return to an initialized state.
    void Reset();

private:
    // The parsing context to keep track of references.
    struct ReferenceSlot {
        uint32_t width;
        uint32_t height;
    };

    bool ParseSuperframe();
    uint8_t ReadProfile();
    bool VerifySyncCode();
    bool ReadBitDepthColorSpaceSampling(Vp9FrameHeader* fhdr);
    void ReadFrameSize(Vp9FrameHeader* fhdr);
    bool ReadFrameSizeFromRefs(Vp9FrameHeader* fhdr);
    void ReadDisplayFrameSize(Vp9FrameHeader* fhdr);
    Vp9InterpFilter ReadInterpFilter();
    void ReadLoopFilter();
    void ReadQuantization(Vp9QuantizationParams* quants);
    void ReadSegmentationMap();
    void ReadSegmentationData();
    void ReadSegmentation();
    void ReadTiles(Vp9FrameHeader* fhdr);
    bool ParseUncompressedHeader(const uint8_t* stream,
        off_t frame_size,
        Vp9FrameHeader* fhdr);
    void UpdateSlots(const Vp9FrameHeader* fhdr);

    void ResetLoopfilter();
    void SetupPastIndependence();
    size_t GetQIndex(const Vp9QuantizationParams& quant, size_t segid) const;
    void SetupSegmentationDequant(const Vp9QuantizationParams& quant);
    void SetupLoopFilter();

    // Current address in the bitstream buffer.
    const uint8_t* stream_;

    // Remaining bytes in stream_.
    off_t bytes_left_;

    // Stores start pointer and size of each frame within the current superframe.
    struct FrameInfo {
        FrameInfo(const uint8_t* ptr, off_t size);

        // Starting address of the frame.
        const uint8_t* ptr;

        // Size of the frame in bytes.
        off_t size;
    };

    // FrameInfo for the remaining frames in the current superframe to be parsed.
    std::deque<FrameInfo> frames_;

    // Raw bits decoder for uncompressed frame header.
    Vp9RawBitsReader reader_;

    // Segmentation and loop filter state that persists across frames.
    Vp9Segmentation segmentation_;
    Vp9LoopFilter loop_filter_;

    // The parsing context to keep track of references.
    ReferenceSlot ref_slots_[kVp9NumRefFrames];

    DISALLOW_COPY_AND_ASSIGN(Vp9Parser);
};

} // namespace media

#endif // MEDIA_FILTERS_VP9_PARSER_H_
