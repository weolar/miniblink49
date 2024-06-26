// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/cc_test_suite.h"

#include "base/message_loop/message_loop.h"
#include "base/threading/thread_id_name_manager.h"
#include "cc/test/paths.h"
#include "ui/gl/test/gl_surface_test_support.h"

namespace cc {

CCTestSuite::CCTestSuite(int argc, char** argv)
    : base::TestSuite(argc, argv)
{
}

CCTestSuite::~CCTestSuite() { }

void CCTestSuite::Initialize()
{
    base::TestSuite::Initialize();
    gfx::GLSurfaceTestSupport::InitializeOneOff();
    CCPaths::RegisterPathProvider();

    message_loop_.reset(new base::MessageLoop);

    base::ThreadIdNameManager::GetInstance()->SetName(
        base::PlatformThread::CurrentId(),
        "Main");

    base::DiscardableMemoryAllocator::SetInstance(&discardable_memory_allocator_);
}

void CCTestSuite::Shutdown()
{
    message_loop_ = nullptr;

    base::TestSuite::Shutdown();
}

} // namespace cc
