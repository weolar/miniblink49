// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_AUDIO_VIDEO_METADATA_EXTRACTOR_H_
#define MEDIA_BASE_AUDIO_VIDEO_METADATA_EXTRACTOR_H_

#include <map>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "media/base/media_export.h"

struct AVDictionary;

namespace media {

class DataSource;

// This class extracts a string dictionary of metadata tags for audio and video
// files. It also provides the format name.
class MEDIA_EXPORT AudioVideoMetadataExtractor {
public:
    typedef std::map<std::string, std::string> TagDictionary;

    struct StreamInfo {
        StreamInfo();
        ~StreamInfo();
        std::string type;
        TagDictionary tags;
    };

    typedef std::vector<StreamInfo> StreamInfoVector;

    AudioVideoMetadataExtractor();
    ~AudioVideoMetadataExtractor();

    // Returns whether or not the fields were successfully extracted. Should only
    // be called once.
    bool Extract(DataSource* source, bool extract_attached_pics);

    // Returns -1 if we cannot extract the duration. In seconds.
    double duration() const;

    // Returns -1 for containers without video.
    int width() const;
    int height() const;

    // Returns -1 if undefined.
    int rotation() const;

    // Returns -1 or an empty string if the value is undefined.
    const std::string& album() const;
    const std::string& artist() const;
    const std::string& comment() const;
    const std::string& copyright() const;
    const std::string& date() const;
    int disc() const;
    const std::string& encoder() const;
    const std::string& encoded_by() const;
    const std::string& genre() const;
    const std::string& language() const;
    const std::string& title() const;
    int track() const;

    // First element is the container. Subsequent elements are the child streams.
    const StreamInfoVector& stream_infos() const;

    // Empty if Extract call did not request attached images, or if no attached
    // images were found.
    const std::vector<std::string>& attached_images_bytes() const;

private:
    void ExtractDictionary(AVDictionary* metadata, TagDictionary* raw_tags);

    bool extracted_;

    int duration_;
    int width_;
    int height_;

    std::string album_;
    std::string artist_;
    std::string comment_;
    std::string copyright_;
    std::string date_;
    int disc_;
    std::string encoder_;
    std::string encoded_by_;
    std::string genre_;
    std::string language_;
    int rotation_;
    std::string title_;
    int track_;

    StreamInfoVector stream_infos_;

    std::vector<std::string> attached_images_bytes_;

    DISALLOW_COPY_AND_ASSIGN(AudioVideoMetadataExtractor);
};

} // namespace media

#endif // MEDIA_BASE_AUDIO_VIDEO_METADATA_EXTRACTOR_H_
