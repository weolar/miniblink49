// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/invalidation_region.h"

#include "base/metrics/histogram.h"

namespace {

const int kMaxInvalidationRectCount = 256;

} // namespace

namespace cc {

InvalidationRegion::InvalidationRegion() { }

InvalidationRegion::~InvalidationRegion() { }

void InvalidationRegion::Swap(Region* region)
{
    region_.Swap(region);
}

void InvalidationRegion::Clear()
{
    region_.Clear();
}

void InvalidationRegion::Union(const gfx::Rect& rect)
{
    region_.Union(rect);
    SimplifyIfNeeded();
}

void InvalidationRegion::SimplifyIfNeeded()
{
    if (region_.GetRegionComplexity() > kMaxInvalidationRectCount)
        region_ = region_.bounds();
}

} // namespace cc
