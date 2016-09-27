// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "File.h"

#include <gtest/gtest.h>

namespace blink {

TEST(FileTest, nativeFile)
{
    File* const file = File::create("/native/path");
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ("/native/path", file->path());
    EXPECT_TRUE(file->fileSystemURL().isEmpty());
}

TEST(FileTest, blobBackingFile)
{
    const RefPtr<BlobDataHandle> blobDataHandle = BlobDataHandle::create();
    File* const file = File::create("name", 0.0, blobDataHandle);
    EXPECT_FALSE(file->hasBackingFile());
    EXPECT_TRUE(file->path().isEmpty());
    EXPECT_TRUE(file->fileSystemURL().isEmpty());
}

TEST(FileTest, fileSystemFileWithNativeSnapshot)
{
    FileMetadata metadata;
    metadata.platformPath = "/native/snapshot";
    File* const file = File::createForFileSystemFile("name", metadata, File::IsUserVisible);
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ("/native/snapshot", file->path());
    EXPECT_TRUE(file->fileSystemURL().isEmpty());
}

TEST(FileTest, fileSystemFileWithoutNativeSnapshot)
{
    KURL url(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/non-native-file");
    FileMetadata metadata;
    File* const file = File::createForFileSystemFile(url, metadata, File::IsUserVisible);
    EXPECT_FALSE(file->hasBackingFile());
    EXPECT_TRUE(file->path().isEmpty());
    EXPECT_EQ(url, file->fileSystemURL());
}

TEST(FileTest, hsaSameSource)
{
    File* const nativeFileA1 = File::create("/native/pathA");
    File* const nativeFileA2 = File::create("/native/pathA");
    File* const nativeFileB = File::create("/native/pathB");

    const RefPtr<BlobDataHandle> blobDataA = BlobDataHandle::create();
    const RefPtr<BlobDataHandle> blobDataB = BlobDataHandle::create();
    File* const blobFileA1 = File::create("name", 0.0, blobDataA);
    File* const blobFileA2 = File::create("name", 0.0, blobDataA);
    File* const blobFileB = File::create("name", 0.0, blobDataB);

    KURL urlA(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/non-native-file-A");
    KURL urlB(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/non-native-file-B");
    FileMetadata metadata;
    File* const fileSystemFileA1 = File::createForFileSystemFile(urlA, metadata, File::IsUserVisible);
    File* const fileSystemFileA2 = File::createForFileSystemFile(urlA, metadata, File::IsUserVisible);
    File* const fileSystemFileB = File::createForFileSystemFile(urlB, metadata, File::IsUserVisible);

    EXPECT_FALSE(nativeFileA1->hasSameSource(*blobFileA1));
    EXPECT_FALSE(blobFileA1->hasSameSource(*fileSystemFileA1));
    EXPECT_FALSE(fileSystemFileA1->hasSameSource(*nativeFileA1));

    EXPECT_TRUE(nativeFileA1->hasSameSource(*nativeFileA1));
    EXPECT_TRUE(nativeFileA1->hasSameSource(*nativeFileA2));
    EXPECT_FALSE(nativeFileA1->hasSameSource(*nativeFileB));

    EXPECT_TRUE(blobFileA1->hasSameSource(*blobFileA1));
    EXPECT_TRUE(blobFileA1->hasSameSource(*blobFileA2));
    EXPECT_FALSE(blobFileA1->hasSameSource(*blobFileB));

    EXPECT_TRUE(fileSystemFileA1->hasSameSource(*fileSystemFileA1));
    EXPECT_TRUE(fileSystemFileA1->hasSameSource(*fileSystemFileA2));
    EXPECT_FALSE(fileSystemFileA1->hasSameSource(*fileSystemFileB));
}

} // namespace blink
