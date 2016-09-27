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

#ifndef DeviceOrientationData_h
#define DeviceOrientationData_h

#include "bindings/core/v8/Nullable.h"
#include "platform/heap/Handle.h"

namespace blink {

class WebDeviceOrientationData;

class DeviceOrientationData final : public GarbageCollected<DeviceOrientationData> {
public:
    static DeviceOrientationData* create();
    static DeviceOrientationData* create(const Nullable<double>& alpha, const Nullable<double>& beta, const Nullable<double>& gamma, const Nullable<bool>& absolute = false);
    static DeviceOrientationData* create(const WebDeviceOrientationData&);
    DEFINE_INLINE_TRACE() { }

    double alpha() const;
    double beta() const;
    double gamma() const;
    bool absolute() const;
    bool canProvideAlpha() const;
    bool canProvideBeta() const;
    bool canProvideGamma() const;
    bool canProvideAbsolute() const;

    bool canProvideEventData() const;

private:
    DeviceOrientationData();
    DeviceOrientationData(const Nullable<double>& alpha, const Nullable<double>& beta, const Nullable<double>& gamma, const Nullable<bool>& absolute);

    Nullable<double> m_alpha;
    Nullable<double> m_beta;
    Nullable<double> m_gamma;
    Nullable<bool> m_absolute;
};

} // namespace blink

#endif // DeviceOrientationData_h
