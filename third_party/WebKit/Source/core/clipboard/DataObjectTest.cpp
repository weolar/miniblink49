// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/clipboard/DataObject.h"

#include "core/clipboard/DataObjectItem.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include <gtest/gtest.h>

namespace blink {

class DataObjectTest : public ::testing::Test {
public:
    DataObjectTest()
        : m_dataObject(DataObject::create())
    {
    }

protected:
    Persistent<DataObject> m_dataObject;
};

TEST_F(DataObjectTest, addItemWithFilenameAndNoTitle)
{
    String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
    filePath.append("/Source/core/clipboard/DataObjectTest.cpp");

    m_dataObject->addFilename(filePath, String());
    EXPECT_EQ(1U, m_dataObject->length());

    DataObjectItem* item = m_dataObject->item(0);
    EXPECT_EQ(DataObjectItem::FileKind, item->kind());

    Blob* blob = item->getAsFile();
    ASSERT_TRUE(blob->isFile());
    File* file = toFile(blob);
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ(File::IsUserVisible, file->userVisibility());
    EXPECT_EQ(filePath, file->path());
}

TEST_F(DataObjectTest, addItemWithFilenameAndTitle)
{
    String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
    filePath.append("/Source/core/clipboard/DataObjectTest.cpp");

    m_dataObject->addFilename(filePath, "name.cpp");
    EXPECT_EQ(1U, m_dataObject->length());

    DataObjectItem* item = m_dataObject->item(0);
    EXPECT_EQ(DataObjectItem::FileKind, item->kind());

    Blob* blob = item->getAsFile();
    ASSERT_TRUE(blob->isFile());
    File* file = toFile(blob);
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ(File::IsUserVisible, file->userVisibility());
    EXPECT_EQ(filePath, file->path());
    EXPECT_EQ("name.cpp", file->name());
}

} // namespace blink
