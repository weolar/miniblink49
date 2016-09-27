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
#include "platform/image-decoders/webp/WEBPImageDecoder.h"

#include "platform/RuntimeEnabledFeatures.h"
#include "platform/SharedBuffer.h"
#include "public/platform/Platform.h"
#include "public/platform/WebData.h"
#include "public/platform/WebSize.h"
#include "public/platform/WebUnitTestSupport.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/StringHasher.h"
#include "wtf/Vector.h"
#include "wtf/dtoa/utils.h"
#include <gtest/gtest.h>

namespace blink {

namespace {

PassRefPtr<SharedBuffer> readFile(const char* fileName)
{
    String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
    filePath.append(fileName);

    return Platform::current()->unitTestSupport()->readFromFile(filePath);
}

PassOwnPtr<WEBPImageDecoder> createDecoder(ImageSource::AlphaOption alphaOption = ImageSource::AlphaNotPremultiplied)
{
    return adoptPtr(new WEBPImageDecoder(alphaOption, ImageSource::GammaAndColorProfileApplied, ImageDecoder::noDecodedImageByteLimit));
}

unsigned hashSkBitmap(const SkBitmap& bitmap)
{
    return StringHasher::hashMemory(bitmap.getPixels(), bitmap.getSize());
}

void createDecodingBaseline(SharedBuffer* data, Vector<unsigned>* baselineHashes)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    decoder->setData(data, true);
    size_t frameCount = decoder->frameCount();
    for (size_t i = 0; i < frameCount; ++i) {
        ImageFrame* frame = decoder->frameBufferAtIndex(i);
        baselineHashes->append(hashSkBitmap(frame->getSkBitmap()));
    }
}

void testRandomFrameDecode(const char* webpFile)
{
    SCOPED_TRACE(webpFile);

    RefPtr<SharedBuffer> fullData = readFile(webpFile);
    ASSERT_TRUE(fullData.get());
    Vector<unsigned> baselineHashes;
    createDecodingBaseline(fullData.get(), &baselineHashes);
    size_t frameCount = baselineHashes.size();

    // Random decoding should get the same results as sequential decoding.
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    decoder->setData(fullData.get(), true);
    const size_t skippingStep = 5;
    for (size_t i = 0; i < skippingStep; ++i) {
        for (size_t j = i; j < frameCount; j += skippingStep) {
            SCOPED_TRACE(testing::Message() << "Random i:" << i << " j:" << j);
            ImageFrame* frame = decoder->frameBufferAtIndex(j);
            EXPECT_EQ(baselineHashes[j], hashSkBitmap(frame->getSkBitmap()));
        }
    }

    // Decoding in reverse order.
    decoder = createDecoder();
    decoder->setData(fullData.get(), true);
    for (size_t i = frameCount; i; --i) {
        SCOPED_TRACE(testing::Message() << "Reverse i:" << i);
        ImageFrame* frame = decoder->frameBufferAtIndex(i - 1);
        EXPECT_EQ(baselineHashes[i - 1], hashSkBitmap(frame->getSkBitmap()));
    }
}

void testRandomDecodeAfterClearFrameBufferCache(const char* webpFile)
{
    SCOPED_TRACE(webpFile);

    RefPtr<SharedBuffer> data = readFile(webpFile);
    ASSERT_TRUE(data.get());
    Vector<unsigned> baselineHashes;
    createDecodingBaseline(data.get(), &baselineHashes);
    size_t frameCount = baselineHashes.size();

    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    decoder->setData(data.get(), true);
    for (size_t clearExceptFrame = 0; clearExceptFrame < frameCount; ++clearExceptFrame) {
        decoder->clearCacheExceptFrame(clearExceptFrame);
        const size_t skippingStep = 5;
        for (size_t i = 0; i < skippingStep; ++i) {
            for (size_t j = 0; j < frameCount; j += skippingStep) {
                SCOPED_TRACE(testing::Message() << "Random i:" << i << " j:" << j);
                ImageFrame* frame = decoder->frameBufferAtIndex(j);
                EXPECT_EQ(baselineHashes[j], hashSkBitmap(frame->getSkBitmap()));
            }
        }
    }
}

void testDecodeAfterReallocatingData(const char* webpFile)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    RefPtr<SharedBuffer> data = readFile(webpFile);
    ASSERT_TRUE(data.get());

    // Parse from 'data'.
    decoder->setData(data.get(), true);
    size_t frameCount = decoder->frameCount();

    // ... and then decode frames from 'reallocatedData'.
    RefPtr<SharedBuffer> reallocatedData = data.get()->copy();
    ASSERT_TRUE(reallocatedData.get());
    data.clear();
    decoder->setData(reallocatedData.get(), true);

    for (size_t i = 0; i < frameCount; ++i) {
        const ImageFrame* const frame = decoder->frameBufferAtIndex(i);
        EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
    }
}

void testByteByByteDecode(const char* webpFile, size_t expectedFrameCount, int expectedRepetitionCount)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    RefPtr<SharedBuffer> data = readFile(webpFile);
    ASSERT_TRUE(data.get());

    size_t frameCount = 0;
    size_t framesDecoded = 0;

    // Pass data to decoder byte by byte.
    for (size_t length = 1; length <= data->size(); ++length) {
        RefPtr<SharedBuffer> tempData = SharedBuffer::create(data->data(), length);
        decoder->setData(tempData.get(), length == data->size());

        EXPECT_LE(frameCount, decoder->frameCount());
        frameCount = decoder->frameCount();

        ImageFrame* frame = decoder->frameBufferAtIndex(frameCount - 1);
        if (frame && frame->status() == ImageFrame::FrameComplete && framesDecoded < frameCount)
            ++framesDecoded;

        if (decoder->failed())
            break;
    }

    EXPECT_FALSE(decoder->failed());
    EXPECT_EQ(expectedFrameCount, decoder->frameCount());
    EXPECT_EQ(expectedFrameCount, framesDecoded);
    EXPECT_EQ(expectedRepetitionCount, decoder->repetitionCount());
}

void testByteByByteSizeAvailable(const char* webpFile, size_t frameOffset, bool hasColorProfile, int expectedRepetitionCount)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    RefPtr<SharedBuffer> data = readFile(webpFile);
    ASSERT_TRUE(data.get());
    EXPECT_LT(frameOffset, data->size());

    // Send data to the decoder byte-by-byte and use the provided frame offset in the data to check
    // isSizeAvailable() changes state only when that offset is reached, and the associated decoder
    // state also: size, colorProfile, frameCount, repetitionCount ...

    for (size_t length = 1; length <= frameOffset; ++length) {
        RefPtr<SharedBuffer> tempData = SharedBuffer::create(data->data(), length);
        decoder->setData(tempData.get(), false);

        if (length < frameOffset) {
            EXPECT_FALSE(decoder->isSizeAvailable());
            EXPECT_TRUE(decoder->size().isEmpty());
            EXPECT_FALSE(decoder->hasColorProfile());
            EXPECT_EQ(0u, decoder->frameCount());
            EXPECT_EQ(cAnimationLoopOnce, decoder->repetitionCount());
            EXPECT_FALSE(decoder->frameBufferAtIndex(0));
        } else {
            EXPECT_TRUE(decoder->isSizeAvailable());
            EXPECT_FALSE(decoder->size().isEmpty());
#if USE(QCMSLIB)
            if (hasColorProfile)
                EXPECT_TRUE(decoder->hasColorProfile());
            else
                EXPECT_FALSE(decoder->hasColorProfile());
#else
            EXPECT_FALSE(decoder->hasColorProfile());
#endif
            EXPECT_EQ(1u, decoder->frameCount());
            EXPECT_EQ(expectedRepetitionCount, decoder->repetitionCount());
        }

        EXPECT_FALSE(decoder->failed());
        if (decoder->failed())
            return;
    }
}

// If 'parseErrorExpected' is true, error is expected during parse (frameCount()
// call); else error is expected during decode (frameBufferAtIndex() call).
void testInvalidImage(const char* webpFile, bool parseErrorExpected)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> data = readFile(webpFile);
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);

    if (parseErrorExpected) {
        EXPECT_EQ(0u, decoder->frameCount());
        EXPECT_FALSE(decoder->frameBufferAtIndex(0));
    } else {
        EXPECT_GT(decoder->frameCount(), 0u);
        ImageFrame* frame = decoder->frameBufferAtIndex(0);
        ASSERT_TRUE(frame);
        EXPECT_EQ(ImageFrame::FramePartial, frame->status());
    }
    EXPECT_EQ(cAnimationLoopOnce, decoder->repetitionCount());
    EXPECT_TRUE(decoder->failed());
}

uint32_t premultiplyColor(uint32_t c)
{
    return SkPremultiplyARGBInline(SkGetPackedA32(c), SkGetPackedR32(c), SkGetPackedG32(c), SkGetPackedB32(c));
}

void verifyFramesMatch(const char* webpFile, const ImageFrame* const a, ImageFrame* const b)
{
    const SkBitmap& bitmapA = a->getSkBitmap();
    const SkBitmap& bitmapB = b->getSkBitmap();
    ASSERT_EQ(bitmapA.width(), bitmapB.width());
    ASSERT_EQ(bitmapA.height(), bitmapB.height());

    int maxDifference = 0;
    for (int y = 0; y < bitmapA.height(); ++y) {
        for (int x = 0; x < bitmapA.width(); ++x) {
            uint32_t colorA = *bitmapA.getAddr32(x, y);
            if (!a->premultiplyAlpha())
                colorA = premultiplyColor(colorA);
            uint32_t colorB = *bitmapB.getAddr32(x, y);
            if (!b->premultiplyAlpha())
                colorB = premultiplyColor(colorB);
            uint8_t* pixelA = reinterpret_cast<uint8_t*>(&colorA);
            uint8_t* pixelB = reinterpret_cast<uint8_t*>(&colorB);
            for (int channel = 0; channel < 4; ++channel) {
                const int difference = abs(pixelA[channel] - pixelB[channel]);
                if (difference > maxDifference)
                    maxDifference = difference;
            }
        }
    }

    // Pre-multiplication could round the RGBA channel values. So, we declare
    // that the frames match if the RGBA channel values differ by at most 2.
    EXPECT_GE(2, maxDifference) << webpFile;
}

// Verify that result of alpha blending is similar for AlphaPremultiplied and AlphaNotPremultiplied cases.
void testAlphaBlending(const char* webpFile)
{
    RefPtr<SharedBuffer> data = readFile(webpFile);
    ASSERT_TRUE(data.get());

    OwnPtr<WEBPImageDecoder> decoderA = createDecoder(ImageSource::AlphaPremultiplied);
    decoderA->setData(data.get(), true);

    OwnPtr<WEBPImageDecoder> decoderB = createDecoder(ImageSource::AlphaNotPremultiplied);
    decoderB->setData(data.get(), true);

    size_t frameCount = decoderA->frameCount();
    ASSERT_EQ(frameCount, decoderB->frameCount());

    for (size_t i = 0; i < frameCount; ++i)
        verifyFramesMatch(webpFile, decoderA->frameBufferAtIndex(i), decoderB->frameBufferAtIndex(i));
}

} // anonymous namespace

TEST(AnimatedWebPTests, uniqueGenerationIDs)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/webp-animated.webp");
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);

    ImageFrame* frame = decoder->frameBufferAtIndex(0);
    uint32_t generationID0 = frame->getSkBitmap().getGenerationID();
    frame = decoder->frameBufferAtIndex(1);
    uint32_t generationID1 = frame->getSkBitmap().getGenerationID();

    EXPECT_TRUE(generationID0 != generationID1);
}

TEST(AnimatedWebPTests, verifyAnimationParametersTransparentImage)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    EXPECT_EQ(cAnimationLoopOnce, decoder->repetitionCount());

    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/webp-animated.webp");
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);

    const int canvasWidth = 11;
    const int canvasHeight = 29;
    const struct AnimParam {
        int xOffset, yOffset, width, height;
        ImageFrame::DisposalMethod disposalMethod;
        ImageFrame::AlphaBlendSource alphaBlendSource;
        unsigned duration;
        bool hasAlpha;
    } frameParameters[] = {
        { 0, 0, 11, 29, ImageFrame::DisposeKeep, ImageFrame::BlendAtopPreviousFrame, 1000u, true },
        { 2, 10, 7, 17, ImageFrame::DisposeKeep, ImageFrame::BlendAtopPreviousFrame, 500u, true },
        { 2, 2, 7, 16, ImageFrame::DisposeKeep, ImageFrame::BlendAtopPreviousFrame, 1000u, true },
    };

    for (size_t i = 0; i < ARRAY_SIZE(frameParameters); ++i) {
        const ImageFrame* const frame = decoder->frameBufferAtIndex(i);
        EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
        EXPECT_EQ(canvasWidth, frame->getSkBitmap().width());
        EXPECT_EQ(canvasHeight, frame->getSkBitmap().height());
        EXPECT_EQ(frameParameters[i].xOffset, frame->originalFrameRect().x());
        EXPECT_EQ(frameParameters[i].yOffset, frame->originalFrameRect().y());
        EXPECT_EQ(frameParameters[i].width, frame->originalFrameRect().width());
        EXPECT_EQ(frameParameters[i].height, frame->originalFrameRect().height());
        EXPECT_EQ(frameParameters[i].disposalMethod, frame->disposalMethod());
        EXPECT_EQ(frameParameters[i].alphaBlendSource, frame->alphaBlendSource());
        EXPECT_EQ(frameParameters[i].duration, frame->duration());
        EXPECT_EQ(frameParameters[i].hasAlpha, frame->hasAlpha());
    }

    EXPECT_EQ(ARRAY_SIZE(frameParameters), decoder->frameCount());
    EXPECT_EQ(cAnimationLoopInfinite, decoder->repetitionCount());
}

TEST(AnimatedWebPTests, verifyAnimationParametersOpaqueFramesTransparentBackground)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    EXPECT_EQ(cAnimationLoopOnce, decoder->repetitionCount());

    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/webp-animated-opaque.webp");
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);

    const int canvasWidth = 94;
    const int canvasHeight = 87;
    const struct AnimParam {
        int xOffset, yOffset, width, height;
        ImageFrame::DisposalMethod disposalMethod;
        ImageFrame::AlphaBlendSource alphaBlendSource;
        unsigned duration;
        bool hasAlpha;
    } frameParameters[] = {
        { 4, 10, 33, 32, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopPreviousFrame, 1000u, true },
        { 34, 30, 33, 32, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopPreviousFrame, 1000u, true },
        { 62, 50, 32, 32, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopPreviousFrame, 1000u, true },
        { 10, 54, 32, 33, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopPreviousFrame, 1000u, true },
    };

    for (size_t i = 0; i < ARRAY_SIZE(frameParameters); ++i) {
        const ImageFrame* const frame = decoder->frameBufferAtIndex(i);
        EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
        EXPECT_EQ(canvasWidth, frame->getSkBitmap().width());
        EXPECT_EQ(canvasHeight, frame->getSkBitmap().height());
        EXPECT_EQ(frameParameters[i].xOffset, frame->originalFrameRect().x());
        EXPECT_EQ(frameParameters[i].yOffset, frame->originalFrameRect().y());
        EXPECT_EQ(frameParameters[i].width, frame->originalFrameRect().width());
        EXPECT_EQ(frameParameters[i].height, frame->originalFrameRect().height());
        EXPECT_EQ(frameParameters[i].disposalMethod, frame->disposalMethod());
        EXPECT_EQ(frameParameters[i].alphaBlendSource, frame->alphaBlendSource());
        EXPECT_EQ(frameParameters[i].duration, frame->duration());
        EXPECT_EQ(frameParameters[i].hasAlpha, frame->hasAlpha());
    }

    EXPECT_EQ(ARRAY_SIZE(frameParameters), decoder->frameCount());
    EXPECT_EQ(cAnimationLoopInfinite, decoder->repetitionCount());
}

TEST(AnimatedWebPTests, verifyAnimationParametersBlendOverwrite)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    EXPECT_EQ(cAnimationLoopOnce, decoder->repetitionCount());

    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/webp-animated-no-blend.webp");
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);

    const int canvasWidth = 94;
    const int canvasHeight = 87;
    const struct AnimParam {
        int xOffset, yOffset, width, height;
        ImageFrame::DisposalMethod disposalMethod;
        ImageFrame::AlphaBlendSource alphaBlendSource;
        unsigned duration;
        bool hasAlpha;
    } frameParameters[] = {
        { 4, 10, 33, 32, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopBgcolor, 1000u, true },
        { 34, 30, 33, 32, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopBgcolor, 1000u, true },
        { 62, 50, 32, 32, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopBgcolor, 1000u, true },
        { 10, 54, 32, 33, ImageFrame::DisposeOverwriteBgcolor, ImageFrame::BlendAtopBgcolor, 1000u, true },
    };

    for (size_t i = 0; i < ARRAY_SIZE(frameParameters); ++i) {
        const ImageFrame* const frame = decoder->frameBufferAtIndex(i);
        EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
        EXPECT_EQ(canvasWidth, frame->getSkBitmap().width());
        EXPECT_EQ(canvasHeight, frame->getSkBitmap().height());
        EXPECT_EQ(frameParameters[i].xOffset, frame->originalFrameRect().x());
        EXPECT_EQ(frameParameters[i].yOffset, frame->originalFrameRect().y());
        EXPECT_EQ(frameParameters[i].width, frame->originalFrameRect().width());
        EXPECT_EQ(frameParameters[i].height, frame->originalFrameRect().height());
        EXPECT_EQ(frameParameters[i].disposalMethod, frame->disposalMethod());
        EXPECT_EQ(frameParameters[i].alphaBlendSource, frame->alphaBlendSource());
        EXPECT_EQ(frameParameters[i].duration, frame->duration());
        EXPECT_EQ(frameParameters[i].hasAlpha, frame->hasAlpha());
    }

    EXPECT_EQ(ARRAY_SIZE(frameParameters), decoder->frameCount());
    EXPECT_EQ(cAnimationLoopInfinite, decoder->repetitionCount());
}

TEST(AnimatedWebPTests, parseAndDecodeByteByByte)
{
    testByteByByteDecode("/LayoutTests/fast/images/resources/webp-animated.webp", 3u, cAnimationLoopInfinite);
    testByteByByteDecode("/LayoutTests/fast/images/resources/webp-animated-icc-xmp.webp", 13u, 31999);
}

TEST(AnimatedWebPTests, invalidImages)
{
    // ANMF chunk size is smaller than ANMF header size.
    testInvalidImage("/LayoutTests/fast/images/resources/invalid-animated-webp.webp", true);
    // One of the frame rectangles extends outside the image boundary.
    testInvalidImage("/LayoutTests/fast/images/resources/invalid-animated-webp3.webp", true);
}

TEST(AnimatedWebPTests, truncatedLastFrame)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/invalid-animated-webp2.webp");
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);

    size_t frameCount = 8;
    EXPECT_EQ(frameCount, decoder->frameCount());
    ImageFrame* frame = decoder->frameBufferAtIndex(0);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
    EXPECT_FALSE(decoder->failed());
    frame = decoder->frameBufferAtIndex(frameCount - 1);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FramePartial, frame->status());
    EXPECT_TRUE(decoder->failed());
    frame = decoder->frameBufferAtIndex(0);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
}

TEST(AnimatedWebPTests, truncatedInBetweenFrame)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> fullData = readFile("/LayoutTests/fast/images/resources/invalid-animated-webp4.webp");
    ASSERT_TRUE(fullData.get());
    RefPtr<SharedBuffer> data = SharedBuffer::create(fullData->data(), fullData->size() - 1);
    decoder->setData(data.get(), false);

    ImageFrame* frame = decoder->frameBufferAtIndex(1);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FrameComplete, frame->status());
    frame = decoder->frameBufferAtIndex(2);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FramePartial, frame->status());
    EXPECT_TRUE(decoder->failed());
}

// Reproduce a crash that used to happen for a specific file with specific sequence of method calls.
TEST(AnimatedWebPTests, reproCrash)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> fullData = readFile("/LayoutTests/fast/images/resources/invalid_vp8_vp8x.webp");
    ASSERT_TRUE(fullData.get());

    // Parse partial data up to which error in bitstream is not detected.
    const size_t partialSize = 32768;
    ASSERT_GT(fullData->size(), partialSize);
    RefPtr<SharedBuffer> data = SharedBuffer::create(fullData->data(), partialSize);
    decoder->setData(data.get(), false);
    EXPECT_EQ(1u, decoder->frameCount());
    ImageFrame* frame = decoder->frameBufferAtIndex(0);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FramePartial, frame->status());
    EXPECT_FALSE(decoder->failed());

    // Parse full data now. The error in bitstream should now be detected.
    decoder->setData(fullData.get(), true);
    EXPECT_EQ(1u, decoder->frameCount());
    frame = decoder->frameBufferAtIndex(0);
    ASSERT_TRUE(frame);
    EXPECT_EQ(ImageFrame::FramePartial, frame->status());
    EXPECT_EQ(cAnimationLoopOnce, decoder->repetitionCount());
    EXPECT_TRUE(decoder->failed());
}

TEST(AnimatedWebPTests, progressiveDecode)
{
    RefPtr<SharedBuffer> fullData = readFile("/LayoutTests/fast/images/resources/webp-animated.webp");
    ASSERT_TRUE(fullData.get());
    const size_t fullLength = fullData->size();

    OwnPtr<WEBPImageDecoder>  decoder;
    ImageFrame* frame;

    Vector<unsigned> truncatedHashes;
    Vector<unsigned> progressiveHashes;

    // Compute hashes when the file is truncated.
    const size_t increment = 1;
    for (size_t i = 1; i <= fullLength; i += increment) {
        decoder = createDecoder();
        RefPtr<SharedBuffer> data = SharedBuffer::create(fullData->data(), i);
        decoder->setData(data.get(), i == fullLength);
        frame = decoder->frameBufferAtIndex(0);
        if (!frame) {
            truncatedHashes.append(0);
            continue;
        }
        truncatedHashes.append(hashSkBitmap(frame->getSkBitmap()));
    }

    // Compute hashes when the file is progressively decoded.
    decoder = createDecoder();
    for (size_t i = 1; i <= fullLength; i += increment) {
        RefPtr<SharedBuffer> data = SharedBuffer::create(fullData->data(), i);
        decoder->setData(data.get(), i == fullLength);
        frame = decoder->frameBufferAtIndex(0);
        if (!frame) {
            progressiveHashes.append(0);
            continue;
        }
        progressiveHashes.append(hashSkBitmap(frame->getSkBitmap()));
    }

    bool match = true;
    for (size_t i = 0; i < truncatedHashes.size(); ++i) {
        if (truncatedHashes[i] != progressiveHashes[i]) {
            match = false;
            break;
        }
    }
    EXPECT_TRUE(match);
}

TEST(AnimatedWebPTests, frameIsCompleteAndDuration)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/webp-animated.webp");
    ASSERT_TRUE(data.get());

    ASSERT_GE(data->size(), 10u);
    RefPtr<SharedBuffer> tempData = SharedBuffer::create(data->data(), data->size() - 10);
    decoder->setData(tempData.get(), false);

    EXPECT_EQ(2u, decoder->frameCount());
    EXPECT_FALSE(decoder->failed());
    EXPECT_TRUE(decoder->frameIsCompleteAtIndex(0));
    EXPECT_EQ(1000, decoder->frameDurationAtIndex(0));
    EXPECT_TRUE(decoder->frameIsCompleteAtIndex(1));
    EXPECT_EQ(500, decoder->frameDurationAtIndex(1));

    decoder->setData(data.get(), true);
    EXPECT_EQ(3u, decoder->frameCount());
    EXPECT_TRUE(decoder->frameIsCompleteAtIndex(0));
    EXPECT_EQ(1000, decoder->frameDurationAtIndex(0));
    EXPECT_TRUE(decoder->frameIsCompleteAtIndex(1));
    EXPECT_EQ(500, decoder->frameDurationAtIndex(1));
    EXPECT_TRUE(decoder->frameIsCompleteAtIndex(2));
    EXPECT_EQ(1000.0, decoder->frameDurationAtIndex(2));
}

TEST(AnimatedWebPTests, updateRequiredPreviousFrameAfterFirstDecode)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    RefPtr<SharedBuffer> fullData = readFile("/LayoutTests/fast/images/resources/webp-animated.webp");
    ASSERT_TRUE(fullData.get());

    // Give it data that is enough to parse but not decode in order to check the status
    // of requiredPreviousFrameIndex before decoding.
    size_t partialSize = 1;
    do {
        RefPtr<SharedBuffer> data = SharedBuffer::create(fullData->data(), partialSize);
        decoder->setData(data.get(), false);
        ++partialSize;
    } while (!decoder->frameCount() || decoder->frameBufferAtIndex(0)->status() == ImageFrame::FrameEmpty);

    EXPECT_EQ(kNotFound, decoder->frameBufferAtIndex(0)->requiredPreviousFrameIndex());
    size_t frameCount = decoder->frameCount();
    for (size_t i = 1; i < frameCount; ++i)
        EXPECT_EQ(i - 1, decoder->frameBufferAtIndex(i)->requiredPreviousFrameIndex());

    decoder->setData(fullData.get(), true);
    for (size_t i = 0; i < frameCount; ++i)
        EXPECT_EQ(kNotFound, decoder->frameBufferAtIndex(i)->requiredPreviousFrameIndex());
}

TEST(AnimatedWebPTests, randomFrameDecode)
{
    testRandomFrameDecode("/LayoutTests/fast/images/resources/webp-animated.webp");
    testRandomFrameDecode("/LayoutTests/fast/images/resources/webp-animated-opaque.webp");
    testRandomFrameDecode("/LayoutTests/fast/images/resources/webp-animated-large.webp");
    testRandomFrameDecode("/LayoutTests/fast/images/resources/webp-animated-icc-xmp.webp");
}

TEST(AnimatedWebPTests, randomDecodeAfterClearFrameBufferCache)
{
    testRandomDecodeAfterClearFrameBufferCache("/LayoutTests/fast/images/resources/webp-animated.webp");
    testRandomDecodeAfterClearFrameBufferCache("/LayoutTests/fast/images/resources/webp-animated-opaque.webp");
    testRandomDecodeAfterClearFrameBufferCache("/LayoutTests/fast/images/resources/webp-animated-large.webp");
    testRandomDecodeAfterClearFrameBufferCache("/LayoutTests/fast/images/resources/webp-animated-icc-xmp.webp");
}

TEST(AnimatedWebPTests, DISABLED_resumePartialDecodeAfterClearFrameBufferCache)
{
    RefPtr<SharedBuffer> fullData = readFile("/LayoutTests/fast/images/resources/webp-animated-large.webp");
    ASSERT_TRUE(fullData.get());
    Vector<unsigned> baselineHashes;
    createDecodingBaseline(fullData.get(), &baselineHashes);
    size_t frameCount = baselineHashes.size();

    OwnPtr<WEBPImageDecoder> decoder = createDecoder();

    // Let frame 0 be partially decoded.
    size_t partialSize = 1;
    do {
        RefPtr<SharedBuffer> data = SharedBuffer::create(fullData->data(), partialSize);
        decoder->setData(data.get(), false);
        ++partialSize;
    } while (!decoder->frameCount() || decoder->frameBufferAtIndex(0)->status() == ImageFrame::FrameEmpty);

    // Skip to the last frame and clear.
    decoder->setData(fullData.get(), true);
    EXPECT_EQ(frameCount, decoder->frameCount());
    ImageFrame* lastFrame = decoder->frameBufferAtIndex(frameCount - 1);
    EXPECT_EQ(baselineHashes[frameCount - 1], hashSkBitmap(lastFrame->getSkBitmap()));
    decoder->clearCacheExceptFrame(kNotFound);

    // Resume decoding of the first frame.
    ImageFrame* firstFrame = decoder->frameBufferAtIndex(0);
    EXPECT_EQ(ImageFrame::FrameComplete, firstFrame->status());
    EXPECT_EQ(baselineHashes[0], hashSkBitmap(firstFrame->getSkBitmap()));
}

TEST(AnimatedWebPTests, decodeAfterReallocatingData)
{
    testDecodeAfterReallocatingData("/LayoutTests/fast/images/resources/webp-animated.webp");
    testDecodeAfterReallocatingData("/LayoutTests/fast/images/resources/webp-animated-icc-xmp.webp");
}

TEST(AnimatedWebPTests, alphaBlending)
{
    testAlphaBlending("/LayoutTests/fast/images/resources/webp-animated.webp");
    testAlphaBlending("/LayoutTests/fast/images/resources/webp-animated-semitransparent1.webp");
    testAlphaBlending("/LayoutTests/fast/images/resources/webp-animated-semitransparent2.webp");
    testAlphaBlending("/LayoutTests/fast/images/resources/webp-animated-semitransparent3.webp");
    testAlphaBlending("/LayoutTests/fast/images/resources/webp-animated-semitransparent4.webp");
}

TEST(AnimatedWebPTests, isSizeAvailable)
{
    testByteByByteSizeAvailable("/LayoutTests/fast/images/resources/webp-animated.webp", 142u, false, cAnimationLoopInfinite);
    // FIXME: Add color profile support for animated webp images.
    testByteByByteSizeAvailable("/LayoutTests/fast/images/resources/webp-animated-icc-xmp.webp", 1404u, false, 31999);
}

TEST(StaticWebPTests, truncatedImage)
{
    // VP8 data is truncated.
    testInvalidImage("/LayoutTests/fast/images/resources/truncated.webp", false);
    // Chunk size in RIFF header doesn't match the file size.
    testInvalidImage("/LayoutTests/fast/images/resources/truncated2.webp", true);
}

TEST(StaticWebPTests, incrementalDecode)
{
    // Regression test for a bug where some valid images were failing to decode incrementally.
    testByteByByteDecode("/LayoutTests/fast/images/resources/crbug.364830.webp", 1u, cAnimationNone);
}

TEST(StaticWebPTests, isSizeAvailable)
{
    testByteByByteSizeAvailable("/LayoutTests/fast/images/resources/webp-color-profile-lossy.webp", 520u, true, cAnimationNone);
    testByteByByteSizeAvailable("/LayoutTests/fast/images/resources/test.webp", 30u, false, cAnimationNone);
}

TEST(StaticWebPTests, notAnimated)
{
    OwnPtr<WEBPImageDecoder> decoder = createDecoder();
    RefPtr<SharedBuffer> data = readFile("/LayoutTests/fast/images/resources/webp-color-profile-lossy.webp");
    ASSERT_TRUE(data.get());
    decoder->setData(data.get(), true);
    EXPECT_EQ(1u, decoder->frameCount());
    EXPECT_EQ(cAnimationNone, decoder->repetitionCount());
}

} // namespace blink
