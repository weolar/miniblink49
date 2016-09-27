/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/device_orientation/DeviceOrientationData.h"
#include "public/platform/modules/device_orientation/WebDeviceOrientationData.h"

namespace blink {

DeviceOrientationData* DeviceOrientationData::create()
{
    return new DeviceOrientationData;
}

DeviceOrientationData* DeviceOrientationData::create(const Nullable<double>& alpha, const Nullable<double>& beta, const Nullable<double>& gamma, const Nullable<bool>& absolute)
{
    return new DeviceOrientationData(alpha, beta, gamma, absolute);
}

DeviceOrientationData* DeviceOrientationData::create(const WebDeviceOrientationData& data)
{
    Nullable<double> alpha;
    Nullable<double> beta;
    Nullable<double> gamma;
    Nullable<bool> absolute;
    if (data.hasAlpha)
        alpha = data.alpha;
    if (data.hasBeta)
        beta = data.beta;
    if (data.hasGamma)
        gamma = data.gamma;
    if (data.hasAbsolute)
        absolute = data.absolute;
    return DeviceOrientationData::create(alpha, beta, gamma, absolute);
}

DeviceOrientationData::DeviceOrientationData()
{
}

DeviceOrientationData::DeviceOrientationData(const Nullable<double>& alpha, const Nullable<double>& beta, const Nullable<double>& gamma, const Nullable<bool>& absolute)
    : m_alpha(alpha)
    , m_beta(beta)
    , m_gamma(gamma)
    , m_absolute(absolute)
{
}

double DeviceOrientationData::alpha() const
{
    return m_alpha.get();
}

double DeviceOrientationData::beta() const
{
    return m_beta.get();
}

double DeviceOrientationData::gamma() const
{
    return m_gamma.get();
}

bool DeviceOrientationData::absolute() const
{
    return m_absolute.get();
}

bool DeviceOrientationData::canProvideAlpha() const
{
    return !m_alpha.isNull();
}

bool DeviceOrientationData::canProvideBeta() const
{
    return !m_beta.isNull();
}

bool DeviceOrientationData::canProvideGamma() const
{
    return !m_gamma.isNull();
}

bool DeviceOrientationData::canProvideAbsolute() const
{
    return !m_absolute.isNull();
}

bool DeviceOrientationData::canProvideEventData() const
{
    return canProvideAlpha() || canProvideBeta() || canProvideGamma();
}

} // namespace blink
