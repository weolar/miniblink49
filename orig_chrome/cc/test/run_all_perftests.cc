// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/test/launcher/unit_test_launcher.h"
#include "base/test/test_suite.h"
#include "cc/test/cc_test_suite.h"

int main(int argc, char** argv)
{
    cc::CCTestSuite test_suite(argc, argv);

    // Always run the perf tests serially, to avoid distorting
    // perf measurements with randomness resulting from running
    // in parallel.
    return base::LaunchUnitTestsSerially(
        argc, argv, base::Bind(&cc::CCTestSuite::Run, base::Unretained(&test_suite)));
}
