// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/weborigin/KnownPorts.h"

#include <gtest/gtest.h>

namespace blink {

TEST(KnownPortsTest, IsDefaultPortForProtocol)
{
    struct TestCase {
        const unsigned short port;
        const char* protocol;
        const bool isKnown;
    };

    TestCase inputs[] = {
        // Known ones.
        { 80, "http", true },
        { 443, "https", true },
        { 21, "ftp", true },
        { 990, "ftps", true },

        // Unknown ones.
        { 5, "foo", false },
        { 80, "http:", false },
        { 443, "http", false },
        { 21, "ftps", false },
        { 990, "ftp", false },

        // With upper cases.
        { 80, "HTTP", false },
        { 443, "Https", false },
    };

    for (TestCase test : inputs) {
        bool result = isDefaultPortForProtocol(test.port, test.protocol);
        EXPECT_EQ(test.isKnown, result);
    }
}

} // namespace blink
