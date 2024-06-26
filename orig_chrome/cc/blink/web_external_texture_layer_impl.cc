// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_external_texture_layer_impl.h"

#include "cc/blink/web_external_bitmap_impl.h"
#include "cc/blink/web_layer_impl.h"
#include "cc/layers/texture_layer.h"
#include "cc/resources/single_release_callback.h"
#include "cc/resources/texture_mailbox.h"
#include "third_party/WebKit/public/platform/WebExternalTextureLayerClient.h"
#include "third_party/WebKit/public/platform/WebExternalTextureMailbox.h"
#include "third_party/WebKit/public/platform/WebFloatRect.h"
#include "third_party/WebKit/public/platform/WebGraphicsContext3D.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/khronos/GLES2/gl2.h"

using cc::TextureLayer;

namespace cc_blink {

WebExternalTextureLayerImpl::WebExternalTextureLayerImpl(
    blink::WebExternalTextureLayerClient* client)
    : client_(client)
{
    cc::TextureLayerClient* cc_client = client_ ? this : nullptr;
    scoped_refptr<TextureLayer> layer = TextureLayer::CreateForMailbox(WebLayerImpl::LayerSettings(), cc_client);
    layer->SetIsDrawable(true);
    layer_.reset(new WebLayerImpl(layer));
}

WebExternalTextureLayerImpl::~WebExternalTextureLayerImpl()
{
    static_cast<TextureLayer*>(layer_->layer())->ClearClient();
}

blink::WebLayer* WebExternalTextureLayerImpl::layer()
{
    return layer_.get();
}

void WebExternalTextureLayerImpl::clearTexture()
{
    TextureLayer* layer = static_cast<TextureLayer*>(layer_->layer());
    layer->ClearTexture();
}

void WebExternalTextureLayerImpl::setOpaque(bool opaque)
{
    static_cast<TextureLayer*>(layer_->layer())->SetContentsOpaque(opaque);
}

void WebExternalTextureLayerImpl::setPremultipliedAlpha(
    bool premultiplied_alpha)
{
    static_cast<TextureLayer*>(layer_->layer())
        ->SetPremultipliedAlpha(premultiplied_alpha);
}

void WebExternalTextureLayerImpl::setBlendBackgroundColor(bool blend)
{
    static_cast<TextureLayer*>(layer_->layer())->SetBlendBackgroundColor(blend);
}

void WebExternalTextureLayerImpl::setNearestNeighbor(bool nearest_neighbor)
{
    static_cast<TextureLayer*>(layer_->layer())
        ->SetNearestNeighbor(nearest_neighbor);
}

void WebExternalTextureLayerImpl::setRateLimitContext(bool)
{
    DebugBreak();
}

bool WebExternalTextureLayerImpl::PrepareTextureMailbox(
    cc::TextureMailbox* mailbox,
    scoped_ptr<cc::SingleReleaseCallback>* release_callback,
    bool use_shared_memory)
{
    blink::WebExternalTextureMailbox client_mailbox;
    WebExternalBitmapImpl* bitmap = nullptr;

    if (use_shared_memory)
        bitmap = AllocateBitmap();
    if (!client_->prepareMailbox(&client_mailbox, bitmap)) {
        if (bitmap)
            free_bitmaps_.push_back(bitmap);
        return false;
    }
    gpu::Mailbox name;
    name.SetName(client_mailbox.name);
    if (bitmap) {
        *mailbox = cc::TextureMailbox(bitmap->shared_bitmap(), bitmap->size());
    } else {
        // TODO(achaulk): pass a valid size here if allowOverlay is set.
        gpu::SyncToken sync_token;
        static_assert(sizeof(sync_token) <= sizeof(client_mailbox.syncToken),
            "Size of web external sync token too small.");
        if (client_mailbox.validSyncToken)
            memcpy(&sync_token, client_mailbox.syncToken, sizeof(sync_token));

        // TODO(achaulk): pass a valid size here if allowOverlay is set.
        *mailbox = cc::TextureMailbox(name, GL_TEXTURE_2D, sync_token, gfx::Size(), client_mailbox.allowOverlay);
    }
    mailbox->set_nearest_neighbor(client_mailbox.nearestNeighbor);

    if (mailbox->IsValid()) {
        *release_callback = cc::SingleReleaseCallback::Create(
            base::Bind(&WebExternalTextureLayerImpl::DidReleaseMailbox,
                this->AsWeakPtr(),
                client_mailbox,
                bitmap));
    }

    return true;
}

WebExternalBitmapImpl* WebExternalTextureLayerImpl::AllocateBitmap()
{
    if (!free_bitmaps_.empty()) {
        WebExternalBitmapImpl* result = free_bitmaps_.back();
        free_bitmaps_.weak_erase(free_bitmaps_.end() - 1);
        return result;
    }
    return new WebExternalBitmapImpl;
}

// static
void WebExternalTextureLayerImpl::DidReleaseMailbox(
    base::WeakPtr<WebExternalTextureLayerImpl> layer,
    const blink::WebExternalTextureMailbox& mailbox,
    WebExternalBitmapImpl* bitmap,
    const gpu::SyncToken& sync_token,
    bool lost_resource)
{
    DCHECK(layer);
    blink::WebExternalTextureMailbox available_mailbox;
    static_assert(sizeof(sync_token) <= sizeof(available_mailbox.syncToken), "Size of web external sync token too small.");
    memcpy(available_mailbox.name, mailbox.name, sizeof(available_mailbox.name));
    memcpy(available_mailbox.syncToken, sync_token.GetConstData(), sizeof(sync_token));
    available_mailbox.validSyncToken = sync_token.HasData();
    if (bitmap)
        layer->free_bitmaps_.push_back(bitmap);
    layer->client_->mailboxReleased(available_mailbox, lost_resource);
}

} // namespace cc_blink
