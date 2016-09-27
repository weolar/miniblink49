// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/MediaQueryEvaluator.h"

#include "core/MediaTypeNames.h"
#include "core/css/MediaList.h"
#include "core/css/MediaValuesCached.h"
#include "core/frame/FrameView.h"
#include "core/testing/DummyPageHolder.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/StringBuilder.h"

#include <gtest/gtest.h>

namespace blink {

typedef struct {
    const char* input;
    const bool output;
} TestCase;

TestCase screenTestCases[] = {
    {"", 1},
    {" ", 1},
    {"screen", 1},
    {"screen and (color)", 1},
    {"not screen and (color)", 0},
    {"screen and (device-aspect-ratio: 16/9)", 0},
    {"screen and (device-aspect-ratio: 1/1)", 1},
    {"all and (min-color: 2)", 1},
    {"all and (min-color: 32)", 0},
    {"all and (min-color-index: 0)", 1},
    {"all and (min-color-index: 1)", 0},
    {"all and (monochrome)", 0},
    {"all and (min-monochrome: 0)", 1},
    {"all and (grid: 0)", 1},
    {"(resolution: 2dppx)", 1},
    {"(resolution: 1dppx)", 0},
    {"(orientation: portrait)", 1},
    {"(orientation: landscape)", 0},
    {"(orientation: url(portrait))", 0},
    {"(orientation: #portrait)", 0},
    {"(orientation: @portrait)", 0},
    {"(orientation: 'portrait')", 0},
    {"(orientation: @junk portrait)", 0},
    {"screen and (orientation: @portrait) and (max-width: 1000px)", 0},
    {"screen and (orientation: @portrait), (max-width: 1000px)", 1},
    {"tv and (scan: progressive)", 0},
    {"(pointer: coarse)", 0},
    {"(pointer: fine)", 1},
    {"(hover: hover)", 1},
    {"(hover: on-demand)", 0},
    {"(hover: none)", 0},
    {"(display-mode)", 0},
    {"(display-mode: fullscreen)", 0},
    {"(display-mode: standalone)", 0},
    {"(display-mode: minimal-ui)", 0},
    {"(display-mode: browser)", 1},
    {"(display-mode: min-browser)", 0},
    {"(display-mode: url(browser))", 0},
    {"(display-mode: #browser)", 0},
    {"(display-mode: @browser)", 0},
    {"(display-mode: 'browser')", 0},
    {"(display-mode: @junk browser)", 0},
    {0, 0} // Do not remove the terminator line.
};

TestCase viewportTestCases[] = {
    {"all and (min-width: 500px)", 1},
    {"(min-width: 500px)", 1},
    {"(min-width: 501px)", 0},
    {"(max-width: 500px)", 1},
    {"(max-width: 499px)", 0},
    {"(width: 500px)", 1},
    {"(width: 501px)", 0},
    {"(min-height: 500px)", 1},
    {"(min-height: 501px)", 0},
    {"(min-height: 500.001px)", 0},
    {"(max-height: 500px)", 1},
    {"(max-height: 499.999px)", 0},
    {"(max-height: 499px)", 0},
    {"(height: 500px)", 1},
    {"(height: 500.001px)", 0},
    {"(height: 499.999px)", 0},
    {"(height: 501px)", 0},
    {"(height)", 1},
    {"(width)", 1},
    {"(width: whatisthis)", 0},
    {"screen and (min-width: 400px) and (max-width: 700px)", 1},
    {0, 0} // Do not remove the terminator line.
};

TestCase printTestCases[] = {
    {"print and (min-resolution: 1dppx)", 1},
    {"print and (min-resolution: 118dpcm)", 1},
    {"print and (min-resolution: 119dpcm)", 0},
    {0, 0} // Do not remove the terminator line.
};

void testMQEvaluator(TestCase* testCases, const MediaQueryEvaluator& mediaQueryEvaluator)
{
    RefPtrWillBePersistent<MediaQuerySet> querySet = nullptr;
    for (unsigned i = 0; testCases[i].input; ++i) {
        querySet = MediaQuerySet::create(testCases[i].input);
        ASSERT_EQ(testCases[i].output, mediaQueryEvaluator.eval(querySet.get()));
    }
}

TEST(MediaQueryEvaluatorTest, Cached)
{
    MediaValuesCached::MediaValuesCachedData data;
    data.viewportWidth = 500;
    data.viewportHeight = 500;
    data.deviceWidth = 500;
    data.deviceHeight = 500;
    data.devicePixelRatio = 2.0;
    data.colorBitsPerComponent = 24;
    data.monochromeBitsPerComponent = 0;
    data.primaryPointerType = PointerTypeFine;
    data.primaryHoverType = HoverTypeHover;
    data.defaultFontSize = 16;
    data.threeDEnabled = true;
    data.mediaType = MediaTypeNames::screen;
    data.strictMode = true;
    data.displayMode = WebDisplayModeBrowser;
    RefPtr<MediaValues> mediaValues = MediaValuesCached::create(data);

    MediaQueryEvaluator mediaQueryEvaluator(*mediaValues);
    testMQEvaluator(screenTestCases, mediaQueryEvaluator);
    testMQEvaluator(viewportTestCases, mediaQueryEvaluator);

    data.mediaType = MediaTypeNames::print;
    mediaValues = MediaValuesCached::create(data);
    MediaQueryEvaluator printMediaQueryEvaluator(*mediaValues);
    testMQEvaluator(printTestCases, printMediaQueryEvaluator);
}

TEST(MediaQueryEvaluatorTest, Dynamic)
{
    OwnPtr<DummyPageHolder> pageHolder = DummyPageHolder::create(IntSize(500, 500));
    pageHolder->frameView().setMediaType(MediaTypeNames::screen);

    MediaQueryEvaluator mediaQueryEvaluator(&pageHolder->frame());
    testMQEvaluator(viewportTestCases, mediaQueryEvaluator);
    pageHolder->frameView().setMediaType(MediaTypeNames::print);
    testMQEvaluator(printTestCases, mediaQueryEvaluator);
}

TEST(MediaQueryEvaluatorTest, DynamicNoView)
{
    OwnPtr<DummyPageHolder> pageHolder = DummyPageHolder::create(IntSize(500, 500));
    RefPtrWillBeRawPtr<LocalFrame> frame = &pageHolder->frame();
    pageHolder.clear();
    ASSERT_EQ(nullptr, frame->view());
    MediaQueryEvaluator mediaQueryEvaluator(frame.get());
    RefPtrWillBeRawPtr<MediaQuerySet> querySet = MediaQuerySet::create("foobar");
    EXPECT_FALSE(mediaQueryEvaluator.eval(querySet.get()));
}

} // namespace
