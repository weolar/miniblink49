// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/network/FormData.h"

#include <gtest/gtest.h>

namespace blink {

namespace {

class FormDataTest : public ::testing::Test {
public:
    void checkDeepCopied(const String& a, const String& b)
    {
        EXPECT_EQ(a, b);
        if (b.impl())
            EXPECT_NE(a.impl(), b.impl());
    }

    void checkDeepCopied(const KURL& a, const KURL& b)
    {
        EXPECT_EQ(a, b);
        checkDeepCopied(a.string(), b.string());
        if (a.innerURL() && b.innerURL())
            checkDeepCopied(*a.innerURL(), *b.innerURL());
    }

    void checkDeepCopied(const FormDataElement& a, const FormDataElement& b)
    {
        EXPECT_EQ(a, b);
        checkDeepCopied(a.m_filename, b.m_filename);
        checkDeepCopied(a.m_blobUUID, b.m_blobUUID);
        checkDeepCopied(a.m_fileSystemURL, b.m_fileSystemURL);
    }
};

TEST_F(FormDataTest, DeepCopy)
{
    RefPtr<FormData> original(FormData::create());
    original->appendData("Foo", 3);
    original->appendFileRange("example.txt", 12345, 56789, 9999.0);
    original->appendBlob("originalUUID", nullptr);
    original->appendFileSystemURLRange(KURL(KURL(), "ws://localhost/"), 23456, 34567, 1111.0);

    Vector<char> boundaryVector;
    boundaryVector.append("----boundaryForTest", 19);
    original->setIdentifier(45678);
    original->setBoundary(boundaryVector);
    original->setContainsPasswordData(true);

    RefPtr<FormData> copy = original->deepCopy();

    // Check that contents are copied (compare the copy with expected values).
    const Vector<FormDataElement>& originalElements = original->elements();
    const Vector<FormDataElement>& copyElements = copy->elements();
    ASSERT_EQ(4ul, copyElements.size());

    Vector<char> fooVector;
    fooVector.append("Foo", 3);

    EXPECT_EQ(FormDataElement::data, copyElements[0].m_type);
    EXPECT_EQ(fooVector, copyElements[0].m_data);

    EXPECT_EQ(FormDataElement::encodedFile, copyElements[1].m_type);
    EXPECT_EQ(String("example.txt"), copyElements[1].m_filename);
    EXPECT_EQ(12345ll, copyElements[1].m_fileStart);
    EXPECT_EQ(56789ll, copyElements[1].m_fileLength);
    EXPECT_EQ(9999.0, copyElements[1].m_expectedFileModificationTime);

    EXPECT_EQ(FormDataElement::encodedBlob, copyElements[2].m_type);
    EXPECT_EQ(String("originalUUID"), copyElements[2].m_blobUUID);

    EXPECT_EQ(FormDataElement::encodedFileSystemURL, copyElements[3].m_type);
    EXPECT_EQ(KURL(KURL(), String("ws://localhost/")), copyElements[3].m_fileSystemURL);
    EXPECT_EQ(23456ll, copyElements[3].m_fileStart);
    EXPECT_EQ(34567ll, copyElements[3].m_fileLength);
    EXPECT_EQ(1111.0, copyElements[3].m_expectedFileModificationTime);

    EXPECT_EQ(45678, copy->identifier());
    EXPECT_EQ(boundaryVector, copy->boundary());
    EXPECT_EQ(true, copy->containsPasswordData());

    // Check that contents are copied (compare the copy with the original).
    EXPECT_EQ(*original, *copy);

    // Check pointers are different, i.e. deep-copied.
    ASSERT_NE(original.get(), copy.get());

    for (size_t i = 0; i < 4; ++i) {
        if (copyElements[i].m_filename.impl()) {
            EXPECT_NE(originalElements[i].m_filename.impl(), copyElements[i].m_filename.impl());
            EXPECT_TRUE(copyElements[i].m_filename.impl()->hasOneRef());
        }

        if (copyElements[i].m_blobUUID.impl()) {
            EXPECT_NE(originalElements[i].m_blobUUID.impl(), copyElements[i].m_blobUUID.impl());
            EXPECT_TRUE(copyElements[i].m_blobUUID.impl()->hasOneRef());
        }

        if (copyElements[i].m_fileSystemURL.string().impl()) {
            EXPECT_NE(originalElements[i].m_fileSystemURL.string().impl(), copyElements[i].m_fileSystemURL.string().impl());
            EXPECT_TRUE(copyElements[i].m_fileSystemURL.string().impl()->hasOneRef());
        }

        EXPECT_EQ(nullptr, copyElements[i].m_fileSystemURL.innerURL());

        // m_optionalBlobDataHandle is not checked, because BlobDataHandle is ThreadSafeRefCounted.
    }
}

} // namespace

} // namespace blink
