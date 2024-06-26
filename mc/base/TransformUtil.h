
#ifndef mc_base_TransformUtil_h
#define mc_base_TransformUtil_h

#include "mc/base/FloatBox.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint3D.h"

namespace mc {
    
// Contains the components of a factored transform. These components may be
// blended and recomposed.
struct DecomposedTransform {
    // The default constructor initializes the components in such a way that
    // if used with Compose below, will produce the identity transform.
    DecomposedTransform() 
    {
        translate[0] = translate[1] = translate[2] = 0.0;
        scale[0] = scale[1] = scale[2] = 1.0;
        skew[0] = skew[1] = skew[2] = 0.0;
        perspective[0] = perspective[1] = perspective[2] = 0.0;
        quaternion[0] = quaternion[1] = quaternion[2] = 0.0;
        perspective[3] = quaternion[3] = 1.0;
    }

    SkMScalar translate[3];
    SkMScalar scale[3];
    SkMScalar skew[3];
    SkMScalar perspective[4];
    SkMScalar quaternion[4];

    // Copy and assign are allowed.
};

// Interpolates the decomposed components |to| with |from| using the
// routines described in http://www.w3.org/TR/css3-3d-transform/.
// |progress| is in the range [0, 1] (0 leaves |out| unchanged, and 1
// assigns |from| to |out|).
bool blendDecomposedTransforms(DecomposedTransform* out,
    const DecomposedTransform& to,
    const DecomposedTransform& from,
    double progress);

// Decomposes this transform into its translation, scale, skew, perspective,
// and rotation components following the routines detailed in this spec:
// http://www.w3.org/TR/css3-3d-transforms/.
bool decomposeTransform(DecomposedTransform* out, const SkMatrix44& matrixIn);

// Composes a transform from the given translation, scale, skew, prespective,
// and rotation components following the routines detailed in this spec:
// http://www.w3.org/TR/css3-3d-transforms/.
SkMatrix44 composeTransform(const DecomposedTransform& decomp);

bool transformBlend(const SkMatrix44& from, double progress, SkMatrix44* inAndOut);
float dotProduct(const blink::FloatPoint3D& lhs, const blink::FloatPoint3D& rhs);
void transformPointInternal(const SkMatrix44& xform, blink::FloatPoint3D* point);
blink::FloatPoint3D scaleVector3d(const blink::FloatPoint3D& v, float x_scale, float y_scale, float z_scale);
blink::FloatPoint3D crossProduct(const blink::FloatPoint3D& lhs, const blink::FloatPoint3D& rhs);
void transformBox(const SkMatrix44& inMatrix, FloatBox* box);
void rotateAbout(SkMatrix44* matrix, const blink::FloatPoint3D& axis, double degrees);
void skewOp(SkMatrix44* matrix, double angle_x, double angle_y);
void applyPerspectiveDepth(SkMatrix44* matrix, SkMScalar depth);
SkMScalar tanDegrees(double degrees);

}

#endif // mc_base_TransformUtil_h