/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
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
#include "core/xml/parser/SharedBufferReader.h"

#include "platform/SharedBuffer.h"

#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>
#include <vector>

namespace blink {

TEST(SharedBufferReaderTest, readDataWithNullSharedBuffer)
{
    SharedBufferReader reader(nullptr);
    char buffer[32];

    EXPECT_EQ(0, reader.readData(buffer, sizeof(buffer)));
}

TEST(SharedBufferReaderTest, readDataWith0BytesRequest)
{
    RefPtr<SharedBuffer> sharedBuffer = SharedBuffer::create();
    SharedBufferReader reader(sharedBuffer);

    EXPECT_EQ(0, reader.readData(0, 0));
}

TEST(SharedBufferReaderTest, readDataWithSizeBiggerThanSharedBufferSize)
{
    static const char testData[] = "hello";
    RefPtr<SharedBuffer> sharedBuffer = SharedBuffer::create(testData, sizeof(testData));

    SharedBufferReader reader(sharedBuffer);

    const int extraBytes = 3;
    char outputBuffer[sizeof(testData) + extraBytes];

    const char initializationByte = 'a';
    memset(outputBuffer, initializationByte, sizeof(outputBuffer));
    EXPECT_EQ(sizeof(testData),
        static_cast<size_t>(reader.readData(outputBuffer, sizeof(outputBuffer))));

    EXPECT_TRUE(std::equal(testData, testData + sizeof(testData), outputBuffer));
    // Check that the bytes past index sizeof(testData) were not touched.
    EXPECT_EQ(extraBytes,
        std::count(outputBuffer, outputBuffer + sizeof(outputBuffer), initializationByte));
}

TEST(SharedBufferReaderTest, readDataInMultiples)
{
    const int iterationsCount = 8;
    const int bytesPerIteration = 64;

    std::vector<char> testData(iterationsCount * bytesPerIteration);
    std::generate(testData.begin(), testData.end(), &std::rand);

    RefPtr<SharedBuffer> sharedBuffer = SharedBuffer::create(&testData[0], testData.size());
    SharedBufferReader reader(sharedBuffer);

    std::vector<char> destinationVector(testData.size());

    for (int i = 0; i < iterationsCount; ++i) {
        const int offset = i * bytesPerIteration;
        const int bytesRead = reader.readData(&destinationVector[0] + offset, bytesPerIteration);
        EXPECT_EQ(bytesPerIteration, bytesRead);
    }

    EXPECT_TRUE(std::equal(testData.begin(), testData.end(), destinationVector.begin()));
}

TEST(SharedBufferReaderTest, clearSharedBufferBetweenCallsToReadData)
{
    std::vector<char> testData(128);
    std::generate(testData.begin(), testData.end(), &std::rand);

    RefPtr<SharedBuffer> sharedBuffer = SharedBuffer::create(&testData[0], testData.size());
    SharedBufferReader reader(sharedBuffer);

    std::vector<char> destinationVector(testData.size());
    const int bytesToRead = testData.size() / 2;
    EXPECT_EQ(bytesToRead, reader.readData(&destinationVector[0], bytesToRead));

    sharedBuffer->clear();

    EXPECT_EQ(0, reader.readData(&destinationVector[0], bytesToRead));
}

} // namespace blink
