/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SharedBuffer_h
#define SharedBuffer_h

#include "platform/PlatformExport.h"
#include "platform/PurgeableVector.h"
#include "third_party/skia/include/core/SkData.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class PLATFORM_EXPORT SharedBuffer : public RefCounted<SharedBuffer> {
public:
    static const unsigned kSegmentSize = 0x1000;

    static PassRefPtr<SharedBuffer> create() { return adoptRef(new SharedBuffer); }
    static PassRefPtr<SharedBuffer> create(size_t size) { return adoptRef(new SharedBuffer(size)); }
    static PassRefPtr<SharedBuffer> create(const char* c, int i) { return adoptRef(new SharedBuffer(c, i)); }
    static PassRefPtr<SharedBuffer> create(const unsigned char* c, int i) { return adoptRef(new SharedBuffer(c, i)); }

    static PassRefPtr<SharedBuffer> createPurgeable(const char* c, unsigned size) { return adoptRef(new SharedBuffer(c, size, PurgeableVector::Purgeable)); }

    static PassRefPtr<SharedBuffer> adoptVector(Vector<char>&);

    ~SharedBuffer();

    // Calling this function will force internal segmented buffers to be merged
    // into a flat buffer. Use getSomeData() whenever possible for better
    // performance.
    const char* data() const;

    unsigned size() const;

    bool isEmpty() const { return !size(); }

    void append(PassRefPtr<SharedBuffer>);
    void append(const char*, unsigned);
    void append(const Vector<char>&);

    void clear();

    PassRefPtr<SharedBuffer> copy() const;

    // Return the number of consecutive bytes after "position". "data"
    // points to the first byte.
    // Return 0 when no more data left.
    // When extracting all data with getSomeData(), the caller should
    // repeat calling it until it returns 0.
    // Usage:
    //      const char* segment;
    //      unsigned pos = 0;
    //      while (unsigned length = sharedBuffer->getSomeData(segment, pos)) {
    //          // Use the data. for example: decoder->decode(segment, length);
    //          pos += length;
    //      }
    unsigned getSomeData(const char*& data, unsigned position = 0) const;

    // Returns the content data into "dest" as a flat buffer. "byteLength" must
    // exactly match with size(). Returns true on success, otherwise the content
    // of "dest" is not guaranteed.
    bool getAsBytes(void* dest, unsigned byteLength) const;

    // Creates an SkData and copies this SharedBuffer's contents to that
    // SkData without merging segmented buffers into a flat buffer.
    PassRefPtr<SkData> getAsSkData() const;

    // See PurgeableVector::lock().
    bool lock();

    // WARNING: Calling unlock() on a SharedBuffer that wasn't created with the
    // purgeability option does an extra memcpy(). Please use
    // SharedBuffer::createPurgeable() if you intend to call unlock().
    void unlock();

    bool isLocked() const;

private:
    SharedBuffer();
    explicit SharedBuffer(size_t);
    SharedBuffer(const char*, int);
    SharedBuffer(const unsigned char*, int);
    SharedBuffer(const char*, unsigned, PurgeableVector::PurgeableOption);

    // See SharedBuffer::data().
    void mergeSegmentsIntoBuffer() const;

    unsigned m_size;
    mutable PurgeableVector m_buffer;
    mutable Vector<char*> m_segments;
};

} // namespace blink

#endif // SharedBuffer_h
