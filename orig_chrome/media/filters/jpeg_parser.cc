// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/jpeg_parser.h"

#include "base/big_endian.h"
#include "base/logging.h"

using base::BigEndianReader;

#define READ_U8_OR_RETURN_FALSE(out)                                             \
    do {                                                                         \
        uint8_t _out;                                                            \
        if (!reader.ReadU8(&_out)) {                                             \
            DVLOG(1)                                                             \
                << "Error in stream: unexpected EOS while trying to read " #out; \
            return false;                                                        \
        }                                                                        \
        *(out) = _out;                                                           \
    } while (0)

#define READ_U16_OR_RETURN_FALSE(out)                                            \
    do {                                                                         \
        uint16_t _out;                                                           \
        if (!reader.ReadU16(&_out)) {                                            \
            DVLOG(1)                                                             \
                << "Error in stream: unexpected EOS while trying to read " #out; \
            return false;                                                        \
        }                                                                        \
        *(out) = _out;                                                           \
    } while (0)

namespace media {

static bool InRange(int value, int a, int b)
{
    return a <= value && value <= b;
}

// Round up |value| to multiple of |mul|. |value| must be non-negative.
// |mul| must be positive.
static int RoundUp(int value, int mul)
{
    DCHECK_GE(value, 0);
    DCHECK_GE(mul, 1);
    return (value + mul - 1) / mul * mul;
}

// |frame_header| is already initialized to 0 in ParseJpegPicture.
static bool ParseSOF(const char* buffer,
    size_t length,
    JpegFrameHeader* frame_header)
{
    // Spec B.2.2 Frame header syntax
    DCHECK(buffer);
    DCHECK(frame_header);
    BigEndianReader reader(buffer, length);

    uint8_t precision;
    READ_U8_OR_RETURN_FALSE(&precision);
    READ_U16_OR_RETURN_FALSE(&frame_header->visible_height);
    READ_U16_OR_RETURN_FALSE(&frame_header->visible_width);
    READ_U8_OR_RETURN_FALSE(&frame_header->num_components);

    if (precision != 8) {
        DLOG(ERROR) << "Only support 8-bit precision, not "
                    << static_cast<int>(precision) << " bit for baseline";
        return false;
    }
    if (!InRange(frame_header->num_components, 1,
            arraysize(frame_header->components))) {
        DLOG(ERROR) << "num_components="
                    << static_cast<int>(frame_header->num_components)
                    << " is not supported";
        return false;
    }

    int max_h_factor = 0;
    int max_v_factor = 0;
    for (size_t i = 0; i < frame_header->num_components; i++) {
        JpegComponent& component = frame_header->components[i];
        READ_U8_OR_RETURN_FALSE(&component.id);
        if (component.id > frame_header->num_components) {
            DLOG(ERROR) << "component id (" << static_cast<int>(component.id)
                        << ") should be <= num_components ("
                        << static_cast<int>(frame_header->num_components) << ")";
            return false;
        }
        uint8_t hv;
        READ_U8_OR_RETURN_FALSE(&hv);
        component.horizontal_sampling_factor = hv / 16;
        component.vertical_sampling_factor = hv % 16;
        if (component.horizontal_sampling_factor > max_h_factor)
            max_h_factor = component.horizontal_sampling_factor;
        if (component.vertical_sampling_factor > max_v_factor)
            max_v_factor = component.vertical_sampling_factor;
        if (!InRange(component.horizontal_sampling_factor, 1, 4)) {
            DVLOG(1) << "Invalid horizontal sampling factor "
                     << static_cast<int>(component.horizontal_sampling_factor);
            return false;
        }
        if (!InRange(component.vertical_sampling_factor, 1, 4)) {
            DVLOG(1) << "Invalid vertical sampling factor "
                     << static_cast<int>(component.horizontal_sampling_factor);
            return false;
        }
        READ_U8_OR_RETURN_FALSE(&component.quantization_table_selector);
    }

    // The size of data unit is 8*8 and the coded size should be extended
    // to complete minimum coded unit, MCU. See Spec A.2.
    frame_header->coded_width = RoundUp(frame_header->visible_width, max_h_factor * 8);
    frame_header->coded_height = RoundUp(frame_header->visible_height, max_v_factor * 8);

    return true;
}

// |q_table| is already initialized to 0 in ParseJpegPicture.
static bool ParseDQT(const char* buffer,
    size_t length,
    JpegQuantizationTable* q_table)
{
    // Spec B.2.4.1 Quantization table-specification syntax
    DCHECK(buffer);
    DCHECK(q_table);
    BigEndianReader reader(buffer, length);
    while (reader.remaining() > 0) {
        uint8_t precision_and_table_id;
        READ_U8_OR_RETURN_FALSE(&precision_and_table_id);
        uint8_t precision = precision_and_table_id / 16;
        uint8_t table_id = precision_and_table_id % 16;
        if (!InRange(precision, 0, 1)) {
            DVLOG(1) << "Invalid precision " << static_cast<int>(precision);
            return false;
        }
        if (precision == 1) { // 1 means 16-bit precision
            DLOG(ERROR) << "An 8-bit DCT-based process shall not use a 16-bit "
                        << "precision quantization table";
            return false;
        }
        if (table_id >= kJpegMaxQuantizationTableNum) {
            DLOG(ERROR) << "Quantization table id (" << static_cast<int>(table_id)
                        << ") exceeded " << kJpegMaxQuantizationTableNum;
            return false;
        }

        if (!reader.ReadBytes(&q_table[table_id].value,
                sizeof(q_table[table_id].value)))
            return false;
        q_table[table_id].valid = true;
    }
    return true;
}

// |dc_table| and |ac_table| are already initialized to 0 in ParseJpegPicture.
static bool ParseDHT(const char* buffer,
    size_t length,
    JpegHuffmanTable* dc_table,
    JpegHuffmanTable* ac_table)
{
    // Spec B.2.4.2 Huffman table-specification syntax
    DCHECK(buffer);
    DCHECK(dc_table);
    DCHECK(ac_table);
    BigEndianReader reader(buffer, length);
    while (reader.remaining() > 0) {
        uint8_t table_class_and_id;
        READ_U8_OR_RETURN_FALSE(&table_class_and_id);
        int table_class = table_class_and_id / 16;
        int table_id = table_class_and_id % 16;
        if (!InRange(table_class, 0, 1)) {
            DVLOG(1) << "Invalid table class " << table_class;
            return false;
        }
        if (table_id >= 2) {
            DLOG(ERROR) << "Table id(" << table_id
                        << ") >= 2 is invalid for baseline profile";
            return false;
        }

        JpegHuffmanTable* table;
        if (table_class == 1)
            table = &ac_table[table_id];
        else
            table = &dc_table[table_id];

        size_t count = 0;
        if (!reader.ReadBytes(&table->code_length, sizeof(table->code_length)))
            return false;
        for (size_t i = 0; i < arraysize(table->code_length); i++)
            count += table->code_length[i];

        if (!InRange(count, 0, sizeof(table->code_value))) {
            DVLOG(1) << "Invalid code count " << count;
            return false;
        }
        if (!reader.ReadBytes(&table->code_value, count))
            return false;
        table->valid = true;
    }
    return true;
}

static bool ParseDRI(const char* buffer,
    size_t length,
    uint16_t* restart_interval)
{
    // Spec B.2.4.4 Restart interval definition syntax
    DCHECK(buffer);
    DCHECK(restart_interval);
    BigEndianReader reader(buffer, length);
    return reader.ReadU16(restart_interval) && reader.remaining() == 0;
}

// |scan| is already initialized to 0 in ParseJpegPicture.
static bool ParseSOS(const char* buffer,
    size_t length,
    const JpegFrameHeader& frame_header,
    JpegScanHeader* scan)
{
    // Spec B.2.3 Scan header syntax
    DCHECK(buffer);
    DCHECK(scan);
    BigEndianReader reader(buffer, length);
    READ_U8_OR_RETURN_FALSE(&scan->num_components);
    if (scan->num_components != frame_header.num_components) {
        DLOG(ERROR) << "The number of scan components ("
                    << static_cast<int>(scan->num_components)
                    << ") mismatches the number of image components ("
                    << static_cast<int>(frame_header.num_components) << ")";
        return false;
    }

    for (int i = 0; i < scan->num_components; i++) {
        JpegScanHeader::Component* component = &scan->components[i];
        READ_U8_OR_RETURN_FALSE(&component->component_selector);
        uint8_t dc_and_ac_selector;
        READ_U8_OR_RETURN_FALSE(&dc_and_ac_selector);
        component->dc_selector = dc_and_ac_selector / 16;
        component->ac_selector = dc_and_ac_selector % 16;
        if (component->component_selector != frame_header.components[i].id) {
            DLOG(ERROR) << "component selector mismatches image component id";
            return false;
        }
        if (component->dc_selector >= kJpegMaxHuffmanTableNumBaseline) {
            DLOG(ERROR) << "DC selector (" << static_cast<int>(component->dc_selector)
                        << ") should be 0 or 1 for baseline mode";
            return false;
        }
        if (component->ac_selector >= kJpegMaxHuffmanTableNumBaseline) {
            DLOG(ERROR) << "AC selector (" << static_cast<int>(component->ac_selector)
                        << ") should be 0 or 1 for baseline mode";
            return false;
        }
    }

    // Unused fields, only for value checking.
    uint8_t spectral_selection_start;
    uint8_t spectral_selection_end;
    uint8_t point_transform;
    READ_U8_OR_RETURN_FALSE(&spectral_selection_start);
    READ_U8_OR_RETURN_FALSE(&spectral_selection_end);
    READ_U8_OR_RETURN_FALSE(&point_transform);
    if (spectral_selection_start != 0 || spectral_selection_end != 63) {
        DLOG(ERROR) << "Spectral selection should be 0,63 for baseline mode";
        return false;
    }
    if (point_transform != 0) {
        DLOG(ERROR) << "Point transform should be 0 for baseline mode";
        return false;
    }

    return true;
}

// |eoi_ptr| will point to the end of image (after EOI marker) after search
// succeeds. Returns true on EOI marker found, or false.
static bool SearchEOI(const char* buffer, size_t length, const char** eoi_ptr)
{
    DCHECK(buffer);
    DCHECK(eoi_ptr);
    BigEndianReader reader(buffer, length);
    uint8_t marker2;

    while (reader.remaining() > 0) {
        const char* marker1_ptr = static_cast<const char*>(
            memchr(reader.ptr(), JPEG_MARKER_PREFIX, reader.remaining()));
        if (!marker1_ptr)
            return false;
        reader.Skip(marker1_ptr - reader.ptr() + 1);

        do {
            READ_U8_OR_RETURN_FALSE(&marker2);
        } while (marker2 == JPEG_MARKER_PREFIX); // skip fill bytes

        switch (marker2) {
        // Compressed data escape.
        case 0x00:
            break;
        // Restart
        case JPEG_RST0:
        case JPEG_RST1:
        case JPEG_RST2:
        case JPEG_RST3:
        case JPEG_RST4:
        case JPEG_RST5:
        case JPEG_RST6:
        case JPEG_RST7:
            break;
        case JPEG_EOI:
            *eoi_ptr = reader.ptr();
            return true;
        default:
            // Skip for other markers.
            uint16_t size;
            READ_U16_OR_RETURN_FALSE(&size);
            if (size < sizeof(size)) {
                DLOG(ERROR) << "Ill-formed JPEG. Segment size (" << size
                            << ") is smaller than size field (" << sizeof(size)
                            << ")";
                return false;
            }
            size -= sizeof(size);

            if (!reader.Skip(size)) {
                DLOG(ERROR) << "Ill-formed JPEG. Remaining size ("
                            << reader.remaining()
                            << ") is smaller than header specified (" << size << ")";
                return false;
            }
            break;
        }
    }
    return false;
}

// |result| is already initialized to 0 in ParseJpegPicture.
static bool ParseSOI(const char* buffer,
    size_t length,
    JpegParseResult* result)
{
    // Spec B.2.1 High-level syntax
    DCHECK(buffer);
    DCHECK(result);
    BigEndianReader reader(buffer, length);
    uint8_t marker1;
    uint8_t marker2;
    bool has_marker_dqt = false;
    bool has_marker_sos = false;

    // Once reached SOS, all neccesary data are parsed.
    while (!has_marker_sos) {
        READ_U8_OR_RETURN_FALSE(&marker1);
        if (marker1 != JPEG_MARKER_PREFIX)
            return false;

        do {
            READ_U8_OR_RETURN_FALSE(&marker2);
        } while (marker2 == JPEG_MARKER_PREFIX); // skip fill bytes

        uint16_t size;
        READ_U16_OR_RETURN_FALSE(&size);
        // The size includes the size field itself.
        if (size < sizeof(size)) {
            DLOG(ERROR) << "Ill-formed JPEG. Segment size (" << size
                        << ") is smaller than size field (" << sizeof(size) << ")";
            return false;
        }
        size -= sizeof(size);

        if (reader.remaining() < size) {
            DLOG(ERROR) << "Ill-formed JPEG. Remaining size (" << reader.remaining()
                        << ") is smaller than header specified (" << size << ")";
            return false;
        }

        switch (marker2) {
        case JPEG_SOF0:
            if (!ParseSOF(reader.ptr(), size, &result->frame_header)) {
                DLOG(ERROR) << "ParseSOF failed";
                return false;
            }
            break;
        case JPEG_SOF1:
        case JPEG_SOF2:
        case JPEG_SOF3:
        case JPEG_SOF5:
        case JPEG_SOF6:
        case JPEG_SOF7:
        case JPEG_SOF9:
        case JPEG_SOF10:
        case JPEG_SOF11:
        case JPEG_SOF13:
        case JPEG_SOF14:
        case JPEG_SOF15:
            DLOG(ERROR) << "Only SOF0 (baseline) is supported, but got SOF"
                        << (marker2 - JPEG_SOF0);
            return false;
        case JPEG_DQT:
            if (!ParseDQT(reader.ptr(), size, result->q_table)) {
                DLOG(ERROR) << "ParseDQT failed";
                return false;
            }
            has_marker_dqt = true;
            break;
        case JPEG_DHT:
            if (!ParseDHT(reader.ptr(), size, result->dc_table, result->ac_table)) {
                DLOG(ERROR) << "ParseDHT failed";
                return false;
            }
            break;
        case JPEG_DRI:
            if (!ParseDRI(reader.ptr(), size, &result->restart_interval)) {
                DLOG(ERROR) << "ParseDRI failed";
                return false;
            }
            break;
        case JPEG_SOS:
            if (!ParseSOS(reader.ptr(), size, result->frame_header,
                    &result->scan)) {
                DLOG(ERROR) << "ParseSOS failed";
                return false;
            }
            has_marker_sos = true;
            break;
        default:
            DVLOG(4) << "unknown marker " << static_cast<int>(marker2);
            break;
        }
        reader.Skip(size);
    }

    if (!has_marker_dqt) {
        DLOG(ERROR) << "No DQT marker found";
        return false;
    }

    // Scan data follows scan header immediately.
    result->data = reader.ptr();
    result->data_size = reader.remaining();
    const size_t kSoiSize = 2;
    result->image_size = length + kSoiSize;

    return true;
}

bool ParseJpegPicture(const uint8_t* buffer,
    size_t length,
    JpegParseResult* result)
{
    DCHECK(buffer);
    DCHECK(result);
    BigEndianReader reader(reinterpret_cast<const char*>(buffer), length);
    memset(result, 0, sizeof(JpegParseResult));

    uint8_t marker1, marker2;
    READ_U8_OR_RETURN_FALSE(&marker1);
    READ_U8_OR_RETURN_FALSE(&marker2);
    if (marker1 != JPEG_MARKER_PREFIX || marker2 != JPEG_SOI) {
        DLOG(ERROR) << "Not a JPEG";
        return false;
    }

    return ParseSOI(reader.ptr(), reader.remaining(), result);
}

bool ParseJpegStream(const uint8_t* buffer,
    size_t length,
    JpegParseResult* result)
{
    DCHECK(buffer);
    DCHECK(result);
    if (!ParseJpegPicture(buffer, length, result))
        return false;

    BigEndianReader reader(
        reinterpret_cast<const char*>(result->data), result->data_size);
    const char* eoi_ptr = nullptr;
    if (!SearchEOI(reader.ptr(), reader.remaining(), &eoi_ptr)) {
        DLOG(ERROR) << "SearchEOI failed";
        return false;
    }
    DCHECK(eoi_ptr);
    result->data_size = eoi_ptr - result->data;
    result->image_size = eoi_ptr - reinterpret_cast<const char*>(buffer);
    return true;
}

} // namespace media
