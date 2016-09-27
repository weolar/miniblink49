/*
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include "core/svg/SVGTransformDistance.h"

#include "platform/geometry/FloatPoint.h"
#include "platform/geometry/FloatSize.h"
#include <math.h>

namespace blink {

SVGTransformDistance::SVGTransformDistance()
    : m_transformType(SVG_TRANSFORM_UNKNOWN)
    , m_angle(0)
    , m_cx(0)
    , m_cy(0)
{
}

SVGTransformDistance::SVGTransformDistance(SVGTransformType transformType, float angle, float cx, float cy, const AffineTransform& transform)
    : m_transformType(transformType)
    , m_angle(angle)
    , m_cx(cx)
    , m_cy(cy)
    , m_transform(transform)
{
}

SVGTransformDistance::SVGTransformDistance(PassRefPtrWillBeRawPtr<SVGTransform> passFromSVGTransform, PassRefPtrWillBeRawPtr<SVGTransform> passToSVGTransform)
    : m_angle(0)
    , m_cx(0)
    , m_cy(0)
{
    RefPtrWillBeRawPtr<SVGTransform> fromSVGTransform = passFromSVGTransform;
    RefPtrWillBeRawPtr<SVGTransform> toSVGTransform = passToSVGTransform;

    m_transformType = fromSVGTransform->transformType();
    ASSERT(m_transformType == toSVGTransform->transformType());

    switch (m_transformType) {
    case SVG_TRANSFORM_MATRIX:
        ASSERT_NOT_REACHED();
    case SVG_TRANSFORM_UNKNOWN:
        break;
    case SVG_TRANSFORM_ROTATE: {
        FloatSize centerDistance = toSVGTransform->rotationCenter() - fromSVGTransform->rotationCenter();
        m_angle = toSVGTransform->angle() - fromSVGTransform->angle();
        m_cx = centerDistance.width();
        m_cy = centerDistance.height();
        break;
    }
    case SVG_TRANSFORM_TRANSLATE: {
        FloatSize translationDistance = toSVGTransform->translate() - fromSVGTransform->translate();
        m_transform.translate(translationDistance.width(), translationDistance.height());
        break;
    }
    case SVG_TRANSFORM_SCALE: {
        float scaleX = toSVGTransform->scale().width() - fromSVGTransform->scale().width();
        float scaleY = toSVGTransform->scale().height() - fromSVGTransform->scale().height();
        m_transform.scaleNonUniform(scaleX, scaleY);
        break;
    }
    case SVG_TRANSFORM_SKEWX:
    case SVG_TRANSFORM_SKEWY:
        m_angle = toSVGTransform->angle() - fromSVGTransform->angle();
        break;
    }
}

SVGTransformDistance SVGTransformDistance::scaledDistance(float scaleFactor) const
{
    switch (m_transformType) {
    case SVG_TRANSFORM_MATRIX:
        ASSERT_NOT_REACHED();
    case SVG_TRANSFORM_UNKNOWN:
        return SVGTransformDistance();
    case SVG_TRANSFORM_ROTATE:
        return SVGTransformDistance(m_transformType, m_angle * scaleFactor, m_cx * scaleFactor, m_cy * scaleFactor, AffineTransform());
    case SVG_TRANSFORM_SCALE:
        return SVGTransformDistance(m_transformType, m_angle * scaleFactor, m_cx * scaleFactor, m_cy * scaleFactor, AffineTransform(m_transform).scale(scaleFactor));
    case SVG_TRANSFORM_TRANSLATE: {
        AffineTransform newTransform(m_transform);
        newTransform.setE(m_transform.e() * scaleFactor);
        newTransform.setF(m_transform.f() * scaleFactor);
        return SVGTransformDistance(m_transformType, 0, 0, 0, newTransform);
    }
    case SVG_TRANSFORM_SKEWX:
    case SVG_TRANSFORM_SKEWY:
        return SVGTransformDistance(m_transformType, m_angle * scaleFactor, m_cx * scaleFactor, m_cy * scaleFactor, AffineTransform());
    }

    ASSERT_NOT_REACHED();
    return SVGTransformDistance();
}

PassRefPtrWillBeRawPtr<SVGTransform> SVGTransformDistance::addSVGTransforms(PassRefPtrWillBeRawPtr<SVGTransform> passFirst, PassRefPtrWillBeRawPtr<SVGTransform> passSecond, unsigned repeatCount)
{
    RefPtrWillBeRawPtr<SVGTransform> first = passFirst;
    RefPtrWillBeRawPtr<SVGTransform> second = passSecond;
    ASSERT(first->transformType() == second->transformType());

    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create();

    switch (first->transformType()) {
    case SVG_TRANSFORM_MATRIX:
        ASSERT_NOT_REACHED();
    case SVG_TRANSFORM_UNKNOWN:
        return transform.release();
    case SVG_TRANSFORM_ROTATE: {
        transform->setRotate(first->angle() + second->angle() * repeatCount, first->rotationCenter().x() + second->rotationCenter().x() * repeatCount, first->rotationCenter().y() + second->rotationCenter().y() * repeatCount);
        return transform.release();
    }
    case SVG_TRANSFORM_TRANSLATE: {
        float dx = first->translate().x() + second->translate().x() * repeatCount;
        float dy = first->translate().y() + second->translate().y() * repeatCount;
        transform->setTranslate(dx, dy);
        return transform.release();
    }
    case SVG_TRANSFORM_SCALE: {
        FloatSize scale = second->scale();
        scale.scale(repeatCount);
        scale += first->scale();
        transform->setScale(scale.width(), scale.height());
        return transform.release();
    }
    case SVG_TRANSFORM_SKEWX:
        transform->setSkewX(first->angle() + second->angle() * repeatCount);
        return transform.release();
    case SVG_TRANSFORM_SKEWY:
        transform->setSkewY(first->angle() + second->angle() * repeatCount);
        return transform.release();
    }
    ASSERT_NOT_REACHED();
    return transform.release();
}

PassRefPtrWillBeRawPtr<SVGTransform> SVGTransformDistance::addToSVGTransform(PassRefPtrWillBeRawPtr<SVGTransform> passTransform) const
{
    RefPtrWillBeRawPtr<SVGTransform> transform = passTransform;
    ASSERT(m_transformType == transform->transformType() || m_transformType == SVG_TRANSFORM_UNKNOWN);

    RefPtrWillBeRawPtr<SVGTransform> newTransform = transform->clone();

    switch (m_transformType) {
    case SVG_TRANSFORM_MATRIX:
        ASSERT_NOT_REACHED();
    case SVG_TRANSFORM_UNKNOWN:
        return SVGTransform::create();
    case SVG_TRANSFORM_TRANSLATE: {
        FloatPoint translation = transform->translate();
        translation += FloatSize::narrowPrecision(m_transform.e(), m_transform.f());
        newTransform->setTranslate(translation.x(), translation.y());
        return newTransform.release();
    }
    case SVG_TRANSFORM_SCALE: {
        FloatSize scale = transform->scale();
        scale += FloatSize::narrowPrecision(m_transform.a(), m_transform.d());
        newTransform->setScale(scale.width(), scale.height());
        return newTransform.release();
    }
    case SVG_TRANSFORM_ROTATE: {
        FloatPoint center = transform->rotationCenter();
        newTransform->setRotate(transform->angle() + m_angle, center.x() + m_cx, center.y() + m_cy);
        return newTransform.release();
    }
    case SVG_TRANSFORM_SKEWX:
        newTransform->setSkewX(transform->angle() + m_angle);
        return newTransform.release();
    case SVG_TRANSFORM_SKEWY:
        newTransform->setSkewY(transform->angle() + m_angle);
        return newTransform.release();
    }

    ASSERT_NOT_REACHED();
    return newTransform.release();
}

float SVGTransformDistance::distance() const
{
    switch (m_transformType) {
    case SVG_TRANSFORM_MATRIX:
        ASSERT_NOT_REACHED();
    case SVG_TRANSFORM_UNKNOWN:
        return 0;
    case SVG_TRANSFORM_ROTATE:
        return sqrtf(m_angle * m_angle + m_cx * m_cx + m_cy * m_cy);
    case SVG_TRANSFORM_SCALE:
        return static_cast<float>(sqrt(m_transform.a() * m_transform.a() + m_transform.d() * m_transform.d()));
    case SVG_TRANSFORM_TRANSLATE:
        return static_cast<float>(sqrt(m_transform.e() * m_transform.e() + m_transform.f() * m_transform.f()));
    case SVG_TRANSFORM_SKEWX:
    case SVG_TRANSFORM_SKEWY:
        return m_angle;
    }
    ASSERT_NOT_REACHED();
    return 0;
}

}
