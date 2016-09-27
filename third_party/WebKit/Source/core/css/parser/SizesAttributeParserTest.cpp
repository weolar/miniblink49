// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/SizesAttributeParser.h"

#include "core/MediaTypeNames.h"
#include "core/css/MediaValuesCached.h"

#include <gtest/gtest.h>

namespace blink {

typedef struct {
    const char* input;
    const float effectiveSize;
} TestCase;

TEST(SizesAttributeParserTest, Basic)
{
    TestCase testCases[] = {
        {"screen", 500},
        {"(min-width:500px)", 500},
        {"(min-width:500px) 200px", 200},
        {"(min-width:500px) 50vw", 250},
        {"(min-width:500px) 200px, 400px", 200},
        {"400px, (min-width:500px) 200px", 400},
        {"40vw, (min-width:500px) 201px", 200},
        {"(min-width:500px) 201px, 40vw", 201},
        {"(min-width:5000px) 40vw, 201px", 201},
        {"(min-width:500px) calc(201px), calc(40vw)", 201},
        {"(min-width:5000px) calc(40vw), calc(201px)", 201},
        {"(min-width:5000px) 200px, 400px", 400},
        {"(blalbadfsdf) 200px, 400px", 400},
        {"0", 0},
        {"-0", 0},
        {"1", 500},
        {"300px, 400px", 300},
        {"(min-width:5000px) 200px, (min-width:500px) 400px", 400},
        {"", 500},
        {"  ", 500},
        {" /**/ ", 500},
        {" /**/ 300px", 300},
        {"300px /**/ ", 300},
        {" /**/ (min-width:500px) /**/ 300px", 300},
        {"-100px, 200px", 200},
        {"-50vw, 20vw", 100},
        {"50asdf, 200px", 200},
        {"asdf, 200px", 200},
        {"(max-width: 3000px) 200w, 400w", 500},
        {",, , /**/ ,200px", 200},
        {"50vw", 250},
        {"50vh", 300},
        {"50vmin", 250},
        {"50vmax", 300},
        {"5em", 80},
        {"5rem", 80},
        {"calc(40vw*2)", 400},
        {"(min-width:5000px) calc(5000px/10), (min-width:500px) calc(1200px/3)", 400},
        {"(min-width:500px) calc(1200/3)", 500},
        {"(min-width:500px) calc(1200px/(0px*14))", 500},
        {"(max-width: 3000px) 200px, 400px", 200},
        {"(max-width: 3000px) 20em, 40em", 320},
        {"(max-width: 3000px) 0, 40em", 0},
        {"(max-width: 3000px) 0px, 40em", 0},
        {"(max-width: 3000px) 50vw, 40em", 250},
        {"(max-width: 3000px) 50px, 40vw", 50},
        {"((),1px", 500},
        {"{{},1px", 500},
        {"[[],1px", 500},
        {"x(x(),1px", 500},
        {"(max-width: 3000px) 50.5px, 40vw", 50.5},
        {"not (blabla) 50px, 40vw", 200},
        {"not (max-width: 100px) 50px, 40vw", 50},
        {0, 0} // Do not remove the terminator line.
    };

    MediaValuesCached::MediaValuesCachedData data;
    data.viewportWidth = 500;
    data.viewportHeight = 600;
    data.deviceWidth = 500;
    data.deviceHeight = 500;
    data.devicePixelRatio = 2.0;
    data.colorBitsPerComponent = 24;
    data.monochromeBitsPerComponent = 0;
    data.primaryPointerType = PointerTypeFine;
    data.defaultFontSize = 16;
    data.threeDEnabled = true;
    data.mediaType = MediaTypeNames::screen;
    data.strictMode = true;
    data.displayMode = WebDisplayModeBrowser;
    RefPtr<MediaValues> mediaValues = MediaValuesCached::create(data);

    for (unsigned i = 0; testCases[i].input; ++i) {
        SizesAttributeParser parser(mediaValues, testCases[i].input);
        ASSERT_EQ(testCases[i].effectiveSize, parser.length());
    }
}

} // namespace
