// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XMLNames_h
#define XMLNames_h

#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class XMLQualifiedName : public QualifiedName { };

namespace XMLNames {

// Namespace
extern const WTF::AtomicString& xmlNamespaceURI;

// Tags

// Attributes
extern const blink::QualifiedName& langAttr;
extern const blink::QualifiedName& spaceAttr;


const unsigned XMLAttrsCount = 2;
PassOwnPtr<const QualifiedName*[]> getXMLAttrs();

void init();

} // XMLNames
} // namespace blink

#endif
