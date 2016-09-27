// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Spatializer_h
#define Spatializer_h

#include "platform/PlatformExport.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class AudioBus;

// Abstract base class for spatializing a mono or stereo source.
class PLATFORM_EXPORT Spatializer {
public:
    enum {
        PanningModelEqualPower = 0

        // We have a plan to add more panning models other than equal-power.
        // (such as binaural-HRTF or N-channel surround panning)
    };

    typedef unsigned PanningModel;

    static PassOwnPtr<Spatializer> create(PanningModel, float sampleRate);
    virtual ~Spatializer();

    // Handle sample-accurate panning by AudioParam automation.
    virtual void panWithSampleAccurateValues(const AudioBus* inputBus, AudioBus* outputBuf, const float* panValues, size_t framesToProcess) = 0;

    // Handle de-zippered panning to a target value.
    virtual void panToTargetValue(const AudioBus* inputBus, AudioBus* outputBuf, float panValue, size_t framesToProcess) = 0;

    virtual void reset() = 0;
    virtual double tailTime() const = 0;
    virtual double latencyTime() const = 0;

protected:
    explicit Spatializer(PanningModel model) { }
};

} // namespace blink

#endif // Spatializer_h
