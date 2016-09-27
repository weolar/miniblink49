/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WebDeviceOrientationData_h
#define WebDeviceOrientationData_h

#include "public/platform/WebCommon.h"

#if INSIDE_BLINK
#include "wtf/Assertions.h"
#endif

namespace blink {

#pragma pack(push, 1)

class WebDeviceOrientationData {
public:
    BLINK_PLATFORM_EXPORT WebDeviceOrientationData();
    ~WebDeviceOrientationData() { }

    double alpha;
    double beta;
    double gamma;

    bool hasAlpha : 1;
    bool hasBeta : 1;
    bool hasGamma : 1;

    bool absolute : 1;
    bool hasAbsolute : 1;

    bool allAvailableSensorsAreActive : 1;
};

#if INSIDE_BLINK
static_assert(sizeof(WebDeviceOrientationData) == (3 * sizeof(double) + 1 * sizeof(char)), "WebDeviceOrientationData has wrong size");
#endif

#pragma pack(pop)

} // namespace blink

#endif // WebDeviceOrientationData_h
