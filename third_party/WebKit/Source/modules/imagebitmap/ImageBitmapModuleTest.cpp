// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/frame/ImageBitmap.h"

#include "bindings/core/v8/UnionTypesCore.h"
#include "core/dom/Document.h"
#include "core/fetch/MemoryCache.h"
#include "core/html/HTMLCanvasElement.h"
#include "modules/canvas2d/CanvasRenderingContext2D.h"
#include "platform/heap/Handle.h"

#include <gtest/gtest.h>

namespace blink {

class ImageBitmapModuleTest : public ::testing::Test {
protected:
    virtual void SetUp()
    {
        // Save the global memory cache to restore it upon teardown.
        m_globalMemoryCache = replaceMemoryCacheForTesting(MemoryCache::create());
    }
    virtual void TearDown()
    {
        // Garbage collection is required prior to switching out the
        // test's memory cache; image resources are released, evicting
        // them from the cache.
        Heap::collectGarbage(ThreadState::NoHeapPointersOnStack, ThreadState::GCWithSweep, Heap::ForcedGC);

        replaceMemoryCacheForTesting(m_globalMemoryCache.release());
    }

    Persistent<MemoryCache> m_globalMemoryCache;
};

// Verifies that ImageBitmaps constructed from ImageBitmaps hold onto their own Image.
TEST_F(ImageBitmapModuleTest, ImageResourceLifetime)
{
    RefPtrWillBeRawPtr<HTMLCanvasElement> canvasElement = HTMLCanvasElement::create(*Document::create().get());
    canvasElement->setHeight(40);
    canvasElement->setWidth(40);
    RefPtrWillBeRawPtr<ImageBitmap> imageBitmapDerived = nullptr;
    {
        RefPtrWillBeRawPtr<ImageBitmap> imageBitmapFromCanvas = ImageBitmap::create(canvasElement.get(), IntRect(0, 0, canvasElement->width(), canvasElement->height()));
        imageBitmapDerived = ImageBitmap::create(imageBitmapFromCanvas.get(), IntRect(0, 0, 20, 20));
    }
    CanvasContextCreationAttributes attributes;
    CanvasRenderingContext2D* context = static_cast<CanvasRenderingContext2D*>(canvasElement->getCanvasRenderingContext("2d", attributes));
    TrackExceptionState exceptionState;
    CanvasImageSourceUnion imageSource;
    imageSource.setImageBitmap(imageBitmapDerived);
    context->drawImage(imageSource, 0, 0, exceptionState);
}

} // namespace
