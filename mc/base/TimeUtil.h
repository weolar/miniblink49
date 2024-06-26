// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_base_TimeUtil_h
#define mc_base_TimeUtil_h

namespace base {
class TimeDelta;
}

namespace mc {

class TimeUtil {
public:
    static base::TimeDelta scale(base::TimeDelta time_delta, double value)
    {
        return base::TimeDelta::FromInternalValue(static_cast<int64>(static_cast<double>(time_delta.ToInternalValue()) * value));
    }

    static double divide(base::TimeDelta dividend, base::TimeDelta divisor)
    {
        return static_cast<double>(dividend.ToInternalValue()) / static_cast<double>(divisor.ToInternalValue());
    }

    static base::TimeDelta mod(base::TimeDelta dividend, base::TimeDelta divisor)
    {
        return base::TimeDelta::FromInternalValue(dividend.ToInternalValue() % divisor.ToInternalValue());
    }
};

}  // namespace mc

#endif  // mc_base_TimeUtil_h
