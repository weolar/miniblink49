/*
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
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
#include "AtomicString.h"

#include <gtest/gtest.h>

namespace WTF {

TEST(AtomicStringTest, Number)
{
    int intValue = 1234;
    EXPECT_EQ("1234", AtomicString::number(intValue));
    intValue = -1234;
    EXPECT_EQ("-1234", AtomicString::number(intValue));
    unsigned unsignedValue = 1234u;
    EXPECT_EQ("1234", AtomicString::number(unsignedValue));
    long longValue = 6553500;
    EXPECT_EQ("6553500", AtomicString::number(longValue));
    longValue = -6553500;
    EXPECT_EQ("-6553500", AtomicString::number(longValue));
    unsigned long unsignedLongValue = 4294967295u;
    EXPECT_EQ("4294967295", AtomicString::number(unsignedLongValue));
    long long longlongValue = 9223372036854775807;
    EXPECT_EQ("9223372036854775807", AtomicString::number(longlongValue));
    longlongValue = -9223372036854775807;
    EXPECT_EQ("-9223372036854775807", AtomicString::number(longlongValue));
    unsigned long long unsignedLongLongValue = 18446744073709551615u;
    EXPECT_EQ("18446744073709551615", AtomicString::number(unsignedLongLongValue));
    double doubleValue = 1234.56;
    EXPECT_EQ("1234.56", AtomicString::number(doubleValue));
    doubleValue = 1234.56789;
    EXPECT_EQ("1234.56789", AtomicString::number(doubleValue, 9));
}

TEST(AtomicStringTest, ImplEquality)
{
    AtomicString foo("foo");
    AtomicString bar("bar");
    AtomicString baz("baz");
    AtomicString foo2("foo");
    AtomicString baz2("baz");
    AtomicString bar2("bar");
    EXPECT_EQ(foo.impl(), foo2.impl());
    EXPECT_EQ(bar.impl(), bar2.impl());
    EXPECT_EQ(baz.impl(), baz2.impl());
    EXPECT_NE(foo.impl(), bar.impl());
    EXPECT_NE(foo.impl(), baz.impl());
    EXPECT_NE(bar.impl(), baz.impl());
}

} // namespace WTF
