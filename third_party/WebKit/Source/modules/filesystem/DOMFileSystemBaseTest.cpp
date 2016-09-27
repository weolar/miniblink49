// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/filesystem/DOMFileSystemBase.h"

#include "core/fileapi/File.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include <gtest/gtest.h>


namespace blink {

class DOMFileSystemBaseTest : public ::testing::Test {
public:
    DOMFileSystemBaseTest()
    {
        m_filePath = Platform::current()->unitTestSupport()->webKitRootDir();
        m_filePath.append("/Source/modules/filesystem/DOMFileSystemBaseTest.cpp");
        getFileMetadata(m_filePath, m_fileMetadata);
        m_fileMetadata.platformPath = m_filePath;
    }

    String encodeFilePathAsURIComponent(const String& fullPath)
    {
        return DOMFileSystemBase::encodeFilePathAsURIComponent(fullPath);
    }

protected:
    String m_filePath;
    FileMetadata m_fileMetadata;
};


TEST_F(DOMFileSystemBaseTest, externalFilesystemFilesAreUserVisible)
{
    KURL rootUrl = DOMFileSystemBase::createFileSystemRootURL("http://chromium.org/", FileSystemTypeExternal);

    File* file = DOMFileSystemBase::createFile(m_fileMetadata, rootUrl, FileSystemTypeExternal, "DOMFileSystemBaseTest.cpp");
    EXPECT_TRUE(file);
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ(File::IsUserVisible, file->userVisibility());
    EXPECT_EQ("DOMFileSystemBaseTest.cpp", file->name());
    EXPECT_EQ(m_filePath, file->path());
}

TEST_F(DOMFileSystemBaseTest, temporaryFilesystemFilesAreNotUserVisible)
{
    KURL rootUrl = DOMFileSystemBase::createFileSystemRootURL("http://chromium.org/", FileSystemTypeTemporary);

    File* file = DOMFileSystemBase::createFile(m_fileMetadata, rootUrl, FileSystemTypeTemporary, "UserVisibleName.txt");
    EXPECT_TRUE(file);
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ(File::IsNotUserVisible, file->userVisibility());
    EXPECT_EQ("UserVisibleName.txt", file->name());
    EXPECT_EQ(m_filePath, file->path());
}

TEST_F(DOMFileSystemBaseTest, persistentFilesystemFilesAreNotUserVisible)
{
    KURL rootUrl = DOMFileSystemBase::createFileSystemRootURL("http://chromium.org/", FileSystemTypePersistent);

    File* file = DOMFileSystemBase::createFile(m_fileMetadata, rootUrl, FileSystemTypePersistent, "UserVisibleName.txt");
    EXPECT_TRUE(file);
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ(File::IsNotUserVisible, file->userVisibility());
    EXPECT_EQ("UserVisibleName.txt", file->name());
    EXPECT_EQ(m_filePath, file->path());
}

TEST_F(DOMFileSystemBaseTest, encodeFilePathAsURIComponent)
{
    // Contain a combining character ('e' + COMBINING OGONEK).
    String combining(String::fromUTF8("\x65\xCC\xA8"));

    // encodeWithURLEscapeSequences should make the string NFC-normalized, but
    // encodeFilePathAsURIComponent should not.
    EXPECT_EQ(encodeWithURLEscapeSequences(combining), "%C4%99");
    EXPECT_EQ(encodeFilePathAsURIComponent(combining), "e%CC%A8");

    // Contain a precomposed character corresponding to |combining|.
    String precomposed(String::fromUTF8("\xC4\x99"));
    EXPECT_EQ(encodeWithURLEscapeSequences(precomposed), "%C4%99");
    EXPECT_EQ(encodeFilePathAsURIComponent(precomposed), "%C4%99");

    // Following tests ensure that encodeFilePathAsURIComponent stays in sync
    // with KURL's encodeWithURLEscapeSequences. Test cases are copied from
    // KURLTest::Encode.

    const char* inputs[] = {
        "hello, world",
        "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F",
        "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F",
        " !\"#$%&'()*+,-./",
        "0123456789:;<=>?",
        "@ABCDEFGHIJKLMNO",
        "PQRSTUVWXYZ[\\]^_",
        "`abcdefghijklmno",
        "pqrstuvwxyz{|}~\x7f",
    };

    for (size_t i = 0; i < arraysize(inputs); i++) {
        EXPECT_EQ(encodeWithURLEscapeSequences(inputs[i]), encodeFilePathAsURIComponent(inputs[i]));
    }

    // Our encode escapes NULLs for safety, so we need to check that too.
    WTF::String input("\x00\x01", 2);
    EXPECT_EQ(encodeWithURLEscapeSequences(input), encodeFilePathAsURIComponent(input));

    // Also test that it gets converted to UTF-8 properly.
    UChar wideInputHelper[3] = { 0x4f60, 0x597d, 0 };
    WTF::String wideInput(reinterpret_cast<const ::UChar*>(wideInputHelper), 2);
    EXPECT_EQ(encodeWithURLEscapeSequences(wideInput), encodeFilePathAsURIComponent(wideInput));
}

} // namespace blink
