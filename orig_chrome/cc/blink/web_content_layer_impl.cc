// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_content_layer_impl.h"

#include "cc/blink/web_display_item_list_impl.h"
#include "cc/layers/picture_layer.h"
#include "cc/playback/display_item_list_settings.h"
#include "third_party/WebKit/public/platform/WebContentLayerClient.h"
#include "third_party/WebKit/public/platform/WebFloatPoint.h"
#include "third_party/WebKit/public/platform/WebFloatRect.h"
#include "third_party/WebKit/public/platform/WebRect.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

using cc::PictureLayer;

namespace cc_blink {

static blink::WebContentLayerClient::PaintingControlSetting
PaintingControlToWeb(
    cc::ContentLayerClient::PaintingControlSetting painting_control)
{
    switch (painting_control) {
    case cc::ContentLayerClient::PAINTING_BEHAVIOR_NORMAL:
        return blink::WebContentLayerClient::PaintDefaultBehavior;
    case cc::ContentLayerClient::DISPLAY_LIST_CONSTRUCTION_DISABLED:
        return blink::WebContentLayerClient::DisplayListConstructionDisabled;
    case cc::ContentLayerClient::DISPLAY_LIST_CACHING_DISABLED:
        return blink::WebContentLayerClient::DisplayListCachingDisabled;
    case cc::ContentLayerClient::DISPLAY_LIST_PAINTING_DISABLED:
        return blink::WebContentLayerClient::DisplayListPaintingDisabled;
    }
    NOTREACHED();
    return blink::WebContentLayerClient::PaintDefaultBehavior;
}

WebContentLayerImpl::WebContentLayerImpl(blink::WebContentLayerClient* client)
    : client_(client)
{
    layer_ = make_scoped_ptr(new WebLayerImpl(
        PictureLayer::Create(WebLayerImpl::LayerSettings(), this)));
    layer_->layer()->SetIsDrawable(true);
}

WebContentLayerImpl::~WebContentLayerImpl()
{
    static_cast<PictureLayer*>(layer_->layer())->ClearClient();
}

blink::WebLayer* WebContentLayerImpl::layer()
{
    return layer_.get();
}

void WebContentLayerImpl::setDoubleSided(bool double_sided)
{
    layer_->layer()->SetDoubleSided(double_sided);
}

void WebContentLayerImpl::setDrawCheckerboardForMissingTiles(bool)
{
}

void WebContentLayerImpl::PaintContents(
    SkCanvas* canvas,
    const gfx::Rect& clip,
    cc::ContentLayerClient::PaintingControlSetting painting_control)
{
    if (!client_)
        return;

    client_->paintContents(canvas, clip, PaintingControlToWeb(painting_control));
}

scoped_refptr<cc::DisplayItemList>
WebContentLayerImpl::PaintContentsToDisplayList(
    const gfx::Rect& clip,
    cc::ContentLayerClient::PaintingControlSetting painting_control)
{
    cc::DisplayItemListSettings settings;
    settings.use_cached_picture = true;

    scoped_refptr<cc::DisplayItemList> display_list = cc::DisplayItemList::Create(clip, settings);
    if (client_) {
        WebDisplayItemListImpl list(display_list.get());
        client_->paintContents(&list, clip, PaintingControlToWeb(painting_control));
    }
    display_list->Finalize();
    return display_list;
}

bool WebContentLayerImpl::FillsBoundsCompletely() const
{
    return false;
}

size_t WebContentLayerImpl::GetApproximateUnsharedMemoryUsage() const
{
    //return client_->approximateUnsharedMemoryUsage();
    return 0;
}

} // namespace cc_blink
