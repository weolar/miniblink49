// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGElementTypeHelpers_h
#define SVGElementTypeHelpers_h

#include "core/dom/Element.h"
#include "SVGNames.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {
// Type checking.
class SVGAElement;
void isSVGAElement(const SVGAElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAElement(const SVGAElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::aTag);
}
inline bool isSVGAElement(const SVGElement* element) { return element && isSVGAElement(*element); }
template<typename T> inline bool isSVGAElement(const PassRefPtr<T>& node) { return isSVGAElement(node.get()); }
template<typename T> inline bool isSVGAElement(const RefPtr<T>& node) { return isSVGAElement(node.get()); }
inline bool isSVGAElement(const Node& node) { return node.isSVGElement() && isSVGAElement(toSVGElement(node)); }
inline bool isSVGAElement(const Node* node) { return node && isSVGAElement(*node); }
template <> inline bool isElementOfType<const SVGAElement>(const Node& node) { return isSVGAElement(node); }
template <> inline bool isElementOfType<const SVGAElement>(const SVGElement& element) { return isSVGAElement(element); }

class SVGAnimateMotionElement;
void isSVGAnimateMotionElement(const SVGAnimateMotionElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAnimateMotionElement(const SVGAnimateMotionElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAnimateMotionElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::animateMotionTag);
}
inline bool isSVGAnimateMotionElement(const SVGElement* element) { return element && isSVGAnimateMotionElement(*element); }
template<typename T> inline bool isSVGAnimateMotionElement(const PassRefPtr<T>& node) { return isSVGAnimateMotionElement(node.get()); }
template<typename T> inline bool isSVGAnimateMotionElement(const RefPtr<T>& node) { return isSVGAnimateMotionElement(node.get()); }
inline bool isSVGAnimateMotionElement(const Node& node) { return node.isSVGElement() && isSVGAnimateMotionElement(toSVGElement(node)); }
inline bool isSVGAnimateMotionElement(const Node* node) { return node && isSVGAnimateMotionElement(*node); }
template <> inline bool isElementOfType<const SVGAnimateMotionElement>(const Node& node) { return isSVGAnimateMotionElement(node); }
template <> inline bool isElementOfType<const SVGAnimateMotionElement>(const SVGElement& element) { return isSVGAnimateMotionElement(element); }

class SVGAnimateTransformElement;
void isSVGAnimateTransformElement(const SVGAnimateTransformElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGAnimateTransformElement(const SVGAnimateTransformElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGAnimateTransformElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::animateTransformTag);
}
inline bool isSVGAnimateTransformElement(const SVGElement* element) { return element && isSVGAnimateTransformElement(*element); }
template<typename T> inline bool isSVGAnimateTransformElement(const PassRefPtr<T>& node) { return isSVGAnimateTransformElement(node.get()); }
template<typename T> inline bool isSVGAnimateTransformElement(const RefPtr<T>& node) { return isSVGAnimateTransformElement(node.get()); }
inline bool isSVGAnimateTransformElement(const Node& node) { return node.isSVGElement() && isSVGAnimateTransformElement(toSVGElement(node)); }
inline bool isSVGAnimateTransformElement(const Node* node) { return node && isSVGAnimateTransformElement(*node); }
template <> inline bool isElementOfType<const SVGAnimateTransformElement>(const Node& node) { return isSVGAnimateTransformElement(node); }
template <> inline bool isElementOfType<const SVGAnimateTransformElement>(const SVGElement& element) { return isSVGAnimateTransformElement(element); }

class SVGCircleElement;
void isSVGCircleElement(const SVGCircleElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGCircleElement(const SVGCircleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGCircleElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::circleTag);
}
inline bool isSVGCircleElement(const SVGElement* element) { return element && isSVGCircleElement(*element); }
template<typename T> inline bool isSVGCircleElement(const PassRefPtr<T>& node) { return isSVGCircleElement(node.get()); }
template<typename T> inline bool isSVGCircleElement(const RefPtr<T>& node) { return isSVGCircleElement(node.get()); }
inline bool isSVGCircleElement(const Node& node) { return node.isSVGElement() && isSVGCircleElement(toSVGElement(node)); }
inline bool isSVGCircleElement(const Node* node) { return node && isSVGCircleElement(*node); }
template <> inline bool isElementOfType<const SVGCircleElement>(const Node& node) { return isSVGCircleElement(node); }
template <> inline bool isElementOfType<const SVGCircleElement>(const SVGElement& element) { return isSVGCircleElement(element); }

class SVGClipPathElement;
void isSVGClipPathElement(const SVGClipPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGClipPathElement(const SVGClipPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGClipPathElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::clipPathTag);
}
inline bool isSVGClipPathElement(const SVGElement* element) { return element && isSVGClipPathElement(*element); }
template<typename T> inline bool isSVGClipPathElement(const PassRefPtr<T>& node) { return isSVGClipPathElement(node.get()); }
template<typename T> inline bool isSVGClipPathElement(const RefPtr<T>& node) { return isSVGClipPathElement(node.get()); }
inline bool isSVGClipPathElement(const Node& node) { return node.isSVGElement() && isSVGClipPathElement(toSVGElement(node)); }
inline bool isSVGClipPathElement(const Node* node) { return node && isSVGClipPathElement(*node); }
template <> inline bool isElementOfType<const SVGClipPathElement>(const Node& node) { return isSVGClipPathElement(node); }
template <> inline bool isElementOfType<const SVGClipPathElement>(const SVGElement& element) { return isSVGClipPathElement(element); }

class SVGCursorElement;
void isSVGCursorElement(const SVGCursorElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGCursorElement(const SVGCursorElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGCursorElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::cursorTag);
}
inline bool isSVGCursorElement(const SVGElement* element) { return element && isSVGCursorElement(*element); }
template<typename T> inline bool isSVGCursorElement(const PassRefPtr<T>& node) { return isSVGCursorElement(node.get()); }
template<typename T> inline bool isSVGCursorElement(const RefPtr<T>& node) { return isSVGCursorElement(node.get()); }
inline bool isSVGCursorElement(const Node& node) { return node.isSVGElement() && isSVGCursorElement(toSVGElement(node)); }
inline bool isSVGCursorElement(const Node* node) { return node && isSVGCursorElement(*node); }
template <> inline bool isElementOfType<const SVGCursorElement>(const Node& node) { return isSVGCursorElement(node); }
template <> inline bool isElementOfType<const SVGCursorElement>(const SVGElement& element) { return isSVGCursorElement(element); }

class SVGDefsElement;
void isSVGDefsElement(const SVGDefsElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGDefsElement(const SVGDefsElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGDefsElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::defsTag);
}
inline bool isSVGDefsElement(const SVGElement* element) { return element && isSVGDefsElement(*element); }
template<typename T> inline bool isSVGDefsElement(const PassRefPtr<T>& node) { return isSVGDefsElement(node.get()); }
template<typename T> inline bool isSVGDefsElement(const RefPtr<T>& node) { return isSVGDefsElement(node.get()); }
inline bool isSVGDefsElement(const Node& node) { return node.isSVGElement() && isSVGDefsElement(toSVGElement(node)); }
inline bool isSVGDefsElement(const Node* node) { return node && isSVGDefsElement(*node); }
template <> inline bool isElementOfType<const SVGDefsElement>(const Node& node) { return isSVGDefsElement(node); }
template <> inline bool isElementOfType<const SVGDefsElement>(const SVGElement& element) { return isSVGDefsElement(element); }

class SVGDescElement;
void isSVGDescElement(const SVGDescElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGDescElement(const SVGDescElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGDescElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::descTag);
}
inline bool isSVGDescElement(const SVGElement* element) { return element && isSVGDescElement(*element); }
template<typename T> inline bool isSVGDescElement(const PassRefPtr<T>& node) { return isSVGDescElement(node.get()); }
template<typename T> inline bool isSVGDescElement(const RefPtr<T>& node) { return isSVGDescElement(node.get()); }
inline bool isSVGDescElement(const Node& node) { return node.isSVGElement() && isSVGDescElement(toSVGElement(node)); }
inline bool isSVGDescElement(const Node* node) { return node && isSVGDescElement(*node); }
template <> inline bool isElementOfType<const SVGDescElement>(const Node& node) { return isSVGDescElement(node); }
template <> inline bool isElementOfType<const SVGDescElement>(const SVGElement& element) { return isSVGDescElement(element); }

class SVGDiscardElement;
void isSVGDiscardElement(const SVGDiscardElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGDiscardElement(const SVGDiscardElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGDiscardElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::discardTag);
}
inline bool isSVGDiscardElement(const SVGElement* element) { return element && isSVGDiscardElement(*element); }
template<typename T> inline bool isSVGDiscardElement(const PassRefPtr<T>& node) { return isSVGDiscardElement(node.get()); }
template<typename T> inline bool isSVGDiscardElement(const RefPtr<T>& node) { return isSVGDiscardElement(node.get()); }
inline bool isSVGDiscardElement(const Node& node) { return node.isSVGElement() && isSVGDiscardElement(toSVGElement(node)); }
inline bool isSVGDiscardElement(const Node* node) { return node && isSVGDiscardElement(*node); }
template <> inline bool isElementOfType<const SVGDiscardElement>(const Node& node) { return isSVGDiscardElement(node); }
template <> inline bool isElementOfType<const SVGDiscardElement>(const SVGElement& element) { return isSVGDiscardElement(element); }

class SVGEllipseElement;
void isSVGEllipseElement(const SVGEllipseElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGEllipseElement(const SVGEllipseElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGEllipseElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::ellipseTag);
}
inline bool isSVGEllipseElement(const SVGElement* element) { return element && isSVGEllipseElement(*element); }
template<typename T> inline bool isSVGEllipseElement(const PassRefPtr<T>& node) { return isSVGEllipseElement(node.get()); }
template<typename T> inline bool isSVGEllipseElement(const RefPtr<T>& node) { return isSVGEllipseElement(node.get()); }
inline bool isSVGEllipseElement(const Node& node) { return node.isSVGElement() && isSVGEllipseElement(toSVGElement(node)); }
inline bool isSVGEllipseElement(const Node* node) { return node && isSVGEllipseElement(*node); }
template <> inline bool isElementOfType<const SVGEllipseElement>(const Node& node) { return isSVGEllipseElement(node); }
template <> inline bool isElementOfType<const SVGEllipseElement>(const SVGElement& element) { return isSVGEllipseElement(element); }

class SVGFEBlendElement;
void isSVGFEBlendElement(const SVGFEBlendElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEBlendElement(const SVGFEBlendElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEBlendElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feBlendTag);
}
inline bool isSVGFEBlendElement(const SVGElement* element) { return element && isSVGFEBlendElement(*element); }
template<typename T> inline bool isSVGFEBlendElement(const PassRefPtr<T>& node) { return isSVGFEBlendElement(node.get()); }
template<typename T> inline bool isSVGFEBlendElement(const RefPtr<T>& node) { return isSVGFEBlendElement(node.get()); }
inline bool isSVGFEBlendElement(const Node& node) { return node.isSVGElement() && isSVGFEBlendElement(toSVGElement(node)); }
inline bool isSVGFEBlendElement(const Node* node) { return node && isSVGFEBlendElement(*node); }
template <> inline bool isElementOfType<const SVGFEBlendElement>(const Node& node) { return isSVGFEBlendElement(node); }
template <> inline bool isElementOfType<const SVGFEBlendElement>(const SVGElement& element) { return isSVGFEBlendElement(element); }

class SVGFEColorMatrixElement;
void isSVGFEColorMatrixElement(const SVGFEColorMatrixElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEColorMatrixElement(const SVGFEColorMatrixElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEColorMatrixElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feColorMatrixTag);
}
inline bool isSVGFEColorMatrixElement(const SVGElement* element) { return element && isSVGFEColorMatrixElement(*element); }
template<typename T> inline bool isSVGFEColorMatrixElement(const PassRefPtr<T>& node) { return isSVGFEColorMatrixElement(node.get()); }
template<typename T> inline bool isSVGFEColorMatrixElement(const RefPtr<T>& node) { return isSVGFEColorMatrixElement(node.get()); }
inline bool isSVGFEColorMatrixElement(const Node& node) { return node.isSVGElement() && isSVGFEColorMatrixElement(toSVGElement(node)); }
inline bool isSVGFEColorMatrixElement(const Node* node) { return node && isSVGFEColorMatrixElement(*node); }
template <> inline bool isElementOfType<const SVGFEColorMatrixElement>(const Node& node) { return isSVGFEColorMatrixElement(node); }
template <> inline bool isElementOfType<const SVGFEColorMatrixElement>(const SVGElement& element) { return isSVGFEColorMatrixElement(element); }

class SVGFEComponentTransferElement;
void isSVGFEComponentTransferElement(const SVGFEComponentTransferElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEComponentTransferElement(const SVGFEComponentTransferElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEComponentTransferElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feComponentTransferTag);
}
inline bool isSVGFEComponentTransferElement(const SVGElement* element) { return element && isSVGFEComponentTransferElement(*element); }
template<typename T> inline bool isSVGFEComponentTransferElement(const PassRefPtr<T>& node) { return isSVGFEComponentTransferElement(node.get()); }
template<typename T> inline bool isSVGFEComponentTransferElement(const RefPtr<T>& node) { return isSVGFEComponentTransferElement(node.get()); }
inline bool isSVGFEComponentTransferElement(const Node& node) { return node.isSVGElement() && isSVGFEComponentTransferElement(toSVGElement(node)); }
inline bool isSVGFEComponentTransferElement(const Node* node) { return node && isSVGFEComponentTransferElement(*node); }
template <> inline bool isElementOfType<const SVGFEComponentTransferElement>(const Node& node) { return isSVGFEComponentTransferElement(node); }
template <> inline bool isElementOfType<const SVGFEComponentTransferElement>(const SVGElement& element) { return isSVGFEComponentTransferElement(element); }

class SVGFECompositeElement;
void isSVGFECompositeElement(const SVGFECompositeElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFECompositeElement(const SVGFECompositeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFECompositeElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feCompositeTag);
}
inline bool isSVGFECompositeElement(const SVGElement* element) { return element && isSVGFECompositeElement(*element); }
template<typename T> inline bool isSVGFECompositeElement(const PassRefPtr<T>& node) { return isSVGFECompositeElement(node.get()); }
template<typename T> inline bool isSVGFECompositeElement(const RefPtr<T>& node) { return isSVGFECompositeElement(node.get()); }
inline bool isSVGFECompositeElement(const Node& node) { return node.isSVGElement() && isSVGFECompositeElement(toSVGElement(node)); }
inline bool isSVGFECompositeElement(const Node* node) { return node && isSVGFECompositeElement(*node); }
template <> inline bool isElementOfType<const SVGFECompositeElement>(const Node& node) { return isSVGFECompositeElement(node); }
template <> inline bool isElementOfType<const SVGFECompositeElement>(const SVGElement& element) { return isSVGFECompositeElement(element); }

class SVGFEConvolveMatrixElement;
void isSVGFEConvolveMatrixElement(const SVGFEConvolveMatrixElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEConvolveMatrixElement(const SVGFEConvolveMatrixElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEConvolveMatrixElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feConvolveMatrixTag);
}
inline bool isSVGFEConvolveMatrixElement(const SVGElement* element) { return element && isSVGFEConvolveMatrixElement(*element); }
template<typename T> inline bool isSVGFEConvolveMatrixElement(const PassRefPtr<T>& node) { return isSVGFEConvolveMatrixElement(node.get()); }
template<typename T> inline bool isSVGFEConvolveMatrixElement(const RefPtr<T>& node) { return isSVGFEConvolveMatrixElement(node.get()); }
inline bool isSVGFEConvolveMatrixElement(const Node& node) { return node.isSVGElement() && isSVGFEConvolveMatrixElement(toSVGElement(node)); }
inline bool isSVGFEConvolveMatrixElement(const Node* node) { return node && isSVGFEConvolveMatrixElement(*node); }
template <> inline bool isElementOfType<const SVGFEConvolveMatrixElement>(const Node& node) { return isSVGFEConvolveMatrixElement(node); }
template <> inline bool isElementOfType<const SVGFEConvolveMatrixElement>(const SVGElement& element) { return isSVGFEConvolveMatrixElement(element); }

class SVGFEDiffuseLightingElement;
void isSVGFEDiffuseLightingElement(const SVGFEDiffuseLightingElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDiffuseLightingElement(const SVGFEDiffuseLightingElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDiffuseLightingElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feDiffuseLightingTag);
}
inline bool isSVGFEDiffuseLightingElement(const SVGElement* element) { return element && isSVGFEDiffuseLightingElement(*element); }
template<typename T> inline bool isSVGFEDiffuseLightingElement(const PassRefPtr<T>& node) { return isSVGFEDiffuseLightingElement(node.get()); }
template<typename T> inline bool isSVGFEDiffuseLightingElement(const RefPtr<T>& node) { return isSVGFEDiffuseLightingElement(node.get()); }
inline bool isSVGFEDiffuseLightingElement(const Node& node) { return node.isSVGElement() && isSVGFEDiffuseLightingElement(toSVGElement(node)); }
inline bool isSVGFEDiffuseLightingElement(const Node* node) { return node && isSVGFEDiffuseLightingElement(*node); }
template <> inline bool isElementOfType<const SVGFEDiffuseLightingElement>(const Node& node) { return isSVGFEDiffuseLightingElement(node); }
template <> inline bool isElementOfType<const SVGFEDiffuseLightingElement>(const SVGElement& element) { return isSVGFEDiffuseLightingElement(element); }

class SVGFEDisplacementMapElement;
void isSVGFEDisplacementMapElement(const SVGFEDisplacementMapElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDisplacementMapElement(const SVGFEDisplacementMapElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDisplacementMapElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feDisplacementMapTag);
}
inline bool isSVGFEDisplacementMapElement(const SVGElement* element) { return element && isSVGFEDisplacementMapElement(*element); }
template<typename T> inline bool isSVGFEDisplacementMapElement(const PassRefPtr<T>& node) { return isSVGFEDisplacementMapElement(node.get()); }
template<typename T> inline bool isSVGFEDisplacementMapElement(const RefPtr<T>& node) { return isSVGFEDisplacementMapElement(node.get()); }
inline bool isSVGFEDisplacementMapElement(const Node& node) { return node.isSVGElement() && isSVGFEDisplacementMapElement(toSVGElement(node)); }
inline bool isSVGFEDisplacementMapElement(const Node* node) { return node && isSVGFEDisplacementMapElement(*node); }
template <> inline bool isElementOfType<const SVGFEDisplacementMapElement>(const Node& node) { return isSVGFEDisplacementMapElement(node); }
template <> inline bool isElementOfType<const SVGFEDisplacementMapElement>(const SVGElement& element) { return isSVGFEDisplacementMapElement(element); }

class SVGFEDistantLightElement;
void isSVGFEDistantLightElement(const SVGFEDistantLightElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDistantLightElement(const SVGFEDistantLightElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDistantLightElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feDistantLightTag);
}
inline bool isSVGFEDistantLightElement(const SVGElement* element) { return element && isSVGFEDistantLightElement(*element); }
template<typename T> inline bool isSVGFEDistantLightElement(const PassRefPtr<T>& node) { return isSVGFEDistantLightElement(node.get()); }
template<typename T> inline bool isSVGFEDistantLightElement(const RefPtr<T>& node) { return isSVGFEDistantLightElement(node.get()); }
inline bool isSVGFEDistantLightElement(const Node& node) { return node.isSVGElement() && isSVGFEDistantLightElement(toSVGElement(node)); }
inline bool isSVGFEDistantLightElement(const Node* node) { return node && isSVGFEDistantLightElement(*node); }
template <> inline bool isElementOfType<const SVGFEDistantLightElement>(const Node& node) { return isSVGFEDistantLightElement(node); }
template <> inline bool isElementOfType<const SVGFEDistantLightElement>(const SVGElement& element) { return isSVGFEDistantLightElement(element); }

class SVGFEDropShadowElement;
void isSVGFEDropShadowElement(const SVGFEDropShadowElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEDropShadowElement(const SVGFEDropShadowElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEDropShadowElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feDropShadowTag);
}
inline bool isSVGFEDropShadowElement(const SVGElement* element) { return element && isSVGFEDropShadowElement(*element); }
template<typename T> inline bool isSVGFEDropShadowElement(const PassRefPtr<T>& node) { return isSVGFEDropShadowElement(node.get()); }
template<typename T> inline bool isSVGFEDropShadowElement(const RefPtr<T>& node) { return isSVGFEDropShadowElement(node.get()); }
inline bool isSVGFEDropShadowElement(const Node& node) { return node.isSVGElement() && isSVGFEDropShadowElement(toSVGElement(node)); }
inline bool isSVGFEDropShadowElement(const Node* node) { return node && isSVGFEDropShadowElement(*node); }
template <> inline bool isElementOfType<const SVGFEDropShadowElement>(const Node& node) { return isSVGFEDropShadowElement(node); }
template <> inline bool isElementOfType<const SVGFEDropShadowElement>(const SVGElement& element) { return isSVGFEDropShadowElement(element); }

class SVGFEFloodElement;
void isSVGFEFloodElement(const SVGFEFloodElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFloodElement(const SVGFEFloodElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFloodElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feFloodTag);
}
inline bool isSVGFEFloodElement(const SVGElement* element) { return element && isSVGFEFloodElement(*element); }
template<typename T> inline bool isSVGFEFloodElement(const PassRefPtr<T>& node) { return isSVGFEFloodElement(node.get()); }
template<typename T> inline bool isSVGFEFloodElement(const RefPtr<T>& node) { return isSVGFEFloodElement(node.get()); }
inline bool isSVGFEFloodElement(const Node& node) { return node.isSVGElement() && isSVGFEFloodElement(toSVGElement(node)); }
inline bool isSVGFEFloodElement(const Node* node) { return node && isSVGFEFloodElement(*node); }
template <> inline bool isElementOfType<const SVGFEFloodElement>(const Node& node) { return isSVGFEFloodElement(node); }
template <> inline bool isElementOfType<const SVGFEFloodElement>(const SVGElement& element) { return isSVGFEFloodElement(element); }

class SVGFEFuncAElement;
void isSVGFEFuncAElement(const SVGFEFuncAElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncAElement(const SVGFEFuncAElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncAElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feFuncATag);
}
inline bool isSVGFEFuncAElement(const SVGElement* element) { return element && isSVGFEFuncAElement(*element); }
template<typename T> inline bool isSVGFEFuncAElement(const PassRefPtr<T>& node) { return isSVGFEFuncAElement(node.get()); }
template<typename T> inline bool isSVGFEFuncAElement(const RefPtr<T>& node) { return isSVGFEFuncAElement(node.get()); }
inline bool isSVGFEFuncAElement(const Node& node) { return node.isSVGElement() && isSVGFEFuncAElement(toSVGElement(node)); }
inline bool isSVGFEFuncAElement(const Node* node) { return node && isSVGFEFuncAElement(*node); }
template <> inline bool isElementOfType<const SVGFEFuncAElement>(const Node& node) { return isSVGFEFuncAElement(node); }
template <> inline bool isElementOfType<const SVGFEFuncAElement>(const SVGElement& element) { return isSVGFEFuncAElement(element); }

class SVGFEFuncBElement;
void isSVGFEFuncBElement(const SVGFEFuncBElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncBElement(const SVGFEFuncBElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncBElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feFuncBTag);
}
inline bool isSVGFEFuncBElement(const SVGElement* element) { return element && isSVGFEFuncBElement(*element); }
template<typename T> inline bool isSVGFEFuncBElement(const PassRefPtr<T>& node) { return isSVGFEFuncBElement(node.get()); }
template<typename T> inline bool isSVGFEFuncBElement(const RefPtr<T>& node) { return isSVGFEFuncBElement(node.get()); }
inline bool isSVGFEFuncBElement(const Node& node) { return node.isSVGElement() && isSVGFEFuncBElement(toSVGElement(node)); }
inline bool isSVGFEFuncBElement(const Node* node) { return node && isSVGFEFuncBElement(*node); }
template <> inline bool isElementOfType<const SVGFEFuncBElement>(const Node& node) { return isSVGFEFuncBElement(node); }
template <> inline bool isElementOfType<const SVGFEFuncBElement>(const SVGElement& element) { return isSVGFEFuncBElement(element); }

class SVGFEFuncGElement;
void isSVGFEFuncGElement(const SVGFEFuncGElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncGElement(const SVGFEFuncGElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncGElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feFuncGTag);
}
inline bool isSVGFEFuncGElement(const SVGElement* element) { return element && isSVGFEFuncGElement(*element); }
template<typename T> inline bool isSVGFEFuncGElement(const PassRefPtr<T>& node) { return isSVGFEFuncGElement(node.get()); }
template<typename T> inline bool isSVGFEFuncGElement(const RefPtr<T>& node) { return isSVGFEFuncGElement(node.get()); }
inline bool isSVGFEFuncGElement(const Node& node) { return node.isSVGElement() && isSVGFEFuncGElement(toSVGElement(node)); }
inline bool isSVGFEFuncGElement(const Node* node) { return node && isSVGFEFuncGElement(*node); }
template <> inline bool isElementOfType<const SVGFEFuncGElement>(const Node& node) { return isSVGFEFuncGElement(node); }
template <> inline bool isElementOfType<const SVGFEFuncGElement>(const SVGElement& element) { return isSVGFEFuncGElement(element); }

class SVGFEFuncRElement;
void isSVGFEFuncRElement(const SVGFEFuncRElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEFuncRElement(const SVGFEFuncRElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEFuncRElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feFuncRTag);
}
inline bool isSVGFEFuncRElement(const SVGElement* element) { return element && isSVGFEFuncRElement(*element); }
template<typename T> inline bool isSVGFEFuncRElement(const PassRefPtr<T>& node) { return isSVGFEFuncRElement(node.get()); }
template<typename T> inline bool isSVGFEFuncRElement(const RefPtr<T>& node) { return isSVGFEFuncRElement(node.get()); }
inline bool isSVGFEFuncRElement(const Node& node) { return node.isSVGElement() && isSVGFEFuncRElement(toSVGElement(node)); }
inline bool isSVGFEFuncRElement(const Node* node) { return node && isSVGFEFuncRElement(*node); }
template <> inline bool isElementOfType<const SVGFEFuncRElement>(const Node& node) { return isSVGFEFuncRElement(node); }
template <> inline bool isElementOfType<const SVGFEFuncRElement>(const SVGElement& element) { return isSVGFEFuncRElement(element); }

class SVGFEGaussianBlurElement;
void isSVGFEGaussianBlurElement(const SVGFEGaussianBlurElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEGaussianBlurElement(const SVGFEGaussianBlurElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEGaussianBlurElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feGaussianBlurTag);
}
inline bool isSVGFEGaussianBlurElement(const SVGElement* element) { return element && isSVGFEGaussianBlurElement(*element); }
template<typename T> inline bool isSVGFEGaussianBlurElement(const PassRefPtr<T>& node) { return isSVGFEGaussianBlurElement(node.get()); }
template<typename T> inline bool isSVGFEGaussianBlurElement(const RefPtr<T>& node) { return isSVGFEGaussianBlurElement(node.get()); }
inline bool isSVGFEGaussianBlurElement(const Node& node) { return node.isSVGElement() && isSVGFEGaussianBlurElement(toSVGElement(node)); }
inline bool isSVGFEGaussianBlurElement(const Node* node) { return node && isSVGFEGaussianBlurElement(*node); }
template <> inline bool isElementOfType<const SVGFEGaussianBlurElement>(const Node& node) { return isSVGFEGaussianBlurElement(node); }
template <> inline bool isElementOfType<const SVGFEGaussianBlurElement>(const SVGElement& element) { return isSVGFEGaussianBlurElement(element); }

class SVGFEImageElement;
void isSVGFEImageElement(const SVGFEImageElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEImageElement(const SVGFEImageElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEImageElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feImageTag);
}
inline bool isSVGFEImageElement(const SVGElement* element) { return element && isSVGFEImageElement(*element); }
template<typename T> inline bool isSVGFEImageElement(const PassRefPtr<T>& node) { return isSVGFEImageElement(node.get()); }
template<typename T> inline bool isSVGFEImageElement(const RefPtr<T>& node) { return isSVGFEImageElement(node.get()); }
inline bool isSVGFEImageElement(const Node& node) { return node.isSVGElement() && isSVGFEImageElement(toSVGElement(node)); }
inline bool isSVGFEImageElement(const Node* node) { return node && isSVGFEImageElement(*node); }
template <> inline bool isElementOfType<const SVGFEImageElement>(const Node& node) { return isSVGFEImageElement(node); }
template <> inline bool isElementOfType<const SVGFEImageElement>(const SVGElement& element) { return isSVGFEImageElement(element); }

class SVGFEMergeElement;
void isSVGFEMergeElement(const SVGFEMergeElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEMergeElement(const SVGFEMergeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEMergeElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feMergeTag);
}
inline bool isSVGFEMergeElement(const SVGElement* element) { return element && isSVGFEMergeElement(*element); }
template<typename T> inline bool isSVGFEMergeElement(const PassRefPtr<T>& node) { return isSVGFEMergeElement(node.get()); }
template<typename T> inline bool isSVGFEMergeElement(const RefPtr<T>& node) { return isSVGFEMergeElement(node.get()); }
inline bool isSVGFEMergeElement(const Node& node) { return node.isSVGElement() && isSVGFEMergeElement(toSVGElement(node)); }
inline bool isSVGFEMergeElement(const Node* node) { return node && isSVGFEMergeElement(*node); }
template <> inline bool isElementOfType<const SVGFEMergeElement>(const Node& node) { return isSVGFEMergeElement(node); }
template <> inline bool isElementOfType<const SVGFEMergeElement>(const SVGElement& element) { return isSVGFEMergeElement(element); }

class SVGFEMergeNodeElement;
void isSVGFEMergeNodeElement(const SVGFEMergeNodeElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEMergeNodeElement(const SVGFEMergeNodeElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEMergeNodeElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feMergeNodeTag);
}
inline bool isSVGFEMergeNodeElement(const SVGElement* element) { return element && isSVGFEMergeNodeElement(*element); }
template<typename T> inline bool isSVGFEMergeNodeElement(const PassRefPtr<T>& node) { return isSVGFEMergeNodeElement(node.get()); }
template<typename T> inline bool isSVGFEMergeNodeElement(const RefPtr<T>& node) { return isSVGFEMergeNodeElement(node.get()); }
inline bool isSVGFEMergeNodeElement(const Node& node) { return node.isSVGElement() && isSVGFEMergeNodeElement(toSVGElement(node)); }
inline bool isSVGFEMergeNodeElement(const Node* node) { return node && isSVGFEMergeNodeElement(*node); }
template <> inline bool isElementOfType<const SVGFEMergeNodeElement>(const Node& node) { return isSVGFEMergeNodeElement(node); }
template <> inline bool isElementOfType<const SVGFEMergeNodeElement>(const SVGElement& element) { return isSVGFEMergeNodeElement(element); }

class SVGFEMorphologyElement;
void isSVGFEMorphologyElement(const SVGFEMorphologyElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEMorphologyElement(const SVGFEMorphologyElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEMorphologyElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feMorphologyTag);
}
inline bool isSVGFEMorphologyElement(const SVGElement* element) { return element && isSVGFEMorphologyElement(*element); }
template<typename T> inline bool isSVGFEMorphologyElement(const PassRefPtr<T>& node) { return isSVGFEMorphologyElement(node.get()); }
template<typename T> inline bool isSVGFEMorphologyElement(const RefPtr<T>& node) { return isSVGFEMorphologyElement(node.get()); }
inline bool isSVGFEMorphologyElement(const Node& node) { return node.isSVGElement() && isSVGFEMorphologyElement(toSVGElement(node)); }
inline bool isSVGFEMorphologyElement(const Node* node) { return node && isSVGFEMorphologyElement(*node); }
template <> inline bool isElementOfType<const SVGFEMorphologyElement>(const Node& node) { return isSVGFEMorphologyElement(node); }
template <> inline bool isElementOfType<const SVGFEMorphologyElement>(const SVGElement& element) { return isSVGFEMorphologyElement(element); }

class SVGFEOffsetElement;
void isSVGFEOffsetElement(const SVGFEOffsetElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEOffsetElement(const SVGFEOffsetElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEOffsetElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feOffsetTag);
}
inline bool isSVGFEOffsetElement(const SVGElement* element) { return element && isSVGFEOffsetElement(*element); }
template<typename T> inline bool isSVGFEOffsetElement(const PassRefPtr<T>& node) { return isSVGFEOffsetElement(node.get()); }
template<typename T> inline bool isSVGFEOffsetElement(const RefPtr<T>& node) { return isSVGFEOffsetElement(node.get()); }
inline bool isSVGFEOffsetElement(const Node& node) { return node.isSVGElement() && isSVGFEOffsetElement(toSVGElement(node)); }
inline bool isSVGFEOffsetElement(const Node* node) { return node && isSVGFEOffsetElement(*node); }
template <> inline bool isElementOfType<const SVGFEOffsetElement>(const Node& node) { return isSVGFEOffsetElement(node); }
template <> inline bool isElementOfType<const SVGFEOffsetElement>(const SVGElement& element) { return isSVGFEOffsetElement(element); }

class SVGFEPointLightElement;
void isSVGFEPointLightElement(const SVGFEPointLightElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFEPointLightElement(const SVGFEPointLightElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFEPointLightElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::fePointLightTag);
}
inline bool isSVGFEPointLightElement(const SVGElement* element) { return element && isSVGFEPointLightElement(*element); }
template<typename T> inline bool isSVGFEPointLightElement(const PassRefPtr<T>& node) { return isSVGFEPointLightElement(node.get()); }
template<typename T> inline bool isSVGFEPointLightElement(const RefPtr<T>& node) { return isSVGFEPointLightElement(node.get()); }
inline bool isSVGFEPointLightElement(const Node& node) { return node.isSVGElement() && isSVGFEPointLightElement(toSVGElement(node)); }
inline bool isSVGFEPointLightElement(const Node* node) { return node && isSVGFEPointLightElement(*node); }
template <> inline bool isElementOfType<const SVGFEPointLightElement>(const Node& node) { return isSVGFEPointLightElement(node); }
template <> inline bool isElementOfType<const SVGFEPointLightElement>(const SVGElement& element) { return isSVGFEPointLightElement(element); }

class SVGFESpecularLightingElement;
void isSVGFESpecularLightingElement(const SVGFESpecularLightingElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFESpecularLightingElement(const SVGFESpecularLightingElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFESpecularLightingElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feSpecularLightingTag);
}
inline bool isSVGFESpecularLightingElement(const SVGElement* element) { return element && isSVGFESpecularLightingElement(*element); }
template<typename T> inline bool isSVGFESpecularLightingElement(const PassRefPtr<T>& node) { return isSVGFESpecularLightingElement(node.get()); }
template<typename T> inline bool isSVGFESpecularLightingElement(const RefPtr<T>& node) { return isSVGFESpecularLightingElement(node.get()); }
inline bool isSVGFESpecularLightingElement(const Node& node) { return node.isSVGElement() && isSVGFESpecularLightingElement(toSVGElement(node)); }
inline bool isSVGFESpecularLightingElement(const Node* node) { return node && isSVGFESpecularLightingElement(*node); }
template <> inline bool isElementOfType<const SVGFESpecularLightingElement>(const Node& node) { return isSVGFESpecularLightingElement(node); }
template <> inline bool isElementOfType<const SVGFESpecularLightingElement>(const SVGElement& element) { return isSVGFESpecularLightingElement(element); }

class SVGFESpotLightElement;
void isSVGFESpotLightElement(const SVGFESpotLightElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFESpotLightElement(const SVGFESpotLightElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFESpotLightElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feSpotLightTag);
}
inline bool isSVGFESpotLightElement(const SVGElement* element) { return element && isSVGFESpotLightElement(*element); }
template<typename T> inline bool isSVGFESpotLightElement(const PassRefPtr<T>& node) { return isSVGFESpotLightElement(node.get()); }
template<typename T> inline bool isSVGFESpotLightElement(const RefPtr<T>& node) { return isSVGFESpotLightElement(node.get()); }
inline bool isSVGFESpotLightElement(const Node& node) { return node.isSVGElement() && isSVGFESpotLightElement(toSVGElement(node)); }
inline bool isSVGFESpotLightElement(const Node* node) { return node && isSVGFESpotLightElement(*node); }
template <> inline bool isElementOfType<const SVGFESpotLightElement>(const Node& node) { return isSVGFESpotLightElement(node); }
template <> inline bool isElementOfType<const SVGFESpotLightElement>(const SVGElement& element) { return isSVGFESpotLightElement(element); }

class SVGFETileElement;
void isSVGFETileElement(const SVGFETileElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFETileElement(const SVGFETileElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFETileElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feTileTag);
}
inline bool isSVGFETileElement(const SVGElement* element) { return element && isSVGFETileElement(*element); }
template<typename T> inline bool isSVGFETileElement(const PassRefPtr<T>& node) { return isSVGFETileElement(node.get()); }
template<typename T> inline bool isSVGFETileElement(const RefPtr<T>& node) { return isSVGFETileElement(node.get()); }
inline bool isSVGFETileElement(const Node& node) { return node.isSVGElement() && isSVGFETileElement(toSVGElement(node)); }
inline bool isSVGFETileElement(const Node* node) { return node && isSVGFETileElement(*node); }
template <> inline bool isElementOfType<const SVGFETileElement>(const Node& node) { return isSVGFETileElement(node); }
template <> inline bool isElementOfType<const SVGFETileElement>(const SVGElement& element) { return isSVGFETileElement(element); }

class SVGFETurbulenceElement;
void isSVGFETurbulenceElement(const SVGFETurbulenceElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFETurbulenceElement(const SVGFETurbulenceElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFETurbulenceElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::feTurbulenceTag);
}
inline bool isSVGFETurbulenceElement(const SVGElement* element) { return element && isSVGFETurbulenceElement(*element); }
template<typename T> inline bool isSVGFETurbulenceElement(const PassRefPtr<T>& node) { return isSVGFETurbulenceElement(node.get()); }
template<typename T> inline bool isSVGFETurbulenceElement(const RefPtr<T>& node) { return isSVGFETurbulenceElement(node.get()); }
inline bool isSVGFETurbulenceElement(const Node& node) { return node.isSVGElement() && isSVGFETurbulenceElement(toSVGElement(node)); }
inline bool isSVGFETurbulenceElement(const Node* node) { return node && isSVGFETurbulenceElement(*node); }
template <> inline bool isElementOfType<const SVGFETurbulenceElement>(const Node& node) { return isSVGFETurbulenceElement(node); }
template <> inline bool isElementOfType<const SVGFETurbulenceElement>(const SVGElement& element) { return isSVGFETurbulenceElement(element); }

class SVGFilterElement;
void isSVGFilterElement(const SVGFilterElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGFilterElement(const SVGFilterElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGFilterElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::filterTag);
}
inline bool isSVGFilterElement(const SVGElement* element) { return element && isSVGFilterElement(*element); }
template<typename T> inline bool isSVGFilterElement(const PassRefPtr<T>& node) { return isSVGFilterElement(node.get()); }
template<typename T> inline bool isSVGFilterElement(const RefPtr<T>& node) { return isSVGFilterElement(node.get()); }
inline bool isSVGFilterElement(const Node& node) { return node.isSVGElement() && isSVGFilterElement(toSVGElement(node)); }
inline bool isSVGFilterElement(const Node* node) { return node && isSVGFilterElement(*node); }
template <> inline bool isElementOfType<const SVGFilterElement>(const Node& node) { return isSVGFilterElement(node); }
template <> inline bool isElementOfType<const SVGFilterElement>(const SVGElement& element) { return isSVGFilterElement(element); }

class SVGForeignObjectElement;
void isSVGForeignObjectElement(const SVGForeignObjectElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGForeignObjectElement(const SVGForeignObjectElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGForeignObjectElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::foreignObjectTag);
}
inline bool isSVGForeignObjectElement(const SVGElement* element) { return element && isSVGForeignObjectElement(*element); }
template<typename T> inline bool isSVGForeignObjectElement(const PassRefPtr<T>& node) { return isSVGForeignObjectElement(node.get()); }
template<typename T> inline bool isSVGForeignObjectElement(const RefPtr<T>& node) { return isSVGForeignObjectElement(node.get()); }
inline bool isSVGForeignObjectElement(const Node& node) { return node.isSVGElement() && isSVGForeignObjectElement(toSVGElement(node)); }
inline bool isSVGForeignObjectElement(const Node* node) { return node && isSVGForeignObjectElement(*node); }
template <> inline bool isElementOfType<const SVGForeignObjectElement>(const Node& node) { return isSVGForeignObjectElement(node); }
template <> inline bool isElementOfType<const SVGForeignObjectElement>(const SVGElement& element) { return isSVGForeignObjectElement(element); }

class SVGGElement;
void isSVGGElement(const SVGGElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGGElement(const SVGGElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGGElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::gTag);
}
inline bool isSVGGElement(const SVGElement* element) { return element && isSVGGElement(*element); }
template<typename T> inline bool isSVGGElement(const PassRefPtr<T>& node) { return isSVGGElement(node.get()); }
template<typename T> inline bool isSVGGElement(const RefPtr<T>& node) { return isSVGGElement(node.get()); }
inline bool isSVGGElement(const Node& node) { return node.isSVGElement() && isSVGGElement(toSVGElement(node)); }
inline bool isSVGGElement(const Node* node) { return node && isSVGGElement(*node); }
template <> inline bool isElementOfType<const SVGGElement>(const Node& node) { return isSVGGElement(node); }
template <> inline bool isElementOfType<const SVGGElement>(const SVGElement& element) { return isSVGGElement(element); }

class SVGImageElement;
void isSVGImageElement(const SVGImageElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGImageElement(const SVGImageElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGImageElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::imageTag);
}
inline bool isSVGImageElement(const SVGElement* element) { return element && isSVGImageElement(*element); }
template<typename T> inline bool isSVGImageElement(const PassRefPtr<T>& node) { return isSVGImageElement(node.get()); }
template<typename T> inline bool isSVGImageElement(const RefPtr<T>& node) { return isSVGImageElement(node.get()); }
inline bool isSVGImageElement(const Node& node) { return node.isSVGElement() && isSVGImageElement(toSVGElement(node)); }
inline bool isSVGImageElement(const Node* node) { return node && isSVGImageElement(*node); }
template <> inline bool isElementOfType<const SVGImageElement>(const Node& node) { return isSVGImageElement(node); }
template <> inline bool isElementOfType<const SVGImageElement>(const SVGElement& element) { return isSVGImageElement(element); }

class SVGLineElement;
void isSVGLineElement(const SVGLineElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGLineElement(const SVGLineElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGLineElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::lineTag);
}
inline bool isSVGLineElement(const SVGElement* element) { return element && isSVGLineElement(*element); }
template<typename T> inline bool isSVGLineElement(const PassRefPtr<T>& node) { return isSVGLineElement(node.get()); }
template<typename T> inline bool isSVGLineElement(const RefPtr<T>& node) { return isSVGLineElement(node.get()); }
inline bool isSVGLineElement(const Node& node) { return node.isSVGElement() && isSVGLineElement(toSVGElement(node)); }
inline bool isSVGLineElement(const Node* node) { return node && isSVGLineElement(*node); }
template <> inline bool isElementOfType<const SVGLineElement>(const Node& node) { return isSVGLineElement(node); }
template <> inline bool isElementOfType<const SVGLineElement>(const SVGElement& element) { return isSVGLineElement(element); }

class SVGLinearGradientElement;
void isSVGLinearGradientElement(const SVGLinearGradientElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGLinearGradientElement(const SVGLinearGradientElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGLinearGradientElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::linearGradientTag);
}
inline bool isSVGLinearGradientElement(const SVGElement* element) { return element && isSVGLinearGradientElement(*element); }
template<typename T> inline bool isSVGLinearGradientElement(const PassRefPtr<T>& node) { return isSVGLinearGradientElement(node.get()); }
template<typename T> inline bool isSVGLinearGradientElement(const RefPtr<T>& node) { return isSVGLinearGradientElement(node.get()); }
inline bool isSVGLinearGradientElement(const Node& node) { return node.isSVGElement() && isSVGLinearGradientElement(toSVGElement(node)); }
inline bool isSVGLinearGradientElement(const Node* node) { return node && isSVGLinearGradientElement(*node); }
template <> inline bool isElementOfType<const SVGLinearGradientElement>(const Node& node) { return isSVGLinearGradientElement(node); }
template <> inline bool isElementOfType<const SVGLinearGradientElement>(const SVGElement& element) { return isSVGLinearGradientElement(element); }

class SVGMPathElement;
void isSVGMPathElement(const SVGMPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMPathElement(const SVGMPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMPathElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::mpathTag);
}
inline bool isSVGMPathElement(const SVGElement* element) { return element && isSVGMPathElement(*element); }
template<typename T> inline bool isSVGMPathElement(const PassRefPtr<T>& node) { return isSVGMPathElement(node.get()); }
template<typename T> inline bool isSVGMPathElement(const RefPtr<T>& node) { return isSVGMPathElement(node.get()); }
inline bool isSVGMPathElement(const Node& node) { return node.isSVGElement() && isSVGMPathElement(toSVGElement(node)); }
inline bool isSVGMPathElement(const Node* node) { return node && isSVGMPathElement(*node); }
template <> inline bool isElementOfType<const SVGMPathElement>(const Node& node) { return isSVGMPathElement(node); }
template <> inline bool isElementOfType<const SVGMPathElement>(const SVGElement& element) { return isSVGMPathElement(element); }

class SVGMarkerElement;
void isSVGMarkerElement(const SVGMarkerElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMarkerElement(const SVGMarkerElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMarkerElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::markerTag);
}
inline bool isSVGMarkerElement(const SVGElement* element) { return element && isSVGMarkerElement(*element); }
template<typename T> inline bool isSVGMarkerElement(const PassRefPtr<T>& node) { return isSVGMarkerElement(node.get()); }
template<typename T> inline bool isSVGMarkerElement(const RefPtr<T>& node) { return isSVGMarkerElement(node.get()); }
inline bool isSVGMarkerElement(const Node& node) { return node.isSVGElement() && isSVGMarkerElement(toSVGElement(node)); }
inline bool isSVGMarkerElement(const Node* node) { return node && isSVGMarkerElement(*node); }
template <> inline bool isElementOfType<const SVGMarkerElement>(const Node& node) { return isSVGMarkerElement(node); }
template <> inline bool isElementOfType<const SVGMarkerElement>(const SVGElement& element) { return isSVGMarkerElement(element); }

class SVGMaskElement;
void isSVGMaskElement(const SVGMaskElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMaskElement(const SVGMaskElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMaskElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::maskTag);
}
inline bool isSVGMaskElement(const SVGElement* element) { return element && isSVGMaskElement(*element); }
template<typename T> inline bool isSVGMaskElement(const PassRefPtr<T>& node) { return isSVGMaskElement(node.get()); }
template<typename T> inline bool isSVGMaskElement(const RefPtr<T>& node) { return isSVGMaskElement(node.get()); }
inline bool isSVGMaskElement(const Node& node) { return node.isSVGElement() && isSVGMaskElement(toSVGElement(node)); }
inline bool isSVGMaskElement(const Node* node) { return node && isSVGMaskElement(*node); }
template <> inline bool isElementOfType<const SVGMaskElement>(const Node& node) { return isSVGMaskElement(node); }
template <> inline bool isElementOfType<const SVGMaskElement>(const SVGElement& element) { return isSVGMaskElement(element); }

class SVGMetadataElement;
void isSVGMetadataElement(const SVGMetadataElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGMetadataElement(const SVGMetadataElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGMetadataElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::metadataTag);
}
inline bool isSVGMetadataElement(const SVGElement* element) { return element && isSVGMetadataElement(*element); }
template<typename T> inline bool isSVGMetadataElement(const PassRefPtr<T>& node) { return isSVGMetadataElement(node.get()); }
template<typename T> inline bool isSVGMetadataElement(const RefPtr<T>& node) { return isSVGMetadataElement(node.get()); }
inline bool isSVGMetadataElement(const Node& node) { return node.isSVGElement() && isSVGMetadataElement(toSVGElement(node)); }
inline bool isSVGMetadataElement(const Node* node) { return node && isSVGMetadataElement(*node); }
template <> inline bool isElementOfType<const SVGMetadataElement>(const Node& node) { return isSVGMetadataElement(node); }
template <> inline bool isElementOfType<const SVGMetadataElement>(const SVGElement& element) { return isSVGMetadataElement(element); }

class SVGPathElement;
void isSVGPathElement(const SVGPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPathElement(const SVGPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPathElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::pathTag);
}
inline bool isSVGPathElement(const SVGElement* element) { return element && isSVGPathElement(*element); }
template<typename T> inline bool isSVGPathElement(const PassRefPtr<T>& node) { return isSVGPathElement(node.get()); }
template<typename T> inline bool isSVGPathElement(const RefPtr<T>& node) { return isSVGPathElement(node.get()); }
inline bool isSVGPathElement(const Node& node) { return node.isSVGElement() && isSVGPathElement(toSVGElement(node)); }
inline bool isSVGPathElement(const Node* node) { return node && isSVGPathElement(*node); }
template <> inline bool isElementOfType<const SVGPathElement>(const Node& node) { return isSVGPathElement(node); }
template <> inline bool isElementOfType<const SVGPathElement>(const SVGElement& element) { return isSVGPathElement(element); }

class SVGPatternElement;
void isSVGPatternElement(const SVGPatternElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPatternElement(const SVGPatternElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPatternElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::patternTag);
}
inline bool isSVGPatternElement(const SVGElement* element) { return element && isSVGPatternElement(*element); }
template<typename T> inline bool isSVGPatternElement(const PassRefPtr<T>& node) { return isSVGPatternElement(node.get()); }
template<typename T> inline bool isSVGPatternElement(const RefPtr<T>& node) { return isSVGPatternElement(node.get()); }
inline bool isSVGPatternElement(const Node& node) { return node.isSVGElement() && isSVGPatternElement(toSVGElement(node)); }
inline bool isSVGPatternElement(const Node* node) { return node && isSVGPatternElement(*node); }
template <> inline bool isElementOfType<const SVGPatternElement>(const Node& node) { return isSVGPatternElement(node); }
template <> inline bool isElementOfType<const SVGPatternElement>(const SVGElement& element) { return isSVGPatternElement(element); }

class SVGPolygonElement;
void isSVGPolygonElement(const SVGPolygonElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPolygonElement(const SVGPolygonElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPolygonElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::polygonTag);
}
inline bool isSVGPolygonElement(const SVGElement* element) { return element && isSVGPolygonElement(*element); }
template<typename T> inline bool isSVGPolygonElement(const PassRefPtr<T>& node) { return isSVGPolygonElement(node.get()); }
template<typename T> inline bool isSVGPolygonElement(const RefPtr<T>& node) { return isSVGPolygonElement(node.get()); }
inline bool isSVGPolygonElement(const Node& node) { return node.isSVGElement() && isSVGPolygonElement(toSVGElement(node)); }
inline bool isSVGPolygonElement(const Node* node) { return node && isSVGPolygonElement(*node); }
template <> inline bool isElementOfType<const SVGPolygonElement>(const Node& node) { return isSVGPolygonElement(node); }
template <> inline bool isElementOfType<const SVGPolygonElement>(const SVGElement& element) { return isSVGPolygonElement(element); }

class SVGPolylineElement;
void isSVGPolylineElement(const SVGPolylineElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGPolylineElement(const SVGPolylineElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGPolylineElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::polylineTag);
}
inline bool isSVGPolylineElement(const SVGElement* element) { return element && isSVGPolylineElement(*element); }
template<typename T> inline bool isSVGPolylineElement(const PassRefPtr<T>& node) { return isSVGPolylineElement(node.get()); }
template<typename T> inline bool isSVGPolylineElement(const RefPtr<T>& node) { return isSVGPolylineElement(node.get()); }
inline bool isSVGPolylineElement(const Node& node) { return node.isSVGElement() && isSVGPolylineElement(toSVGElement(node)); }
inline bool isSVGPolylineElement(const Node* node) { return node && isSVGPolylineElement(*node); }
template <> inline bool isElementOfType<const SVGPolylineElement>(const Node& node) { return isSVGPolylineElement(node); }
template <> inline bool isElementOfType<const SVGPolylineElement>(const SVGElement& element) { return isSVGPolylineElement(element); }

class SVGRadialGradientElement;
void isSVGRadialGradientElement(const SVGRadialGradientElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGRadialGradientElement(const SVGRadialGradientElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGRadialGradientElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::radialGradientTag);
}
inline bool isSVGRadialGradientElement(const SVGElement* element) { return element && isSVGRadialGradientElement(*element); }
template<typename T> inline bool isSVGRadialGradientElement(const PassRefPtr<T>& node) { return isSVGRadialGradientElement(node.get()); }
template<typename T> inline bool isSVGRadialGradientElement(const RefPtr<T>& node) { return isSVGRadialGradientElement(node.get()); }
inline bool isSVGRadialGradientElement(const Node& node) { return node.isSVGElement() && isSVGRadialGradientElement(toSVGElement(node)); }
inline bool isSVGRadialGradientElement(const Node* node) { return node && isSVGRadialGradientElement(*node); }
template <> inline bool isElementOfType<const SVGRadialGradientElement>(const Node& node) { return isSVGRadialGradientElement(node); }
template <> inline bool isElementOfType<const SVGRadialGradientElement>(const SVGElement& element) { return isSVGRadialGradientElement(element); }

class SVGRectElement;
void isSVGRectElement(const SVGRectElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGRectElement(const SVGRectElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGRectElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::rectTag);
}
inline bool isSVGRectElement(const SVGElement* element) { return element && isSVGRectElement(*element); }
template<typename T> inline bool isSVGRectElement(const PassRefPtr<T>& node) { return isSVGRectElement(node.get()); }
template<typename T> inline bool isSVGRectElement(const RefPtr<T>& node) { return isSVGRectElement(node.get()); }
inline bool isSVGRectElement(const Node& node) { return node.isSVGElement() && isSVGRectElement(toSVGElement(node)); }
inline bool isSVGRectElement(const Node* node) { return node && isSVGRectElement(*node); }
template <> inline bool isElementOfType<const SVGRectElement>(const Node& node) { return isSVGRectElement(node); }
template <> inline bool isElementOfType<const SVGRectElement>(const SVGElement& element) { return isSVGRectElement(element); }

class SVGSVGElement;
void isSVGSVGElement(const SVGSVGElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSVGElement(const SVGSVGElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSVGElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::svgTag);
}
inline bool isSVGSVGElement(const SVGElement* element) { return element && isSVGSVGElement(*element); }
template<typename T> inline bool isSVGSVGElement(const PassRefPtr<T>& node) { return isSVGSVGElement(node.get()); }
template<typename T> inline bool isSVGSVGElement(const RefPtr<T>& node) { return isSVGSVGElement(node.get()); }
inline bool isSVGSVGElement(const Node& node) { return node.isSVGElement() && isSVGSVGElement(toSVGElement(node)); }
inline bool isSVGSVGElement(const Node* node) { return node && isSVGSVGElement(*node); }
template <> inline bool isElementOfType<const SVGSVGElement>(const Node& node) { return isSVGSVGElement(node); }
template <> inline bool isElementOfType<const SVGSVGElement>(const SVGElement& element) { return isSVGSVGElement(element); }

class SVGSetElement;
void isSVGSetElement(const SVGSetElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSetElement(const SVGSetElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSetElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::setTag);
}
inline bool isSVGSetElement(const SVGElement* element) { return element && isSVGSetElement(*element); }
template<typename T> inline bool isSVGSetElement(const PassRefPtr<T>& node) { return isSVGSetElement(node.get()); }
template<typename T> inline bool isSVGSetElement(const RefPtr<T>& node) { return isSVGSetElement(node.get()); }
inline bool isSVGSetElement(const Node& node) { return node.isSVGElement() && isSVGSetElement(toSVGElement(node)); }
inline bool isSVGSetElement(const Node* node) { return node && isSVGSetElement(*node); }
template <> inline bool isElementOfType<const SVGSetElement>(const Node& node) { return isSVGSetElement(node); }
template <> inline bool isElementOfType<const SVGSetElement>(const SVGElement& element) { return isSVGSetElement(element); }

class SVGStopElement;
void isSVGStopElement(const SVGStopElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGStopElement(const SVGStopElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGStopElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::stopTag);
}
inline bool isSVGStopElement(const SVGElement* element) { return element && isSVGStopElement(*element); }
template<typename T> inline bool isSVGStopElement(const PassRefPtr<T>& node) { return isSVGStopElement(node.get()); }
template<typename T> inline bool isSVGStopElement(const RefPtr<T>& node) { return isSVGStopElement(node.get()); }
inline bool isSVGStopElement(const Node& node) { return node.isSVGElement() && isSVGStopElement(toSVGElement(node)); }
inline bool isSVGStopElement(const Node* node) { return node && isSVGStopElement(*node); }
template <> inline bool isElementOfType<const SVGStopElement>(const Node& node) { return isSVGStopElement(node); }
template <> inline bool isElementOfType<const SVGStopElement>(const SVGElement& element) { return isSVGStopElement(element); }

class SVGSwitchElement;
void isSVGSwitchElement(const SVGSwitchElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSwitchElement(const SVGSwitchElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSwitchElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::switchTag);
}
inline bool isSVGSwitchElement(const SVGElement* element) { return element && isSVGSwitchElement(*element); }
template<typename T> inline bool isSVGSwitchElement(const PassRefPtr<T>& node) { return isSVGSwitchElement(node.get()); }
template<typename T> inline bool isSVGSwitchElement(const RefPtr<T>& node) { return isSVGSwitchElement(node.get()); }
inline bool isSVGSwitchElement(const Node& node) { return node.isSVGElement() && isSVGSwitchElement(toSVGElement(node)); }
inline bool isSVGSwitchElement(const Node* node) { return node && isSVGSwitchElement(*node); }
template <> inline bool isElementOfType<const SVGSwitchElement>(const Node& node) { return isSVGSwitchElement(node); }
template <> inline bool isElementOfType<const SVGSwitchElement>(const SVGElement& element) { return isSVGSwitchElement(element); }

class SVGSymbolElement;
void isSVGSymbolElement(const SVGSymbolElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGSymbolElement(const SVGSymbolElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGSymbolElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::symbolTag);
}
inline bool isSVGSymbolElement(const SVGElement* element) { return element && isSVGSymbolElement(*element); }
template<typename T> inline bool isSVGSymbolElement(const PassRefPtr<T>& node) { return isSVGSymbolElement(node.get()); }
template<typename T> inline bool isSVGSymbolElement(const RefPtr<T>& node) { return isSVGSymbolElement(node.get()); }
inline bool isSVGSymbolElement(const Node& node) { return node.isSVGElement() && isSVGSymbolElement(toSVGElement(node)); }
inline bool isSVGSymbolElement(const Node* node) { return node && isSVGSymbolElement(*node); }
template <> inline bool isElementOfType<const SVGSymbolElement>(const Node& node) { return isSVGSymbolElement(node); }
template <> inline bool isElementOfType<const SVGSymbolElement>(const SVGElement& element) { return isSVGSymbolElement(element); }

class SVGTSpanElement;
void isSVGTSpanElement(const SVGTSpanElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTSpanElement(const SVGTSpanElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTSpanElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::tspanTag);
}
inline bool isSVGTSpanElement(const SVGElement* element) { return element && isSVGTSpanElement(*element); }
template<typename T> inline bool isSVGTSpanElement(const PassRefPtr<T>& node) { return isSVGTSpanElement(node.get()); }
template<typename T> inline bool isSVGTSpanElement(const RefPtr<T>& node) { return isSVGTSpanElement(node.get()); }
inline bool isSVGTSpanElement(const Node& node) { return node.isSVGElement() && isSVGTSpanElement(toSVGElement(node)); }
inline bool isSVGTSpanElement(const Node* node) { return node && isSVGTSpanElement(*node); }
template <> inline bool isElementOfType<const SVGTSpanElement>(const Node& node) { return isSVGTSpanElement(node); }
template <> inline bool isElementOfType<const SVGTSpanElement>(const SVGElement& element) { return isSVGTSpanElement(element); }

class SVGTextElement;
void isSVGTextElement(const SVGTextElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTextElement(const SVGTextElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTextElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::textTag);
}
inline bool isSVGTextElement(const SVGElement* element) { return element && isSVGTextElement(*element); }
template<typename T> inline bool isSVGTextElement(const PassRefPtr<T>& node) { return isSVGTextElement(node.get()); }
template<typename T> inline bool isSVGTextElement(const RefPtr<T>& node) { return isSVGTextElement(node.get()); }
inline bool isSVGTextElement(const Node& node) { return node.isSVGElement() && isSVGTextElement(toSVGElement(node)); }
inline bool isSVGTextElement(const Node* node) { return node && isSVGTextElement(*node); }
template <> inline bool isElementOfType<const SVGTextElement>(const Node& node) { return isSVGTextElement(node); }
template <> inline bool isElementOfType<const SVGTextElement>(const SVGElement& element) { return isSVGTextElement(element); }

class SVGTextPathElement;
void isSVGTextPathElement(const SVGTextPathElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTextPathElement(const SVGTextPathElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTextPathElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::textPathTag);
}
inline bool isSVGTextPathElement(const SVGElement* element) { return element && isSVGTextPathElement(*element); }
template<typename T> inline bool isSVGTextPathElement(const PassRefPtr<T>& node) { return isSVGTextPathElement(node.get()); }
template<typename T> inline bool isSVGTextPathElement(const RefPtr<T>& node) { return isSVGTextPathElement(node.get()); }
inline bool isSVGTextPathElement(const Node& node) { return node.isSVGElement() && isSVGTextPathElement(toSVGElement(node)); }
inline bool isSVGTextPathElement(const Node* node) { return node && isSVGTextPathElement(*node); }
template <> inline bool isElementOfType<const SVGTextPathElement>(const Node& node) { return isSVGTextPathElement(node); }
template <> inline bool isElementOfType<const SVGTextPathElement>(const SVGElement& element) { return isSVGTextPathElement(element); }

class SVGTitleElement;
void isSVGTitleElement(const SVGTitleElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGTitleElement(const SVGTitleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGTitleElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::titleTag);
}
inline bool isSVGTitleElement(const SVGElement* element) { return element && isSVGTitleElement(*element); }
template<typename T> inline bool isSVGTitleElement(const PassRefPtr<T>& node) { return isSVGTitleElement(node.get()); }
template<typename T> inline bool isSVGTitleElement(const RefPtr<T>& node) { return isSVGTitleElement(node.get()); }
inline bool isSVGTitleElement(const Node& node) { return node.isSVGElement() && isSVGTitleElement(toSVGElement(node)); }
inline bool isSVGTitleElement(const Node* node) { return node && isSVGTitleElement(*node); }
template <> inline bool isElementOfType<const SVGTitleElement>(const Node& node) { return isSVGTitleElement(node); }
template <> inline bool isElementOfType<const SVGTitleElement>(const SVGElement& element) { return isSVGTitleElement(element); }

class SVGUseElement;
void isSVGUseElement(const SVGUseElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGUseElement(const SVGUseElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGUseElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::useTag);
}
inline bool isSVGUseElement(const SVGElement* element) { return element && isSVGUseElement(*element); }
template<typename T> inline bool isSVGUseElement(const PassRefPtr<T>& node) { return isSVGUseElement(node.get()); }
template<typename T> inline bool isSVGUseElement(const RefPtr<T>& node) { return isSVGUseElement(node.get()); }
inline bool isSVGUseElement(const Node& node) { return node.isSVGElement() && isSVGUseElement(toSVGElement(node)); }
inline bool isSVGUseElement(const Node* node) { return node && isSVGUseElement(*node); }
template <> inline bool isElementOfType<const SVGUseElement>(const Node& node) { return isSVGUseElement(node); }
template <> inline bool isElementOfType<const SVGUseElement>(const SVGElement& element) { return isSVGUseElement(element); }

class SVGViewElement;
void isSVGViewElement(const SVGViewElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGViewElement(const SVGViewElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGViewElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::viewTag);
}
inline bool isSVGViewElement(const SVGElement* element) { return element && isSVGViewElement(*element); }
template<typename T> inline bool isSVGViewElement(const PassRefPtr<T>& node) { return isSVGViewElement(node.get()); }
template<typename T> inline bool isSVGViewElement(const RefPtr<T>& node) { return isSVGViewElement(node.get()); }
inline bool isSVGViewElement(const Node& node) { return node.isSVGElement() && isSVGViewElement(toSVGElement(node)); }
inline bool isSVGViewElement(const Node* node) { return node && isSVGViewElement(*node); }
template <> inline bool isElementOfType<const SVGViewElement>(const Node& node) { return isSVGViewElement(node); }
template <> inline bool isElementOfType<const SVGViewElement>(const SVGElement& element) { return isSVGViewElement(element); }

class SVGScriptElement;
void isSVGScriptElement(const SVGScriptElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGScriptElement(const SVGScriptElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGScriptElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::scriptTag);
}
inline bool isSVGScriptElement(const SVGElement* element) { return element && isSVGScriptElement(*element); }
template<typename T> inline bool isSVGScriptElement(const PassRefPtr<T>& node) { return isSVGScriptElement(node.get()); }
template<typename T> inline bool isSVGScriptElement(const RefPtr<T>& node) { return isSVGScriptElement(node.get()); }
inline bool isSVGScriptElement(const Node& node) { return node.isSVGElement() && isSVGScriptElement(toSVGElement(node)); }
inline bool isSVGScriptElement(const Node* node) { return node && isSVGScriptElement(*node); }
template <> inline bool isElementOfType<const SVGScriptElement>(const Node& node) { return isSVGScriptElement(node); }
template <> inline bool isElementOfType<const SVGScriptElement>(const SVGElement& element) { return isSVGScriptElement(element); }

class SVGStyleElement;
void isSVGStyleElement(const SVGStyleElement&); // Catch unnecessary runtime check of type known at compile time.
void isSVGStyleElement(const SVGStyleElement*); // Catch unnecessary runtime check of type known at compile time.

inline bool isSVGStyleElement(const SVGElement& element) {
    return element.hasTagName(SVGNames::styleTag);
}
inline bool isSVGStyleElement(const SVGElement* element) { return element && isSVGStyleElement(*element); }
template<typename T> inline bool isSVGStyleElement(const PassRefPtr<T>& node) { return isSVGStyleElement(node.get()); }
template<typename T> inline bool isSVGStyleElement(const RefPtr<T>& node) { return isSVGStyleElement(node.get()); }
inline bool isSVGStyleElement(const Node& node) { return node.isSVGElement() && isSVGStyleElement(toSVGElement(node)); }
inline bool isSVGStyleElement(const Node* node) { return node && isSVGStyleElement(*node); }
template <> inline bool isElementOfType<const SVGStyleElement>(const Node& node) { return isSVGStyleElement(node); }
template <> inline bool isElementOfType<const SVGStyleElement>(const SVGElement& element) { return isSVGStyleElement(element); }

// Using macros because the types are forward-declared and we don't want to use reinterpret_cast in the
// casting functions above. reinterpret_cast would be unsafe due to multiple inheritence.

#define toSVGAElement(x) blink::toElement<blink::SVGAElement>(x)
#define toSVGAnimateMotionElement(x) blink::toElement<blink::SVGAnimateMotionElement>(x)
#define toSVGAnimateTransformElement(x) blink::toElement<blink::SVGAnimateTransformElement>(x)
#define toSVGCircleElement(x) blink::toElement<blink::SVGCircleElement>(x)
#define toSVGClipPathElement(x) blink::toElement<blink::SVGClipPathElement>(x)
#define toSVGCursorElement(x) blink::toElement<blink::SVGCursorElement>(x)
#define toSVGDefsElement(x) blink::toElement<blink::SVGDefsElement>(x)
#define toSVGDescElement(x) blink::toElement<blink::SVGDescElement>(x)
#define toSVGDiscardElement(x) blink::toElement<blink::SVGDiscardElement>(x)
#define toSVGEllipseElement(x) blink::toElement<blink::SVGEllipseElement>(x)
#define toSVGFEBlendElement(x) blink::toElement<blink::SVGFEBlendElement>(x)
#define toSVGFEColorMatrixElement(x) blink::toElement<blink::SVGFEColorMatrixElement>(x)
#define toSVGFEComponentTransferElement(x) blink::toElement<blink::SVGFEComponentTransferElement>(x)
#define toSVGFECompositeElement(x) blink::toElement<blink::SVGFECompositeElement>(x)
#define toSVGFEConvolveMatrixElement(x) blink::toElement<blink::SVGFEConvolveMatrixElement>(x)
#define toSVGFEDiffuseLightingElement(x) blink::toElement<blink::SVGFEDiffuseLightingElement>(x)
#define toSVGFEDisplacementMapElement(x) blink::toElement<blink::SVGFEDisplacementMapElement>(x)
#define toSVGFEDistantLightElement(x) blink::toElement<blink::SVGFEDistantLightElement>(x)
#define toSVGFEDropShadowElement(x) blink::toElement<blink::SVGFEDropShadowElement>(x)
#define toSVGFEFloodElement(x) blink::toElement<blink::SVGFEFloodElement>(x)
#define toSVGFEFuncAElement(x) blink::toElement<blink::SVGFEFuncAElement>(x)
#define toSVGFEFuncBElement(x) blink::toElement<blink::SVGFEFuncBElement>(x)
#define toSVGFEFuncGElement(x) blink::toElement<blink::SVGFEFuncGElement>(x)
#define toSVGFEFuncRElement(x) blink::toElement<blink::SVGFEFuncRElement>(x)
#define toSVGFEGaussianBlurElement(x) blink::toElement<blink::SVGFEGaussianBlurElement>(x)
#define toSVGFEImageElement(x) blink::toElement<blink::SVGFEImageElement>(x)
#define toSVGFEMergeElement(x) blink::toElement<blink::SVGFEMergeElement>(x)
#define toSVGFEMergeNodeElement(x) blink::toElement<blink::SVGFEMergeNodeElement>(x)
#define toSVGFEMorphologyElement(x) blink::toElement<blink::SVGFEMorphologyElement>(x)
#define toSVGFEOffsetElement(x) blink::toElement<blink::SVGFEOffsetElement>(x)
#define toSVGFEPointLightElement(x) blink::toElement<blink::SVGFEPointLightElement>(x)
#define toSVGFESpecularLightingElement(x) blink::toElement<blink::SVGFESpecularLightingElement>(x)
#define toSVGFESpotLightElement(x) blink::toElement<blink::SVGFESpotLightElement>(x)
#define toSVGFETileElement(x) blink::toElement<blink::SVGFETileElement>(x)
#define toSVGFETurbulenceElement(x) blink::toElement<blink::SVGFETurbulenceElement>(x)
#define toSVGFilterElement(x) blink::toElement<blink::SVGFilterElement>(x)
#define toSVGForeignObjectElement(x) blink::toElement<blink::SVGForeignObjectElement>(x)
#define toSVGGElement(x) blink::toElement<blink::SVGGElement>(x)
#define toSVGImageElement(x) blink::toElement<blink::SVGImageElement>(x)
#define toSVGLineElement(x) blink::toElement<blink::SVGLineElement>(x)
#define toSVGLinearGradientElement(x) blink::toElement<blink::SVGLinearGradientElement>(x)
#define toSVGMPathElement(x) blink::toElement<blink::SVGMPathElement>(x)
#define toSVGMarkerElement(x) blink::toElement<blink::SVGMarkerElement>(x)
#define toSVGMaskElement(x) blink::toElement<blink::SVGMaskElement>(x)
#define toSVGMetadataElement(x) blink::toElement<blink::SVGMetadataElement>(x)
#define toSVGPathElement(x) blink::toElement<blink::SVGPathElement>(x)
#define toSVGPatternElement(x) blink::toElement<blink::SVGPatternElement>(x)
#define toSVGPolygonElement(x) blink::toElement<blink::SVGPolygonElement>(x)
#define toSVGPolylineElement(x) blink::toElement<blink::SVGPolylineElement>(x)
#define toSVGRadialGradientElement(x) blink::toElement<blink::SVGRadialGradientElement>(x)
#define toSVGRectElement(x) blink::toElement<blink::SVGRectElement>(x)
#define toSVGSVGElement(x) blink::toElement<blink::SVGSVGElement>(x)
#define toSVGSetElement(x) blink::toElement<blink::SVGSetElement>(x)
#define toSVGStopElement(x) blink::toElement<blink::SVGStopElement>(x)
#define toSVGSwitchElement(x) blink::toElement<blink::SVGSwitchElement>(x)
#define toSVGSymbolElement(x) blink::toElement<blink::SVGSymbolElement>(x)
#define toSVGTSpanElement(x) blink::toElement<blink::SVGTSpanElement>(x)
#define toSVGTextElement(x) blink::toElement<blink::SVGTextElement>(x)
#define toSVGTextPathElement(x) blink::toElement<blink::SVGTextPathElement>(x)
#define toSVGTitleElement(x) blink::toElement<blink::SVGTitleElement>(x)
#define toSVGUseElement(x) blink::toElement<blink::SVGUseElement>(x)
#define toSVGViewElement(x) blink::toElement<blink::SVGViewElement>(x)
#define toSVGScriptElement(x) blink::toElement<blink::SVGScriptElement>(x)
#define toSVGStyleElement(x) blink::toElement<blink::SVGStyleElement>(x)
} // namespace blink

#endif
