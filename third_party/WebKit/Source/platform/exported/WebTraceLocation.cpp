// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebTraceLocation.h"

namespace blink {

WebTraceLocation::WebTraceLocation()
    : m_functionName("unknown")
    , m_fileName("unknown")
{ }

WebTraceLocation::WebTraceLocation(const char* functionName, const char* fileName)
    : m_functionName(functionName)
    , m_fileName(fileName)
{ }

const char* WebTraceLocation::functionName() const
{
    return m_functionName;
}

const char* WebTraceLocation::fileName() const
{
    return m_fileName;
}

} // namespace blink
