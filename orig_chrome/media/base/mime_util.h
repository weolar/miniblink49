// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_MIME_UTIL_H__
#define MEDIA_BASE_MIME_UTIL_H__

#include <string>
#include <vector>

#include "media/base/media_export.h"

namespace media {

// Check to see if a particular MIME type is in the list of
// supported/recognized MIME types.
MEDIA_EXPORT bool IsSupportedMediaMimeType(const std::string& mime_type);

// Returns true if and only if all codecs are supported, false otherwise.
MEDIA_EXPORT bool AreSupportedMediaCodecs(
    const std::vector<std::string>& codecs);

// Parses a codec string, populating |codecs_out| with the prefix of each codec
// in the string |codecs_in|. For example, passed "aaa.b.c,dd.eee", if
// |strip| == true |codecs_out| will contain {"aaa", "dd"}, if |strip| == false
// |codecs_out| will contain {"aaa.b.c", "dd.eee"}.
// See http://www.ietf.org/rfc/rfc4281.txt.
MEDIA_EXPORT void ParseCodecString(const std::string& codecs,
    std::vector<std::string>* codecs_out,
    bool strip);

// Check to see if a particular MIME type is in our list which only supports a
// certain subset of codecs.
MEDIA_EXPORT bool IsStrictMediaMimeType(const std::string& mime_type);

// Indicates that the MIME type and (possible codec string) are supported by the
// underlying platform.
enum SupportsType {
    // The underlying platform is known not to support the given MIME type and
    // codec combination.
    IsNotSupported,

    // The underlying platform is known to support the given MIME type and codec
    // combination.
    IsSupported,

    // The underlying platform is unsure whether the given MIME type and codec
    // combination can be rendered or not before actually trying to play it.
    MayBeSupported
};

// Checks the |mime_type| and |codecs| against the MIME types known to support
// only a particular subset of codecs.
// * Returns IsSupported if the |mime_type| is supported and all the codecs
//   within the |codecs| are supported for the |mime_type|.
// * Returns MayBeSupported if the |mime_type| is supported and is known to
//   support only a subset of codecs, but |codecs| was empty. Also returned if
//   all the codecs in |codecs| are supported, but additional codec parameters
//   were supplied (such as profile) for which the support cannot be decided.
// * Returns IsNotSupported if either the |mime_type| is not supported or the
//   |mime_type| is supported but at least one of the codecs within |codecs| is
//   not supported for the |mime_type|.
MEDIA_EXPORT SupportsType IsSupportedStrictMediaMimeType(
    const std::string& mime_type,
    const std::vector<std::string>& codecs);

// Test only method that removes proprietary media types and codecs from the
// list of supported MIME types and codecs. These types and codecs must be
// removed to ensure consistent layout test results across all Chromium
// variations.
MEDIA_EXPORT void RemoveProprietaryMediaTypesAndCodecsForTests();

} // namespace media

#endif // MEDIA_BASE_MIME_UTIL_H__
