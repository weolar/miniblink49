// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/proto/gfx_conversions.h"

#include "cc/proto/point.pb.h"
#include "cc/proto/pointf.pb.h"
#include "cc/proto/rect.pb.h"
#include "cc/proto/rectf.pb.h"
#include "cc/proto/size.pb.h"
#include "cc/proto/sizef.pb.h"
#include "cc/proto/transform.pb.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/point_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/size_f.h"
#include "ui/gfx/transform.h"

namespace cc {
namespace {

    TEST(GfxProtoConversionsTest, IntSerializationLimits)
    {
        // Test Point with the minimum int value.
        {
            gfx::Point point(std::numeric_limits<int>::min(),
                std::numeric_limits<int>::min());
            proto::Point proto;
            PointToProto(point, &proto);
            EXPECT_EQ(point, ProtoToPoint(proto));
        }

        // Test Point with the maximum int value.
        {
            gfx::Point point(std::numeric_limits<int>::max(),
                std::numeric_limits<int>::max());
            proto::Point proto;
            PointToProto(point, &proto);
            EXPECT_EQ(point, ProtoToPoint(proto));
        }

        // Test Size with the minimum int value.
        {
            gfx::Size size(std::numeric_limits<int>::min(),
                std::numeric_limits<int>::min());
            proto::Size proto;
            SizeToProto(size, &proto);
            EXPECT_EQ(size, ProtoToSize(proto));
        }

        // Test Size with the maximum int value.
        {
            gfx::Size size(std::numeric_limits<int>::max(),
                std::numeric_limits<int>::max());
            proto::Size proto;
            SizeToProto(size, &proto);
            EXPECT_EQ(size, ProtoToSize(proto));
        }
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializePoint)
    {
        const gfx::Point point(5, 10);

        // Test PointToProto
        proto::Point proto;
        PointToProto(point, &proto);
        EXPECT_EQ(point.x(), proto.x());
        EXPECT_EQ(point.y(), proto.y());

        // Test protoToPoint
        EXPECT_EQ(point, ProtoToPoint(proto));
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializePointF)
    {
        const gfx::PointF point(5.1f, 10.2f);

        // Test PointFToProto
        proto::PointF proto;
        PointFToProto(point, &proto);
        EXPECT_EQ(point.x(), proto.x());
        EXPECT_EQ(point.y(), proto.y());

        // Test ProtoToPointF
        EXPECT_EQ(point, ProtoToPointF(proto));
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializeSize)
    {
        const gfx::Size size(5, 10);

        // Test SizeToProto
        proto::Size proto;
        SizeToProto(size, &proto);
        EXPECT_EQ(size.width(), proto.width());
        EXPECT_EQ(size.height(), proto.height());

        // Test ProtoToSize
        EXPECT_EQ(size, ProtoToSize(proto));
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializeSizeF)
    {
        const gfx::SizeF size(5.1f, 10.2f);

        // Test SizeFToProto
        proto::SizeF proto;
        SizeFToProto(size, &proto);
        EXPECT_EQ(size.width(), proto.width());
        EXPECT_EQ(size.height(), proto.height());

        // Test ProtoToSizeF
        EXPECT_EQ(size, ProtoToSizeF(proto));
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializeRect)
    {
        const gfx::Rect rect(1, 2, 3, 4);

        // Test RectToProto
        proto::Rect proto;
        RectToProto(rect, &proto);
        EXPECT_EQ(rect.origin().x(), proto.origin().x());
        EXPECT_EQ(rect.origin().y(), proto.origin().y());
        EXPECT_EQ(rect.size().width(), proto.size().width());
        EXPECT_EQ(rect.size().height(), proto.size().height());

        // Test ProtoToRect
        EXPECT_EQ(rect, ProtoToRect(proto));
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializeRectF)
    {
        const gfx::RectF rect(1.4f, 2.3f, 3.2f, 4.1f);

        // Test RectFToProto
        proto::RectF proto;
        RectFToProto(rect, &proto);
        EXPECT_EQ(rect.origin().x(), proto.origin().x());
        EXPECT_EQ(rect.origin().y(), proto.origin().y());
        EXPECT_EQ(rect.size().width(), proto.size().width());
        EXPECT_EQ(rect.size().height(), proto.size().height());

        // Test ProtoToRectF
        EXPECT_EQ(rect, ProtoToRectF(proto));
    }

    TEST(GfxProtoConversionsTest, SerializeDeserializeTransform)
    {
        gfx::Transform transform(1.16f, 2.15f, 3.14f, 4.13f, 5.12f, 6.11f, 7.1f, 8.9f,
            9.8f, 10.7f, 11.6f, 12.5f, 13.4f, 14.3f, 15.2f,
            16.1f);

        // Test TransformToProto
        proto::Transform proto;
        TransformToProto(transform, &proto);
        EXPECT_EQ(16, proto.matrix_size());
        EXPECT_EQ(transform.matrix().get(0, 0), proto.matrix(0));
        EXPECT_EQ(transform.matrix().get(0, 1), proto.matrix(1));
        EXPECT_EQ(transform.matrix().get(0, 2), proto.matrix(2));
        EXPECT_EQ(transform.matrix().get(0, 3), proto.matrix(3));
        EXPECT_EQ(transform.matrix().get(1, 0), proto.matrix(4));
        EXPECT_EQ(transform.matrix().get(1, 1), proto.matrix(5));
        EXPECT_EQ(transform.matrix().get(1, 2), proto.matrix(6));
        EXPECT_EQ(transform.matrix().get(1, 3), proto.matrix(7));
        EXPECT_EQ(transform.matrix().get(2, 0), proto.matrix(8));
        EXPECT_EQ(transform.matrix().get(2, 1), proto.matrix(9));
        EXPECT_EQ(transform.matrix().get(2, 2), proto.matrix(10));
        EXPECT_EQ(transform.matrix().get(2, 3), proto.matrix(11));
        EXPECT_EQ(transform.matrix().get(3, 0), proto.matrix(12));
        EXPECT_EQ(transform.matrix().get(3, 1), proto.matrix(13));
        EXPECT_EQ(transform.matrix().get(3, 2), proto.matrix(14));
        EXPECT_EQ(transform.matrix().get(3, 3), proto.matrix(15));

        // Test ProtoToTransform
        EXPECT_EQ(transform, ProtoToTransform(proto));
    }

} // namespace
} // namespace cc
