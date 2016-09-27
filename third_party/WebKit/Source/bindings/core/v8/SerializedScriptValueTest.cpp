// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/SerializedScriptValue.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/SerializedScriptValueFactory.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "bindings/core/v8/V8File.h"
#include "core/fileapi/File.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include <gtest/gtest.h>

namespace blink {

class SerializedScriptValueTest : public ::testing::Test {
public:
    SerializedScriptValueTest()
        : m_scope(v8::Isolate::GetCurrent())
    {
    }

    v8::Isolate* isolate() const { return m_scope.isolate(); }
    v8::Local<v8::Object> creationContext() const { return m_scope.scriptState()->context()->Global(); }

protected:
    V8TestingScope m_scope;
};

TEST_F(SerializedScriptValueTest, UserSelectedFile)
{
    String filePath = Platform::current()->unitTestSupport()->webKitRootDir();
    filePath.append("/Source/bindings/core/v8/SerializedScriptValueTest.cpp");
    File* originalFile = File::create(filePath);
    ASSERT_TRUE(originalFile->hasBackingFile());
    ASSERT_EQ(File::IsUserVisible, originalFile->userVisibility());
    ASSERT_EQ(filePath, originalFile->path());

    v8::Local<v8::Value> v8OriginalFile = toV8(originalFile, creationContext(), isolate());
    RefPtr<SerializedScriptValue> serializedScriptValue =
    SerializedScriptValueFactory::instance().create(isolate(), v8OriginalFile, nullptr, nullptr, ASSERT_NO_EXCEPTION);
    v8::Local<v8::Value> v8File = serializedScriptValue->deserialize(isolate());

    ASSERT_TRUE(V8File::hasInstance(v8File, isolate()));
    File* file = V8File::toImpl(v8::Local<v8::Object>::Cast(v8File));
    EXPECT_TRUE(file->hasBackingFile());
    EXPECT_EQ(File::IsUserVisible, file->userVisibility());
    EXPECT_EQ(filePath, file->path());
}

TEST_F(SerializedScriptValueTest, FileConstructorFile)
{
    RefPtr<BlobDataHandle> blobDataHandle = BlobDataHandle::create();
    File* originalFile = File::create("hello.txt", 12345678.0, blobDataHandle);
    ASSERT_FALSE(originalFile->hasBackingFile());
    ASSERT_EQ(File::IsNotUserVisible, originalFile->userVisibility());
    ASSERT_EQ("hello.txt", originalFile->name());

    v8::Local<v8::Value> v8OriginalFile = toV8(originalFile, creationContext(), isolate());
    RefPtr<SerializedScriptValue> serializedScriptValue =
    SerializedScriptValueFactory::instance().create(isolate(), v8OriginalFile, nullptr, nullptr, ASSERT_NO_EXCEPTION);
    v8::Local<v8::Value> v8File = serializedScriptValue->deserialize(isolate());

    ASSERT_TRUE(V8File::hasInstance(v8File, isolate()));
    File* file = V8File::toImpl(v8::Local<v8::Object>::Cast(v8File));
    EXPECT_FALSE(file->hasBackingFile());
    EXPECT_EQ(File::IsNotUserVisible, file->userVisibility());
    EXPECT_EQ("hello.txt", file->name());
}

} // namespace blink
