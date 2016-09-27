// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TabSize_h
#define TabSize_h

namespace blink {

struct TabSize {
    TabSize(float pixels)
        : m_floatValue(pixels)
        , m_isSpaces(0)
    {
    }

    TabSize(int spaces)
        : m_floatValue(spaces)
        , m_isSpaces(1)
    {
    }

    bool isSpaces() const
    {
        return m_isSpaces;
    }

    float getPixelSize(float spaceWidth) const
    {
        return m_isSpaces ? m_floatValue * spaceWidth : m_floatValue;
    }

    float m_floatValue;
    unsigned m_isSpaces : 1;
};

inline bool operator==(const TabSize& a, const TabSize& b)
{
    return (a.m_floatValue == b.m_floatValue) && (a.m_isSpaces == b.m_isSpaces);
}

inline bool operator!=(const TabSize& a, const TabSize& b)
{
    return !(a == b);
}

} // namespace blink

#endif // TabSize_h
