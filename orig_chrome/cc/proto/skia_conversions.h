// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PROTO_SKIA_CONVERSIONS_H_
#define CC_PROTO_SKIA_CONVERSIONS_H_

#include "cc/base/cc_export.h"
#include "cc/proto/skregion.pb.h"
#include "cc/proto/skxfermode.pb.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkXfermode.h"

class SkRegion;
class SkRRect;
class SkXferMode;

namespace cc {

namespace proto {
    class SkRRect;
}

// TODO(dtrainor): Move these to a class and make them static
// (crbug.com/548432).
CC_EXPORT SkRegion::Op SkRegionOpFromProto(proto::SkRegion::Op op);
CC_EXPORT proto::SkRegion::Op SkRegionOpToProto(SkRegion::Op op);

CC_EXPORT SkXfermode::Mode SkXfermodeModeFromProto(
    proto::SkXfermode::Mode mode);
CC_EXPORT proto::SkXfermode::Mode SkXfermodeModeToProto(SkXfermode::Mode mode);

CC_EXPORT void SkRRectToProto(const SkRRect& rect, proto::SkRRect* proto);
CC_EXPORT SkRRect ProtoToSkRRect(const proto::SkRRect& proto);

} // namespace cc

#endif // CC_PROTO_SKIA_CONVERSIONS_H_
