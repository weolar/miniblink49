// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/test/cc_blink_test_suite.h"

#include "base/message_loop/message_loop.h"
#include "base/threading/thread_id_name_manager.h"

namespace cc_blink {

CCBlinkTestSuite::CCBlinkTestSuite(int argc, char** argv)
    : base::TestSuite(argc, argv)
{
}

CCBlinkTestSuite::~CCBlinkTestSuite() { }

void CCBlinkTestSuite::Initialize()
{
    base::TestSuite::Initialize();

    message_loop_.reset(new base::MessageLoop);

    base::ThreadIdNameManager::GetInstance()->SetName(
        base::PlatformThread::CurrentId(),
        "Main");
}

void CCBlinkTestSuite::Shutdown()
{
    message_loop_ = nullptr;

    base::TestSuite::Shutdown();
}

} // namespace cc_blink
