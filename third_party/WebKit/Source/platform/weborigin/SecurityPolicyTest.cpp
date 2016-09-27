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
#include "platform/weborigin/SecurityPolicy.h"

#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include <gtest/gtest.h>

namespace blink {

TEST(SecurityPolicyTest, ReferrerIsAlwaysAWebURL)
{
    EXPECT_TRUE(String() == SecurityPolicy::generateReferrer(ReferrerPolicyAlways, KURL(ParsedURLString, "http://example.com/"), String::fromUTF8("chrome://somepage/")).referrer);
}

TEST(SecurityPolicyTest, GenerateReferrer)
{
    struct TestCase {
        ReferrerPolicy policy;
        const char* referrer;
        const char* destination;
        const char* expected;
    };

    const char insecureURLA[] = "http://a.test/path/to/file.html";
    const char insecureURLB[] = "http://b.test/path/to/file.html";
    const char insecureOriginA[] = "http://a.test/";

    const char secureURLA[] = "https://a.test/path/to/file.html";
    const char secureURLB[] = "https://b.test/path/to/file.html";
    const char secureOriginA[] = "https://a.test/";

    const char blobURL[] = "blob:http%3A//a.test/b3aae9c8-7f90-440d-8d7c-43aa20d72fde";
    const char filesystemURL[] = "filesystem:http%3A//a.test/path/t/file.html";

    TestCase inputs[] = {
        // HTTP -> HTTP: Same Origin
        { ReferrerPolicyAlways, insecureURLA, insecureURLA, insecureURLA },
        { ReferrerPolicyDefault, insecureURLA, insecureURLA, insecureURLA },
        { ReferrerPolicyNoReferrerWhenDowngrade, insecureURLA, insecureURLA, insecureURLA },
        { ReferrerPolicyNever, insecureURLA, insecureURLA, 0 },
        { ReferrerPolicyOrigin, insecureURLA, insecureURLA, insecureOriginA },
        { ReferrerPolicyOriginWhenCrossOrigin, insecureURLA, insecureURLA, insecureURLA },

        // HTTP -> HTTP: Cross Origin
        { ReferrerPolicyAlways, insecureURLA, insecureURLB, insecureURLA },
        { ReferrerPolicyDefault, insecureURLA, insecureURLB, insecureURLA },
        { ReferrerPolicyNoReferrerWhenDowngrade, insecureURLA, insecureURLB, insecureURLA },
        { ReferrerPolicyNever, insecureURLA, insecureURLB, 0 },
        { ReferrerPolicyOrigin, insecureURLA, insecureURLB, insecureOriginA },
        { ReferrerPolicyOriginWhenCrossOrigin, insecureURLA, insecureURLB, insecureOriginA },

        // HTTPS -> HTTPS: Same Origin
        { ReferrerPolicyAlways, secureURLA, secureURLA, secureURLA },
        { ReferrerPolicyDefault, secureURLA, secureURLA, secureURLA },
        { ReferrerPolicyNoReferrerWhenDowngrade, secureURLA, secureURLA, secureURLA },
        { ReferrerPolicyNever, secureURLA, secureURLA, 0 },
        { ReferrerPolicyOrigin, secureURLA, secureURLA, secureOriginA },
        { ReferrerPolicyOriginWhenCrossOrigin, secureURLA, secureURLA, secureURLA },

        // HTTPS -> HTTPS: Cross Origin
        { ReferrerPolicyAlways, secureURLA, secureURLB, secureURLA },
        { ReferrerPolicyDefault, secureURLA, secureURLB, secureURLA },
        { ReferrerPolicyNoReferrerWhenDowngrade, secureURLA, secureURLB, secureURLA },
        { ReferrerPolicyNever, secureURLA, secureURLB, 0 },
        { ReferrerPolicyOrigin, secureURLA, secureURLB, secureOriginA },
        { ReferrerPolicyOriginWhenCrossOrigin, secureURLA, secureURLB, secureOriginA },

        // HTTP -> HTTPS
        { ReferrerPolicyAlways, insecureURLA, secureURLB, insecureURLA },
        { ReferrerPolicyDefault, insecureURLA, secureURLB, insecureURLA },
        { ReferrerPolicyNoReferrerWhenDowngrade, insecureURLA, secureURLB, insecureURLA },
        { ReferrerPolicyNever, insecureURLA, secureURLB, 0 },
        { ReferrerPolicyOrigin, insecureURLA, secureURLB, insecureOriginA },
        { ReferrerPolicyOriginWhenCrossOrigin, insecureURLA, secureURLB, insecureOriginA },

        // HTTPS -> HTTP
        { ReferrerPolicyAlways, secureURLA, insecureURLB, secureURLA },
        { ReferrerPolicyDefault, secureURLA, insecureURLB, 0 },
        { ReferrerPolicyNoReferrerWhenDowngrade, secureURLA, insecureURLB, 0 },
        { ReferrerPolicyNever, secureURLA, insecureURLB, 0 },
        { ReferrerPolicyOrigin, secureURLA, insecureURLB, secureOriginA },
        { ReferrerPolicyOriginWhenCrossOrigin, secureURLA, secureURLB, secureOriginA },

        // blob and filesystem URL handling
        { ReferrerPolicyAlways, insecureURLA, blobURL, 0 },
        { ReferrerPolicyAlways, blobURL, insecureURLA, 0 },
        { ReferrerPolicyAlways, insecureURLA, filesystemURL, 0 },
        { ReferrerPolicyAlways, filesystemURL, insecureURLA, 0 },
    };

    for (TestCase test : inputs) {
        KURL destination(ParsedURLString, test.destination);
        Referrer result = SecurityPolicy::generateReferrer(test.policy, destination, String::fromUTF8(test.referrer));
        if (test.expected) {
            EXPECT_EQ(String::fromUTF8(test.expected), result.referrer)
                << "'" << test.referrer << "' to '" << test.destination
                << "' should have been '" << test.expected << "': was '"
                << result.referrer.utf8().data() << "'.";
        } else {
            EXPECT_TRUE(result.referrer.isEmpty())
                << "'" << test.referrer << "' to '" << test.destination
                << "' should have been empty: was '" << result.referrer.utf8().data() << "'.";
        }
        EXPECT_EQ(test.policy, result.referrerPolicy);
    }
}

TEST(SecurityPolicyTest, TrustworthyWhiteList)
{
    const char* insecureURLs[] = {
        "http://a.test/path/to/file.html",
        "http://b.test/path/to/file.html",
        "blob:http://c.test/b3aae9c8-7f90-440d-8d7c-43aa20d72fde",
        "filesystem:http://d.test/path/t/file.html",
    };

    for (const char* url : insecureURLs) {
        String errorMessage;
        RefPtr<SecurityOrigin> origin = SecurityOrigin::createFromString(url);
        EXPECT_FALSE(origin->isPotentiallyTrustworthy(errorMessage));
        SecurityPolicy::addOriginTrustworthyWhiteList(origin);
        EXPECT_TRUE(origin->isPotentiallyTrustworthy(errorMessage));
    }

    // Tests that adding URLs that have inner-urls to the whitelist
    // takes effect on the origins of the inner-urls (and vice versa).
    struct TestCase {
        const char* url;
        const char* anotherUrlInOrigin;
    };
    TestCase insecureURLsWithInnerOrigin[] = {
        {
            "blob:http://e.test/b3aae9c8-7f90-440d-8d7c-43aa20d72fde",
            "http://e.test/foo.html"
        }, {
            "filesystem:http://f.test/path/t/file.html",
            "http://f.test/bar.html"
        }, {
            "http://g.test/foo.html",
            "blob:http://g.test/b3aae9c8-7f90-440d-8d7c-43aa20d72fde"
        }, {
            "http://h.test/bar.html",
            "filesystem:http://h.test/path/t/file.html"
        },
    };
    for (const TestCase& test : insecureURLsWithInnerOrigin) {
        String errorMessage;

        // Actually origins of both URLs should be same.
        RefPtr<SecurityOrigin> origin1 = SecurityOrigin::createFromString(test.url);
        RefPtr<SecurityOrigin> origin2 = SecurityOrigin::createFromString(test.anotherUrlInOrigin);

        EXPECT_FALSE(origin1->isPotentiallyTrustworthy(errorMessage));
        EXPECT_FALSE(origin2->isPotentiallyTrustworthy(errorMessage));
        SecurityPolicy::addOriginTrustworthyWhiteList(origin1);
        EXPECT_TRUE(origin1->isPotentiallyTrustworthy(errorMessage));
        EXPECT_TRUE(origin2->isPotentiallyTrustworthy(errorMessage));
    }
}

} // namespace blink
