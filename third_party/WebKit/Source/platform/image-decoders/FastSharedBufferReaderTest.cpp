/*
 * Copyright (C) 2015 Google Inc. All rights reserved.
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

#include "config.h"

#include "platform/image-decoders/FastSharedBufferReader.h"

#include <gtest/gtest.h>

namespace blink {

namespace {

const unsigned kDefaultTestSize = 4 * SharedBuffer::kSegmentSize;

void prepareReferenceData(char* buffer, size_t size)
{
    for (size_t i = 0; i < size; ++i)
        buffer[i] = i;
}

} // namespace

TEST(FastSharedBufferReaderTest, nonSequentialReads)
{
    char referenceData[kDefaultTestSize];
    prepareReferenceData(referenceData, sizeof(referenceData));
    RefPtr<SharedBuffer> data = SharedBuffer::create();
    data->append(referenceData, sizeof(referenceData));

    FastSharedBufferReader reader(data);

    // Read size is prime such there will be a segment-spanning
    // read eventually.
    char tempBuffer[17];
    for (size_t dataPosition = 0; dataPosition + sizeof(tempBuffer) < sizeof(referenceData); dataPosition += sizeof(tempBuffer)) {
        const char* block = reader.getConsecutiveData(
            dataPosition, sizeof(tempBuffer), tempBuffer);
        ASSERT_FALSE(memcmp(block, referenceData + dataPosition, sizeof(tempBuffer)));
    }
}

TEST(FastSharedBufferReaderTest, readBackwards)
{
    char referenceData[kDefaultTestSize];
    prepareReferenceData(referenceData, sizeof(referenceData));
    RefPtr<SharedBuffer> data = SharedBuffer::create();
    data->append(referenceData, sizeof(referenceData));

    FastSharedBufferReader reader(data);

    // Read size is prime such there will be a segment-spanning
    // read eventually.
    char tempBuffer[17];
    for (size_t dataOffset = sizeof(tempBuffer); dataOffset < sizeof(referenceData); dataOffset += sizeof(tempBuffer)) {
        const char* block = reader.getConsecutiveData(
            sizeof(referenceData) - dataOffset, sizeof(tempBuffer), tempBuffer);
        ASSERT_FALSE(memcmp(block, referenceData + sizeof(referenceData) - dataOffset, sizeof(tempBuffer)));
    }
}

TEST(FastSharedBufferReaderTest, byteByByte)
{
    char referenceData[kDefaultTestSize];
    prepareReferenceData(referenceData, sizeof(referenceData));
    RefPtr<SharedBuffer> data = SharedBuffer::create();
    data->append(referenceData, sizeof(referenceData));

    FastSharedBufferReader reader(data);
    for (size_t i = 0; i < sizeof(referenceData); ++i) {
        ASSERT_EQ(referenceData[i], reader.getOneByte(i));
    }
}

// Tests that a read from inside the penultimate segment to the very end of the
// buffer doesn't try to read off the end of the buffer.
TEST(FastSharedBufferReaderTest, readAllOverlappingLastSegmentBoundary)
{
    const unsigned dataSize = 2 * SharedBuffer::kSegmentSize;
    char referenceData[dataSize];
    prepareReferenceData(referenceData, dataSize);
    RefPtr<SharedBuffer> data = SharedBuffer::create();
    data->append(referenceData, dataSize);

    char buffer[dataSize];
    FastSharedBufferReader reader(data);
    reader.getConsecutiveData(0, dataSize, buffer);

    ASSERT_FALSE(memcmp(buffer, referenceData, dataSize));
}

} // namespace blink
