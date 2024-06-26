// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/TransformOperations.h"

#include <algorithm>

#include "mc/base/TransformUtil.h"
#include "mc/base/FloatBox.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {

TransformOperations::TransformOperations()
    : m_decomposedTransformDirty(true)
{
}

TransformOperations::TransformOperations(const TransformOperations& other) 
{
    m_operations = other.m_operations;
    m_decomposedTransformDirty = other.m_decomposedTransformDirty;
    if (!m_decomposedTransformDirty) {
        m_decomposedTransform = adoptPtr(new DecomposedTransform(*other.m_decomposedTransform.get()));
    }
}

TransformOperations::~TransformOperations() 
{
}

SkMatrix44 TransformOperations::apply() const
{
    SkMatrix44 to_return(SkMatrix44::kIdentity_Constructor);
    for (size_t i = 0; i < m_operations.size(); ++i)
        to_return.preConcat(m_operations[i].matrix);
    return to_return;
}

SkMatrix44 TransformOperations::blend(const TransformOperations& from, SkMScalar progress) const
{
    SkMatrix44 to_return(SkMatrix44::kIdentity_Constructor);
    blendInternal(from, progress, &to_return);
    return to_return;
}

bool TransformOperations::blendedBoundsForBox(const FloatBox& box, const TransformOperations& from, SkMScalar min_progress, SkMScalar max_progress,  FloatBox* bounds) const
{
    *bounds = box;

    bool from_identity = from.isIdentity();
    bool to_identity = isIdentity();
    if (from_identity && to_identity)
        return true;

    if (!matchesTypes(from))
        return false;

    size_t num_operations = std::max(from_identity ? 0 : from.m_operations.size(),
        to_identity ? 0 : m_operations.size());

    // Because we are squashing all of the matrices together when applying
    // them to the animation, we must apply them in reverse order when
    // not squashing them.
    for (size_t i = 0; i < num_operations; ++i) {
        size_t operation_index = num_operations - 1 - i;
        FloatBox bounds_for_operation;
        const TransformOperation* from_op =
            from_identity ? nullptr : &from.m_operations[operation_index];
        const TransformOperation* to_op =
            to_identity ? nullptr : &m_operations[operation_index];
        if (!TransformOperation::blendedBoundsForBox(*bounds, from_op, to_op,
            min_progress, max_progress,
            &bounds_for_operation)) {
            return false;
        }
        *bounds = bounds_for_operation;
    }

    return true;
}

bool TransformOperations::affectsScale() const 
{
    for (size_t i = 0; i < m_operations.size(); ++i) {
        if (m_operations[i].type == TransformOperation::TRANSFORM_OPERATION_SCALE)
            return true;
        if (m_operations[i].type == TransformOperation::TRANSFORM_OPERATION_MATRIX &&
            !m_operations[i].matrix.isScaleTranslate())
            return true;
    }
    return false;
}

bool TransformOperations::preservesAxisAlignment() const 
{
    for (size_t i = 0; i < m_operations.size(); ++i) {
        switch (m_operations[i].type) {
        case TransformOperation::TRANSFORM_OPERATION_IDENTITY:
        case TransformOperation::TRANSFORM_OPERATION_TRANSLATE:
        case TransformOperation::TRANSFORM_OPERATION_SCALE:
            continue;
        case TransformOperation::TRANSFORM_OPERATION_MATRIX:
            if (!m_operations[i].matrix.isIdentity() &&
                !m_operations[i].matrix.isScaleTranslate())
                return false;
            continue;
        case TransformOperation::TRANSFORM_OPERATION_ROTATE:
        case TransformOperation::TRANSFORM_OPERATION_SKEW:
        case TransformOperation::TRANSFORM_OPERATION_PERSPECTIVE:
            return false;
        }
    }
    return true;
}

bool TransformOperations::isTranslation() const 
{
    for (size_t i = 0; i < m_operations.size(); ++i) {
        switch (m_operations[i].type) {
        case TransformOperation::TRANSFORM_OPERATION_IDENTITY:
        case TransformOperation::TRANSFORM_OPERATION_TRANSLATE:
            continue;
        case TransformOperation::TRANSFORM_OPERATION_MATRIX:
            if (!m_operations[i].matrix.isScaleTranslate())
                return false;
            continue;
        case TransformOperation::TRANSFORM_OPERATION_ROTATE:
        case TransformOperation::TRANSFORM_OPERATION_SCALE:
        case TransformOperation::TRANSFORM_OPERATION_SKEW:
        case TransformOperation::TRANSFORM_OPERATION_PERSPECTIVE:
            return false;
        }
    }
    return true;
}

bool TransformOperations::scaleComponent(blink::FloatPoint3D* scale) const
{
    *scale = blink::FloatPoint3D(1.f, 1.f, 1.f);
    bool has_scale_component = false;
    for (size_t i = 0; i < m_operations.size(); ++i) {
        switch (m_operations[i].type) {
        case TransformOperation::TRANSFORM_OPERATION_IDENTITY:
        case TransformOperation::TRANSFORM_OPERATION_TRANSLATE:
            continue;
        case TransformOperation::TRANSFORM_OPERATION_MATRIX:
            if (!m_operations[i].matrix.isScaleTranslate())
                return false;
            continue;
        case TransformOperation::TRANSFORM_OPERATION_ROTATE:
        case TransformOperation::TRANSFORM_OPERATION_SKEW:
        case TransformOperation::TRANSFORM_OPERATION_PERSPECTIVE:
            return false;
        case TransformOperation::TRANSFORM_OPERATION_SCALE:
            if (has_scale_component)
                return false;
            has_scale_component = true;
            scale->scale(m_operations[i].scale.x,
                m_operations[i].scale.y,
                m_operations[i].scale.z);
        }
    }
    return true;
}

bool TransformOperations::matchesTypes(const TransformOperations& other) const 
{
    if (m_operations.size() == 0 || other.m_operations.size() == 0)
        return true;

    if (m_operations.size() != other.m_operations.size())
        return false;

    for (size_t i = 0; i < m_operations.size(); ++i) {
        if (m_operations[i].type != other.m_operations[i].type)
            return false;
    }

    return true;
}

bool TransformOperations::canBlendWith(const TransformOperations& other) const
{
    SkMatrix44 dummy(SkMatrix44::kIdentity_Constructor);
    return blendInternal(other, 0.5, &dummy);
}

void TransformOperations::appendTranslate(SkMScalar x, SkMScalar y, SkMScalar z)
{
    TransformOperation to_add;
    to_add.matrix.preTranslate(x, y, z);
    to_add.type = TransformOperation::TRANSFORM_OPERATION_TRANSLATE;
    to_add.translate.x = x;
    to_add.translate.y = y;
    to_add.translate.z = z;
    m_operations.push_back(to_add);
    m_decomposedTransformDirty = true;
}

void TransformOperations::appendRotate(SkMScalar x, SkMScalar y, SkMScalar z, SkMScalar degrees) 
{
    TransformOperation to_add;
    rotateAbout(&(to_add.matrix), blink::FloatPoint3D(x, y, z), degrees);
    to_add.type = TransformOperation::TRANSFORM_OPERATION_ROTATE;
    to_add.rotate.axis.x = x;
    to_add.rotate.axis.y = y;
    to_add.rotate.axis.z = z;
    to_add.rotate.angle = degrees;
    m_operations.push_back(to_add);
    m_decomposedTransformDirty = true;
}

void TransformOperations::appendScale(SkMScalar x, SkMScalar y, SkMScalar z)
{
    TransformOperation to_add;
    to_add.matrix.preScale(x, y, z);
    to_add.type = TransformOperation::TRANSFORM_OPERATION_SCALE;
    to_add.scale.x = x;
    to_add.scale.y = y;
    to_add.scale.z = z;
    m_operations.push_back(to_add);
    m_decomposedTransformDirty = true;
}

void TransformOperations::appendSkew(SkMScalar x, SkMScalar y)
{
    TransformOperation to_add;
    skewOp(&(to_add.matrix), x, y);
    to_add.type = TransformOperation::TRANSFORM_OPERATION_SKEW;
    to_add.skew.x = x;
    to_add.skew.y = y;
    m_operations.push_back(to_add);
    m_decomposedTransformDirty = true;
}

void TransformOperations::appendPerspective(SkMScalar depth) 
{
    TransformOperation to_add;
    applyPerspectiveDepth(&(to_add.matrix), depth);
    to_add.type = TransformOperation::TRANSFORM_OPERATION_PERSPECTIVE;
    to_add.perspective_depth = depth;
    m_operations.push_back(to_add);
    m_decomposedTransformDirty = true;
}

void TransformOperations::appendMatrix(const SkMatrix44& matrix)
{
    TransformOperation to_add;
    to_add.matrix = matrix;
    to_add.type = TransformOperation::TRANSFORM_OPERATION_MATRIX;
    m_operations.push_back(to_add);
    m_decomposedTransformDirty = true;
}

void TransformOperations::appendIdentity()
{
    m_operations.push_back(TransformOperation());
}

bool TransformOperations::isIdentity() const 
{
    for (size_t i = 0; i < m_operations.size(); ++i) {
        if (!m_operations[i].isIdentity())
            return false;
    }
    return true;
}

bool TransformOperations::blendInternal(const TransformOperations& from, SkMScalar progress, SkMatrix44* result) const
{
    bool from_identity = from.isIdentity();
    bool to_identity = isIdentity();
    if (from_identity && to_identity)
        return true;

    if (matchesTypes(from)) {
        size_t num_operations = std::max(
            from_identity ? 0 : from.m_operations.size(),
            to_identity ? 0 : m_operations.size());
        for (size_t i = 0; i < num_operations; ++i) {
            SkMatrix44 blended(SkMatrix44::kIdentity_Constructor);
            if (!TransformOperation::blendTransformOperations(
                from_identity ? 0 : &from.m_operations[i],
                to_identity ? 0 : &m_operations[i],
                progress,
                &blended))
                return false;
            result->preConcat(blended);
        }
        return true;
    }

    if (!computeDecomposedTransform() || !from.computeDecomposedTransform())
        return false;

    DecomposedTransform to_return;
    if (!blendDecomposedTransforms(&to_return,
        *m_decomposedTransform.get(),
        *from.m_decomposedTransform.get(),
        progress))
        return false;

    *result = composeTransform(to_return);
    return true;
}

bool TransformOperations::computeDecomposedTransform() const
{
    if (m_decomposedTransformDirty) {
        if (!m_decomposedTransform)
            m_decomposedTransform = adoptPtr(new DecomposedTransform());
        SkMatrix44 transform = apply();
        if (!decomposeTransform(m_decomposedTransform.get(), transform))
            return false;
        m_decomposedTransformDirty = false;
    }
    return true;
}

}  // namespace mc
