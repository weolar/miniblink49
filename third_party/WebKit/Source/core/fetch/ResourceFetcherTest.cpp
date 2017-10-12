/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
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
#include "core/fetch/ResourceFetcher.h"

#include "core/fetch/FetchInitiatorInfo.h"
#include "core/fetch/FetchRequest.h"
#include "core/fetch/ImageResource.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/ResourcePtr.h"
#include "platform/heap/Handle.h"
#include "platform/network/ResourceRequest.h"
#include "platform/weborigin/KURL.h"
#include <gtest/gtest.h>

namespace blink {

class ResourceFetcherTest : public ::testing::Test {
};

class TestResourceFactory : public ResourceFactory {
public:
    TestResourceFactory()
        : ResourceFactory(Resource::Image) { }

    Resource* create(const ResourceRequest& request, const String& charset) const override
    {
        return new Resource(request, Resource::Image);
    }
};

TEST_F(ResourceFetcherTest, StartLoadAfterFrameDetach)
{
    KURL secureURL(ParsedURLString, "https://secureorigin.test/image.png");
    // Try to request a url. The request should fail, no resource should be returned,
    // and no resource should be present in the cache.
    ResourceFetcher* fetcher = ResourceFetcher::create(nullptr);
    FetchRequest fetchRequest = FetchRequest(ResourceRequest(secureURL), FetchInitiatorInfo());
    ResourcePtr<Resource> resource = fetcher->requestResource(fetchRequest, TestResourceFactory());
    EXPECT_EQ(resource.get(), static_cast<Resource*>(nullptr));
    EXPECT_EQ(memoryCache()->resourceForURL(secureURL), static_cast<Resource*>(nullptr));
}

} // namespace blink
