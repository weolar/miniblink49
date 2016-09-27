/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AudioUtilities_h
#define AudioUtilities_h

#include "platform/PlatformExport.h"

namespace blink {
namespace AudioUtilities {

// Standard functions for converting to and from decibel values from linear.
PLATFORM_EXPORT float linearToDecibels(float);
PLATFORM_EXPORT float decibelsToLinear(float);

// timeConstant is the time it takes a first-order linear time-invariant system
// to reach the value 1 - 1/e (around 63.2%) given a step input response.
// discreteTimeConstantForSampleRate() will return the discrete time-constant for the specific sampleRate.
PLATFORM_EXPORT double discreteTimeConstantForSampleRate(double timeConstant, double sampleRate);

// Convert the time to a sample frame at the given sample rate.
PLATFORM_EXPORT size_t timeToSampleFrame(double time, double sampleRate);

// Check that |sampleRate| is a valid rate for AudioBuffers.
PLATFORM_EXPORT bool isValidAudioBufferSampleRate(float sampleRate);

// Return max/min sample rate supported by AudioBuffers.
PLATFORM_EXPORT float minAudioBufferSampleRate();
PLATFORM_EXPORT float maxAudioBufferSampleRate();
} // AudioUtilites
} // namespace blink

#endif // AudioUtilities_h
