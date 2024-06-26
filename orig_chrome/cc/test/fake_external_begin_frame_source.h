// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_EXTERNAL_BEGIN_FRAME_SOURCE_H_
#define CC_TEST_FAKE_EXTERNAL_BEGIN_FRAME_SOURCE_H_

#include "base/memory/weak_ptr.h"
#include "cc/scheduler/begin_frame_source.h"

namespace cc {

class FakeExternalBeginFrameSource
    : public BeginFrameSourceBase,
      public NON_EXPORTED_BASE(base::NonThreadSafe) {
public:
    explicit FakeExternalBeginFrameSource(double refresh_rate);
    ~FakeExternalBeginFrameSource() override;

    bool is_ready() const { return is_ready_; }

    // BeginFrameSource implementation.
    void SetClientReady() override;

    // BeginFrameSourceBase overrides.
    void OnNeedsBeginFramesChange(bool needs_begin_frames) override;

    void TestOnBeginFrame();
    void PostTestOnBeginFrame();

private:
    double milliseconds_per_frame_;
    bool is_ready_;
    base::WeakPtrFactory<FakeExternalBeginFrameSource> weak_ptr_factory_;
};

} // namespace cc

#endif // CC_TEST_FAKE_EXTERNAL_BEGIN_FRAME_SOURCE_H_
