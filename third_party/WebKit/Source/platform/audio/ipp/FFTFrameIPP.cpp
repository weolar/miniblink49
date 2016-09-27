/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 * Copyright (C) 2012 Intel Inc. All rights reserved.
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

// FFTFrame implementation using Intel IPP's DFT algorithm,
// suitable for use on Linux.

#include "config.h"

#if ENABLE(WEB_AUDIO)

#if USE(WEBAUDIO_IPP)

#include "platform/audio/FFTFrame.h"

#include "wtf/MathExtras.h"

namespace blink {

const unsigned maximumFFTPower2Size = 24;

// Normal constructor: allocates for a given fftSize.
FFTFrame::FFTFrame(unsigned fftSize)
    : m_FFTSize(fftSize)
    , m_log2FFTSize(static_cast<unsigned>(log2(fftSize)))
    , m_realData(fftSize / 2)
    , m_imagData(fftSize / 2)
    , m_complexData(fftSize)
{
    // We only allow power of two.
    ASSERT(1UL << m_log2FFTSize == m_FFTSize);
    ASSERT(m_log2FFTSize <= maximumFFTPower2Size);

    ippsDFTInitAlloc_R_32f(&m_DFTSpec, m_FFTSize, IPP_FFT_NODIV_BY_ANY, ippAlgHintFast);
    int bufferSize = 0;
    ippsDFTGetBufSize_R_32f(m_DFTSpec, &bufferSize);
    m_buffer = ippsMalloc_8u(bufferSize);
}

// Creates a blank/empty frame (interpolate() must later be called).
FFTFrame::FFTFrame()
    : m_FFTSize(0)
    , m_log2FFTSize(0)
{
}

// Copy constructor.
FFTFrame::FFTFrame(const FFTFrame& frame)
    : m_FFTSize(frame.m_FFTSize)
    , m_log2FFTSize(frame.m_log2FFTSize)
    , m_realData(frame.m_FFTSize / 2)
    , m_imagData(frame.m_FFTSize / 2)
    , m_complexData(frame.m_FFTSize)
{
    ippsDFTInitAlloc_R_32f(&m_DFTSpec, m_FFTSize, IPP_FFT_NODIV_BY_ANY, ippAlgHintFast);
    int bufferSize = 0;
    ippsDFTGetBufSize_R_32f(m_DFTSpec, &bufferSize);
    m_buffer = ippsMalloc_8u(bufferSize);

    // Copy/setup frame data.
    unsigned numberOfBytes = sizeof(float) * m_FFTSize;
    memcpy(realData(), frame.realData(), numberOfBytes);
    memcpy(imagData(), frame.imagData(), numberOfBytes);
}

void FFTFrame::initialize()
{
}

void FFTFrame::cleanup()
{
}

FFTFrame::~FFTFrame()
{
    ippsFree(m_buffer);
    ippsDFTFree_R_32f(m_DFTSpec);
}

void FFTFrame::doFFT(const float* data)
{
    Ipp32f* complexP = m_complexData.data();

    // Compute Forward transform to perm format.
    ippsDFTFwd_RToPerm_32f(reinterpret_cast<Ipp32f*>(const_cast<float*>(data)), complexP, m_DFTSpec, m_buffer);

    Ipp32f* realP = m_realData.data();
    Ipp32f* imagP = m_imagData.data();
    ippsCplxToReal_32fc(reinterpret_cast<Ipp32fc*>(complexP), realP, imagP, m_FFTSize >> 1);
}

void FFTFrame::doInverseFFT(float* data)
{
    Ipp32f* complexP = getUpToDateComplexData();

    // Compute inverse transform.
    ippsDFTInv_PermToR_32f(complexP, reinterpret_cast<Ipp32f*>(data), m_DFTSpec, m_buffer);

    // Scale so that a forward then inverse FFT yields exactly the original data.
    const float scale = 1.0 / m_FFTSize;

    ippsMulC_32f_I(scale, reinterpret_cast<Ipp32f*>(data), m_FFTSize);
}

float* FFTFrame::getUpToDateComplexData()
{
    int len = m_FFTSize >> 1;
    // Merge the real and imagimary vectors to complex vector.
    Ipp32f* realP = m_realData.data();
    Ipp32f* imagP = m_imagData.data();
    Ipp32fc* complexP = reinterpret_cast<Ipp32fc*>(m_complexData.data());
    ippsRealToCplx_32f(realP, imagP, complexP, len);

    return const_cast<float*>(m_complexData.data());
}

} // namespace blink

#endif // USE(WEBAUDIO_IPP)

#endif // ENABLE(WEB_AUDIO)
