// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebTransformOperationsImpl.h"

#include <algorithm>

namespace mc_blink {

WebTransformOperationsImpl::WebTransformOperationsImpl()
{
}

const mc::TransformOperations& WebTransformOperationsImpl::asTransformOperations() const
{
    return m_transformOperations;
}

bool WebTransformOperationsImpl::canBlendWith(const blink::WebTransformOperations& other) const
{
    const WebTransformOperationsImpl& otherImpl = static_cast<const WebTransformOperationsImpl&>(other);
    return m_transformOperations.canBlendWith(otherImpl.m_transformOperations);
}

void WebTransformOperationsImpl::appendTranslate(double x, double y, double z)
{
    m_transformOperations.appendTranslate(x, y, z);
}

void WebTransformOperationsImpl::appendRotate(double x, double y, double z, double degrees)
{
    m_transformOperations.appendRotate(x, y, z, degrees);
}

void WebTransformOperationsImpl::appendScale(double x, double y, double z)
{
    m_transformOperations.appendScale(x, y, z);
}

void WebTransformOperationsImpl::appendSkew(double x, double y)
{
    m_transformOperations.appendSkew(x, y);
}

void WebTransformOperationsImpl::appendPerspective(double depth)
{
    m_transformOperations.appendPerspective(depth);
}

void WebTransformOperationsImpl::appendMatrix(const SkMatrix44& matrix)
{
    m_transformOperations.appendMatrix(matrix);
}

void WebTransformOperationsImpl::appendIdentity()
{
    m_transformOperations.appendIdentity();
}

bool WebTransformOperationsImpl::isIdentity() const
{
    return m_transformOperations.isIdentity();
}

WebTransformOperationsImpl::~WebTransformOperationsImpl()
{
}

}  // namespace mc_blink
