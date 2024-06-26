// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/point.h"

#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"

namespace media {

std::string PointsToString(const std::vector<Point>& points)
{
    std::string points_string;
    if (!points.empty()) {
        for (size_t i = 0; i < points.size() - 1; ++i) {
            points_string.append(points[i].ToString());
            points_string.append(", ");
        }
        points_string.append(points.back().ToString());
    }
    return points_string;
}

std::vector<Point> ParsePointsFromString(const std::string& points_string)
{
    std::vector<Point> points;
    if (points_string.empty())
        return points;

    const auto& tokens = base::SplitString(points_string, base::kWhitespaceASCII,
        base::KEEP_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
    if (tokens.size() < 3 || tokens.size() % 3 != 0) {
        LOG(ERROR) << "Malformed points string: " << points_string;
        return points;
    }

    std::vector<float> float_tokens;
    float_tokens.reserve(tokens.size());
    for (const auto& token : tokens) {
        double float_token;
        if (!base::StringToDouble(token, &float_token)) {
            LOG(ERROR) << "Unable to convert token=" << token
                       << " to double from points string: " << points_string;
            return points;
        }
        float_tokens.push_back(float_token);
    }

    points.reserve(float_tokens.size() / 3);
    for (size_t i = 0; i < float_tokens.size(); i += 3) {
        points.push_back(
            Point(float_tokens[i + 0], float_tokens[i + 1], float_tokens[i + 2]));
    }

    return points;
}

} // namespace media
