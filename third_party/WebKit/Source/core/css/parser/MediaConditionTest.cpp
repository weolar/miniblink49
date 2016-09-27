// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/css/MediaList.h"
#include "core/css/MediaQuery.h"
#include "core/css/parser/CSSTokenizer.h"
#include "core/css/parser/MediaQueryParser.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/StringBuilder.h"

#include <gtest/gtest.h>

namespace blink {

typedef struct {
    const char* input;
    const char* output;
} TestCase;

TEST(MediaConditionParserTest, Basic)
{
    // The first string represents the input string.
    // The second string represents the output string, if present.
    // Otherwise, the output string is identical to the first string.
    TestCase testCases[] = {
        {"screen", "not all"},
        {"screen and (color)", "not all"},
        {"all and (min-width:500px)", "not all"},
        {"(min-width:500px)", "(min-width: 500px)"},
        {"(min-width: -100px)", "not all"},
        {"(min-width: 100px) and print", "not all"},
        {"(min-width: 100px) and (max-width: 900px)", "(max-width: 900px) and (min-width: 100px)"},
        {"(min-width: [100px) and (max-width: 900px)", "not all"},
        {"not (min-width: 900px)", "not all and (min-width: 900px)"},
        {"not (blabla)", "not all"},
        {0, 0} // Do not remove the terminator line.
    };

    // FIXME: We should test comma-seperated media conditions
    for (unsigned i = 0; testCases[i].input; ++i) {
        CSSTokenizer::Scope scope(testCases[i].input);
        RefPtrWillBeRawPtr<MediaQuerySet> mediaConditionQuerySet = MediaQueryParser::parseMediaCondition(scope.tokenRange());
        ASSERT_EQ(mediaConditionQuerySet->queryVector().size(), (unsigned)1);
        String queryText = mediaConditionQuerySet->queryVector()[0]->cssText();
        ASSERT_STREQ(testCases[i].output, queryText.ascii().data());
    }
}

} // namespace
