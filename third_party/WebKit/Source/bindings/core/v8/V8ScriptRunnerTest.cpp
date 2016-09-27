// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/V8ScriptRunner.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "core/fetch/CachedMetadataHandler.h"
#include "core/fetch/ScriptResource.h"
#include "platform/heap/Handle.h"
#include <gtest/gtest.h>
#include <v8.h>

namespace blink {

namespace {

class V8ScriptRunnerTest : public ::testing::Test {
public:
    V8ScriptRunnerTest() : m_scope(v8::Isolate::GetCurrent()) { }
    ~V8ScriptRunnerTest() override { }

    void SetUp() override
    {
        // To trick various layers of caching, increment a counter for each
        // test and use it in code(), fielname() and url().
        counter++;
    }

    void TearDown() override
    {
        m_resourceRequest.clear();
        m_resource.clear();
    }

    v8::Isolate* isolate() const
    {
        return m_scope.isolate();
    }
    WTF::String code() const
    {
        // Simple function for testing. Note:
        // - Add counter to trick V8 code cache.
        // - Pad counter to 1000 digits, to trick minimal cacheability threshold.
        return WTF::String::format("a = function() { 1 + 1; } // %01000d\n", counter);
    }
    WTF::String filename() const
    {
        return WTF::String::format("whatever%d.js", counter);
    }
    WTF::String url() const
    {
        return WTF::String::format("http://bla.com/bla%d", counter);
    }
    unsigned tagForParserCache(CachedMetadataHandler* cacheHandler) const
    {
        return V8ScriptRunner::tagForParserCache(cacheHandler);
    }
    unsigned tagForCodeCache(CachedMetadataHandler* cacheHandler) const
    {
        return V8ScriptRunner::tagForCodeCache(cacheHandler);
    }

    bool compileScript(V8CacheOptions cacheOptions)
    {
        return !V8ScriptRunner::compileScript(
            v8String(isolate(), code()), filename(), String(), WTF::TextPosition(),
            isolate(), m_resource.get(), nullptr, m_resource.get() ? m_resource->cacheHandler(): nullptr, NotSharableCrossOrigin, cacheOptions)
            .IsEmpty();
    }

    void setEmptyResource()
    {
        m_resourceRequest = WTF::adoptPtr(new ResourceRequest);
        m_resource = new ScriptResource(*m_resourceRequest.get(), "UTF-8");
    }

    void setResource()
    {
        m_resourceRequest = WTF::adoptPtr(new ResourceRequest(url()));
        m_resource = new ScriptResource(*m_resourceRequest.get(), "UTF-8");
    }

    CachedMetadataHandler* cacheHandler()
    {
        return m_resource->cacheHandler();
    }

protected:
    WTF::OwnPtr<ResourceRequest> m_resourceRequest;
    ResourcePtr<ScriptResource> m_resource;
    V8TestingScope m_scope;

    static int counter;
};

int V8ScriptRunnerTest::counter = 0;

TEST_F(V8ScriptRunnerTest, resourcelessShouldPass)
{
    EXPECT_TRUE(compileScript(V8CacheOptionsNone));
    EXPECT_TRUE(compileScript(V8CacheOptionsParse));
    EXPECT_TRUE(compileScript(V8CacheOptionsCode));
}

TEST_F(V8ScriptRunnerTest, emptyResourceDoesNotHaveCacheHandler)
{
    setEmptyResource();
    EXPECT_FALSE(cacheHandler());
}

TEST_F(V8ScriptRunnerTest, parseOption)
{
    setResource();
    EXPECT_TRUE(compileScript(V8CacheOptionsParse));
    EXPECT_TRUE(cacheHandler()->cachedMetadata(tagForParserCache(cacheHandler())));
    EXPECT_FALSE(cacheHandler()->cachedMetadata(tagForCodeCache(cacheHandler())));
    // The cached data is associated with the encoding.
    ResourceRequest request(url());
    ResourcePtr<ScriptResource> anotherResource = new ScriptResource(request, "UTF-16");
    EXPECT_FALSE(cacheHandler()->cachedMetadata(tagForParserCache(anotherResource->cacheHandler())));
}

TEST_F(V8ScriptRunnerTest, codeOption)
{
    setResource();

    // Compile twice, since 'code' has a probation period before it caches.
    EXPECT_TRUE(compileScript(V8CacheOptionsCode));
    EXPECT_TRUE(compileScript(V8CacheOptionsCode));

    EXPECT_FALSE(cacheHandler()->cachedMetadata(tagForParserCache(cacheHandler())));
    EXPECT_TRUE(cacheHandler()->cachedMetadata(tagForCodeCache(cacheHandler())));
    // The cached data is associated with the encoding.
    ResourceRequest request(url());
    ResourcePtr<ScriptResource> anotherResource = new ScriptResource(request, "UTF-16");
    EXPECT_FALSE(cacheHandler()->cachedMetadata(tagForCodeCache(anotherResource->cacheHandler())));
}

} // namespace

} // namespace blink
