// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebBatteryStatus_h
#define WebBatteryStatus_h

#include <limits>

namespace blink {

class WebBatteryStatus {
public:
    WebBatteryStatus()
        : charging(true)
        , chargingTime(0.0)
        , dischargingTime(std::numeric_limits<double>::infinity())
        , level(1.0)
    {
    }

    bool charging;
    double chargingTime;
    double dischargingTime;
    double level;
};

} // namespace blink

#endif // WebBatteryStatus_h
