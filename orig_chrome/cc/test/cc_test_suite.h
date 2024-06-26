// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_CC_TEST_SUITE_H_
#define CC_TEST_CC_TEST_SUITE_H_

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/test/test_discardable_memory_allocator.h"
#include "base/test/test_suite.h"

namespace base {
class MessageLoop;
}

namespace cc {

class CCTestSuite : public base::TestSuite {
public:
    CCTestSuite(int argc, char** argv);
    ~CCTestSuite() override;

protected:
    // Overridden from base::TestSuite:
    void Initialize() override;
    void Shutdown() override;

private:
    scoped_ptr<base::MessageLoop> message_loop_;

    base::TestDiscardableMemoryAllocator discardable_memory_allocator_;
    DISALLOW_COPY_AND_ASSIGN(CCTestSuite);
};

} // namespace cc

#endif // CC_TEST_CC_TEST_SUITE_H_
