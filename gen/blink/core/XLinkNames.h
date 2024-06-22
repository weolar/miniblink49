// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XLinkNames_h
#define XLinkNames_h

#include "core/CoreExport.h"
#include "core/dom/QualifiedName.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class XLinkQualifiedName : public QualifiedName { };

namespace XLinkNames {

// Namespace
CORE_EXPORT extern const WTF::AtomicString& xlinkNamespaceURI;

// Tags

// Attributes
CORE_EXPORT extern const blink::QualifiedName& actuateAttr;
CORE_EXPORT extern const blink::QualifiedName& arcroleAttr;
CORE_EXPORT extern const blink::QualifiedName& hrefAttr;
CORE_EXPORT extern const blink::QualifiedName& roleAttr;
CORE_EXPORT extern const blink::QualifiedName& showAttr;
CORE_EXPORT extern const blink::QualifiedName& titleAttr;
CORE_EXPORT extern const blink::QualifiedName& typeAttr;


const unsigned XLinkAttrsCount = 7;
PassOwnPtr<const QualifiedName*[]> getXLinkAttrs();

void init();

} // XLinkNames
} // namespace blink

#endif
