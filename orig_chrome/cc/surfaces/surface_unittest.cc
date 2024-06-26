// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/surfaces/surface.h"
#include "cc/surfaces/surface_factory.h"
#include "cc/surfaces/surface_manager.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/size.h"

namespace cc {
namespace {

    TEST(SurfaceTest, SurfaceLifetime)
    {
        SurfaceManager manager;
        SurfaceFactory factory(&manager, NULL);

        SurfaceId surface_id(6);
        {
            factory.Create(surface_id);
            EXPECT_TRUE(manager.GetSurfaceForId(surface_id));
            factory.Destroy(surface_id);
        }

        EXPECT_EQ(NULL, manager.GetSurfaceForId(surface_id));
    }

} // namespace
} // namespace cc
