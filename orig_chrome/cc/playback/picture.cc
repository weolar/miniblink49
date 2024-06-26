// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/picture.h"

#include <set>
#include <string>

#include "base/base64.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "base/values.h"
#include "cc/base/math_util.h"
#include "cc/debug/picture_debug_util.h"
// #include "cc/debug/traced_picture.h"
// #include "cc/debug/traced_value.h"
#include "cc/layers/content_layer_client.h"
#include "skia/ext/pixel_ref_utils.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/utils/SkNullCanvas.h"
#include "third_party/skia/include/utils/SkPictureUtils.h"
#include "ui/gfx/codec/jpeg_codec.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/skia_util.h"

namespace cc {

namespace {

    bool DecodeBitmap(const void* buffer, size_t size, SkBitmap* bm)
    {
        const unsigned char* data = static_cast<const unsigned char*>(buffer);

        // Try PNG first.
        if (gfx::PNGCodec::Decode(data, size, bm))
            return true;

        // Try JPEG.
        scoped_ptr<SkBitmap> decoded_jpeg(gfx::JPEGCodec::Decode(data, size));
        if (decoded_jpeg) {
            *bm = *decoded_jpeg;
            return true;
        }
        return false;
    }

} // namespace

scoped_refptr<Picture> Picture::Create(
    const gfx::Rect& layer_rect,
    ContentLayerClient* client,
    const gfx::Size& tile_grid_size,
    bool gather_pixel_refs,
    RecordingSource::RecordingMode recording_mode)
{
    scoped_refptr<Picture> picture = make_scoped_refptr(new Picture(layer_rect, tile_grid_size));

    picture->Record(client, recording_mode);
    if (gather_pixel_refs)
        picture->GatherPixelRefs();

    return picture;
}

Picture::Picture(const gfx::Rect& layer_rect, const gfx::Size& tile_grid_size)
    : layer_rect_(layer_rect)
    , pixel_refs_(tile_grid_size)
{
    // Instead of recording a trace event for object creation here, we wait for
    // the picture to be recorded in Picture::Record.
}

scoped_refptr<Picture> Picture::CreateFromSkpValue(const base::Value* value)
{
    // Decode the picture from base64.
    std::string encoded;
    if (!value->GetAsString(&encoded))
        return NULL;

    std::string decoded;
    base::Base64Decode(encoded, &decoded);
    SkMemoryStream stream(decoded.data(), decoded.size());

    // Read the picture. This creates an empty picture on failure.
    SkPicture* skpicture = SkPicture::CreateFromStream(&stream, &DecodeBitmap);
    if (skpicture == NULL)
        return NULL;

    gfx::Rect layer_rect(gfx::SkIRectToRect(skpicture->cullRect().roundOut()));
    return make_scoped_refptr(new Picture(skpicture, layer_rect));
}

scoped_refptr<Picture> Picture::CreateFromValue(const base::Value* raw_value)
{
    const base::DictionaryValue* value = NULL;
    if (!raw_value->GetAsDictionary(&value))
        return NULL;

    // Decode the picture from base64.
    std::string encoded;
    if (!value->GetString("skp64", &encoded))
        return NULL;

    std::string decoded;
    base::Base64Decode(encoded, &decoded);
    SkMemoryStream stream(decoded.data(), decoded.size());

    const base::Value* layer_rect_value = NULL;
    if (!value->Get("params.layer_rect", &layer_rect_value))
        return NULL;

    gfx::Rect layer_rect;
    if (!MathUtil::FromValue(layer_rect_value, &layer_rect))
        return NULL;

    // Read the picture. This creates an empty picture on failure.
    SkPicture* skpicture = SkPicture::CreateFromStream(&stream, &DecodeBitmap);
    if (skpicture == NULL)
        return NULL;

    return make_scoped_refptr(new Picture(skpicture, layer_rect));
}

Picture::Picture(SkPicture* picture, const gfx::Rect& layer_rect)
    : layer_rect_(layer_rect)
    , picture_(skia::AdoptRef(picture))
    , pixel_refs_(layer_rect.size())
{
}

Picture::Picture(const skia::RefPtr<SkPicture>& picture,
    const gfx::Rect& layer_rect,
    const PixelRefMap& pixel_refs)
    : layer_rect_(layer_rect)
    , picture_(picture)
    , pixel_refs_(pixel_refs)
{
}

Picture::~Picture()
{
    TRACE_EVENT_OBJECT_DELETED_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug.picture"), "cc::Picture", this);
}

bool Picture::IsSuitableForGpuRasterization(const char** reason) const
{
    DCHECK(picture_);

    // TODO(hendrikw): SkPicture::suitableForGpuRasterization takes a GrContext.
    // Currently the GrContext isn't used, and should probably be removed from
    // skia.
    return picture_->suitableForGpuRasterization(nullptr, reason);
}

int Picture::ApproximateOpCount() const
{
    DCHECK(picture_);
    return picture_->approximateOpCount();
}

size_t Picture::ApproximateMemoryUsage() const
{
    DCHECK(picture_);
    return SkPictureUtils::ApproximateBytesUsed(picture_.get());
}

bool Picture::HasText() const
{
    DCHECK(picture_);
    return picture_->hasText();
}

void Picture::Record(ContentLayerClient* painter,
    RecordingSource::RecordingMode recording_mode)
{
    TRACE_EVENT2("cc",
        "Picture::Record",
        "data",
        AsTraceableRecordData(),
        "recording_mode",
        recording_mode);

    DCHECK(!picture_);

    SkRTreeFactory factory;
    SkPictureRecorder recorder;

    skia::RefPtr<SkCanvas> canvas;
    canvas = skia::SharePtr(recorder.beginRecording(
        layer_rect_.width(), layer_rect_.height(), &factory,
        SkPictureRecorder::kComputeSaveLayerInfo_RecordFlag));

    ContentLayerClient::PaintingControlSetting painting_control = ContentLayerClient::PAINTING_BEHAVIOR_NORMAL;

    switch (recording_mode) {
    case RecordingSource::RECORD_NORMALLY:
        // Already setup for normal recording.
        break;
    case RecordingSource::RECORD_WITH_SK_NULL_CANVAS:
        canvas = skia::AdoptRef(SkCreateNullCanvas());
        break;
    case RecordingSource::RECORD_WITH_PAINTING_DISABLED:
        // We pass a disable flag through the paint calls when perfromance
        // testing (the only time this case should ever arise) when we want to
        // prevent the Blink GraphicsContext object from consuming any compute
        // time.
        canvas = skia::AdoptRef(SkCreateNullCanvas());
        painting_control = ContentLayerClient::DISPLAY_LIST_PAINTING_DISABLED;
        break;
    case RecordingSource::RECORD_WITH_CACHING_DISABLED:
        // This mode should give the same results as RECORD_NORMALLY.
        painting_control = ContentLayerClient::DISPLAY_LIST_CACHING_DISABLED;
        break;
    default:
        // case RecordingSource::RECORD_WITH_CONSTRUCTION_DISABLED should
        // not be reached
        NOTREACHED();
    }

    canvas->save();
    canvas->translate(SkFloatToScalar(-layer_rect_.x()),
        SkFloatToScalar(-layer_rect_.y()));

    canvas->clipRect(gfx::RectToSkRect(layer_rect_));

    painter->PaintContents(canvas.get(), layer_rect_, painting_control);

    canvas->restore();
    picture_ = skia::AdoptRef(recorder.endRecordingAsPicture());
    DCHECK(picture_);

    EmitTraceSnapshot();
}

void Picture::GatherPixelRefs()
{
    TRACE_EVENT2("cc", "Picture::GatherPixelRefs",
        "width", layer_rect_.width(),
        "height", layer_rect_.height());

    DCHECK(picture_);
    DCHECK(pixel_refs_.empty());
    if (!WillPlayBackBitmaps())
        return;

    pixel_refs_.GatherPixelRefsFromPicture(picture_.get());
}

int Picture::Raster(SkCanvas* canvas,
    SkPicture::AbortCallback* callback,
    const Region& negated_content_region,
    float contents_scale) const
{
    TRACE_EVENT_BEGIN1(
        "cc",
        "Picture::Raster",
        "data",
        AsTraceableRasterData(contents_scale));

    DCHECK(picture_);

    canvas->save();

    for (Region::Iterator it(negated_content_region); it.has_rect(); it.next())
        canvas->clipRect(gfx::RectToSkRect(it.rect()), SkRegion::kDifference_Op);

    canvas->scale(contents_scale, contents_scale);
    canvas->translate(layer_rect_.x(), layer_rect_.y());
    if (callback) {
        // If we have a callback, we need to call |draw()|, |drawPicture()| doesn't
        // take a callback.  This is used by |AnalysisCanvas| to early out.
        picture_->playback(canvas, callback);
    } else {
        // Prefer to call |drawPicture()| on the canvas since it could place the
        // entire picture on the canvas instead of parsing the skia operations.
        canvas->drawPicture(picture_.get());
    }
    SkIRect bounds;
    canvas->getClipDeviceBounds(&bounds);
    canvas->restore();
    TRACE_EVENT_END1(
        "cc", "Picture::Raster",
        "num_pixels_rasterized", bounds.width() * bounds.height());
    return bounds.width() * bounds.height();
}

void Picture::Replay(SkCanvas* canvas, SkPicture::AbortCallback* callback)
{
    TRACE_EVENT_BEGIN0("cc", "Picture::Replay");
    DCHECK(picture_);
    picture_->playback(canvas, callback);
    SkIRect bounds;
    canvas->getClipDeviceBounds(&bounds);
    TRACE_EVENT_END1("cc", "Picture::Replay",
        "num_pixels_replayed", bounds.width() * bounds.height());
}

scoped_ptr<base::Value> Picture::AsValue() const
{
    // Encode the picture as base64.
    scoped_ptr<base::DictionaryValue> res(new base::DictionaryValue());
    res->Set("params.layer_rect", MathUtil::AsValue(layer_rect_).release());
    std::string b64_picture;
    PictureDebugUtil::SerializeAsBase64(picture_.get(), &b64_picture);
    res->SetString("skp64", b64_picture);
    return res.Pass();
}

void Picture::EmitTraceSnapshot() const
{
    TRACE_EVENT_OBJECT_SNAPSHOT_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug.picture") "," TRACE_DISABLED_BY_DEFAULT("devtools.timeline.picture"),
        "cc::Picture",
        this,
        TracedPicture::AsTraceablePicture(this));
}

void Picture::EmitTraceSnapshotAlias(Picture* original) const
{
    TRACE_EVENT_OBJECT_SNAPSHOT_WITH_ID(
        TRACE_DISABLED_BY_DEFAULT("cc.debug.picture") "," TRACE_DISABLED_BY_DEFAULT("devtools.timeline.picture"),
        "cc::Picture",
        this,
        TracedPicture::AsTraceablePictureAlias(original));
}

PixelRefMap::Iterator Picture::GetPixelRefMapIterator(
    const gfx::Rect& layer_rect) const
{
    return PixelRefMap::Iterator(layer_rect, this);
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
Picture::AsTraceableRasterData(float scale) const
{
    scoped_refptr<base::trace_event::TracedValue> raster_data = new base::trace_event::TracedValue();
    TracedValue::SetIDRef(this, raster_data.get(), "picture_id");
    raster_data->SetDouble("scale", scale);
    return raster_data;
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
Picture::AsTraceableRecordData() const
{
    scoped_refptr<base::trace_event::TracedValue> record_data = new base::trace_event::TracedValue();
    TracedValue::SetIDRef(this, record_data.get(), "picture_id");
    MathUtil::AddToTracedValue("layer_rect", layer_rect_, record_data.get());
    return record_data;
}

} // namespace cc
