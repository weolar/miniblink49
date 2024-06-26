// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_FAKE_RENDERER_CLIENT_H_
#define CC_TEST_FAKE_RENDERER_CLIENT_H_

#include "cc/output/renderer.h"

namespace cc {

class FakeRendererClient : public RendererClient {
public:
    FakeRendererClient();

    // RendererClient methods.
    void SetFullRootLayerDamage() override;

    // Methods added for test.
    int set_full_root_layer_damage_count() const
    {
        return set_full_root_layer_damage_count_;
    }

private:
    int set_full_root_layer_damage_count_;
};

} // namespace cc

#endif // CC_TEST_FAKE_RENDERER_CLIENT_H_
