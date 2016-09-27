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

#ifndef HTMLDimension_h
#define HTMLDimension_h

#include "core/CoreExport.h"
#include "wtf/Forward.h"
#include "wtf/Vector.h"

namespace blink {

// This class corresponds to a dimension as described in HTML5 by the
// "rules for parsing a list of dimensions" (section 2.4.4.6).
class HTMLDimension {
public:
    enum HTMLDimensionType {
        Relative, Percentage, Absolute
    };

    HTMLDimension()
        : m_type(Absolute)
        , m_value(0)
    {
    }

    HTMLDimension(double value, HTMLDimensionType type)
        : m_type(type)
        , m_value(value)
    {
    }

    HTMLDimensionType type() const { return m_type; }

    bool isRelative() const { return m_type == Relative; }
    bool isPercentage() const { return m_type == Percentage; }
    bool isAbsolute() const { return m_type == Absolute; }

    double value() const { return m_value; }

    bool operator==(const HTMLDimension& other) const
    {
        return m_type == other.m_type && m_value == other.m_value;
    }
    bool operator!=(const HTMLDimension& other) const { return !(*this == other); }

private:
    HTMLDimensionType m_type;
    double m_value;
};

CORE_EXPORT Vector<HTMLDimension> parseListOfDimensions(const String&);

} // namespace blink

#endif // HTMLDimension_h
