// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/fake_renderer_client.h"

namespace cc {

FakeRendererClient::FakeRendererClient()
    : set_full_root_layer_damage_count_(0)
{
}

void FakeRendererClient::SetFullRootLayerDamage()
{
    ++set_full_root_layer_damage_count_;
}

} // namespace cc
