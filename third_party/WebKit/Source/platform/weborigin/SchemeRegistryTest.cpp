// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/weborigin/SchemeRegistry.h"

#include <gtest/gtest.h>

namespace blink {
namespace {

const char kTestScheme[] = "test-scheme";
const char kTestSchemeUppercase[] = "TEST-SCHEME";
const char kTestScheme2[] = "test-scheme-2";

class SchemeRegistryTest : public ::testing::Test {
    void TearDown() override
    {
        SchemeRegistry::removeURLSchemeRegisteredAsBypassingContentSecurityPolicy(kTestScheme);
    }
};

TEST_F(SchemeRegistryTest, NoCSPBypass)
{
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestSchemeUppercase));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme, SchemeRegistry::PolicyAreaImage));
}

TEST_F(SchemeRegistryTest, FullCSPBypass)
{
    SchemeRegistry::registerURLSchemeAsBypassingContentSecurityPolicy(kTestScheme);
    EXPECT_TRUE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme));
    EXPECT_TRUE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestSchemeUppercase));
    EXPECT_TRUE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme, SchemeRegistry::PolicyAreaImage));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme2));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme2, SchemeRegistry::PolicyAreaImage));
}

TEST_F(SchemeRegistryTest, PartialCSPBypass)
{
    SchemeRegistry::registerURLSchemeAsBypassingContentSecurityPolicy(kTestScheme, SchemeRegistry::PolicyAreaImage);
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestSchemeUppercase));
    EXPECT_TRUE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme, SchemeRegistry::PolicyAreaImage));
    EXPECT_TRUE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestSchemeUppercase, SchemeRegistry::PolicyAreaImage));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme, SchemeRegistry::PolicyAreaStyle));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestSchemeUppercase, SchemeRegistry::PolicyAreaStyle));
    EXPECT_FALSE(SchemeRegistry::schemeShouldBypassContentSecurityPolicy(kTestScheme2, SchemeRegistry::PolicyAreaImage));
}

} // namespace
} // namespace blink
