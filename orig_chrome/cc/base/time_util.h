// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BASE_TIME_UTIL_H_
#define CC_BASE_TIME_UTIL_H_

namespace base {
class TimeDelta;
}

namespace cc {

class CC_EXPORT TimeUtil {
public:
    static base::TimeDelta Scale(base::TimeDelta time_delta, double value)
    {
        return base::TimeDelta::FromInternalValue(static_cast<int64>(
            static_cast<double>(time_delta.ToInternalValue()) * value));
    }

    static double Divide(base::TimeDelta dividend, base::TimeDelta divisor)
    {
        return static_cast<double>(dividend.ToInternalValue()) / static_cast<double>(divisor.ToInternalValue());
    }

    static base::TimeDelta Mod(base::TimeDelta dividend,
        base::TimeDelta divisor)
    {
        return base::TimeDelta::FromInternalValue(dividend.ToInternalValue() % divisor.ToInternalValue());
    }
};

} // namespace cc

#endif // CC_BASE_TIME_UTIL_H_
