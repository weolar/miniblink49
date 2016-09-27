/*
 * Copyright (C) 2013, Opera Software ASA. All rights reserved.
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
#include "core/html/track/vtt/BufferedLineReader.h"

#include "wtf/text/CString.h"
#include "wtf/text/CharacterNames.h"
#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

namespace blink {

TEST(BufferedLineReaderTest, Constructor)
{
    BufferedLineReader reader;
    ASSERT_FALSE(reader.isAtEndOfStream());
    String line;
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, EOSNoInput)
{
    BufferedLineReader reader;
    String line;
    ASSERT_FALSE(reader.getLine(line));
    reader.setEndOfStream();
    // No input observed, so still no line.
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, EOSInput)
{
    BufferedLineReader reader;
    reader.append("A");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "A");
}

TEST(BufferedLineReaderTest, EOSMultipleReads_1)
{
    BufferedLineReader reader;
    reader.append("A");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "A");
    // No more lines returned.
    ASSERT_FALSE(reader.getLine(line));
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, EOSMultipleReads_2)
{
    BufferedLineReader reader;
    reader.append("A\n");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "A");
    // No more lines returned.
    ASSERT_FALSE(reader.getLine(line));
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, LineEndingCR)
{
    BufferedLineReader reader;
    reader.append("X\rY");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "Y");
}

TEST(BufferedLineReaderTest, LineEndingCR_EOS)
{
    BufferedLineReader reader;
    reader.append("X\r");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, LineEndingLF)
{
    BufferedLineReader reader;
    reader.append("X\nY");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "Y");
}

TEST(BufferedLineReaderTest, LineEndingLF_EOS)
{
    BufferedLineReader reader;
    reader.append("X\n");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, LineEndingCRLF)
{
    BufferedLineReader reader;
    reader.append("X\r\nY");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "Y");
}

TEST(BufferedLineReaderTest, LineEndingCRLF_EOS)
{
    BufferedLineReader reader;
    reader.append("X\r\n");
    reader.setEndOfStream();
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_FALSE(reader.getLine(line));
}

enum LineBreakType {
    Cr,
    Lf,
    CrLf
};

String LineBreakString(LineBreakType type)
{
    static const char breakStrings[] = "\r\n";
    return String(type == Lf ? breakStrings + 1 : breakStrings, type == CrLf ? 2 : 1);
}

String MakeTestData(const char** lines, const LineBreakType* breaks, int count)
{
    StringBuilder builder;
    for (int i = 0; i < count; ++i) {
        builder.append(lines[i]);
        builder.append(LineBreakString(breaks[i]));
    }
    return builder.toString();
}

const size_t blockSizes[] = { 64, 32, 16, 8, 4, 2, 1, 3, 5, 7, 9, 11, 13, 17, 19, 23 };

TEST(BufferedLineReaderTest, BufferSizes)
{
    const char* lines[] = {
        "aaaaaaaaaaaaaaaa",
        "bbbbbbbbbb",
        "ccccccccccccc",
        "",
        "dddddd",
        "",
        "eeeeeeeeee"
    };
    const LineBreakType breaks[] = { Lf, Lf, Lf, Lf, Lf, Lf, Lf };
    const size_t numTestLines = WTF_ARRAY_LENGTH(lines);
    static_assert(numTestLines == WTF_ARRAY_LENGTH(breaks), "number of test lines and breaks should be the same");
    String data = MakeTestData(lines, breaks, numTestLines);

    for (size_t k = 0; k < WTF_ARRAY_LENGTH(blockSizes); ++k) {
        size_t lineCount = 0;
        BufferedLineReader reader;
        size_t blockSize = blockSizes[k];
        for (size_t i = 0; i < data.length(); i += blockSize) {
            reader.append(data.substring(i, blockSize));

            String line;
            while (reader.getLine(line)) {
                ASSERT_LT(lineCount, numTestLines);
                ASSERT_EQ(line, lines[lineCount++]);
            }
        }
        ASSERT_EQ(lineCount, numTestLines);
    }
}

TEST(BufferedLineReaderTest, BufferSizesMixedEndings)
{
    const char* lines[] = {
        "aaaaaaaaaaaaaaaa",
        "bbbbbbbbbb",
        "ccccccccccccc",
        "",
        "dddddd",
        "eeeeeeeeee",
        "fffffffffffffffffff"
    };
    const LineBreakType breaks[] = { Cr, Lf, CrLf, Cr, Lf, CrLf, Lf };
    const size_t numTestLines = WTF_ARRAY_LENGTH(lines);
    static_assert(numTestLines == WTF_ARRAY_LENGTH(breaks), "number of test lines and breaks should be the same");
    String data = MakeTestData(lines, breaks, numTestLines);

    for (size_t k = 0; k < WTF_ARRAY_LENGTH(blockSizes); ++k) {
        size_t lineCount = 0;
        BufferedLineReader reader;
        size_t blockSize = blockSizes[k];
        for (size_t i = 0; i < data.length(); i += blockSize) {
            reader.append(data.substring(i, blockSize));

            String line;
            while (reader.getLine(line)) {
                ASSERT_LT(lineCount, numTestLines);
                ASSERT_EQ(line, lines[lineCount++]);
            }
        }
        ASSERT_EQ(lineCount, numTestLines);
    }
}

TEST(BufferedLineReaderTest, BufferBoundaryInCRLF_1)
{
    BufferedLineReader reader;
    reader.append("X\r");
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    reader.append("\n");
    ASSERT_FALSE(reader.getLine(line));
}

TEST(BufferedLineReaderTest, BufferBoundaryInCRLF_2)
{
    BufferedLineReader reader;
    reader.append("X\r");
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "X");
    ASSERT_FALSE(reader.getLine(line));
    reader.append("\n");
    ASSERT_FALSE(reader.getLine(line));
    reader.append("Y\n");
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line, "Y");
}

TEST(BufferedLineReaderTest, NormalizedNUL)
{
    BufferedLineReader reader;
    reader.append(String("X\0Y\n", 4));
    String line;
    ASSERT_TRUE(reader.getLine(line));
    ASSERT_EQ(line[1], replacementCharacter);
}

} // namespace blink
