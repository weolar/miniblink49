// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_CHANNEL_MIXER_H_
#define MEDIA_BASE_CHANNEL_MIXER_H_

#include <vector>

#include "base/macros.h"
#include "media/base/channel_layout.h"
#include "media/base/media_export.h"

namespace media {

class AudioBus;
class AudioParameters;

// ChannelMixer is for converting audio between channel layouts.  The conversion
// matrix is built upon construction and used during each Transform() call.  The
// algorithm works by generating a conversion matrix mapping each output channel
// to list of input channels.  The transform renders all of the output channels,
// with each output channel rendered according to a weighted sum of the relevant
// input channels as defined in the matrix.
class MEDIA_EXPORT ChannelMixer {
public:
    ChannelMixer(ChannelLayout input_layout, ChannelLayout output_layout);
    ChannelMixer(const AudioParameters& input, const AudioParameters& output);
    ~ChannelMixer();

    // Transforms all channels from |input| into |output| channels.
    void Transform(const AudioBus* input, AudioBus* output);

private:
    void Initialize(ChannelLayout input_layout, int input_channels,
        ChannelLayout output_layout, int output_channels);

    // 2D matrix of output channels to input channels.
    std::vector<std::vector<float>> matrix_;

    // Optimization case for when we can simply remap the input channels to output
    // channels and don't need to do a multiply-accumulate loop over |matrix_|.
    bool remapping_;

    DISALLOW_COPY_AND_ASSIGN(ChannelMixer);
};

} // namespace media

#endif // MEDIA_BASE_CHANNEL_MIXER_H_
