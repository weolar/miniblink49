/*
 * Copyright (c) 2013, Opera Software ASA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Opera Software ASA nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/vtt/VTTScanner.h"

#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

namespace blink {

TEST(VTTScannerTest, Constructor)
{
    String data8("foo");
    EXPECT_TRUE(data8.is8Bit());
    VTTScanner scanner8(data8);
    EXPECT_FALSE(scanner8.isAtEnd());

    String data16(data8);
    data16.ensure16Bit();
    EXPECT_FALSE(data16.is8Bit());
    VTTScanner scanner16(data16);
    EXPECT_FALSE(scanner16.isAtEnd());

    VTTScanner scannerEmpty(emptyString());
    EXPECT_TRUE(scannerEmpty.isAtEnd());
}

void scanSequenceHelper1(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_FALSE(scanner.isAtEnd());
    EXPECT_TRUE(scanner.match('f'));
    EXPECT_FALSE(scanner.match('o'));

    EXPECT_TRUE(scanner.scan('f'));
    EXPECT_FALSE(scanner.match('f'));
    EXPECT_TRUE(scanner.match('o'));

    EXPECT_FALSE(scanner.scan('e'));
    EXPECT_TRUE(scanner.scan('o'));

    EXPECT_TRUE(scanner.scan('e'));
    EXPECT_FALSE(scanner.match('e'));

    EXPECT_TRUE(scanner.isAtEnd());
}

// Run TESTFUNC with DATA in Latin and then UTF-16. (Requires DATA being Latin.)
#define TEST_WITH(TESTFUNC, DATA) do {      \
    String data8(DATA);                     \
    EXPECT_TRUE(data8.is8Bit());            \
    TESTFUNC(data8);                        \
    \
    String data16(data8);                   \
    data16.ensure16Bit();                   \
    EXPECT_FALSE(data16.is8Bit());          \
    TESTFUNC(data16);                       \
} while (false)


// Exercises match(c) and scan(c).
TEST(VTTScannerTest, BasicOperations1)
{
    TEST_WITH(scanSequenceHelper1, "foe");
}

void scanSequenceHelper2(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_FALSE(scanner.isAtEnd());
    EXPECT_FALSE(scanner.scan("fe"));

    EXPECT_TRUE(scanner.scan("fo"));
    EXPECT_FALSE(scanner.isAtEnd());

    EXPECT_FALSE(scanner.scan("ee"));

    EXPECT_TRUE(scanner.scan('e'));
    EXPECT_TRUE(scanner.isAtEnd());
}

// Exercises scan(<literal>[, length]).
TEST(VTTScannerTest, BasicOperations2)
{
    TEST_WITH(scanSequenceHelper2, "foe");
}

bool lowerCaseAlpha(UChar c)
{
    return c >= 'a' && c <= 'z';
}

void scanWithPredicate(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_FALSE(scanner.isAtEnd());
    // Collect "bad".
    VTTScanner::Run lcRun = scanner.collectWhile<lowerCaseAlpha>();
    // collectWhile doesn't move the scan position.
    EXPECT_TRUE(scanner.match('b'));
    // Consume "bad".
    scanner.skipWhile<lowerCaseAlpha>();
    EXPECT_TRUE(scanner.match('A'));
    EXPECT_TRUE(scanner.isAt(lcRun.end()));

    // Consume "A".
    EXPECT_TRUE(scanner.scan('A'));

    // Collect "bing".
    lcRun = scanner.collectWhile<lowerCaseAlpha>();
    // collectWhile doesn't move the scan position.
    EXPECT_FALSE(scanner.isAtEnd());
    // Consume "bing".
    scanner.skipWhile<lowerCaseAlpha>();
    EXPECT_TRUE(scanner.isAt(lcRun.end()));
    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests skipWhile() and collectWhile().
TEST(VTTScannerTest, PredicateScanning)
{
    TEST_WITH(scanWithPredicate, "badAbing");
}

void scanWithInvPredicate(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_FALSE(scanner.isAtEnd());
    // Collect "BAD".
    VTTScanner::Run ucRun = scanner.collectUntil<lowerCaseAlpha>();
    // collectUntil doesn't move the scan position.
    EXPECT_TRUE(scanner.match('B'));
    // Consume "BAD".
    scanner.skipUntil<lowerCaseAlpha>();
    EXPECT_TRUE(scanner.match('a'));
    EXPECT_TRUE(scanner.isAt(ucRun.end()));

    // Consume "a".
    EXPECT_TRUE(scanner.scan('a'));

    // Collect "BING".
    ucRun = scanner.collectUntil<lowerCaseAlpha>();
    // collectUntil doesn't move the scan position.
    EXPECT_FALSE(scanner.isAtEnd());
    // Consume "BING".
    scanner.skipUntil<lowerCaseAlpha>();
    EXPECT_TRUE(scanner.isAt(ucRun.end()));
    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests skipUntil() and collectUntil().
TEST(VTTScannerTest, InversePredicateScanning)
{
    TEST_WITH(scanWithInvPredicate, "BADaBING");
}

void scanRuns(const String& input)
{
    String fooString("foo");
    String barString("bar");
    VTTScanner scanner(input);
    EXPECT_FALSE(scanner.isAtEnd());
    VTTScanner::Run word = scanner.collectWhile<lowerCaseAlpha>();
    EXPECT_FALSE(scanner.scanRun(word, barString));
    EXPECT_TRUE(scanner.scanRun(word, fooString));

    EXPECT_TRUE(scanner.match(':'));
    EXPECT_TRUE(scanner.scan(':'));

    // Skip 'baz'.
    scanner.skipRun(scanner.collectWhile<lowerCaseAlpha>());

    EXPECT_TRUE(scanner.match(':'));
    EXPECT_TRUE(scanner.scan(':'));

    word = scanner.collectWhile<lowerCaseAlpha>();
    EXPECT_FALSE(scanner.scanRun(word, fooString));
    EXPECT_TRUE(scanner.scanRun(word, barString));
    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests scanRun/skipRun.
TEST(VTTScannerTest, RunScanning)
{
    TEST_WITH(scanRuns, "foo:baz:bar");
}

void scanRunsToStrings(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_FALSE(scanner.isAtEnd());
    VTTScanner::Run word = scanner.collectWhile<lowerCaseAlpha>();
    String fooString = scanner.extractString(word);
    EXPECT_EQ(fooString, "foo");
    EXPECT_TRUE(scanner.isAt(word.end()));

    EXPECT_TRUE(scanner.match(':'));
    EXPECT_TRUE(scanner.scan(':'));

    word = scanner.collectWhile<lowerCaseAlpha>();
    String barString = scanner.extractString(word);
    EXPECT_EQ(barString, "bar");
    EXPECT_TRUE(scanner.isAt(word.end()));
    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests extractString.
TEST(VTTScannerTest, ExtractString)
{
    TEST_WITH(scanRunsToStrings, "foo:bar");
}

void tailStringExtract(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_TRUE(scanner.scan("foo"));
    EXPECT_TRUE(scanner.scan(':'));
    String barSuffix = scanner.restOfInputAsString();
    EXPECT_EQ(barSuffix, "bar");

    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests restOfInputAsString().
TEST(VTTScannerTest, ExtractRestAsString)
{
    TEST_WITH(tailStringExtract, "foo:bar");
}

void scanDigits1(const String& input)
{
    VTTScanner scanner(input);
    EXPECT_TRUE(scanner.scan("foo"));
    int number;
    EXPECT_EQ(scanner.scanDigits(number), 0u);
    EXPECT_EQ(number, 0);
    EXPECT_TRUE(scanner.scan(' '));
    EXPECT_EQ(scanner.scanDigits(number), 3u);
    EXPECT_TRUE(scanner.match(' '));
    EXPECT_EQ(number, 123);

    EXPECT_TRUE(scanner.scan(' '));
    EXPECT_TRUE(scanner.scan("bar"));
    EXPECT_TRUE(scanner.scan(' '));

    EXPECT_EQ(scanner.scanDigits(number), 5u);
    EXPECT_EQ(number, 45678);

    EXPECT_TRUE(scanner.isAtEnd());
}

void scanDigits2(const String& input)
{
    VTTScanner scanner(input);
    int number;
    EXPECT_EQ(scanner.scanDigits(number), 0u);
    EXPECT_EQ(number, 0);
    EXPECT_TRUE(scanner.scan('-'));
    EXPECT_EQ(scanner.scanDigits(number), 3u);
    EXPECT_EQ(number, 654);

    EXPECT_TRUE(scanner.scan(' '));

    EXPECT_EQ(scanner.scanDigits(number), 19u);
    EXPECT_EQ(number, std::numeric_limits<int>::max());

    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests scanDigits().
TEST(VTTScannerTest, ScanDigits)
{
    TEST_WITH(scanDigits1, "foo 123 bar 45678");
    TEST_WITH(scanDigits2, "-654 1000000000000000000");
}

void scanFloatValue(const String& input)
{
    VTTScanner scanner(input);
    float value;
    // "1."
    EXPECT_TRUE(scanner.scanFloat(value));
    EXPECT_EQ(value, 1.0f);
    EXPECT_TRUE(scanner.scan(' '));

    // "1.0"
    EXPECT_TRUE(scanner.scanFloat(value));
    EXPECT_EQ(value, 1.0f);
    EXPECT_TRUE(scanner.scan(' '));

    // ".0"
    EXPECT_TRUE(scanner.scanFloat(value));
    EXPECT_EQ(value, 0.0f);
    EXPECT_TRUE(scanner.scan(' '));

    // "." (invalid)
    EXPECT_FALSE(scanner.scanFloat(value));
    EXPECT_TRUE(scanner.match('.'));
    EXPECT_TRUE(scanner.scan('.'));
    EXPECT_TRUE(scanner.scan(' '));

    // "1.0000"
    EXPECT_TRUE(scanner.scanFloat(value));
    EXPECT_EQ(value, 1.0f);
    EXPECT_TRUE(scanner.scan(' '));

    // "01.000"
    EXPECT_TRUE(scanner.scanFloat(value));
    EXPECT_EQ(value, 1.0f);

    EXPECT_TRUE(scanner.isAtEnd());
}

// Tests scanFloat().
TEST(VTTScannerTest, ScanFloat)
{
    TEST_WITH(scanFloatValue, "1. 1.0 .0 . 1.0000 01.000");
}

#undef TEST_WITH

} // namespace blink
