/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "wtf/text/CString.h"

#include <gtest/gtest.h>

namespace WTF {

TEST(CStringTest, NullStringConstructor)
{
    CString string;
    EXPECT_TRUE(string.isNull());
    EXPECT_EQ(static_cast<const char*>(0), string.data());
    EXPECT_EQ(static_cast<size_t>(0), string.length());

    CString stringFromCharPointer(static_cast<const char*>(0));
    EXPECT_TRUE(stringFromCharPointer.isNull());
    EXPECT_EQ(static_cast<const char*>(0), stringFromCharPointer.data());
    EXPECT_EQ(static_cast<size_t>(0), stringFromCharPointer.length());

    CString stringFromCharAndLength(static_cast<const char*>(0), 0);
    EXPECT_TRUE(stringFromCharAndLength.isNull());
    EXPECT_EQ(static_cast<const char*>(0), stringFromCharAndLength.data());
    EXPECT_EQ(static_cast<size_t>(0), stringFromCharAndLength.length());
}

TEST(CStringTest, EmptyEmptyConstructor)
{
    const char* emptyString = "";
    CString string(emptyString);
    EXPECT_FALSE(string.isNull());
    EXPECT_EQ(static_cast<size_t>(0), string.length());
    EXPECT_EQ(0, string.data()[0]);

    CString stringWithLength(emptyString, 0);
    EXPECT_FALSE(stringWithLength.isNull());
    EXPECT_EQ(static_cast<size_t>(0), stringWithLength.length());
    EXPECT_EQ(0, stringWithLength.data()[0]);
}

TEST(CStringTest, EmptyRegularConstructor)
{
    const char* referenceString = "WebKit";

    CString string(referenceString);
    EXPECT_FALSE(string.isNull());
    EXPECT_EQ(strlen(referenceString), string.length());
    EXPECT_STREQ(referenceString, string.data());

    CString stringWithLength(referenceString, 6);
    EXPECT_FALSE(stringWithLength.isNull());
    EXPECT_EQ(strlen(referenceString), stringWithLength.length());
    EXPECT_STREQ(referenceString, stringWithLength.data());
}

TEST(CStringTest, UninitializedConstructor)
{
    char* buffer;
    CString emptyString = CString::newUninitialized(0, buffer);
    EXPECT_FALSE(emptyString.isNull());
    EXPECT_EQ(buffer, emptyString.data());
    EXPECT_EQ(0, buffer[0]);

    const size_t length = 25;
    CString uninitializedString = CString::newUninitialized(length, buffer);
    EXPECT_FALSE(uninitializedString.isNull());
    EXPECT_EQ(buffer, uninitializedString.data());
    EXPECT_EQ(0, uninitializedString.data()[length]);
}

TEST(CStringTest, ZeroTerminated)
{
    const char* referenceString = "WebKit";
    CString stringWithLength(referenceString, 3);
    EXPECT_EQ(0, stringWithLength.data()[3]);
}

TEST(CStringTest, CopyOnWrite)
{
    const char* initialString = "Webkit";
    CString string(initialString);
    CString copy = string;

    string.mutableData()[3] = 'K';
    EXPECT_TRUE(string != copy);
    EXPECT_STREQ("WebKit", string.data());
    EXPECT_STREQ(initialString, copy.data());
}

TEST(CStringTest, Comparison)
{
    // Comparison with another CString.
    CString a;
    CString b;
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    a = "a";
    b = CString();
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    a = "a";
    b = "b";
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    a = "a";
    b = "a";
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    a = "a";
    b = "aa";
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    a = "";
    b = "";
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a != b);
    a = "";
    b = CString();
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);
    a = "a";
    b = "";
    EXPECT_FALSE(a == b);
    EXPECT_TRUE(a != b);

    // Comparison with a const char*.
    CString c;
    const char* d = 0;
    EXPECT_TRUE(c == d);
    EXPECT_FALSE(c != d);
    c = "c";
    d = 0;
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = CString();
    d = "d";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = "c";
    d = "d";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = "c";
    d = "c";
    EXPECT_TRUE(c == d);
    EXPECT_FALSE(c != d);
    c = "c";
    d = "cc";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = "cc";
    d = "c";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = "";
    d = "";
    EXPECT_TRUE(c == d);
    EXPECT_FALSE(c != d);
    c = "";
    d = 0;
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = CString();
    d = "";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = "a";
    d = "";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
    c = "";
    d = "b";
    EXPECT_FALSE(c == d);
    EXPECT_TRUE(c != d);
}

} // namespace WTF
