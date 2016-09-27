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
#include "platform/weborigin/OriginAccessEntry.h"

#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebPublicSuffixList.h"
#include <gtest/gtest.h>

namespace blink {

class OriginAccessEntryTestSuffixList : public blink::WebPublicSuffixList {
public:
    size_t getPublicSuffixLength(const blink::WebString&) override
    {
        return m_length;
    }

    void setPublicSuffix(const blink::WebString& suffix)
    {
        m_length = suffix.length();
    }

private:
    size_t m_length;
};

class OriginAccessEntryTestPlatform : public blink::Platform {
public:
    blink::WebPublicSuffixList* publicSuffixList() override
    {
        return &m_suffixList;
    }

    // Stub for pure virtual method.
    void cryptographicallyRandomValues(unsigned char*, size_t) override { ASSERT_NOT_REACHED(); }

    void setPublicSuffix(const blink::WebString& suffix)
    {
        m_suffixList.setPublicSuffix(suffix);
    }

private:
    OriginAccessEntryTestSuffixList m_suffixList;
};

TEST(OriginAccessEntryTest, PublicSuffixListTest)
{
    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("com");
    Platform::initialize(&platform);

    RefPtr<SecurityOrigin> origin = SecurityOrigin::createFromString("http://www.google.com");
    OriginAccessEntry entry1("http", "google.com", OriginAccessEntry::AllowSubdomains);
    OriginAccessEntry entry2("http", "hamster.com", OriginAccessEntry::AllowSubdomains);
    OriginAccessEntry entry3("http", "com", OriginAccessEntry::AllowSubdomains);
    EXPECT_EQ(OriginAccessEntry::MatchesOrigin, entry1.matchesOrigin(*origin));
    EXPECT_EQ(OriginAccessEntry::DoesNotMatchOrigin, entry2.matchesOrigin(*origin));
    EXPECT_EQ(OriginAccessEntry::MatchesOriginButIsPublicSuffix, entry3.matchesOrigin(*origin));

    Platform::shutdown();
}

TEST(OriginAccessEntryTest, AllowSubdomainsTest)
{
    struct TestCase {
        const char* protocol;
        const char* host;
        const char* origin;
        OriginAccessEntry::MatchResult expected;
    } inputs[] = {
        { "http", "example.com", "http://example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.com", "http://www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.com", "http://www.www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.com", "http://example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "www.example.com", "http://www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.com", "http://www.www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "com", "http://example.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "http", "com", "http://www.example.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "http", "com", "http://www.www.example.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "https", "example.com", "http://example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://www.www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "example.com", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "", "http://example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "", "http://beispiel.de/", OriginAccessEntry::MatchesOrigin },
        { "https", "", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
    };

    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("com");
    Platform::initialize(&platform);

    for (const auto& test : inputs) {
        SCOPED_TRACE(testing::Message() << "Host: " << test.host << ", Origin: " << test.origin);
        RefPtr<SecurityOrigin> originToTest = SecurityOrigin::createFromString(test.origin);
        OriginAccessEntry entry1(test.protocol, test.host, OriginAccessEntry::AllowSubdomains);
        EXPECT_EQ(test.expected, entry1.matchesOrigin(*originToTest));
    }

    Platform::shutdown();
}

TEST(OriginAccessEntryTest, AllowRegisterableDomainsTest)
{
    struct TestCase {
        const char* protocol;
        const char* host;
        const char* origin;
        OriginAccessEntry::MatchResult expected;
    } inputs[] = {
        { "http", "example.com", "http://example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.com", "http://www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.com", "http://www.www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.com", "http://example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.com", "http://www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.com", "http://www.www.example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "com", "http://example.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "http", "com", "http://www.example.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "http", "com", "http://www.www.example.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "https", "example.com", "http://example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://www.www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "example.com", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "", "http://example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "", "http://beispiel.de/", OriginAccessEntry::MatchesOrigin },
        { "https", "", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
    };

    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("com");
    Platform::initialize(&platform);

    for (const auto& test : inputs) {
        RefPtr<SecurityOrigin> originToTest = SecurityOrigin::createFromString(test.origin);
        OriginAccessEntry entry1(test.protocol, test.host, OriginAccessEntry::AllowRegisterableDomains);

        SCOPED_TRACE(testing::Message() << "Host: " << test.host << ", Origin: " << test.origin << ", Domain: " << entry1.registerable().utf8().data());
        EXPECT_EQ(test.expected, entry1.matchesOrigin(*originToTest));
    }

    Platform::shutdown();
}

TEST(OriginAccessEntryTest, AllowRegisterableDomainsTestWithDottedSuffix)
{
    struct TestCase {
        const char* protocol;
        const char* host;
        const char* origin;
        OriginAccessEntry::MatchResult expected;
    } inputs[] = {
        { "http", "example.appspot.com", "http://example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.appspot.com", "http://www.example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.appspot.com", "http://www.www.example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.appspot.com", "http://example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.appspot.com", "http://www.example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "www.example.appspot.com", "http://www.www.example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "appspot.com", "http://example.appspot.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "http", "appspot.com", "http://www.example.appspot.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "http", "appspot.com", "http://www.www.example.appspot.com/", OriginAccessEntry::MatchesOriginButIsPublicSuffix },
        { "https", "example.appspot.com", "http://example.appspot.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.appspot.com", "http://www.example.appspot.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.appspot.com", "http://www.www.example.appspot.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "example.appspot.com", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "", "http://example.appspot.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "", "http://beispiel.de/", OriginAccessEntry::MatchesOrigin },
        { "https", "", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
    };

    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("appspot.com");
    Platform::initialize(&platform);

    for (const auto& test : inputs) {
        RefPtr<SecurityOrigin> originToTest = SecurityOrigin::createFromString(test.origin);
        OriginAccessEntry entry1(test.protocol, test.host, OriginAccessEntry::AllowRegisterableDomains);

        SCOPED_TRACE(testing::Message() << "Host: " << test.host << ", Origin: " << test.origin << ", Domain: " << entry1.registerable().utf8().data());
        EXPECT_EQ(test.expected, entry1.matchesOrigin(*originToTest));
    }

    Platform::shutdown();
}

TEST(OriginAccessEntryTest, DisallowSubdomainsTest)
{
    struct TestCase {
        const char* protocol;
        const char* host;
        const char* origin;
        OriginAccessEntry::MatchResult expected;
    } inputs[] = {
        { "http", "example.com", "http://example.com/", OriginAccessEntry::MatchesOrigin },
        { "http", "example.com", "http://www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "example.com", "http://www.www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "com", "http://example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "com", "http://www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "com", "http://www.www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "example.com", "http://www.www.example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "example.com", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "", "http://example.com/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
        { "https", "", "http://beispiel.de/", OriginAccessEntry::DoesNotMatchOrigin },
    };

    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("com");
    Platform::initialize(&platform);

    for (const auto& test : inputs) {
        SCOPED_TRACE(testing::Message() << "Host: " << test.host << ", Origin: " << test.origin);
        RefPtr<SecurityOrigin> originToTest = SecurityOrigin::createFromString(test.origin);
        OriginAccessEntry entry1(test.protocol, test.host, OriginAccessEntry::DisallowSubdomains);
        EXPECT_EQ(test.expected, entry1.matchesOrigin(*originToTest));
    }

    Platform::shutdown();
}

TEST(OriginAccessEntryTest, IPAddressTest)
{
    struct TestCase {
        const char* protocol;
        const char* host;
        bool isIPAddress;
    } inputs[] = {
        { "http", "1.1.1.1", true },
        { "http", "1.1.1.1.1", false },
        { "http", "example.com", false },
        { "http", "hostname.that.ends.with.a.number1", false },
        { "http", "2001:db8::1", false },
        { "http", "[2001:db8::1]", true },
        { "http", "2001:db8::a", false },
        { "http", "[2001:db8::a]", true },
        { "http", "", false },
    };

    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("com");
    Platform::initialize(&platform);

    for (const auto& test : inputs) {
        SCOPED_TRACE(testing::Message() << "Host: " << test.host);
        OriginAccessEntry entry(test.protocol, test.host, OriginAccessEntry::DisallowSubdomains);
        EXPECT_EQ(test.isIPAddress, entry.hostIsIPAddress()) << test.host;
    }

    Platform::shutdown();
}

TEST(OriginAccessEntryTest, IPAddressMatchingTest)
{
    struct TestCase {
        const char* protocol;
        const char* host;
        const char* origin;
        OriginAccessEntry::MatchResult expected;
    } inputs[] = {
        { "http", "192.0.0.123", "http://192.0.0.123/", OriginAccessEntry::MatchesOrigin },
        { "http", "0.0.123", "http://192.0.0.123/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "0.123", "http://192.0.0.123/", OriginAccessEntry::DoesNotMatchOrigin },
        { "http", "1.123", "http://192.0.0.123/", OriginAccessEntry::DoesNotMatchOrigin },
    };

    OriginAccessEntryTestPlatform platform;
    platform.setPublicSuffix("com");
    Platform::initialize(&platform);

    for (const auto& test : inputs) {
        SCOPED_TRACE(testing::Message() << "Host: " << test.host << ", Origin: " << test.origin);
        RefPtr<SecurityOrigin> originToTest = SecurityOrigin::createFromString(test.origin);
        OriginAccessEntry entry1(test.protocol, test.host, OriginAccessEntry::AllowSubdomains);
        EXPECT_EQ(test.expected, entry1.matchesOrigin(*originToTest));

        OriginAccessEntry entry2(test.protocol, test.host, OriginAccessEntry::DisallowSubdomains);
        EXPECT_EQ(test.expected, entry2.matchesOrigin(*originToTest));
    }

    Platform::shutdown();
}

} // namespace blink

