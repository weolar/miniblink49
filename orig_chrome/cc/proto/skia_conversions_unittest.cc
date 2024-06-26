// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/proto/skia_conversions.h"

#include "cc/proto/skregion.pb.h"
#include "cc/proto/skrrect.pb.h"
#include "cc/proto/skxfermode.pb.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkXfermode.h"

namespace cc {
namespace {

    TEST(SkiaProtoConversionsTest, SerializeDeserializeSkRegionOp)
    {
        for (size_t i = 0; i < SkRegion::Op::kLastOp; i++) {
            SkRegion::Op op = static_cast<SkRegion::Op>(i);
            EXPECT_EQ(op, SkRegionOpFromProto(SkRegionOpToProto(op)));
        }
    }

    TEST(SkiaProtoConversionsTest, SerializeDeserializeSkXfermodeMode)
    {
        for (size_t i = 0; i < SkXfermode::Mode::kLastMode; i++) {
            SkXfermode::Mode mode = static_cast<SkXfermode::Mode>(i);
            EXPECT_EQ(mode, SkXfermodeModeFromProto(SkXfermodeModeToProto(mode)));
        }
    }

    TEST(SkiaProtoConversionsTest, SerializeDeserializeSkRRect)
    {
        SkRect rect = SkRect::MakeXYWH(0, 10, 15, 20);
        SkVector radii[4];
        radii[0] = SkVector::Make(0.1f, 0.2f);
        radii[1] = SkVector::Make(0.3f, 0.4f);
        radii[2] = SkVector::Make(0.5f, 0.6f);
        radii[3] = SkVector::Make(0.7f, 0.8f);

        SkRRect rrect;
        rrect.setRectRadii(rect, radii);

        // Test RRectToProto
        proto::SkRRect proto;
        SkRRectToProto(rrect, &proto);
        EXPECT_EQ(rrect.rect().x(), proto.rect().origin().x());
        EXPECT_EQ(rrect.rect().y(), proto.rect().origin().y());
        EXPECT_EQ(rrect.rect().width(), proto.rect().size().width());
        EXPECT_EQ(rrect.rect().height(), proto.rect().size().height());

        EXPECT_EQ(rrect.radii(SkRRect::kUpperLeft_Corner).x(),
            proto.radii_upper_left().x());
        EXPECT_EQ(rrect.radii(SkRRect::kUpperLeft_Corner).y(),
            proto.radii_upper_left().y());
        EXPECT_EQ(rrect.radii(SkRRect::kUpperRight_Corner).x(),
            proto.radii_upper_right().x());
        EXPECT_EQ(rrect.radii(SkRRect::kUpperRight_Corner).y(),
            proto.radii_upper_right().y());
        EXPECT_EQ(rrect.radii(SkRRect::kLowerRight_Corner).x(),
            proto.radii_lower_right().x());
        EXPECT_EQ(rrect.radii(SkRRect::kLowerRight_Corner).y(),
            proto.radii_lower_right().y());
        EXPECT_EQ(rrect.radii(SkRRect::kLowerLeft_Corner).x(),
            proto.radii_lower_left().x());
        EXPECT_EQ(rrect.radii(SkRRect::kLowerLeft_Corner).y(),
            proto.radii_lower_left().y());

        // Test ProtoToRRect
        EXPECT_EQ(rrect, ProtoToSkRRect(proto));
    }

} // namespace
} // namespace cc
