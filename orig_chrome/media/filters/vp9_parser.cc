// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// This file contains an implementation of a VP9 bitstream parser.

#include "media/filters/vp9_parser.h"

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"

namespace {

const int kMaxLoopFilterLevel = 63;

// Helper function for Vp9Parser::ReadTiles. Defined as get_min_log2_tile_cols
// in spec.
int GetMinLog2TileCols(int sb64_cols)
{
    const int kMaxTileWidthB64 = 64;
    int min_log2 = 0;
    while ((kMaxTileWidthB64 << min_log2) < sb64_cols)
        min_log2++;
    return min_log2;
}

// Helper function for Vp9Parser::ReadTiles. Defined as get_max_log2_tile_cols
// in spec.
int GetMaxLog2TileCols(int sb64_cols)
{
    const int kMinTileWidthB64 = 4;
    int max_log2 = 1;
    while ((sb64_cols >> max_log2) >= kMinTileWidthB64)
        max_log2++;
    return max_log2 - 1;
}

} // namespace

namespace media {

bool Vp9FrameHeader::IsKeyframe() const
{
    // When show_existing_frame is true, the frame header does not precede an
    // actual frame to be decoded, so frame_type does not apply (and is not read
    // from the stream).
    return !show_existing_frame && frame_type == KEYFRAME;
}

Vp9Parser::FrameInfo::FrameInfo(const uint8_t* ptr, off_t size)
    : ptr(ptr)
    , size(size)
{
}

Vp9Parser::Vp9Parser()
{
    Reset();
}

Vp9Parser::~Vp9Parser() { }

void Vp9Parser::SetStream(const uint8_t* stream, off_t stream_size)
{
    DCHECK(stream);
    stream_ = stream;
    bytes_left_ = stream_size;
    frames_.clear();
}

void Vp9Parser::Reset()
{
    stream_ = nullptr;
    bytes_left_ = 0;
    frames_.clear();

    memset(&segmentation_, 0, sizeof(segmentation_));
    memset(&loop_filter_, 0, sizeof(loop_filter_));
    memset(&ref_slots_, 0, sizeof(ref_slots_));
}

uint8_t Vp9Parser::ReadProfile()
{
    uint8_t profile = 0;

    // LSB first.
    if (reader_.ReadBool())
        profile |= 1;
    if (reader_.ReadBool())
        profile |= 2;
    if (profile > 2 && reader_.ReadBool())
        profile += 1;
    return profile;
}

bool Vp9Parser::VerifySyncCode()
{
    const int kSyncCode = 0x498342;
    if (reader_.ReadLiteral(8 * 3) != kSyncCode) {
        DVLOG(1) << "Invalid frame sync code";
        return false;
    }
    return true;
}

bool Vp9Parser::ReadBitDepthColorSpaceSampling(Vp9FrameHeader* fhdr)
{
    if (fhdr->profile == 2 || fhdr->profile == 3) {
        fhdr->bit_depth = reader_.ReadBool() ? 12 : 10;
    } else {
        fhdr->bit_depth = 8;
    }

    fhdr->color_space = static_cast<Vp9ColorSpace>(reader_.ReadLiteral(3));
    if (fhdr->color_space != Vp9ColorSpace::SRGB) {
        fhdr->yuv_range = reader_.ReadBool();
        if (fhdr->profile == 1 || fhdr->profile == 3) {
            fhdr->subsampling_x = reader_.ReadBool() ? 1 : 0;
            fhdr->subsampling_y = reader_.ReadBool() ? 1 : 0;
            if (fhdr->subsampling_x == 1 && fhdr->subsampling_y == 1) {
                DVLOG(1) << "4:2:0 color not supported in profile 1 or 3";
                return false;
            }
            bool reserved = reader_.ReadBool();
            if (reserved) {
                DVLOG(1) << "reserved bit set";
                return false;
            }
        } else {
            fhdr->subsampling_x = fhdr->subsampling_y = 1;
        }
    } else {
        if (fhdr->profile == 1 || fhdr->profile == 3) {
            fhdr->subsampling_x = fhdr->subsampling_y = 0;

            bool reserved = reader_.ReadBool();
            if (reserved) {
                DVLOG(1) << "reserved bit set";
                return false;
            }
        } else {
            DVLOG(1) << "4:4:4 color not supported in profile 0 or 2";
            return false;
        }
    }

    return true;
}

void Vp9Parser::ReadFrameSize(Vp9FrameHeader* fhdr)
{
    fhdr->width = reader_.ReadLiteral(16) + 1;
    fhdr->height = reader_.ReadLiteral(16) + 1;
}

bool Vp9Parser::ReadFrameSizeFromRefs(Vp9FrameHeader* fhdr)
{
    for (size_t i = 0; i < kVp9NumRefsPerFrame; i++) {
        if (reader_.ReadBool()) {
            fhdr->width = ref_slots_[i].width;
            fhdr->height = ref_slots_[i].height;

            const int kMaxDimension = 1 << 16;
            if (fhdr->width == 0 || fhdr->width > kMaxDimension || fhdr->height == 0 || fhdr->height > kMaxDimension) {
                DVLOG(1) << "The size of reference frame is out of range: "
                         << ref_slots_[i].width << "," << ref_slots_[i].height;
                return false;
            }
            return true;
        }
    }

    fhdr->width = reader_.ReadLiteral(16) + 1;
    fhdr->height = reader_.ReadLiteral(16) + 1;
    return true;
}

void Vp9Parser::ReadDisplayFrameSize(Vp9FrameHeader* fhdr)
{
    if (reader_.ReadBool()) {
        fhdr->display_width = reader_.ReadLiteral(16) + 1;
        fhdr->display_height = reader_.ReadLiteral(16) + 1;
    } else {
        fhdr->display_width = fhdr->width;
        fhdr->display_height = fhdr->height;
    }
}

Vp9InterpFilter Vp9Parser::ReadInterpFilter()
{
    if (reader_.ReadBool())
        return Vp9InterpFilter::SWICHABLE;

    // The mapping table for next two bits.
    const Vp9InterpFilter table[] = {
        Vp9InterpFilter::EIGHTTAP_SMOOTH,
        Vp9InterpFilter::EIGHTTAP,
        Vp9InterpFilter::EIGHTTAP_SHARP,
        Vp9InterpFilter::BILINEAR,
    };
    return table[reader_.ReadLiteral(2)];
}

void Vp9Parser::ReadLoopFilter()
{
    loop_filter_.filter_level = reader_.ReadLiteral(6);
    loop_filter_.sharpness_level = reader_.ReadLiteral(3);
    loop_filter_.mode_ref_delta_update = false;

    loop_filter_.mode_ref_delta_enabled = reader_.ReadBool();
    if (loop_filter_.mode_ref_delta_enabled) {
        loop_filter_.mode_ref_delta_update = reader_.ReadBool();
        if (loop_filter_.mode_ref_delta_update) {
            for (size_t i = 0; i < Vp9LoopFilter::VP9_FRAME_MAX; i++) {
                loop_filter_.update_ref_deltas[i] = reader_.ReadBool();
                if (loop_filter_.update_ref_deltas[i])
                    loop_filter_.ref_deltas[i] = reader_.ReadSignedLiteral(6);
            }

            for (size_t i = 0; i < Vp9LoopFilter::kNumModeDeltas; i++) {
                loop_filter_.update_mode_deltas[i] = reader_.ReadBool();
                if (loop_filter_.update_mode_deltas[i])
                    loop_filter_.mode_deltas[i] = reader_.ReadLiteral(6);
            }
        }
    }
}

void Vp9Parser::ReadQuantization(Vp9QuantizationParams* quants)
{
    quants->base_qindex = reader_.ReadLiteral(8);

    if (reader_.ReadBool())
        quants->y_dc_delta = reader_.ReadSignedLiteral(4);

    if (reader_.ReadBool())
        quants->uv_ac_delta = reader_.ReadSignedLiteral(4);

    if (reader_.ReadBool())
        quants->uv_dc_delta = reader_.ReadSignedLiteral(4);
}

void Vp9Parser::ReadSegmentationMap()
{
    for (size_t i = 0; i < Vp9Segmentation::kNumTreeProbs; i++) {
        segmentation_.tree_probs[i] = reader_.ReadBool() ? reader_.ReadLiteral(8) : kVp9MaxProb;
    }

    for (size_t i = 0; i < Vp9Segmentation::kNumPredictionProbs; i++)
        segmentation_.pred_probs[i] = kVp9MaxProb;

    segmentation_.temporal_update = reader_.ReadBool();
    if (segmentation_.temporal_update) {
        for (size_t i = 0; i < Vp9Segmentation::kNumPredictionProbs; i++) {
            if (reader_.ReadBool())
                segmentation_.pred_probs[i] = reader_.ReadLiteral(8);
        }
    }
}

void Vp9Parser::ReadSegmentationData()
{
    segmentation_.abs_delta = reader_.ReadBool();

    const int kFeatureDataBits[] = { 7, 6, 2, 0 };
    const bool kFeatureDataSigned[] = { true, true, false, false };

    for (size_t i = 0; i < Vp9Segmentation::kNumSegments; i++) {
        for (size_t j = 0; j < Vp9Segmentation::SEG_LVL_MAX; j++) {
            int8_t data = 0;
            segmentation_.feature_enabled[i][j] = reader_.ReadBool();
            if (segmentation_.feature_enabled[i][j]) {
                data = reader_.ReadLiteral(kFeatureDataBits[j]);
                if (kFeatureDataSigned[j])
                    if (reader_.ReadBool())
                        data = -data;
            }
            segmentation_.feature_data[i][j] = data;
        }
    }
}

void Vp9Parser::ReadSegmentation()
{
    segmentation_.update_map = false;
    segmentation_.update_data = false;

    segmentation_.enabled = reader_.ReadBool();
    if (!segmentation_.enabled)
        return;

    segmentation_.update_map = reader_.ReadBool();
    if (segmentation_.update_map)
        ReadSegmentationMap();

    segmentation_.update_data = reader_.ReadBool();
    if (segmentation_.update_data)
        ReadSegmentationData();
}

void Vp9Parser::ReadTiles(Vp9FrameHeader* fhdr)
{
    int sb64_cols = (fhdr->width + 63) / 64;

    int min_log2_tile_cols = GetMinLog2TileCols(sb64_cols);
    int max_log2_tile_cols = GetMaxLog2TileCols(sb64_cols);

    int max_ones = max_log2_tile_cols - min_log2_tile_cols;
    fhdr->log2_tile_cols = min_log2_tile_cols;
    while (max_ones-- && reader_.ReadBool())
        fhdr->log2_tile_cols++;

    if (reader_.ReadBool())
        fhdr->log2_tile_rows = reader_.ReadLiteral(2) - 1;
}

bool Vp9Parser::ParseUncompressedHeader(const uint8_t* stream,
    off_t frame_size,
    Vp9FrameHeader* fhdr)
{
    reader_.Initialize(stream, frame_size);

    fhdr->data = stream;
    fhdr->frame_size = frame_size;

    // frame marker
    if (reader_.ReadLiteral(2) != 0x2)
        return false;

    fhdr->profile = ReadProfile();
    if (fhdr->profile >= kVp9MaxProfile) {
        DVLOG(1) << "Unsupported bitstream profile";
        return false;
    }

    fhdr->show_existing_frame = reader_.ReadBool();
    if (fhdr->show_existing_frame) {
        fhdr->frame_to_show = reader_.ReadLiteral(3);
        fhdr->show_frame = true;

        if (!reader_.IsValid()) {
            DVLOG(1) << "parser reads beyond the end of buffer";
            return false;
        }
        fhdr->uncompressed_header_size = reader_.GetBytesRead();
        return true;
    }

    fhdr->frame_type = static_cast<Vp9FrameHeader::FrameType>(reader_.ReadBool());
    fhdr->show_frame = reader_.ReadBool();
    fhdr->error_resilient_mode = reader_.ReadBool();

    if (fhdr->IsKeyframe()) {
        if (!VerifySyncCode())
            return false;

        if (!ReadBitDepthColorSpaceSampling(fhdr))
            return false;

        fhdr->refresh_flags = 0xff;

        ReadFrameSize(fhdr);
        ReadDisplayFrameSize(fhdr);
    } else {
        if (!fhdr->show_frame)
            fhdr->intra_only = reader_.ReadBool();

        if (!fhdr->error_resilient_mode)
            fhdr->reset_context = reader_.ReadLiteral(2);

        if (fhdr->intra_only) {
            if (!VerifySyncCode())
                return false;

            if (fhdr->profile > 0) {
                if (!ReadBitDepthColorSpaceSampling(fhdr))
                    return false;
            } else {
                fhdr->bit_depth = 8;
                fhdr->color_space = Vp9ColorSpace::BT_601;
                fhdr->subsampling_x = fhdr->subsampling_y = 1;
            }

            fhdr->refresh_flags = reader_.ReadLiteral(8);

            ReadFrameSize(fhdr);
            ReadDisplayFrameSize(fhdr);
        } else {
            fhdr->refresh_flags = reader_.ReadLiteral(8);

            for (size_t i = 0; i < kVp9NumRefsPerFrame; i++) {
                fhdr->frame_refs[i] = reader_.ReadLiteral(kVp9NumRefFramesLog2);
                fhdr->ref_sign_biases[i] = reader_.ReadBool();
            }

            if (!ReadFrameSizeFromRefs(fhdr))
                return false;
            ReadDisplayFrameSize(fhdr);

            fhdr->allow_high_precision_mv = reader_.ReadBool();
            fhdr->interp_filter = ReadInterpFilter();
        }
    }

    if (fhdr->error_resilient_mode) {
        fhdr->frame_parallel_decoding_mode = true;
    } else {
        fhdr->refresh_frame_context = reader_.ReadBool();
        fhdr->frame_parallel_decoding_mode = reader_.ReadBool();
    }

    fhdr->frame_context_idx = reader_.ReadLiteral(2);

    if (fhdr->IsKeyframe() || fhdr->intra_only)
        SetupPastIndependence();

    ReadLoopFilter();
    ReadQuantization(&fhdr->quant_params);
    ReadSegmentation();

    ReadTiles(fhdr);

    fhdr->first_partition_size = reader_.ReadLiteral(16);
    if (fhdr->first_partition_size == 0) {
        DVLOG(1) << "invalid header size";
        return false;
    }

    if (!reader_.IsValid()) {
        DVLOG(1) << "parser reads beyond the end of buffer";
        return false;
    }
    fhdr->uncompressed_header_size = reader_.GetBytesRead();

    SetupSegmentationDequant(fhdr->quant_params);
    SetupLoopFilter();

    UpdateSlots(fhdr);

    return true;
}

void Vp9Parser::UpdateSlots(const Vp9FrameHeader* fhdr)
{
    for (size_t i = 0; i < kVp9NumRefFrames; i++) {
        if (fhdr->RefreshFlag(i)) {
            ref_slots_[i].width = fhdr->width;
            ref_slots_[i].height = fhdr->height;
        }
    }
}

Vp9Parser::Result Vp9Parser::ParseNextFrame(Vp9FrameHeader* fhdr)
{
    if (frames_.empty()) {
        // No frames to be decoded, if there is no more stream, request more.
        if (!stream_)
            return kEOStream;

        // New stream to be parsed, parse it and fill frames_.
        if (!ParseSuperframe()) {
            DVLOG(1) << "Failed parsing superframes";
            return kInvalidStream;
        }
    }

    DCHECK(!frames_.empty());
    FrameInfo frame_info = frames_.front();
    frames_.pop_front();

    memset(fhdr, 0, sizeof(*fhdr));
    if (!ParseUncompressedHeader(frame_info.ptr, frame_info.size, fhdr))
        return kInvalidStream;

    return kOk;
}

bool Vp9Parser::ParseSuperframe()
{
    const uint8_t* stream = stream_;
    off_t bytes_left = bytes_left_;

    DCHECK(frames_.empty());

    // Make sure we don't parse stream_ more than once.
    stream_ = nullptr;
    bytes_left_ = 0;

    if (bytes_left < 1)
        return false;

    // If this is a superframe, the last byte in the stream will contain the
    // superframe marker. If not, the whole buffer contains a single frame.
    uint8_t marker = *(stream + bytes_left - 1);
    if ((marker & 0xe0) != 0xc0) {
        frames_.push_back(FrameInfo(stream, bytes_left));
        return true;
    }

    DVLOG(1) << "Parsing a superframe";

    // The bytes immediately before the superframe marker constitute superframe
    // index, which stores information about sizes of each frame in it.
    // Calculate its size and set index_ptr to the beginning of it.
    size_t num_frames = (marker & 0x7) + 1;
    size_t mag = ((marker >> 3) & 0x3) + 1;
    off_t index_size = 2 + mag * num_frames;

    if (bytes_left < index_size)
        return false;

    const uint8_t* index_ptr = stream + bytes_left - index_size;
    if (marker != *index_ptr)
        return false;

    ++index_ptr;
    bytes_left -= index_size;

    // Parse frame information contained in the index and add a pointer to and
    // size of each frame to frames_.
    for (size_t i = 0; i < num_frames; ++i) {
        uint32_t size = 0;
        for (size_t j = 0; j < mag; ++j) {
            size |= *index_ptr << (j * 8);
            ++index_ptr;
        }

        if (base::checked_cast<off_t>(size) > bytes_left) {
            DVLOG(1) << "Not enough data in the buffer for frame " << i;
            return false;
        }

        frames_.push_back(FrameInfo(stream, size));
        stream += size;
        bytes_left -= size;

        DVLOG(1) << "Frame " << i << ", size: " << size;
    }

    return true;
}

void Vp9Parser::ResetLoopfilter()
{
    loop_filter_.mode_ref_delta_enabled = true;
    loop_filter_.mode_ref_delta_update = true;

    const int8_t default_ref_deltas[] = { 1, 0, -1, -1 };
    static_assert(
        arraysize(default_ref_deltas) == arraysize(loop_filter_.ref_deltas),
        "ref_deltas arrays of incorrect size");
    for (size_t i = 0; i < arraysize(loop_filter_.ref_deltas); ++i)
        loop_filter_.ref_deltas[i] = default_ref_deltas[i];

    memset(loop_filter_.mode_deltas, 0, sizeof(loop_filter_.mode_deltas));
}

void Vp9Parser::SetupPastIndependence()
{
    memset(&segmentation_, 0, sizeof(segmentation_));
    ResetLoopfilter();
}

const size_t QINDEX_RANGE = 256;
const int16_t kDcQLookup[QINDEX_RANGE] = {
    4,
    8,
    8,
    9,
    10,
    11,
    12,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    38,
    39,
    40,
    41,
    42,
    43,
    43,
    44,
    45,
    46,
    47,
    48,
    48,
    49,
    50,
    51,
    52,
    53,
    53,
    54,
    55,
    56,
    57,
    57,
    58,
    59,
    60,
    61,
    62,
    62,
    63,
    64,
    65,
    66,
    66,
    67,
    68,
    69,
    70,
    70,
    71,
    72,
    73,
    74,
    74,
    75,
    76,
    77,
    78,
    78,
    79,
    80,
    81,
    81,
    82,
    83,
    84,
    85,
    85,
    87,
    88,
    90,
    92,
    93,
    95,
    96,
    98,
    99,
    101,
    102,
    104,
    105,
    107,
    108,
    110,
    111,
    113,
    114,
    116,
    117,
    118,
    120,
    121,
    123,
    125,
    127,
    129,
    131,
    134,
    136,
    138,
    140,
    142,
    144,
    146,
    148,
    150,
    152,
    154,
    156,
    158,
    161,
    164,
    166,
    169,
    172,
    174,
    177,
    180,
    182,
    185,
    187,
    190,
    192,
    195,
    199,
    202,
    205,
    208,
    211,
    214,
    217,
    220,
    223,
    226,
    230,
    233,
    237,
    240,
    243,
    247,
    250,
    253,
    257,
    261,
    265,
    269,
    272,
    276,
    280,
    284,
    288,
    292,
    296,
    300,
    304,
    309,
    313,
    317,
    322,
    326,
    330,
    335,
    340,
    344,
    349,
    354,
    359,
    364,
    369,
    374,
    379,
    384,
    389,
    395,
    400,
    406,
    411,
    417,
    423,
    429,
    435,
    441,
    447,
    454,
    461,
    467,
    475,
    482,
    489,
    497,
    505,
    513,
    522,
    530,
    539,
    549,
    559,
    569,
    579,
    590,
    602,
    614,
    626,
    640,
    654,
    668,
    684,
    700,
    717,
    736,
    755,
    775,
    796,
    819,
    843,
    869,
    896,
    925,
    955,
    988,
    1022,
    1058,
    1098,
    1139,
    1184,
    1232,
    1282,
    1336,
};

const int16_t kAcQLookup[QINDEX_RANGE] = {
    4,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21,
    22,
    23,
    24,
    25,
    26,
    27,
    28,
    29,
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    38,
    39,
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    48,
    49,
    50,
    51,
    52,
    53,
    54,
    55,
    56,
    57,
    58,
    59,
    60,
    61,
    62,
    63,
    64,
    65,
    66,
    67,
    68,
    69,
    70,
    71,
    72,
    73,
    74,
    75,
    76,
    77,
    78,
    79,
    80,
    81,
    82,
    83,
    84,
    85,
    86,
    87,
    88,
    89,
    90,
    91,
    92,
    93,
    94,
    95,
    96,
    97,
    98,
    99,
    100,
    101,
    102,
    104,
    106,
    108,
    110,
    112,
    114,
    116,
    118,
    120,
    122,
    124,
    126,
    128,
    130,
    132,
    134,
    136,
    138,
    140,
    142,
    144,
    146,
    148,
    150,
    152,
    155,
    158,
    161,
    164,
    167,
    170,
    173,
    176,
    179,
    182,
    185,
    188,
    191,
    194,
    197,
    200,
    203,
    207,
    211,
    215,
    219,
    223,
    227,
    231,
    235,
    239,
    243,
    247,
    251,
    255,
    260,
    265,
    270,
    275,
    280,
    285,
    290,
    295,
    300,
    305,
    311,
    317,
    323,
    329,
    335,
    341,
    347,
    353,
    359,
    366,
    373,
    380,
    387,
    394,
    401,
    408,
    416,
    424,
    432,
    440,
    448,
    456,
    465,
    474,
    483,
    492,
    501,
    510,
    520,
    530,
    540,
    550,
    560,
    571,
    582,
    593,
    604,
    615,
    627,
    639,
    651,
    663,
    676,
    689,
    702,
    715,
    729,
    743,
    757,
    771,
    786,
    801,
    816,
    832,
    848,
    864,
    881,
    898,
    915,
    933,
    951,
    969,
    988,
    1007,
    1026,
    1046,
    1066,
    1087,
    1108,
    1129,
    1151,
    1173,
    1196,
    1219,
    1243,
    1267,
    1292,
    1317,
    1343,
    1369,
    1396,
    1423,
    1451,
    1479,
    1508,
    1537,
    1567,
    1597,
    1628,
    1660,
    1692,
    1725,
    1759,
    1793,
    1828,
};

static_assert(arraysize(kDcQLookup) == arraysize(kAcQLookup),
    "quantizer lookup arrays of incorrect size");

#define CLAMP_Q(q) \
    std::min(std::max(static_cast<size_t>(0), q), arraysize(kDcQLookup) - 1)

size_t Vp9Parser::GetQIndex(const Vp9QuantizationParams& quant,
    size_t segid) const
{
    if (segmentation_.FeatureEnabled(segid, Vp9Segmentation::SEG_LVL_ALT_Q)) {
        int8_t feature_data = segmentation_.FeatureData(segid, Vp9Segmentation::SEG_LVL_ALT_Q);
        size_t q_index = segmentation_.abs_delta ? feature_data
                                                 : quant.base_qindex + feature_data;
        return CLAMP_Q(q_index);
    }

    return quant.base_qindex;
}

void Vp9Parser::SetupSegmentationDequant(const Vp9QuantizationParams& quant)
{
    if (segmentation_.enabled) {
        for (size_t i = 0; i < Vp9Segmentation::kNumSegments; ++i) {
            const size_t q_index = GetQIndex(quant, i);
            segmentation_.y_dequant[i][0] = kDcQLookup[CLAMP_Q(q_index + quant.y_dc_delta)];
            segmentation_.y_dequant[i][1] = kAcQLookup[CLAMP_Q(q_index)];
            segmentation_.uv_dequant[i][0] = kDcQLookup[CLAMP_Q(q_index + quant.uv_dc_delta)];
            segmentation_.uv_dequant[i][1] = kAcQLookup[CLAMP_Q(q_index + quant.uv_ac_delta)];
        }
    } else {
        const size_t q_index = quant.base_qindex;
        segmentation_.y_dequant[0][0] = kDcQLookup[CLAMP_Q(q_index + quant.y_dc_delta)];
        segmentation_.y_dequant[0][1] = kAcQLookup[CLAMP_Q(q_index)];
        segmentation_.uv_dequant[0][0] = kDcQLookup[CLAMP_Q(q_index + quant.uv_dc_delta)];
        segmentation_.uv_dequant[0][1] = kAcQLookup[CLAMP_Q(q_index + quant.uv_ac_delta)];
    }
}
#undef CLAMP_Q

#define CLAMP_LF(l) std::min(std::max(0, l), kMaxLoopFilterLevel)
void Vp9Parser::SetupLoopFilter()
{
    if (!loop_filter_.filter_level)
        return;

    int scale = loop_filter_.filter_level < 32 ? 1 : 2;

    for (size_t i = 0; i < Vp9Segmentation::kNumSegments; ++i) {
        int level = loop_filter_.filter_level;

        if (segmentation_.FeatureEnabled(i, Vp9Segmentation::SEG_LVL_ALT_LF)) {
            int feature_data = segmentation_.FeatureData(i, Vp9Segmentation::SEG_LVL_ALT_LF);
            level = CLAMP_LF(segmentation_.abs_delta ? feature_data
                                                     : level + feature_data);
        }

        if (!loop_filter_.mode_ref_delta_enabled) {
            memset(loop_filter_.lvl[i], level, sizeof(loop_filter_.lvl[i]));
        } else {
            loop_filter_.lvl[i][Vp9LoopFilter::VP9_FRAME_INTRA][0] = CLAMP_LF(
                level + loop_filter_.ref_deltas[Vp9LoopFilter::VP9_FRAME_INTRA] * scale);
            loop_filter_.lvl[i][Vp9LoopFilter::VP9_FRAME_INTRA][1] = 0;

            for (size_t type = Vp9LoopFilter::VP9_FRAME_LAST;
                 type < Vp9LoopFilter::VP9_FRAME_MAX; ++type) {
                for (size_t mode = 0; mode < Vp9LoopFilter::kNumModeDeltas; ++mode) {
                    loop_filter_.lvl[i][type][mode] = CLAMP_LF(level + loop_filter_.ref_deltas[type] * scale + loop_filter_.mode_deltas[mode] * scale);
                }
            }
        }
    }
}
#undef CLAMP_LF

} // namespace media
