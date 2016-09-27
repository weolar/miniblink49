/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Mac OS X - specific FFTFrame implementation

#include "config.h"

#if ENABLE(WEB_AUDIO)

#if OS(MACOSX)

#include "platform/audio/FFTFrame.h"

#include "platform/audio/VectorMath.h"

namespace blink {

const int kMaxFFTPow2Size = 24;

FFTSetup* FFTFrame::fftSetups = nullptr;

// Normal constructor: allocates for a given fftSize
FFTFrame::FFTFrame(unsigned fftSize)
    : m_realData(fftSize)
    , m_imagData(fftSize)
{
    m_FFTSize = fftSize;
    m_log2FFTSize = static_cast<unsigned>(log2(fftSize));

    // We only allow power of two
    ASSERT(1UL << m_log2FFTSize == m_FFTSize);

    // Lazily create and share fftSetup with other frames
    m_FFTSetup = fftSetupForSize(fftSize);

    // Setup frame data
    m_frame.realp = m_realData.data();
    m_frame.imagp = m_imagData.data();
}

// Creates a blank/empty frame (interpolate() must later be called)
FFTFrame::FFTFrame()
    : m_realData(0)
    , m_imagData(0)
{
    // Later will be set to correct values when interpolate() is called
    m_frame.realp = 0;
    m_frame.imagp = 0;

    m_FFTSize = 0;
    m_log2FFTSize = 0;
}

// Copy constructor
FFTFrame::FFTFrame(const FFTFrame& frame)
    : m_FFTSize(frame.m_FFTSize)
    , m_log2FFTSize(frame.m_log2FFTSize)
    , m_realData(frame.m_FFTSize)
    , m_imagData(frame.m_FFTSize)
    , m_FFTSetup(frame.m_FFTSetup)
{
    // Setup frame data
    m_frame.realp = m_realData.data();
    m_frame.imagp = m_imagData.data();

    // Copy/setup frame data
    unsigned nbytes = sizeof(float) * m_FFTSize;
    memcpy(realData(), frame.m_frame.realp, nbytes);
    memcpy(imagData(), frame.m_frame.imagp, nbytes);
}

FFTFrame::~FFTFrame()
{
}

void FFTFrame::doFFT(const float* data)
{
    AudioFloatArray scaledData(m_FFTSize);
    // veclib fft returns a result that is twice as large as would be expected. Compensate for that
    // by scaling the input by half so the FFT has the correct scaling.
    float scale = 0.5f;
    VectorMath::vsmul(data, 1, &scale, scaledData.data(), 1, m_FFTSize);

    vDSP_ctoz((DSPComplex*)scaledData.data(), 2, &m_frame, 1, m_FFTSize / 2);
    vDSP_fft_zrip(m_FFTSetup, &m_frame, 1, m_log2FFTSize, FFT_FORWARD);
}

void FFTFrame::doInverseFFT(float* data)
{
    vDSP_fft_zrip(m_FFTSetup, &m_frame, 1, m_log2FFTSize, FFT_INVERSE);
    vDSP_ztoc(&m_frame, 1, (DSPComplex*)data, 2, m_FFTSize / 2);

    // Do final scaling so that x == IFFT(FFT(x))
    float scale = 1.0f / m_FFTSize;
    VectorMath::vsmul(data, 1, &scale, data, 1, m_FFTSize);
}

FFTSetup FFTFrame::fftSetupForSize(unsigned fftSize)
{
    if (!fftSetups) {
        fftSetups = (FFTSetup*)malloc(sizeof(FFTSetup) * kMaxFFTPow2Size);
        memset(fftSetups, 0, sizeof(FFTSetup) * kMaxFFTPow2Size);
    }

    int pow2size = static_cast<int>(log2(fftSize));
    ASSERT(pow2size < kMaxFFTPow2Size);
    if (!fftSetups[pow2size])
        fftSetups[pow2size] = vDSP_create_fftsetup(pow2size, FFT_RADIX2);

    return fftSetups[pow2size];
}

void FFTFrame::initialize()
{
}

void FFTFrame::cleanup()
{
    if (!fftSetups)
        return;

    for (int i = 0; i < kMaxFFTPow2Size; ++i) {
        if (fftSetups[i])
            vDSP_destroy_fftsetup(fftSetups[i]);
    }

    free(fftSetups);
    fftSetups = nullptr;
}

} // namespace blink

#endif // #if OS(MACOSX)

#endif // ENABLE(WEB_AUDIO)
