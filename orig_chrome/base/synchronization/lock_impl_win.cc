// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/synchronization/lock_impl.h"

namespace base {
namespace internal {

    LockImpl::LockImpl()
    {
        // The second parameter is the spin count, for short-held locks it avoid the
        // contending thread from going to sleep which helps performance greatly.
        ::InitializeCriticalSectionAndSpinCount((CRITICAL_SECTION*)&native_handle_, 2000);
    }

    LockImpl::~LockImpl()
    {
        ::DeleteCriticalSection((CRITICAL_SECTION*)&native_handle_);
    }

    bool LockImpl::Try()
    {
        if (::TryEnterCriticalSection((CRITICAL_SECTION*)&native_handle_) != FALSE) {
            return true;
        }
        return false;
    }

    void LockImpl::Lock()
    {
        ::EnterCriticalSection((CRITICAL_SECTION*)&native_handle_);
    }

    void LockImpl::Unlock()
    {
        ::LeaveCriticalSection((CRITICAL_SECTION*)&native_handle_);
    }

} // namespace internal
} // namespace base
