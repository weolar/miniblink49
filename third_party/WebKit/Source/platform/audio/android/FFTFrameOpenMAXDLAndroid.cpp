/* Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "config.h"

#if ENABLE(WEB_AUDIO)

#if OS(ANDROID) && USE(WEBAUDIO_OPENMAX_DL_FFT)

#include "platform/audio/FFTFrame.h"

#include "platform/audio/AudioArray.h"
#include "wtf/MathExtras.h"
#include <dl/sp/api/armSP.h>
#include <dl/sp/api/omxSP.h>

namespace blink {

#if ENABLE(ASSERT)
const unsigned kMaxFFTPow2Size = 15;
#endif

// Normal constructor: allocates for a given fftSize.
FFTFrame::FFTFrame(unsigned fftSize)
    : m_FFTSize(fftSize)
    , m_log2FFTSize(static_cast<unsigned>(log2(fftSize)))
    , m_realData(fftSize / 2)
    , m_imagData(fftSize / 2)
    , m_forwardContext(nullptr)
    , m_inverseContext(nullptr)
    , m_complexData(fftSize)
{
    // We only allow power of two.
    ASSERT(1UL << m_log2FFTSize == m_FFTSize);

    m_forwardContext = contextForSize(m_log2FFTSize);
    m_inverseContext = contextForSize(m_log2FFTSize);
}

// Creates a blank/empty frame (interpolate() must later be called).
FFTFrame::FFTFrame()
    : m_FFTSize(0)
    , m_log2FFTSize(0)
    , m_forwardContext(nullptr)
    , m_inverseContext(nullptr)
{
}

// Copy constructor.
FFTFrame::FFTFrame(const FFTFrame& frame)
    : m_FFTSize(frame.m_FFTSize)
    , m_log2FFTSize(frame.m_log2FFTSize)
    , m_realData(frame.m_FFTSize / 2)
    , m_imagData(frame.m_FFTSize / 2)
    , m_forwardContext(nullptr)
    , m_inverseContext(nullptr)
    , m_complexData(frame.m_FFTSize)
{
    m_forwardContext = contextForSize(m_log2FFTSize);
    m_inverseContext = contextForSize(m_log2FFTSize);

    // Copy/setup frame data.
    unsigned nbytes = sizeof(float) * (m_FFTSize / 2);
    memcpy(realData(), frame.realData(), nbytes);
    memcpy(imagData(), frame.imagData(), nbytes);
}

void FFTFrame::initialize()
{
}

void FFTFrame::cleanup()
{
}

FFTFrame::~FFTFrame()
{
    if (m_forwardContext)
        free(m_forwardContext);
    if (m_inverseContext)
        free(m_inverseContext);
}

void FFTFrame::doFFT(const float* data)
{
    ASSERT(m_forwardContext);

    if (m_forwardContext) {
        AudioFloatArray complexFFT(m_FFTSize + 2);

        omxSP_FFTFwd_RToCCS_F32(data, complexFFT.data(), m_forwardContext);

        unsigned len = m_FFTSize / 2;

        // Split FFT data into real and imaginary arrays.
        const float* c = complexFFT.data();
        float* real = m_realData.data();
        float* imag = m_imagData.data();
        for (unsigned k = 1; k < len; ++k) {
            int index = 2 * k;
            real[k] = c[index];
            imag[k] = c[index + 1];
        }
        real[0] = c[0];
        imag[0] = c[m_FFTSize];
    }
}

void FFTFrame::doInverseFFT(float* data)
{
    ASSERT(m_inverseContext);

    if (m_inverseContext) {
        AudioFloatArray fftDataArray(m_FFTSize + 2);

        unsigned len = m_FFTSize / 2;

        // Pack the real and imaginary data into the complex array format
        float* fftData = fftDataArray.data();
        const float* real = m_realData.data();
        const float* imag = m_imagData.data();
        for (unsigned k = 1; k < len; ++k) {
            int index = 2 * k;
            fftData[index] = real[k];
            fftData[index + 1] = imag[k];
        }
        fftData[0] = real[0];
        fftData[1] = 0;
        fftData[m_FFTSize] = imag[0];
        fftData[m_FFTSize + 1] = 0;

        omxSP_FFTInv_CCSToR_F32(fftData, data, m_inverseContext);
    }
}

OMXFFTSpec_R_F32* FFTFrame::contextForSize(unsigned log2FFTSize)
{
    ASSERT(log2FFTSize);
    ASSERT(log2FFTSize <= kMaxFFTPow2Size);
    int bufSize;
    OMXResult status = omxSP_FFTGetBufSize_R_F32(log2FFTSize, &bufSize);

    if (status == OMX_Sts_NoErr) {
        OMXFFTSpec_R_F32* context = static_cast<OMXFFTSpec_R_F32*>(malloc(bufSize));
        omxSP_FFTInit_R_F32(context, log2FFTSize);
        return context;
    }

    return nullptr;
}

} // namespace blink

#endif // #if OS(ANDROID) && !USE(WEBAUDIO_OPENMAX_DL_FFT)

#endif // ENABLE(WEB_AUDIO)
