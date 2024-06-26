// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/resources/ui_resource_request.h"

namespace cc {

UIResourceRequest::UIResourceRequest(UIResourceRequestType type,
    UIResourceId id)
    : type_(type)
    , id_(id)
{
}

UIResourceRequest::UIResourceRequest(UIResourceRequestType type,
    UIResourceId id,
    const UIResourceBitmap& bitmap)
    : type_(type)
    , id_(id)
    , bitmap_(new UIResourceBitmap(bitmap))
{
}

UIResourceRequest::UIResourceRequest(const UIResourceRequest& request)
{
    (*this) = request;
}

UIResourceRequest& UIResourceRequest::operator=(
    const UIResourceRequest& request)
{
    type_ = request.type_;
    id_ = request.id_;
    if (request.bitmap_) {
        bitmap_ = make_scoped_ptr(new UIResourceBitmap(*request.bitmap_.get()));
    } else {
        bitmap_ = nullptr;
    }

    return *this;
}

UIResourceRequest::~UIResourceRequest() { }

} // namespace cc
