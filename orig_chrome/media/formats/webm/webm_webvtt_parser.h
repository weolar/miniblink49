// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_FORMATS_WEBM_WEBM_WEBVTT_PARSER_H_
#define MEDIA_FORMATS_WEBM_WEBM_WEBVTT_PARSER_H_

#include <string>

#include "base/basictypes.h"
#include "media/base/media_export.h"

namespace media {

class MEDIA_EXPORT WebMWebVTTParser {
public:
    // Utility function to parse the WebVTT cue from a byte stream.
    static void Parse(const uint8* payload, int payload_size,
        std::string* id,
        std::string* settings,
        std::string* content);

private:
    // The payload is the embedded WebVTT cue, stored in a WebM block.
    // The parser treats this as a UTF-8 byte stream.
    WebMWebVTTParser(const uint8* payload, int payload_size);

    // Parse the cue identifier, settings, and content from the stream.
    void Parse(std::string* id, std::string* settings, std::string* content);
    // Remove a byte from the stream, advancing the stream pointer.
    // Returns true if a character was returned; false means "end of stream".
    bool GetByte(uint8* byte);

    // Backup the stream pointer.
    void UngetByte();

    // Parse a line of text from the stream.
    void ParseLine(std::string* line);

    // Represents the portion of the stream that has not been consumed yet.
    const uint8* ptr_;
    const uint8* const ptr_end_;

    DISALLOW_COPY_AND_ASSIGN(WebMWebVTTParser);
};

} // namespace media

#endif // MEDIA_FORMATS_WEBM_WEBM_WEBVTT_PARSER_H_
