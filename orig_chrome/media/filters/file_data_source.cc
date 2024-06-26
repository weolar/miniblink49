// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/filters/file_data_source.h"

#include <algorithm>

#include "base/logging.h"

namespace media {

FileDataSource::FileDataSource()
    : force_read_errors_(false)
    , force_streaming_(false)
    , bytes_read_(0)
{
}

FileDataSource::FileDataSource(base::File file)
    : force_read_errors_(false)
    , force_streaming_(false)
    , bytes_read_(0)
{
    file_.Initialize(file.Pass());
}

bool FileDataSource::Initialize(const base::FilePath& file_path)
{
    DCHECK(!file_.IsValid());
    return file_.Initialize(file_path);
}

void FileDataSource::Stop()
{
}

void FileDataSource::Read(int64 position, int size, uint8* data,
    const DataSource::ReadCB& read_cb)
{
    if (force_read_errors_ || !file_.IsValid()) {
        read_cb.Run(kReadError);
        return;
    }

    int64 file_size = file_.length();

    CHECK_GE(file_size, 0);
    CHECK_GE(position, 0);
    CHECK_GE(size, 0);

    // Cap position and size within bounds.
    position = std::min(position, file_size);
    int64 clamped_size = std::min(static_cast<int64>(size), file_size - position);

    memcpy(data, file_.data() + position, clamped_size);
    bytes_read_ += clamped_size;
    read_cb.Run(clamped_size);
}

bool FileDataSource::GetSize(int64* size_out)
{
    *size_out = file_.length();
    return true;
}

bool FileDataSource::IsStreaming()
{
    return force_streaming_;
}

void FileDataSource::SetBitrate(int bitrate) { }

FileDataSource::~FileDataSource() { }

} // namespace media
