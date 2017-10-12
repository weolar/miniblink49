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
#include "core/fetch/ImageResource.h"

#include "core/fetch/ImageResourceClient.h"
#include "core/fetch/MemoryCache.h"
#include "core/fetch/MockImageResourceClient.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/ResourceLoader.h"
#include "core/fetch/ResourcePtr.h"
#include "core/fetch/UniqueIdentifier.h"
#include "platform/SharedBuffer.h"
#include "platform/exported/WrappedResourceResponse.h"
#include "platform/graphics/Image.h"
#include "platform/testing/URLTestHelpers.h"
#include "platform/testing/UnitTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLResponse.h"
#include "public/platform/WebUnitTestSupport.h"

namespace blink {

static Vector<unsigned char> jpegImage()
{
    Vector<unsigned char> jpeg;

    static const unsigned char data[] = {
        0xff, 0xd8, 0xff, 0xe0, 0x00, 0x10, 0x4a, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00,
        0x48, 0x00, 0x48, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x13, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65,
        0x64, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x47, 0x49, 0x4d, 0x50, 0xff, 0xdb, 0x00, 0x43,
        0x00, 0x05, 0x03, 0x04, 0x04, 0x04, 0x03, 0x05, 0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x06,
        0x07, 0x0c, 0x08, 0x07, 0x07, 0x07, 0x07, 0x0f, 0x0b, 0x0b, 0x09, 0x0c, 0x11, 0x0f, 0x12,
        0x12, 0x11, 0x0f, 0x11, 0x11, 0x13, 0x16, 0x1c, 0x17, 0x13, 0x14, 0x1a, 0x15, 0x11, 0x11,
        0x18, 0x21, 0x18, 0x1a, 0x1d, 0x1d, 0x1f, 0x1f, 0x1f, 0x13, 0x17, 0x22, 0x24, 0x22, 0x1e,
        0x24, 0x1c, 0x1e, 0x1f, 0x1e, 0xff, 0xdb, 0x00, 0x43, 0x01, 0x05, 0x05, 0x05, 0x07, 0x06,
        0x07, 0x0e, 0x08, 0x08, 0x0e, 0x1e, 0x14, 0x11, 0x14, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
        0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
        0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e,
        0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0xff,
        0xc0, 0x00, 0x11, 0x08, 0x00, 0x01, 0x00, 0x01, 0x03, 0x01, 0x22, 0x00, 0x02, 0x11, 0x01,
        0x03, 0x11, 0x01, 0xff, 0xc4, 0x00, 0x15, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0xff, 0xc4, 0x00, 0x14,
        0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0xff, 0xc4, 0x00, 0x14, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc4, 0x00, 0x14, 0x11,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0xff, 0xda, 0x00, 0x0c, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3f,
        0x00, 0xb2, 0xc0, 0x07, 0xff, 0xd9
    };

    jpeg.append(data, sizeof(data));
    return jpeg;
}

TEST(ImageResourceTest, MultipartImage)
{
    ResourceFetcher* fetcher = ResourceFetcher::create(nullptr);
    KURL testURL(ParsedURLString, "http://www.test.com/cancelTest.html");
    URLTestHelpers::registerMockedURLLoad(testURL, "cancelTest.html", "text/html");

    // Emulate starting a real load, but don't expect any "real" WebURLLoaderClient callbacks.
    ResourcePtr<ImageResource> cachedImage = new ImageResource(ResourceRequest(testURL), nullptr);
    cachedImage->setIdentifier(createUniqueIdentifier());
    cachedImage->load(fetcher, ResourceLoaderOptions());
    Platform::current()->unitTestSupport()->unregisterMockedURL(testURL);

    MockImageResourceClient client;
    cachedImage->addClient(&client);
    EXPECT_EQ(Resource::Pending, cachedImage->status());

    // Send the multipart response. No image or data buffer is created.
    // Note that the response must be routed through ResourceLoader to
    // ensure the load is flagged as multipart.
    ResourceResponse multipartResponse(KURL(), "multipart/x-mixed-replace", 0, nullAtom, String());
    cachedImage->loader()->didReceiveResponse(nullptr, WrappedResourceResponse(multipartResponse), nullptr);
    ASSERT_FALSE(cachedImage->resourceBuffer());
    ASSERT_FALSE(cachedImage->hasImage());
    ASSERT_EQ(client.imageChangedCount(), 0);
    ASSERT_FALSE(client.notifyFinishedCalled());

    // Send the response for the first real part. No image or data buffer is created.
    const char* svgData = "<svg xmlns='http://www.w3.org/2000/svg' width='1' height='1'><rect width='1' height='1' fill='green'/></svg>";
    unsigned svgDataLength = strlen(svgData);
    ResourceResponse payloadResponse(KURL(), "image/svg+xml", svgDataLength, nullAtom, String());
    cachedImage->loader()->didReceiveResponse(nullptr, WrappedResourceResponse(payloadResponse), nullptr);
    ASSERT_FALSE(cachedImage->resourceBuffer());
    ASSERT_FALSE(cachedImage->hasImage());
    ASSERT_EQ(client.imageChangedCount(), 0);
    ASSERT_FALSE(client.notifyFinishedCalled());

    // The first bytes arrive. The data buffer is created, but no image is created.
    cachedImage->appendData(svgData, svgDataLength);
    ASSERT_TRUE(cachedImage->resourceBuffer());
    ASSERT_EQ(cachedImage->resourceBuffer()->size(), svgDataLength);
    ASSERT_FALSE(cachedImage->hasImage());
    ASSERT_EQ(client.imageChangedCount(), 0);
    ASSERT_FALSE(client.notifyFinishedCalled());

    // This part finishes. The image is created, callbacks are sent, and the data buffer is cleared.
    cachedImage->finish();
    ASSERT_FALSE(cachedImage->resourceBuffer());
    ASSERT_FALSE(cachedImage->errorOccurred());
    ASSERT_TRUE(cachedImage->hasImage());
    ASSERT_FALSE(cachedImage->image()->isNull());
    ASSERT_EQ(cachedImage->image()->width(), 1);
    ASSERT_EQ(cachedImage->image()->height(), 1);
    ASSERT_EQ(client.imageChangedCount(), 2);
    ASSERT_TRUE(client.notifyFinishedCalled());
}

TEST(ImageResourceTest, CancelOnDetach)
{
    KURL testURL(ParsedURLString, "http://www.test.com/cancelTest.html");
    URLTestHelpers::registerMockedURLLoad(testURL, "cancelTest.html", "text/html");

    ResourceFetcher* fetcher = ResourceFetcher::create(nullptr);

    // Emulate starting a real load.
    ResourcePtr<ImageResource> cachedImage = new ImageResource(ResourceRequest(testURL), nullptr);
    cachedImage->setIdentifier(createUniqueIdentifier());

    cachedImage->load(fetcher, ResourceLoaderOptions());
    memoryCache()->add(cachedImage.get());

    MockImageResourceClient client;
    cachedImage->addClient(&client);
    EXPECT_EQ(Resource::Pending, cachedImage->status());

    // The load should still be alive, but a timer should be started to cancel the load inside removeClient().
    cachedImage->removeClient(&client);
    EXPECT_EQ(Resource::Pending, cachedImage->status());
    EXPECT_NE(reinterpret_cast<Resource*>(0), memoryCache()->resourceForURL(testURL));

    // Trigger the cancel timer, ensure the load was cancelled and the resource was evicted from the cache.
    blink::testing::runPendingTasks();
    EXPECT_EQ(Resource::LoadError, cachedImage->status());
    EXPECT_EQ(reinterpret_cast<Resource*>(0), memoryCache()->resourceForURL(testURL));

    Platform::current()->unitTestSupport()->unregisterMockedURL(testURL);
}

TEST(ImageResourceTest, DecodedDataRemainsWhileHasClients)
{
    ResourcePtr<ImageResource> cachedImage = new ImageResource(ResourceRequest(), nullptr);
    cachedImage->setLoading(true);

    MockImageResourceClient client;
    cachedImage->addClient(&client);

    // Send the image response.
    cachedImage->responseReceived(ResourceResponse(KURL(), "multipart/x-mixed-replace", 0, nullAtom, String()), nullptr);

    Vector<unsigned char> jpeg = jpegImage();
    cachedImage->responseReceived(ResourceResponse(KURL(), "image/jpeg", jpeg.size(), nullAtom, String()), nullptr);
    cachedImage->appendData(reinterpret_cast<const char*>(jpeg.data()), jpeg.size());
    cachedImage->finish();
    ASSERT_FALSE(cachedImage->errorOccurred());
    ASSERT_TRUE(cachedImage->hasImage());
    ASSERT_FALSE(cachedImage->image()->isNull());
    ASSERT_TRUE(client.notifyFinishedCalled());

    // The prune comes when the ImageResource still has clients. The image should not be deleted.
    cachedImage->prune();
    ASSERT_TRUE(cachedImage->hasClients());
    ASSERT_TRUE(cachedImage->hasImage());
    ASSERT_FALSE(cachedImage->image()->isNull());

    // The ImageResource no longer has clients. The image should be deleted by prune.
    cachedImage->removeClient(&client);
    cachedImage->prune();
    ASSERT_FALSE(cachedImage->hasClients());
    ASSERT_FALSE(cachedImage->hasImage());
    ASSERT_TRUE(cachedImage->image()->isNull());
}

TEST(ImageResourceTest, UpdateBitmapImages)
{
    ResourcePtr<ImageResource> cachedImage = new ImageResource(ResourceRequest(), nullptr);
    cachedImage->setLoading(true);

    MockImageResourceClient client;
    cachedImage->addClient(&client);

    // Send the image response.
    Vector<unsigned char> jpeg = jpegImage();
    cachedImage->responseReceived(ResourceResponse(KURL(), "image/jpeg", jpeg.size(), nullAtom, String()), nullptr);
    cachedImage->appendData(reinterpret_cast<const char*>(jpeg.data()), jpeg.size());
    cachedImage->finish();
    ASSERT_FALSE(cachedImage->errorOccurred());
    ASSERT_TRUE(cachedImage->hasImage());
    ASSERT_FALSE(cachedImage->image()->isNull());
    ASSERT_EQ(client.imageChangedCount(), 2);
    ASSERT_TRUE(client.notifyFinishedCalled());

    HashSet<ImageResource*> bitmapImages;
    ASSERT_TRUE(cachedImage->image()->isBitmapImage());
    bitmapImages.add(cachedImage.get());
}

} // namespace blink
