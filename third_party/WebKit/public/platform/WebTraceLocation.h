// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebTraceLocation_h
#define WebTraceLocation_h

#include "WebCommon.h"

namespace blink {

// This class is used to keep track of where posted tasks originate. See base/location.h in Chromium.
// Currently only store the bits used in Blink, base::Location stores more.
class BLINK_PLATFORM_EXPORT WebTraceLocation {
public:
    // These char*s are not copied and must live for the duration of the program.
    WebTraceLocation(const char* function, const char* file);
    WebTraceLocation();

    const char* functionName() const;
    const char* fileName() const;

private:
    const char* m_functionName;
    const char* m_fileName;
};

#if INSIDE_BLINK
#define FROM_HERE ::blink::WebTraceLocation(__FUNCTION__, __FILE__)
#endif // INSIDE_BLINK

}

#endif // WebTraceLocation_h
