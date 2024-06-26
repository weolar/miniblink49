// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_GL_ANGLE_PLATFORM_IMPL_H_
#define UI_GL_ANGLE_PLATFORM_IMPL_H_

// Implements the ANGLE platform interface, for functionality like
// histograms and trace profiling.

#include "base/macros.h"
#include "third_party/angle/include/platform/Platform.h"

namespace gfx {

// Derives the base ANGLE platform and provides implementations
class ANGLEPlatformImpl : public angle::Platform {
public:
    ANGLEPlatformImpl();
    ~ANGLEPlatformImpl() override;

    // angle::Platform:
    double currentTime() override;
    double monotonicallyIncreasingTime() override;
    void logError(const char* errorMessage) override;
    void logWarning(const char* warningMessage) override;
    const unsigned char* getTraceCategoryEnabledFlag(
        const char* category_group) override;
    TraceEventHandle addTraceEvent(char phase,
        const unsigned char* category_group_enabled,
        const char* name,
        unsigned long long id,
        double timestamp,
        int num_args,
        const char** arg_names,
        const unsigned char* arg_types,
        const unsigned long long* arg_values,
        unsigned char flags) override;
    void updateTraceEventDuration(const unsigned char* category_group_enabled,
        const char* name,
        TraceEventHandle handle) override;
    void histogramCustomCounts(const char* name,
        int sample,
        int min,
        int max,
        int bucket_count) override;
    void histogramEnumeration(const char* name,
        int sample,
        int boundary_value) override;
    void histogramSparse(const char* name, int sample) override;
    void histogramBoolean(const char* name, bool sample) override;

private:
    DISALLOW_COPY_AND_ASSIGN(ANGLEPlatformImpl);
};

} // namespace gfx

#endif // UI_GL_ANGLE_PLATFORM_IMPL_H_
