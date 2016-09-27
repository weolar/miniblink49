// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "FileInputType.h"

#include "core/clipboard/DataObject.h"
#include "core/dom/Document.h"
#include "core/fileapi/FileList.h"
#include "core/html/HTMLInputElement.h"
#include "core/page/DragData.h"
#include "wtf/DateMath.h"
#include <gtest/gtest.h>

namespace blink {

TEST(FileInputTypeTest, createFileList)
{
    Vector<FileChooserFileInfo> files;

    // Native file.
    files.append(FileChooserFileInfo("/native/path/native-file", "display-name"));

    // Non-native file.
    KURL url(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/non-native-file");
    FileMetadata metadata;
    metadata.length = 64;
    metadata.modificationTime = 1.0 * msPerDay + 3;
    files.append(FileChooserFileInfo(url, metadata));


    FileList* list = FileInputType::createFileList(files, false);
    ASSERT_TRUE(list);
    ASSERT_EQ(2u, list->length());

    EXPECT_EQ("/native/path/native-file", list->item(0)->path());
    EXPECT_EQ("display-name", list->item(0)->name());
    EXPECT_TRUE(list->item(0)->fileSystemURL().isEmpty());

    EXPECT_TRUE(list->item(1)->path().isEmpty());
    EXPECT_EQ("non-native-file", list->item(1)->name());
    EXPECT_EQ(url, list->item(1)->fileSystemURL());
    EXPECT_EQ(64u, list->item(1)->size());
    EXPECT_EQ(1.0 * msPerDay + 3, list->item(1)->lastModified());
}

TEST(FileInputTypeTest, ignoreDroppedNonNativeFiles)
{
    const RefPtrWillBeRawPtr<Document> document = Document::create();
    const RefPtrWillBeRawPtr<HTMLInputElement> input =
        HTMLInputElement::create(*document, nullptr, false);
    const RefPtrWillBeRawPtr<InputType> fileInput = FileInputType::create(*input);

    DataObject* nativeFileRawDragData = DataObject::create();
    const DragData nativeFileDragData(nativeFileRawDragData, IntPoint(), IntPoint(), DragOperationCopy);
    nativeFileDragData.platformData()->add(File::create("/native/path"));
    nativeFileDragData.platformData()->setFilesystemId("fileSystemId");
    fileInput->receiveDroppedFiles(&nativeFileDragData);
    EXPECT_EQ("fileSystemId", fileInput->droppedFileSystemId());
    ASSERT_EQ(1u, fileInput->files()->length());
    EXPECT_EQ(String("/native/path"), fileInput->files()->item(0)->path());

    DataObject* nonNativeFileRawDragData = DataObject::create();
    const DragData nonNativeFileDragData(nonNativeFileRawDragData, IntPoint(), IntPoint(), DragOperationCopy);
    FileMetadata metadata;
    metadata.length = 1234;
    const KURL url(ParsedURLStringTag(), "filesystem:http://example.com/isolated/hash/non-native-file");
    nonNativeFileDragData.platformData()->add(File::createForFileSystemFile(url, metadata, File::IsUserVisible));
    nonNativeFileDragData.platformData()->setFilesystemId("fileSystemId");
    fileInput->receiveDroppedFiles(&nonNativeFileDragData);
    // Dropping non-native files should not change the existing files.
    EXPECT_EQ("fileSystemId", fileInput->droppedFileSystemId());
    ASSERT_EQ(1u, fileInput->files()->length());
    EXPECT_EQ(String("/native/path"), fileInput->files()->item(0)->path());
}

} // namespace blink
