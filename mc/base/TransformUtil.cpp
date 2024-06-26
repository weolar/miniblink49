
#include "mc/base/TransformUtil.h"
#include "mc/base/FloatBox.h"

namespace mc {

void rotateAbout(SkMatrix44* matrix, const blink::FloatPoint3D& axis, double degrees)
{
    if (matrix->isIdentity()) {
        matrix->setRotateDegreesAbout(SkFloatToMScalar(axis.x()),
            SkFloatToMScalar(axis.y()),
            SkFloatToMScalar(axis.z()),
            SkDoubleToMScalar(degrees));
    } else {
        SkMatrix44 rot(SkMatrix44::kUninitialized_Constructor);
        rot.setRotateDegreesAbout(SkFloatToMScalar(axis.x()),
            SkFloatToMScalar(axis.y()),
            SkFloatToMScalar(axis.z()),
            SkDoubleToMScalar(degrees));
        matrix->preConcat(rot);
    }
}

void skewOp(SkMatrix44* matrix, double angle_x, double angle_y)
{
    if (matrix->isIdentity()) {
        matrix->set(0, 1, tanDegrees(angle_x));
        matrix->set(1, 0, tanDegrees(angle_y));
    } else {
        SkMatrix44 skew(SkMatrix44::kIdentity_Constructor);
        skew.set(0, 1, tanDegrees(angle_x));
        skew.set(1, 0, tanDegrees(angle_y));
        matrix->preConcat(skew);
    }
}

void applyPerspectiveDepth(SkMatrix44* matrix, SkMScalar depth)
{
    if (depth == 0)
        return;
    if (matrix->isIdentity()) {
        matrix->set(3, 2, -SK_MScalar1 / depth);
    } else {
        SkMatrix44 m(SkMatrix44::kIdentity_Constructor);
        m.set(3, 2, -SK_MScalar1 / depth);
        matrix->preConcat(m);
    }
}

SkMScalar tanDegrees(double degrees) 
{
    double radians = degrees * M_PI / 180;
    return SkDoubleToMScalar(std::tan(radians));
}
    
SkMScalar Length3(SkMScalar v[3]) 
{
    double vd[3] = { SkMScalarToDouble(v[0]), SkMScalarToDouble(v[1]),
        SkMScalarToDouble(v[2]) };
    return SkDoubleToMScalar(
        std::sqrt(vd[0] * vd[0] + vd[1] * vd[1] + vd[2] * vd[2]));
}

template <int n>
SkMScalar Dot(const SkMScalar* a, const SkMScalar* b)
{
    double total = 0.0;
    for (int i = 0; i < n; ++i)
        total += a[i] * b[i];
    return SkDoubleToMScalar(total);
}

template <int n>
void Combine(SkMScalar* out, const SkMScalar* a, const SkMScalar* b, double scale_a, double scale_b)
{
    for (int i = 0; i < n; ++i)
        out[i] = SkDoubleToMScalar(a[i] * scale_a + b[i] * scale_b);
}

void Cross3(SkMScalar out[3], SkMScalar a[3], SkMScalar b[3]) {
    SkMScalar x = a[1] * b[2] - a[2] * b[1];
    SkMScalar y = a[2] * b[0] - a[0] * b[2];
    SkMScalar z = a[0] * b[1] - a[1] * b[0];
    out[0] = x;
    out[1] = y;
    out[2] = z;
}

// Taken from http://www.w3.org/TR/css3-transforms/.
bool Slerp(SkMScalar out[4], const SkMScalar q1[4], const SkMScalar q2[4], double progress)
{
    double product = Dot<4>(q1, q2);

    // Clamp product to -1.0 <= product <= 1.0.
    product = std::min(std::max(product, -1.0), 1.0);

    const double epsilon = 1e-5;
    if (std::abs(product - 1.0) < epsilon) {
        for (int i = 0; i < 4; ++i)
            out[i] = q1[i];
        return true;
    }

    // TODO(vmpstr): In case the product is -1, the vectors are exactly opposite
    // of each other. In this case, it's technically not correct to just pick one
    // of the vectors, we instead need to pick how to interpolate. However, the
    // spec isn't clear on this. If we don't handle the -1 case explicitly, it
    // results in inf and nans however, which is worse. See crbug.com/506543 for
    // more discussion.
    if (std::abs(product + 1.0) < epsilon) {
        for (int i = 0; i < 4; ++i)
            out[i] = q1[i];
        return true;
    }

    double denom = std::sqrt(1.0 - product * product);
    double theta = std::acos(product);
    double w = std::sin(progress * theta) * (1.0 / denom);

    double scale1 = std::cos(progress * theta) - product * w;
    double scale2 = w;
    Combine<4>(out, q1, q2, scale1, scale2);

    return true;
}

bool Normalize(SkMatrix44& m) 
{
    if (m.get(3, 3) == 0.0)
        // Cannot normalize.
        return false;

    SkMScalar scale = SK_MScalar1 / m.get(3, 3);
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m.set(i, j, m.get(i, j) * scale);

    return true;
}

// Taken from http://www.w3.org/TR/css3-transforms/.
bool decomposeTransform(DecomposedTransform* decomp, const SkMatrix44& matrixIn) 
{
    if (!decomp)
        return false;

    // We'll operate on a copy of the matrixIn.
    SkMatrix44 matrix = matrixIn;

    // If we cannot normalize the matrix, then bail early as we cannot decompose.
    if (!Normalize(matrix))
        return false;

    SkMatrix44 perspectiveMatrix = matrix;

    for (int i = 0; i < 3; ++i)
        perspectiveMatrix.set(3, i, 0.0);

    perspectiveMatrix.set(3, 3, 1.0);

    // If the perspective matrix is not invertible, we are also unable to
    // decompose, so we'll bail early. Constant taken from SkMatrix44::invert.
    if (std::abs(perspectiveMatrix.determinant()) < 1e-8)
        return false;

    if (matrix.get(3, 0) != 0.0 || matrix.get(3, 1) != 0.0 ||
        matrix.get(3, 2) != 0.0) {
        // rhs is the right hand side of the equation.
        SkMScalar rhs[4] = {
            matrix.get(3, 0),
            matrix.get(3, 1),
            matrix.get(3, 2),
            matrix.get(3, 3)
        };

        // Solve the equation by inverting perspectiveMatrix and multiplying
        // rhs by the inverse.
        SkMatrix44 inversePerspectiveMatrix(SkMatrix44::kUninitialized_Constructor);
        if (!perspectiveMatrix.invert(&inversePerspectiveMatrix))
            return false;

        SkMatrix44 transposedInversePerspectiveMatrix = inversePerspectiveMatrix;

        transposedInversePerspectiveMatrix.transpose();
        transposedInversePerspectiveMatrix.mapMScalars(rhs);

        for (int i = 0; i < 4; ++i)
            decomp->perspective[i] = rhs[i];

    } else {
        // No perspective.
        for (int i = 0; i < 3; ++i)
            decomp->perspective[i] = 0.0;
        decomp->perspective[3] = 1.0;
    }

    for (int i = 0; i < 3; i++)
        decomp->translate[i] = matrix.get(i, 3);

    SkMScalar row[3][3];
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; ++j)
            row[i][j] = matrix.get(j, i);

    // Compute X scale factor and normalize first row.
    decomp->scale[0] = Length3(row[0]);
    if (decomp->scale[0] != 0.0) {
        row[0][0] /= decomp->scale[0];
        row[0][1] /= decomp->scale[0];
        row[0][2] /= decomp->scale[0];
    }

    // Compute XY shear factor and make 2nd row orthogonal to 1st.
    decomp->skew[0] = Dot<3>(row[0], row[1]);
    Combine<3>(row[1], row[1], row[0], 1.0, -decomp->skew[0]);

    // Now, compute Y scale and normalize 2nd row.
    decomp->scale[1] = Length3(row[1]);
    if (decomp->scale[1] != 0.0) {
        row[1][0] /= decomp->scale[1];
        row[1][1] /= decomp->scale[1];
        row[1][2] /= decomp->scale[1];
    }

    decomp->skew[0] /= decomp->scale[1];

    // Compute XZ and YZ shears, orthogonalize 3rd row
    decomp->skew[1] = Dot<3>(row[0], row[2]);
    Combine<3>(row[2], row[2], row[0], 1.0, -decomp->skew[1]);
    decomp->skew[2] = Dot<3>(row[1], row[2]);
    Combine<3>(row[2], row[2], row[1], 1.0, -decomp->skew[2]);

    // Next, get Z scale and normalize 3rd row.
    decomp->scale[2] = Length3(row[2]);
    if (decomp->scale[2] != 0.0) {
        row[2][0] /= decomp->scale[2];
        row[2][1] /= decomp->scale[2];
        row[2][2] /= decomp->scale[2];
    }

    decomp->skew[1] /= decomp->scale[2];
    decomp->skew[2] /= decomp->scale[2];

    // At this point, the matrix (in rows) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
    SkMScalar pdum3[3];
    Cross3(pdum3, row[1], row[2]);
    if (Dot<3>(row[0], pdum3) < 0) {
        for (int i = 0; i < 3; i++) {
            decomp->scale[i] *= -1.0;
            for (int j = 0; j < 3; ++j)
                row[i][j] *= -1.0;
        }
    }

    double row00 = SkMScalarToDouble(row[0][0]);
    double row11 = SkMScalarToDouble(row[1][1]);
    double row22 = SkMScalarToDouble(row[2][2]);
    decomp->quaternion[0] = SkDoubleToMScalar(0.5 * std::sqrt(std::max(1.0 + row00 - row11 - row22, 0.0)));
    decomp->quaternion[1] = SkDoubleToMScalar(0.5 * std::sqrt(std::max(1.0 - row00 + row11 - row22, 0.0)));
    decomp->quaternion[2] = SkDoubleToMScalar(0.5 * std::sqrt(std::max(1.0 - row00 - row11 + row22, 0.0)));
    decomp->quaternion[3] = SkDoubleToMScalar(0.5 * std::sqrt(std::max(1.0 + row00 + row11 + row22, 0.0)));

    if (row[2][1] > row[1][2])
        decomp->quaternion[0] = -decomp->quaternion[0];
    if (row[0][2] > row[2][0])
        decomp->quaternion[1] = -decomp->quaternion[1];
    if (row[1][0] > row[0][1])
        decomp->quaternion[2] = -decomp->quaternion[2];

    return true;
}

bool blendDecomposedTransforms(DecomposedTransform* out, const DecomposedTransform& to, const DecomposedTransform& from, double progress) 
{
    double scalea = progress;
    double scaleb = 1.0 - progress;
    Combine<3>(out->translate, to.translate, from.translate, scalea, scaleb);
    Combine<3>(out->scale, to.scale, from.scale, scalea, scaleb);
    Combine<3>(out->skew, to.skew, from.skew, scalea, scaleb);
    Combine<4>(out->perspective, to.perspective, from.perspective, scalea, scaleb);

    return Slerp(out->quaternion, from.quaternion, to.quaternion, progress);
}

SkMatrix44 composeTransform(
    const SkMatrix44& perspective,
    const SkMatrix44& translation,
    const SkMatrix44& rotation,
    const SkMatrix44& skew,
    const SkMatrix44& scale
    ) 
{
    SkMatrix44 matrix(SkMatrix44::kIdentity_Constructor);

    matrix.preConcat(perspective);
    matrix.preConcat(translation);
    matrix.preConcat(rotation);
    matrix.preConcat(skew);
    matrix.preConcat(scale);

    return matrix;
}

SkMatrix44 BuildPerspectiveMatrix(const DecomposedTransform& decomp) 
{
    SkMatrix44 matrix(SkMatrix44::kIdentity_Constructor);

    for (int i = 0; i < 4; i++)
        matrix.setDouble(3, i, decomp.perspective[i]);
    return matrix;
}

SkMatrix44 BuildTranslationMatrix(const DecomposedTransform& decomp) 
{
    SkMatrix44 matrix(SkMatrix44::kUninitialized_Constructor);
    // Implicitly calls matrix.setIdentity()
    matrix.setTranslate(SkDoubleToMScalar(decomp.translate[0]),
        SkDoubleToMScalar(decomp.translate[1]),
        SkDoubleToMScalar(decomp.translate[2]));
    return matrix;
}

SkMatrix44 BuildRotationMatrix(const DecomposedTransform& decomp) 
{
    double x = decomp.quaternion[0];
    double y = decomp.quaternion[1];
    double z = decomp.quaternion[2];
    double w = decomp.quaternion[3];

    SkMatrix44 matrix(SkMatrix44::kUninitialized_Constructor);

    // Implicitly calls matrix.setIdentity()
    matrix.set3x3(SkDoubleToMScalar(1.0 - 2.0 * (y * y + z * z)),
        SkDoubleToMScalar(2.0 * (x * y + z * w)),
        SkDoubleToMScalar(2.0 * (x * z - y * w)),
        SkDoubleToMScalar(2.0 * (x * y - z * w)),
        SkDoubleToMScalar(1.0 - 2.0 * (x * x + z * z)),
        SkDoubleToMScalar(2.0 * (y * z + x * w)),
        SkDoubleToMScalar(2.0 * (x * z + y * w)),
        SkDoubleToMScalar(2.0 * (y * z - x * w)),
        SkDoubleToMScalar(1.0 - 2.0 * (x * x + y * y)));
    return matrix;
}

SkMatrix44 BuildSkewMatrix(const DecomposedTransform& decomp) {
    SkMatrix44 matrix(SkMatrix44::kIdentity_Constructor);

    SkMatrix44 temp(SkMatrix44::kIdentity_Constructor);
    if (decomp.skew[2]) {
        temp.setDouble(1, 2, decomp.skew[2]);
        matrix.preConcat(temp);
    }

    if (decomp.skew[1]) {
        temp.setDouble(1, 2, 0);
        temp.setDouble(0, 2, decomp.skew[1]);
        matrix.preConcat(temp);
    }

    if (decomp.skew[0]) {
        temp.setDouble(0, 2, 0);
        temp.setDouble(0, 1, decomp.skew[0]);
        matrix.preConcat(temp);
    }
    return matrix;
}

SkMatrix44 BuildScaleMatrix(const DecomposedTransform& decomp) 
{
    SkMatrix44 matrix(SkMatrix44::kUninitialized_Constructor);
    matrix.setScale(SkDoubleToMScalar(decomp.scale[0]),
        SkDoubleToMScalar(decomp.scale[1]),
        SkDoubleToMScalar(decomp.scale[2]));
    return matrix;
}

// Taken from http://www.w3.org/TR/css3-transforms/.
SkMatrix44 composeTransform(const DecomposedTransform& decomp) 
{
    SkMatrix44 perspective = BuildPerspectiveMatrix(decomp);
    SkMatrix44 translation = BuildTranslationMatrix(decomp);
    SkMatrix44 rotation = BuildRotationMatrix(decomp);
    SkMatrix44 skew = BuildSkewMatrix(decomp);
    SkMatrix44 scale = BuildScaleMatrix(decomp);

    return composeTransform(perspective, translation, rotation, skew, scale);
}

bool transformBlend(const SkMatrix44& from, double progress, SkMatrix44* inAndOut)
{
    DecomposedTransform to_decomp;
    DecomposedTransform from_decomp;
    if (!decomposeTransform(&to_decomp, *inAndOut) || !decomposeTransform(&from_decomp, from))
        return false;

    if (!blendDecomposedTransforms(&to_decomp, to_decomp, from_decomp, progress))
        return false;

    *inAndOut = composeTransform(to_decomp);
    return true;
}

float dotProduct(const blink::FloatPoint3D& lhs, const blink::FloatPoint3D& rhs)
{
    return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

blink::FloatPoint3D scaleVector3d(const blink::FloatPoint3D& v, float x_scale, float y_scale, float z_scale)
{
    blink::FloatPoint3D scaled_v(v);
    scaled_v.scale(x_scale, y_scale, z_scale);
    return scaled_v;
}

// double crossProduct(const blink::FloatPoint& lhs, const blink::FloatPoint& rhs)
// {
//     return static_cast<double>(lhs.x()) * rhs.y() - static_cast<double>(lhs.y()) * rhs.x();
// }

blink::FloatPoint3D crossProduct(const blink::FloatPoint3D& lhs, const blink::FloatPoint3D& rhs)
{
    blink::FloatPoint3D result = lhs;
    //result.Cross(rhs);

    float x = result.y() * rhs.z() - result.z() * rhs.y();
    float y = result.z() * rhs.x() - result.x() * rhs.z();
    float z = result.x() * rhs.y() - result.y() * rhs.x();
    result.setX(x);
    result.setY(y);
    result.setZ(z);


    return result;
}

void transformPointInternal(const SkMatrix44& xform, blink::FloatPoint3D* point)
{
    if (xform.isIdentity())
        return;

    SkMScalar p[4] = { SkFloatToMScalar(point->x()), SkFloatToMScalar(point->y()), SkFloatToMScalar(point->z()), 1 };

    xform.mapMScalars(p);

    if (p[3] != SK_MScalar1 && p[3] != 0.f) {
        float w_inverse = SK_MScalar1 / p[3];
        *point = blink::FloatPoint3D(p[0] * w_inverse, p[1] * w_inverse, p[2] * w_inverse);
    } else {
        *point = blink::FloatPoint3D(p[0], p[1], p[2]);
    }
}

void transformBox(const SkMatrix44& inMatrix, FloatBox* box)
{
    FloatBox bounds;
    bool first_point = true;
    for (int corner = 0; corner < 8; ++corner) {
        blink::FloatPoint3D point = box->origin();
        point += blink::FloatPoint3D(corner & 1 ? box->width() : 0.f,
            corner & 2 ? box->height() : 0.f,
            corner & 4 ? box->depth() : 0.f);

        transformPointInternal(inMatrix, &point);
        if (first_point) {
            bounds.set_origin(point);
            first_point = false;
        } else {
            bounds.ExpandTo(point);
        }
    }
    *box = bounds;
}

}