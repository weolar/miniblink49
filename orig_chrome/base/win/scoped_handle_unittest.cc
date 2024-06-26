// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/win/scoped_handle.h"

#include "testing/gtest/include/gtest/gtest.h"

TEST(ScopedHandleTest, ScopedHandle)
{
    // Any illegal error code will do. We just need to test that it is preserved
    // by ScopedHandle to avoid bug 528394.
    const DWORD magic_error = 0x12345678;

    HANDLE handle = ::CreateMutex(nullptr, FALSE, nullptr);
    // Call SetLastError after creating the handle.
    ::SetLastError(magic_error);
    base::win::ScopedHandle handle_holder(handle);
    EXPECT_EQ(magic_error, ::GetLastError());

    // Create a new handle and then set LastError again.
    handle = ::CreateMutex(nullptr, FALSE, nullptr);
    ::SetLastError(magic_error);
    handle_holder.Set(handle);
    EXPECT_EQ(magic_error, ::GetLastError());

    // Create a new handle and then set LastError again.
    handle = ::CreateMutex(nullptr, FALSE, nullptr);
    base::win::ScopedHandle handle_source(handle);
    ::SetLastError(magic_error);
    handle_holder = handle_source.Pass();
    EXPECT_EQ(magic_error, ::GetLastError());
}
