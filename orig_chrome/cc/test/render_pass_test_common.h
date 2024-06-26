// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_RENDER_PASS_TEST_COMMON_H_
#define CC_TEST_RENDER_PASS_TEST_COMMON_H_

#include "cc/quads/render_pass.h"

namespace cc {
class ResourceProvider;
}

namespace cc {

class TestRenderPass : public RenderPass {
public:
    static scoped_ptr<TestRenderPass> Create()
    {
        return make_scoped_ptr(new TestRenderPass);
    }

    static const unsigned int kSyncPointForMailboxTextureQuad = 30;

    void AppendOneOfEveryQuadType(ResourceProvider* resource_provider,
        RenderPassId child_pass);

protected:
    TestRenderPass()
        : RenderPass()
    {
    }
};

} // namespace cc

#endif // CC_TEST_RENDER_PASS_TEST_COMMON_H_
