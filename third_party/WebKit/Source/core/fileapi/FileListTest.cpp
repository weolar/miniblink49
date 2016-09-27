// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "FileList.h"

#include <gtest/gtest.h>

namespace blink {

TEST(FileListTest, pathsForUserVisibleFiles)
{
    FileList* const fileList = FileList::create();

    // Native file.
    fileList->append(File::create("/native/path"));

    // Blob file.
    const RefPtr<BlobDataHandle> blobDataHandle = BlobDataHandle::create();
    fileList->append(File::create("name", 0.0, blobDataHandle));

    // User visible snapshot file.
    {
        FileMetadata metadata;
        metadata.platformPath = "/native/visible/snapshot";
        fileList->append(File::createForFileSystemFile("name", metadata, File::IsUserVisible));
    }

    // Not user visible snapshot file.
    {
        FileMetadata metadata;
        metadata.platformPath = "/native/not-visible/snapshot";
        fileList->append(File::createForFileSystemFile("name", metadata, File::IsNotUserVisible));
    }

    // User visible file system URL file.
    {
        KURL url(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/visible-non-native-file");
        fileList->append(File::createForFileSystemFile(url, FileMetadata(), File::IsUserVisible));
    }

    // Not user visible file system URL file.
    {
        KURL url(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/not-visible-non-native-file");
        fileList->append(File::createForFileSystemFile(url, FileMetadata(), File::IsNotUserVisible));
    }

    Vector<String> paths = fileList->pathsForUserVisibleFiles();

    ASSERT_EQ(3u, paths.size());
    EXPECT_EQ("/native/path", paths[0]);
    EXPECT_EQ("/native/visible/snapshot", paths[1]);
    EXPECT_EQ("visible-non-native-file", paths[2]) << "Files not backed by a native file should return name.";
}

} // namespace blink
