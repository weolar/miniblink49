/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "platform/weborigin/SecurityOrigin.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

namespace blink {

const int MaxAllowedPort = 65535;

class SecurityOriginTest : public ::testing::Test { };

TEST_F(SecurityOriginTest, InvalidPortsCreateUniqueOrigins)
{
    int ports[] = { -100, -1, MaxAllowedPort + 1, 1000000 };

    for (size_t i = 0; i < arraysize(ports); ++i) {
        RefPtr<SecurityOrigin> origin = SecurityOrigin::create("http", "example.com", ports[i]);
        EXPECT_TRUE(origin->isUnique()) << "Port " << ports[i] << " should have generated a unique origin.";
    }
}

TEST_F(SecurityOriginTest, ValidPortsCreateNonUniqueOrigins)
{
    int ports[] = { 0, 80, 443, 5000, MaxAllowedPort };

    for (size_t i = 0; i < arraysize(ports); ++i) {
        RefPtr<SecurityOrigin> origin = SecurityOrigin::create("http", "example.com", ports[i]);
        EXPECT_FALSE(origin->isUnique()) << "Port " << ports[i] << " should not have generated a unique origin.";
    }
}

TEST_F(SecurityOriginTest, IsPotentiallyTrustworthy)
{
    struct TestCase {
        bool accessGranted;
        const char* url;
    };

    TestCase inputs[] = {
        // Access is granted to webservers running on localhost.
        { true, "http://localhost" },
        { true, "http://LOCALHOST" },
        { true, "http://localhost:100" },
        { true, "http://127.0.0.1" },
        { true, "http://127.0.0.2" },
        { true, "http://127.1.0.2" },
        { true, "http://0177.00.00.01" },
        { true, "http://[::1]" },
        { true, "http://[0:0::1]" },
        { true, "http://[0:0:0:0:0:0:0:1]" },
        { true, "http://[::1]:21" },
        { true, "http://127.0.0.1:8080" },
        { true, "ftp://127.0.0.1" },
        { true, "ftp://127.0.0.1:443" },
        { true, "ws://127.0.0.1" },

        // Access is denied to non-localhost over HTTP
        { false, "http://[1::]" },
        { false, "http://[::2]" },
        { false, "http://[1::1]" },
        { false, "http://[1:2::3]" },
        { false, "http://[::127.0.0.1]" },
        { false, "http://a.127.0.0.1" },
        { false, "http://127.0.0.1.b" },
        { false, "http://localhost.a" },
        { false, "http://a.localhost" },

        // Access is granted to all secure transports.
        { true, "https://foobar.com" },
        { true, "wss://foobar.com" },

        // Access is denied to insecure transports.
        { false, "ftp://foobar.com" },
        { false, "http://foobar.com" },
        { false, "http://foobar.com:443" },
        { false, "ws://foobar.com" },

        // Access is granted to local files
        { true, "file:///home/foobar/index.html" },

        // blob: URLs must look to the inner URL's origin, and apply the same
        // rules as above. Spot check some of them
        { true, "blob:http://localhost:1000/578223a1-8c13-17b3-84d5-eca045ae384a" },
        { true, "blob:https://foopy:99/578223a1-8c13-17b3-84d5-eca045ae384a" },
        { false, "blob:http://baz:99/578223a1-8c13-17b3-84d5-eca045ae384a" },
        { false, "blob:ftp://evil:99/578223a1-8c13-17b3-84d5-eca045ae384a" },

        // filesystem: URLs work the same as blob: URLs, and look to the inner
        // URL for security origin.
        { true, "filesystem:http://localhost:1000/foo" },
        { true, "filesystem:https://foopy:99/foo" },
        { false, "filesystem:http://baz:99/foo" },
        { false, "filesystem:ftp://evil:99/foo" },
    };

    for (size_t i = 0; i < arraysize(inputs); ++i) {
        SCOPED_TRACE(i);
        RefPtr<SecurityOrigin> origin = SecurityOrigin::createFromString(inputs[i].url);
        String errorMessage;
        EXPECT_EQ(inputs[i].accessGranted, origin->isPotentiallyTrustworthy(errorMessage));
        EXPECT_EQ(inputs[i].accessGranted, errorMessage.isEmpty());
    }

    // Unique origins are not considered secure.
    RefPtr<SecurityOrigin> uniqueOrigin = SecurityOrigin::createUnique();
    String errorMessage;
    EXPECT_FALSE(uniqueOrigin->isPotentiallyTrustworthy(errorMessage));
    EXPECT_EQ("Only secure origins are allowed (see: https://goo.gl/Y0ZkNV).", errorMessage);
}

TEST_F(SecurityOriginTest, IsSecure)
{
    struct TestCase {
        bool isSecure;
        const char* url;
    } inputs[] = {
        { false, "blob:ftp://evil:99/578223a1-8c13-17b3-84d5-eca045ae384a" },
        { false, "blob:http://example.com/578223a1-8c13-17b3-84d5-eca045ae384a" },
        { false, "file:///etc/passwd" },
        { false, "ftp://example.com/" },
        { false, "http://example.com/" },
        { false, "ws://example.com/" },
        { true, "blob:https://example.com/578223a1-8c13-17b3-84d5-eca045ae384a" },
        { true, "https://example.com/" },
        { true, "wss://example.com/" },

        { true, "about:blank" },
        { false, "" },
        { false, "\0" },
    };

    for (auto test : inputs)
        EXPECT_EQ(test.isSecure, SecurityOrigin::isSecure(KURL(ParsedURLString, test.url))) << "URL: '" << test.url << "'";

    EXPECT_FALSE(SecurityOrigin::isSecure(KURL()));
}

TEST_F(SecurityOriginTest, Suborigins)
{
    RuntimeEnabledFeatures::setSuboriginsEnabled(true);

    RefPtr<SecurityOrigin> origin = SecurityOrigin::createFromString("https://test.com");
    EXPECT_FALSE(origin->hasSuborigin());
    origin->addSuborigin("foobar");
    EXPECT_TRUE(origin->hasSuborigin());
    EXPECT_EQ("foobar", origin->suboriginName());

    origin = SecurityOrigin::createFromString("https://foobar_test.com");
    EXPECT_EQ("https", origin->protocol());
    EXPECT_EQ("test.com", origin->host());
    EXPECT_EQ("foobar", origin->suboriginName());

    origin = SecurityOrigin::createFromString("https://foobar_test.com");
    EXPECT_TRUE(origin->hasSuborigin());
    EXPECT_EQ("foobar", origin->suboriginName());

    origin = SecurityOrigin::createFromString("https://foobar+test.com");
    EXPECT_FALSE(origin->hasSuborigin());

    origin = SecurityOrigin::createFromString("https://_test.com");
    EXPECT_FALSE(origin->hasSuborigin());

    origin = adoptRef<SecurityOrigin>(new SecurityOrigin);
    EXPECT_FALSE(origin->hasSuborigin());

    origin = SecurityOrigin::createFromString("https://foobar_test.com");
    EXPECT_DEATH(origin->addSuborigin("shouldhitassert"), "");
}

TEST_F(SecurityOriginTest, SuboriginsParsing)
{
    RuntimeEnabledFeatures::setSuboriginsEnabled(true);
    String host, realHost, suborigin;
    host = "test.com";
    EXPECT_FALSE(SecurityOrigin::deserializeSuboriginAndHost(host, suborigin, realHost));

    host = "foobar_test.com";
    EXPECT_TRUE(SecurityOrigin::deserializeSuboriginAndHost(host, suborigin, realHost));
    EXPECT_EQ("test.com", realHost);
    EXPECT_EQ("foobar", suborigin);

    RefPtr<SecurityOrigin> origin;
    StringBuilder builder;

    origin = SecurityOrigin::createFromString("https://foobar_test.com");
    origin->buildRawString(builder);
    EXPECT_EQ("https://foobar_test.com", builder.toString());

    builder.clear();
    origin = SecurityOrigin::createFromString("https://test.com");
    origin->addSuborigin("foobar");
    origin->buildRawString(builder);
    EXPECT_EQ("https://foobar_test.com", builder.toString());
}

TEST_F(SecurityOriginTest, SuboriginsIsSameSchemeHostPortAndSuborigin)
{
    blink::RuntimeEnabledFeatures::setSuboriginsEnabled(true);
    RefPtr<SecurityOrigin> origin = SecurityOrigin::createFromString("https://foobar_test.com");
    RefPtr<SecurityOrigin> other1 = SecurityOrigin::createFromString("https://bazbar_test.com");
    RefPtr<SecurityOrigin> other2 = SecurityOrigin::createFromString("http://foobar_test.com");
    RefPtr<SecurityOrigin> other3 = SecurityOrigin::createFromString("https://foobar_test.com:1234");
    RefPtr<SecurityOrigin> other4 = SecurityOrigin::createFromString("https://test.com");

    EXPECT_TRUE(origin->isSameSchemeHostPortAndSuborigin(origin.get()));
    EXPECT_FALSE(origin->isSameSchemeHostPortAndSuborigin(other1.get()));
    EXPECT_FALSE(origin->isSameSchemeHostPortAndSuborigin(other2.get()));
    EXPECT_FALSE(origin->isSameSchemeHostPortAndSuborigin(other3.get()));
    EXPECT_FALSE(origin->isSameSchemeHostPortAndSuborigin(other4.get()));
}

TEST_F(SecurityOriginTest, CanAccess)
{
    RuntimeEnabledFeatures::setSuboriginsEnabled(true);

    struct TestCase {
        bool canAccess;
        bool canAccessCheckSuborigins;
        const char* origin1;
        const char* origin2;
    };

    TestCase tests[] = {
        { true, true, "https://foobar.com", "https://foobar.com" },
        { false, false, "https://foobar.com", "https://bazbar.com" },
        { true, false, "https://foobar.com", "https://name_foobar.com" },
        { true, false, "https://name_foobar.com", "https://foobar.com" },
        { true, true, "https://name_foobar.com", "https://name_foobar.com" },
    };

    for (size_t i = 0; i < arraysize(tests); ++i) {
        RefPtr<SecurityOrigin> origin1 = SecurityOrigin::createFromString(tests[i].origin1);
        RefPtr<SecurityOrigin> origin2 = SecurityOrigin::createFromString(tests[i].origin2);
        EXPECT_EQ(tests[i].canAccess, origin1->canAccess(origin2.get()));
        EXPECT_EQ(tests[i].canAccessCheckSuborigins, origin1->canAccessCheckSuborigins(origin2.get()));
    }
}

TEST_F(SecurityOriginTest, CanRequest)
{
    RuntimeEnabledFeatures::setSuboriginsEnabled(true);

    struct TestCase {
        bool canRequest;
        bool canRequestNoSuborigin;
        const char* origin;
        const char* url;
    };

    TestCase tests[] = {
        { true, true, "https://foobar.com", "https://foobar.com" },
        { false, false, "https://foobar.com", "https://bazbar.com" },
        { true, false, "https://name_foobar.com", "https://foobar.com" },
        { false, false, "https://name_foobar.com", "https://bazbar.com" },
    };

    for (size_t i = 0; i < arraysize(tests); ++i) {
        RefPtr<SecurityOrigin> origin = SecurityOrigin::createFromString(tests[i].origin);
        blink::KURL url(blink::ParsedURLString, tests[i].url);
        EXPECT_EQ(tests[i].canRequest, origin->canRequest(url));
        EXPECT_EQ(tests[i].canRequestNoSuborigin, origin->canRequestNoSuborigin(url));
    }
}

} // namespace blink
