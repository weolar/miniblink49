// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_TEST_CC_BLINK_TEST_SUITE_H_
#define CC_BLINK_TEST_CC_BLINK_TEST_SUITE_H_

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/test/test_suite.h"

namespace base {
class MessageLoop;
}

namespace cc_blink {

class CCBlinkTestSuite : public base::TestSuite {
public:
    CCBlinkTestSuite(int argc, char** argv);
    ~CCBlinkTestSuite() override;

protected:
    // Overridden from base::TestSuite:
    void Initialize() override;
    void Shutdown() override;

private:
    scoped_ptr<base::MessageLoop> message_loop_;

    DISALLOW_COPY_AND_ASSIGN(CCBlinkTestSuite);
};

} // namespace cc_blink

#endif // CC_BLINK_TEST_CC_BLINK_TEST_SUITE_H_
