// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/proto/skia_conversions.h"

#include "base/logging.h"
#include "cc/proto/gfx_conversions.h"
#include "cc/proto/skrrect.pb.h"
#include "third_party/skia/include/core/SkRRect.h"
#include "ui/gfx/skia_util.h"

namespace cc {

namespace {

    void SkPointToProto(const SkPoint& point, proto::PointF* proto)
    {
        PointFToProto(gfx::PointF(point.x(), point.y()), proto);
    }

    SkPoint ProtoToSkPoint(const proto::PointF& proto)
    {
        gfx::PointF point = ProtoToPointF(proto);
        return SkPoint::Make(point.x(), point.y());
    }

} // namespace

SkRegion::Op SkRegionOpFromProto(proto::SkRegion::Op op)
{
    switch (op) {
    case proto::SkRegion::Op_Difference:
        return SkRegion::Op::kDifference_Op;
    case proto::SkRegion::Op_Intersect:
        return SkRegion::Op::kIntersect_Op;
    case proto::SkRegion::Op_Union:
        return SkRegion::Op::kUnion_Op;
    case proto::SkRegion::Op_XOR:
        return SkRegion::Op::kXOR_Op;
    case proto::SkRegion::Op_ReverseDifference:
        return SkRegion::Op::kReverseDifference_Op;
    case proto::SkRegion::Op_Replace:
        return SkRegion::Op::kReplace_Op;
    }
    return SkRegion::Op::kDifference_Op;
}

proto::SkRegion::Op SkRegionOpToProto(SkRegion::Op op)
{
    switch (op) {
    case SkRegion::Op::kDifference_Op:
        return proto::SkRegion::Op_Difference;
    case SkRegion::Op::kIntersect_Op:
        return proto::SkRegion::Op_Intersect;
    case SkRegion::Op::kUnion_Op:
        return proto::SkRegion::Op_Union;
    case SkRegion::Op::kXOR_Op:
        return proto::SkRegion::Op_XOR;
    case SkRegion::Op::kReverseDifference_Op:
        return proto::SkRegion::Op_ReverseDifference;
    case SkRegion::Op::kReplace_Op:
        return proto::SkRegion::Op_Replace;
    }
    return proto::SkRegion::Op_Difference;
}

SkXfermode::Mode SkXfermodeModeFromProto(proto::SkXfermode::Mode mode)
{
    switch (mode) {
    case proto::SkXfermode::Mode_Clear:
        return SkXfermode::Mode::kClear_Mode;
    case proto::SkXfermode::Mode_Src:
        return SkXfermode::Mode::kSrc_Mode;
    case proto::SkXfermode::Mode_Dst:
        return SkXfermode::Mode::kDst_Mode;
    case proto::SkXfermode::Mode_SrcOver:
        return SkXfermode::Mode::kSrcOver_Mode;
    case proto::SkXfermode::Mode_DstOver:
        return SkXfermode::Mode::kDstOver_Mode;
    case proto::SkXfermode::Mode_SrcIn:
        return SkXfermode::Mode::kSrcIn_Mode;
    case proto::SkXfermode::Mode_DstIn:
        return SkXfermode::Mode::kDstIn_Mode;
    case proto::SkXfermode::Mode_SrcOut:
        return SkXfermode::Mode::kSrcOut_Mode;
    case proto::SkXfermode::Mode_DstOut:
        return SkXfermode::Mode::kDstOut_Mode;
    case proto::SkXfermode::Mode_SrcATop:
        return SkXfermode::Mode::kSrcATop_Mode;
    case proto::SkXfermode::Mode_DstATop:
        return SkXfermode::Mode::kDstATop_Mode;
    case proto::SkXfermode::Mode_XOR:
        return SkXfermode::Mode::kXor_Mode;
    case proto::SkXfermode::Mode_Plus:
        return SkXfermode::Mode::kPlus_Mode;
    case proto::SkXfermode::Mode_Modulate:
        return SkXfermode::Mode::kModulate_Mode;
    case proto::SkXfermode::Mode_Screen:
        return SkXfermode::Mode::kScreen_Mode;
    case proto::SkXfermode::Mode_Overlay:
        return SkXfermode::Mode::kOverlay_Mode;
    case proto::SkXfermode::Mode_Darken:
        return SkXfermode::Mode::kDarken_Mode;
    case proto::SkXfermode::Mode_Lighten:
        return SkXfermode::Mode::kLighten_Mode;
    case proto::SkXfermode::Mode_ColorDodge:
        return SkXfermode::Mode::kColorDodge_Mode;
    case proto::SkXfermode::Mode_ColorBurn:
        return SkXfermode::Mode::kColorBurn_Mode;
    case proto::SkXfermode::Mode_HardLight:
        return SkXfermode::Mode::kHardLight_Mode;
    case proto::SkXfermode::Mode_SoftLight:
        return SkXfermode::Mode::kSoftLight_Mode;
    case proto::SkXfermode::Mode_Difference:
        return SkXfermode::Mode::kDifference_Mode;
    case proto::SkXfermode::Mode_Exclusion:
        return SkXfermode::Mode::kExclusion_Mode;
    case proto::SkXfermode::Mode_Multiply:
        return SkXfermode::Mode::kMultiply_Mode;
    case proto::SkXfermode::Mode_Hue:
        return SkXfermode::Mode::kHue_Mode;
    case proto::SkXfermode::Mode_Saturation:
        return SkXfermode::Mode::kSaturation_Mode;
    case proto::SkXfermode::Mode_Color:
        return SkXfermode::Mode::kColor_Mode;
    case proto::SkXfermode::Mode_Luminosity:
        return SkXfermode::Mode::kLuminosity_Mode;
    }
    return SkXfermode::Mode::kClear_Mode;
}

proto::SkXfermode::Mode SkXfermodeModeToProto(SkXfermode::Mode mode)
{
    switch (mode) {
    case SkXfermode::Mode::kClear_Mode:
        return proto::SkXfermode::Mode_Clear;
    case SkXfermode::Mode::kSrc_Mode:
        return proto::SkXfermode::Mode_Src;
    case SkXfermode::Mode::kDst_Mode:
        return proto::SkXfermode::Mode_Dst;
    case SkXfermode::Mode::kSrcOver_Mode:
        return proto::SkXfermode::Mode_SrcOver;
    case SkXfermode::Mode::kDstOver_Mode:
        return proto::SkXfermode::Mode_DstOver;
    case SkXfermode::Mode::kSrcIn_Mode:
        return proto::SkXfermode::Mode_SrcIn;
    case SkXfermode::Mode::kDstIn_Mode:
        return proto::SkXfermode::Mode_DstIn;
    case SkXfermode::Mode::kSrcOut_Mode:
        return proto::SkXfermode::Mode_SrcOut;
    case SkXfermode::Mode::kDstOut_Mode:
        return proto::SkXfermode::Mode_DstOut;
    case SkXfermode::Mode::kSrcATop_Mode:
        return proto::SkXfermode::Mode_SrcATop;
    case SkXfermode::Mode::kDstATop_Mode:
        return proto::SkXfermode::Mode_DstATop;
    case SkXfermode::Mode::kXor_Mode:
        return proto::SkXfermode::Mode_XOR;
    case SkXfermode::Mode::kPlus_Mode:
        return proto::SkXfermode::Mode_Plus;
    case SkXfermode::Mode::kModulate_Mode:
        return proto::SkXfermode::Mode_Modulate;
    case SkXfermode::Mode::kScreen_Mode:
        return proto::SkXfermode::Mode_Screen;
    case SkXfermode::Mode::kOverlay_Mode:
        return proto::SkXfermode::Mode_Overlay;
    case SkXfermode::Mode::kDarken_Mode:
        return proto::SkXfermode::Mode_Darken;
    case SkXfermode::Mode::kLighten_Mode:
        return proto::SkXfermode::Mode_Lighten;
    case SkXfermode::Mode::kColorDodge_Mode:
        return proto::SkXfermode::Mode_ColorDodge;
    case SkXfermode::Mode::kColorBurn_Mode:
        return proto::SkXfermode::Mode_ColorBurn;
    case SkXfermode::Mode::kHardLight_Mode:
        return proto::SkXfermode::Mode_HardLight;
    case SkXfermode::Mode::kSoftLight_Mode:
        return proto::SkXfermode::Mode_SoftLight;
    case SkXfermode::Mode::kDifference_Mode:
        return proto::SkXfermode::Mode_Difference;
    case SkXfermode::Mode::kExclusion_Mode:
        return proto::SkXfermode::Mode_Exclusion;
    case SkXfermode::Mode::kMultiply_Mode:
        return proto::SkXfermode::Mode_Multiply;
    case SkXfermode::Mode::kHue_Mode:
        return proto::SkXfermode::Mode_Hue;
    case SkXfermode::Mode::kSaturation_Mode:
        return proto::SkXfermode::Mode_Saturation;
    case SkXfermode::Mode::kColor_Mode:
        return proto::SkXfermode::Mode_Color;
    case SkXfermode::Mode::kLuminosity_Mode:
        return proto::SkXfermode::Mode_Luminosity;
    }
    return proto::SkXfermode::Mode_Clear;
}

void SkRRectToProto(const SkRRect& rect, proto::SkRRect* proto)
{
    RectFToProto(gfx::SkRectToRectF(rect.rect()), proto->mutable_rect());

    SkPointToProto(rect.radii(SkRRect::kUpperLeft_Corner),
        proto->mutable_radii_upper_left());
    SkPointToProto(rect.radii(SkRRect::kUpperRight_Corner),
        proto->mutable_radii_upper_right());
    SkPointToProto(rect.radii(SkRRect::kLowerRight_Corner),
        proto->mutable_radii_lower_right());
    SkPointToProto(rect.radii(SkRRect::kLowerLeft_Corner),
        proto->mutable_radii_lower_left());
}

SkRRect ProtoToSkRRect(const proto::SkRRect& proto)
{
    SkRect parsed_rect = gfx::RectFToSkRect(ProtoToRectF(proto.rect()));
    SkVector parsed_radii[4];
    parsed_radii[SkRRect::kUpperLeft_Corner] = ProtoToSkPoint(proto.radii_upper_left());
    parsed_radii[SkRRect::kUpperRight_Corner] = ProtoToSkPoint(proto.radii_upper_right());
    parsed_radii[SkRRect::kLowerRight_Corner] = ProtoToSkPoint(proto.radii_lower_right());
    parsed_radii[SkRRect::kLowerLeft_Corner] = ProtoToSkPoint(proto.radii_lower_left());
    SkRRect rect;
    rect.setRectRadii(parsed_rect, parsed_radii);
    return rect;
}

} // namespace cc
