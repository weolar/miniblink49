// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGElementFactory_h
#define SVGElementFactory_h

#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class Document;
class SVGElement;

class SVGElementFactory {
public:
    static PassRefPtrWillBeRawPtr<SVGElement> createSVGElement(
        const AtomicString& localName,
        Document&,
        bool createdByParser = true);
};

} // namespace blink

#endif
