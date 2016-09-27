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

#ifndef SVGPathByteStream_h
#define SVGPathByteStream_h

#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"

namespace blink {

template<typename DataType>
union ByteType {
    DataType value;
    unsigned char bytes[sizeof(DataType)];
};

class SVGPathByteStream {
    WTF_MAKE_FAST_ALLOCATED(SVGPathByteStream);
public:
    static PassOwnPtr<SVGPathByteStream> create()
    {
        return adoptPtr(new SVGPathByteStream);
    }

    PassOwnPtr<SVGPathByteStream> copy() const
    {
        return adoptPtr(new SVGPathByteStream(m_data));
    }

    typedef Vector<unsigned char> Data;
    typedef Data::const_iterator DataIterator;

    DataIterator begin() const { return m_data.begin(); }
    DataIterator end() const { return m_data.end(); }
    void append(unsigned char byte) { m_data.append(byte); }
    void append(SVGPathByteStream* other) { m_data.appendVector(other->m_data); }
    void clear() { m_data.clear(); }
    void reserveInitialCapacity(size_t size) { m_data.reserveInitialCapacity(size); }
    void shrinkToFit() { m_data.shrinkToFit(); }
    bool isEmpty() const { return m_data.isEmpty(); }
    unsigned size() const { return m_data.size(); }

private:
    SVGPathByteStream() { }
    SVGPathByteStream(const Data& data)
        : m_data(data)
    {
    }

    Data m_data;
};

} // namespace blink

#endif // SVGPathByteStream_h
