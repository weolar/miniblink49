// Copyright 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/picture_image_layer.h"

#include "cc/layers/picture_image_layer_impl.h"
#include "cc/playback/display_item_list_settings.h"
#include "cc/playback/drawing_display_item.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "ui/gfx/skia_util.h"

namespace cc {

scoped_refptr<PictureImageLayer> PictureImageLayer::Create(
    const LayerSettings& settings)
{
    return make_scoped_refptr(new PictureImageLayer(settings));
}

PictureImageLayer::PictureImageLayer(const LayerSettings& settings)
    : PictureLayer(settings, this)
{
}

PictureImageLayer::~PictureImageLayer()
{
    ClearClient();
}

scoped_ptr<LayerImpl> PictureImageLayer::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return PictureImageLayerImpl::Create(tree_impl, id(), is_mask());
}

bool PictureImageLayer::HasDrawableContent() const
{
    return image_ && PictureLayer::HasDrawableContent();
}

void PictureImageLayer::SetImage(skia::RefPtr<const SkImage> image)
{
    // SetImage() currently gets called whenever there is any
    // style change that affects the layer even if that change doesn't
    // affect the actual contents of the image (e.g. a CSS animation).
    // With this check in place we avoid unecessary texture uploads.
    if (image_.get() == image.get())
        return;

    image_ = image /*.Pass()*/;
    UpdateDrawsContent(HasDrawableContent());
    SetNeedsDisplay();
}

void PictureImageLayer::PaintContents(
    SkCanvas* canvas,
    const gfx::Rect& clip,
    ContentLayerClient::PaintingControlSetting painting_control)
{
    DCHECK(image_);
    DCHECK_GT(image_->width(), 0);
    DCHECK_GT(image_->height(), 0);

    SkScalar content_to_layer_scale_x = SkFloatToScalar(static_cast<float>(bounds().width()) / image_->width());
    SkScalar content_to_layer_scale_y = SkFloatToScalar(static_cast<float>(bounds().height()) / image_->height());
    canvas->scale(content_to_layer_scale_x, content_to_layer_scale_y);

    // Because Android WebView resourceless software draw mode rasters directly
    // to the root canvas, this draw must use the kSrcOver_Mode so that
    // transparent images blend correctly.
    canvas->drawImage(image_.get(), 0, 0);
}

scoped_refptr<DisplayItemList> PictureImageLayer::PaintContentsToDisplayList(
    const gfx::Rect& clip,
    ContentLayerClient::PaintingControlSetting painting_control)
{
    // Picture image layers can be used with GatherPixelRefs, so cached SkPictures
    // are currently required.
    DisplayItemListSettings settings;
    settings.use_cached_picture = true;
    scoped_refptr<DisplayItemList> display_list = DisplayItemList::Create(clip, settings);

    SkPictureRecorder recorder;
    SkCanvas* canvas = recorder.beginRecording(gfx::RectToSkRect(clip));
    PaintContents(canvas, clip, painting_control);

    skia::RefPtr<SkPicture> picture = skia::AdoptRef(recorder.endRecordingAsPicture());
    auto* item = display_list->CreateAndAppendItem<DrawingDisplayItem>();
    item->SetNew(picture /*.Pass()*/);

    display_list->Finalize();
    return display_list;
}

bool PictureImageLayer::FillsBoundsCompletely() const
{
    return false;
}

size_t PictureImageLayer::GetApproximateUnsharedMemoryUsage() const
{
    return 0;
}

} // namespace cc
