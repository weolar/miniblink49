// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MathMLNames_h
#define MathMLNames_h

#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class MathMLQualifiedName : public QualifiedName { };

namespace MathMLNames {

// Namespace
extern const WTF::AtomicString& mathmlNamespaceURI;

// Tags
extern const blink::MathMLQualifiedName& annotation_xmlTag;
extern const blink::MathMLQualifiedName& malignmarkTag;
extern const blink::MathMLQualifiedName& mathTag;
extern const blink::MathMLQualifiedName& mglyphTag;
extern const blink::MathMLQualifiedName& miTag;
extern const blink::MathMLQualifiedName& mnTag;
extern const blink::MathMLQualifiedName& moTag;
extern const blink::MathMLQualifiedName& msTag;
extern const blink::MathMLQualifiedName& mtextTag;

// Attributes
extern const blink::QualifiedName& definitionURLAttr;
extern const blink::QualifiedName& encodingAttr;

const unsigned MathMLTagsCount = 9;
PassOwnPtr<const MathMLQualifiedName*[]> getMathMLTags();

const unsigned MathMLAttrsCount = 2;
PassOwnPtr<const QualifiedName*[]> getMathMLAttrs();

void init();

} // MathMLNames
} // namespace blink

#endif
