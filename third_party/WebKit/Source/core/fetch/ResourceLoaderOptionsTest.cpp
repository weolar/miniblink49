// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ResourceLoaderOptionsTest_h
#define ResourceLoaderOptionsTest_h

#include "config.h"
#include "core/fetch/ResourceLoaderOptions.h"

#include "wtf/TypeTraits.h"

#include <gtest/gtest.h>

namespace blink {

namespace {

TEST(ResourceLoaderOptionsTest, DeepCopy)
{
    // Check that the fields of ResourceLoaderOptions are enums,
    // except for initiatorInfo and securityOrigin.
    static_assert(WTF::IsEnum<DataBufferingPolicy>::value, "DataBufferingPolicy should be an enum");
    static_assert(WTF::IsEnum<StoredCredentials>::value, "StoredCredentials should be an enum");
    static_assert(WTF::IsEnum<CredentialRequest>::value, "CredentialRequest should be an enum");
    static_assert(WTF::IsEnum<ContentSecurityPolicyDisposition>::value, "ContentSecurityPolicyDisposition should be an enum");
    static_assert(WTF::IsEnum<RequestInitiatorContext>::value, "RequestInitiatorContext should be an enum");
    static_assert(WTF::IsEnum<SynchronousPolicy>::value, "SynchronousPolicy should be an enum");
    static_assert(WTF::IsEnum<CORSEnabled>::value, "CORSEnabled should be an enum");

    ResourceLoaderOptions original;
    RefPtr<SecurityOrigin> securityOrigin = SecurityOrigin::createFromString("http://www.google.com");
    original.securityOrigin = securityOrigin;
    original.initiatorInfo.name = AtomicString("xmlhttprequest");

    CrossThreadResourceLoaderOptionsData copyData = CrossThreadCopier<ResourceLoaderOptions>::copy(original);
    ResourceLoaderOptions copy = copyData;

    // Check that contents are correctly copied to |copyData|
    EXPECT_EQ(original.dataBufferingPolicy, copyData.dataBufferingPolicy);
    EXPECT_EQ(original.allowCredentials, copyData.allowCredentials);
    EXPECT_EQ(original.credentialsRequested, copyData.credentialsRequested);
    EXPECT_EQ(original.contentSecurityPolicyOption, copyData.contentSecurityPolicyOption);
    EXPECT_EQ(original.initiatorInfo.name, copyData.initiatorInfo.name);
    EXPECT_EQ(original.initiatorInfo.position, copyData.initiatorInfo.position);
    EXPECT_EQ(original.initiatorInfo.startTime, copyData.initiatorInfo.startTime);
    EXPECT_EQ(original.requestInitiatorContext, copyData.requestInitiatorContext);
    EXPECT_EQ(original.synchronousPolicy, copyData.synchronousPolicy);
    EXPECT_EQ(original.corsEnabled, copyData.corsEnabled);
    EXPECT_EQ(original.securityOrigin->protocol(), copyData.securityOrigin->protocol());
    EXPECT_EQ(original.securityOrigin->host(), copyData.securityOrigin->host());
    EXPECT_EQ(original.securityOrigin->domain(), copyData.securityOrigin->domain());

    // Check that pointers are different between |original| and |copyData|
    EXPECT_NE(original.initiatorInfo.name.impl(), copyData.initiatorInfo.name.impl());
    EXPECT_NE(original.securityOrigin.get(), copyData.securityOrigin.get());
    EXPECT_NE(original.securityOrigin->protocol().impl(), copyData.securityOrigin->protocol().impl());
    EXPECT_NE(original.securityOrigin->host().impl(), copyData.securityOrigin->host().impl());
    EXPECT_NE(original.securityOrigin->domain().impl(), copyData.securityOrigin->domain().impl());

    // Check that contents are correctly copied to |copy|
    EXPECT_EQ(original.dataBufferingPolicy, copy.dataBufferingPolicy);
    EXPECT_EQ(original.allowCredentials, copy.allowCredentials);
    EXPECT_EQ(original.credentialsRequested, copy.credentialsRequested);
    EXPECT_EQ(original.contentSecurityPolicyOption, copy.contentSecurityPolicyOption);
    EXPECT_EQ(original.initiatorInfo.name, copy.initiatorInfo.name);
    EXPECT_EQ(original.initiatorInfo.position, copy.initiatorInfo.position);
    EXPECT_EQ(original.initiatorInfo.startTime, copy.initiatorInfo.startTime);
    EXPECT_EQ(original.requestInitiatorContext, copy.requestInitiatorContext);
    EXPECT_EQ(original.synchronousPolicy, copy.synchronousPolicy);
    EXPECT_EQ(original.corsEnabled, copy.corsEnabled);
    EXPECT_EQ(original.securityOrigin->protocol(), copy.securityOrigin->protocol());
    EXPECT_EQ(original.securityOrigin->host(), copy.securityOrigin->host());
    EXPECT_EQ(original.securityOrigin->domain(), copy.securityOrigin->domain());

    // Check that pointers are different between |original| and |copy|
    // FIXME: When |original| and |copy| are in different threads, then
    // EXPECT_NE(original.initiatorInfo.name.impl(), copy.initiatorInfo.name.impl());
    // should pass. However, in the unit test here, these two pointers are the
    // same, because initiatorInfo.name is AtomicString.
    EXPECT_NE(original.securityOrigin.get(), copy.securityOrigin.get());
    EXPECT_NE(original.securityOrigin->protocol().impl(), copy.securityOrigin->protocol().impl());
    EXPECT_NE(original.securityOrigin->host().impl(), copy.securityOrigin->host().impl());
    EXPECT_NE(original.securityOrigin->domain().impl(), copy.securityOrigin->domain().impl());

    // FIXME: The checks for content equality/pointer inequality for
    // securityOrigin here is not complete (i.e. m_filePath is not checked).
    // A unit test for SecurityOrigin::isolatedCopy() that covers these checks
    // should be added.
}

} // namespace

} // namespace blink

#endif // ResourceLoaderOptionsTest_h
