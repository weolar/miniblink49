// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/TreeScopeStyleSheetCollection.h"

#include "core/css/CSSStyleSheet.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/parser/CSSParserMode.h"
#include <gtest/gtest.h>

namespace blink {

class TreeScopeStyleSheetCollectionTest : public testing::Test {
protected:
    using SheetVector = WillBeHeapVector<RefPtrWillBeMember<CSSStyleSheet>>;
    using ContentsVector = WillBeHeapVector<RawPtrWillBeMember<StyleSheetContents>>;

    enum UpdateType {
        Reconstruct = TreeScopeStyleSheetCollection::Reconstruct,
        Reset = TreeScopeStyleSheetCollection::Reset,
        Additive = TreeScopeStyleSheetCollection::Additive
    };

    static PassRefPtrWillBeRawPtr<CSSStyleSheet> createSheet()
    {
        return CSSStyleSheet::create(StyleSheetContents::create(CSSParserContext(HTMLStandardMode, nullptr)));
    }

    static void compareStyleSheets(const SheetVector& oldStyleSheets, const SheetVector& newStyleSheets, const ContentsVector& expAddedSheets, UpdateType testUpdateType)
    {
        ContentsVector addedSheets;
        UpdateType updateType = static_cast<UpdateType>(TreeScopeStyleSheetCollection::compareStyleSheets(oldStyleSheets, newStyleSheets, addedSheets));
        EXPECT_EQ(testUpdateType, updateType);
        EXPECT_EQ(expAddedSheets.size(), addedSheets.size());
        if (expAddedSheets.size() == addedSheets.size()) {
            for (unsigned i = 0; i < addedSheets.size(); i++)
                EXPECT_EQ(expAddedSheets.at(i), addedSheets.at(i));
        }
    }
};

TEST_F(TreeScopeStyleSheetCollectionTest, CompareStyleSheetsAppend)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet1 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet2 = createSheet();

    ContentsVector added;
    SheetVector previous;
    SheetVector current;

    previous.append(sheet1);

    current.append(sheet1);
    current.append(sheet2);

    added.append(sheet2->contents());

    compareStyleSheets(previous, current, added, Additive);
}

TEST_F(TreeScopeStyleSheetCollectionTest, CompareStyleSheetsPrepend)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet1 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet2 = createSheet();

    ContentsVector added;
    SheetVector previous;
    SheetVector current;

    previous.append(sheet2);

    current.append(sheet1);
    current.append(sheet2);

    added.append(sheet1->contents());

    compareStyleSheets(previous, current, added, Reconstruct);
}

TEST_F(TreeScopeStyleSheetCollectionTest, CompareStyleSheetsInsert)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet1 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet2 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet3 = createSheet();

    ContentsVector added;
    SheetVector previous;
    SheetVector current;

    previous.append(sheet1);
    previous.append(sheet3);

    current.append(sheet1);
    current.append(sheet2);
    current.append(sheet3);

    added.append(sheet2->contents());

    compareStyleSheets(previous, current, added, Reconstruct);
}

TEST_F(TreeScopeStyleSheetCollectionTest, CompareStyleSheetsRemove)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet1 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet2 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet3 = createSheet();

    ContentsVector added;
    SheetVector previous;
    SheetVector current;

    previous.append(sheet1);
    previous.append(sheet2);
    previous.append(sheet3);

    current.append(sheet1);
    current.append(sheet3);

    added.append(sheet2->contents());

    // This is really the same as Insert. TreeScopeStyleSheetCollection::compareStyleSheets
    // will assert if you pass an array that is longer as the first parameter.
    compareStyleSheets(current, previous, added, Reconstruct);
}

TEST_F(TreeScopeStyleSheetCollectionTest, CompareStyleSheetsInsertRemove)
{
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet1 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet2 = createSheet();
    RefPtrWillBeRawPtr<CSSStyleSheet> sheet3 = createSheet();

    ContentsVector added;
    SheetVector previous;
    SheetVector current;

    previous.append(sheet1);
    previous.append(sheet2);

    current.append(sheet2);
    current.append(sheet3);

    // TODO(rune@opera.com): This is clearly wrong. We add sheet3 and remove sheet1 and
    // compareStyleSheets returns sheet2 and sheet3 as added (crbug/475858).
    added.append(sheet2->contents());
    added.append(sheet3->contents());

    compareStyleSheets(previous, current, added, Reconstruct);
}

}
