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

#include "config.h"
#include "core/svg/SVGPathByteStreamBuilder.h"

#include "core/svg/SVGPathByteStream.h"
#include "core/svg/SVGPathSeg.h"
#include "platform/geometry/FloatPoint.h"

namespace blink {

// Helper class that coalesces writes to a SVGPathByteStream to a local buffer.
class CoalescingBuffer {
public:
    CoalescingBuffer(SVGPathByteStream& byteStream)
        : m_currentOffset(0)
        , m_byteStream(byteStream)
    {
    }
    ~CoalescingBuffer()
    {
        for (size_t i = 0; i < m_currentOffset; ++i)
            m_byteStream.append(m_bytes[i]);
    }

    template<typename DataType>
    void writeType(DataType value)
    {
        ByteType<DataType> data;
        data.value = value;
        size_t typeSize = sizeof(ByteType<DataType>);
        ASSERT(m_currentOffset + typeSize <= sizeof(m_bytes));
        memcpy(m_bytes + m_currentOffset, data.bytes, typeSize);
        m_currentOffset += typeSize;
    }

    void writeFlag(bool value) { writeType<bool>(value); }
    void writeFloat(float value) { writeType<float>(value); }
    void writeFloatPoint(const FloatPoint& point)
    {
        writeType<float>(point.x());
        writeType<float>(point.y());
    }
    void writeSegmentType(unsigned short value) { writeType<unsigned short>(value); }

private:
    // Adjust size to fit the largest command (in serialized/byte-stream format).
    // Currently a cubic segment.
    size_t m_currentOffset;
    unsigned char m_bytes[sizeof(unsigned short) + sizeof(FloatPoint) * 3];
    SVGPathByteStream& m_byteStream;
};

SVGPathByteStreamBuilder::SVGPathByteStreamBuilder(SVGPathByteStream& byteStream)
    : m_byteStream(byteStream)
{
}

void SVGPathByteStreamBuilder::emitSegment(const PathSegmentData& segment)
{
    CoalescingBuffer buffer(m_byteStream);
    buffer.writeSegmentType(segment.command);

    switch (segment.command) {
    case PathSegMoveToRel:
    case PathSegMoveToAbs:
    case PathSegLineToRel:
    case PathSegLineToAbs:
    case PathSegCurveToQuadraticSmoothRel:
    case PathSegCurveToQuadraticSmoothAbs:
        buffer.writeFloatPoint(segment.targetPoint);
        break;
    case PathSegLineToHorizontalRel:
    case PathSegLineToHorizontalAbs:
        buffer.writeFloat(segment.targetPoint.x());
        break;
    case PathSegLineToVerticalRel:
    case PathSegLineToVerticalAbs:
        buffer.writeFloat(segment.targetPoint.y());
        break;
    case PathSegClosePath:
        break;
    case PathSegCurveToCubicRel:
    case PathSegCurveToCubicAbs:
        buffer.writeFloatPoint(segment.point1);
        buffer.writeFloatPoint(segment.point2);
        buffer.writeFloatPoint(segment.targetPoint);
        break;
    case PathSegCurveToCubicSmoothRel:
    case PathSegCurveToCubicSmoothAbs:
        buffer.writeFloatPoint(segment.point2);
        buffer.writeFloatPoint(segment.targetPoint);
        break;
    case PathSegCurveToQuadraticRel:
    case PathSegCurveToQuadraticAbs:
        buffer.writeFloatPoint(segment.point1);
        buffer.writeFloatPoint(segment.targetPoint);
        break;
    case PathSegArcRel:
    case PathSegArcAbs:
        buffer.writeFloatPoint(segment.point1);
        buffer.writeFloat(segment.point2.x());
        buffer.writeFlag(segment.arcLarge);
        buffer.writeFlag(segment.arcSweep);
        buffer.writeFloatPoint(segment.targetPoint);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

} // namespace blink
