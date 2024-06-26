// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PROTO_GFX_CONVERSIONS_H_
#define CC_PROTO_GFX_CONVERSIONS_H_

#include "cc/base/cc_export.h"

namespace gfx {
class Point;
class PointF;
class Rect;
class RectF;
class Size;
class SizeF;
class Transform;
}

namespace cc {

namespace proto {
    class Point;
    class PointF;
    class Rect;
    class RectF;
    class Size;
    class SizeF;
    class Transform;
}

// TODO(dtrainor): Move these to a class and make them static
// (crbug.com/548432).
CC_EXPORT void PointToProto(const gfx::Point& point, proto::Point* proto);
CC_EXPORT gfx::Point ProtoToPoint(const proto::Point& proto);

CC_EXPORT void PointFToProto(const gfx::PointF& point, proto::PointF* proto);
CC_EXPORT gfx::PointF ProtoToPointF(const proto::PointF& proto);

CC_EXPORT void RectToProto(const gfx::Rect& rect, proto::Rect* proto);
CC_EXPORT gfx::Rect ProtoToRect(const proto::Rect& proto);

CC_EXPORT void RectFToProto(const gfx::RectF& rect, proto::RectF* proto);
CC_EXPORT gfx::RectF ProtoToRectF(const proto::RectF& proto);

CC_EXPORT void SizeToProto(const gfx::Size& size, proto::Size* proto);
CC_EXPORT gfx::Size ProtoToSize(const proto::Size& proto);

CC_EXPORT void SizeFToProto(const gfx::SizeF& size, proto::SizeF* proto);
CC_EXPORT gfx::SizeF ProtoToSizeF(const proto::SizeF& proto);

CC_EXPORT void TransformToProto(const gfx::Transform& transform,
    proto::Transform* proto);
CC_EXPORT gfx::Transform ProtoToTransform(const proto::Transform& proto);

} // namespace cc

#endif // CC_PROTO_GFX_CONVERSIONS_H_
