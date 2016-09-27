/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGPathByteStreamSource_h
#define SVGPathByteStreamSource_h

#include "core/svg/SVGPathByteStream.h"
#include "core/svg/SVGPathSource.h"
#include "platform/geometry/FloatPoint.h"

namespace blink {

class SVGPathByteStreamSource final : public SVGPathSource {
public:
    explicit SVGPathByteStreamSource(const SVGPathByteStream& stream)
        : m_streamCurrent(stream.begin())
        , m_streamEnd(stream.end())
    {
    }

private:
    bool hasMoreData() const override;
    SVGPathSegType peekSegmentType() override;
    PathSegmentData parseSegment() override;

#if COMPILER(MSVC)
#pragma warning(disable: 4701)
#endif
    template<typename DataType>
    DataType readType()
    {
        ByteType<DataType> data;
        size_t typeSize = sizeof(ByteType<DataType>);
        ASSERT(m_streamCurrent + typeSize <= m_streamEnd);
        memcpy(data.bytes, m_streamCurrent, typeSize);
        m_streamCurrent += typeSize;
        return data.value;
    }

    bool readFlag() { return readType<bool>(); }
    float readFloat() { return readType<float>(); }
    unsigned short readSVGSegmentType() { return readType<unsigned short>(); }
    FloatPoint readFloatPoint()
    {
        float x = readType<float>();
        float y = readType<float>();
        return FloatPoint(x, y);
    }

    SVGPathByteStream::DataIterator m_streamCurrent;
    SVGPathByteStream::DataIterator m_streamEnd;
};

} // namespace blink

#endif // SVGPathByteStreamSource_h
