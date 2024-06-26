// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/formats/mp4/cenc.h"

#include <cstring>

#include "media/formats/mp4/box_reader.h"
#include "media/formats/mp4/rcheck.h"

namespace media {
namespace mp4 {

    FrameCENCInfo::FrameCENCInfo() { }
    FrameCENCInfo::~FrameCENCInfo() { }

    bool FrameCENCInfo::Parse(int iv_size, BufferReader* reader)
    {
        const int kEntrySize = 6;
        // Mandated by CENC spec
        RCHECK(iv_size == 8 || iv_size == 16);

        memset(iv, 0, sizeof(iv));
        for (int i = 0; i < iv_size; i++)
            RCHECK(reader->Read1(&iv[i]));

        if (!reader->HasBytes(1))
            return true;

        uint16 subsample_count;
        RCHECK(reader->Read2(&subsample_count) && reader->HasBytes(subsample_count * kEntrySize));

        subsamples.resize(subsample_count);
        for (int i = 0; i < subsample_count; i++) {
            uint16 clear_bytes;
            uint32 cypher_bytes;
            RCHECK(reader->Read2(&clear_bytes) && reader->Read4(&cypher_bytes));
            subsamples[i].clear_bytes = clear_bytes;
            subsamples[i].cypher_bytes = cypher_bytes;
        }
        return true;
    }

    bool FrameCENCInfo::GetTotalSizeOfSubsamples(size_t* total_size) const
    {
        size_t size = 0;
        for (size_t i = 0; i < subsamples.size(); i++) {
            size += subsamples[i].clear_bytes;
            RCHECK(size >= subsamples[i].clear_bytes); // overflow
            size += subsamples[i].cypher_bytes;
            RCHECK(size >= subsamples[i].cypher_bytes); // overflow
        }
        *total_size = size;
        return true;
    }

} // namespace mp4
} // namespace media
