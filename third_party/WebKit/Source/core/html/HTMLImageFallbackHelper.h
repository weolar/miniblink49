// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLImageFallbackHelper_h
#define HTMLImageFallbackHelper_h

#include "wtf/PassRefPtr.h"

namespace blink {

class Element;
class ComputedStyle;

class HTMLImageFallbackHelper {
public:
    static void createAltTextShadowTree(Element&);
    static PassRefPtr<ComputedStyle> customStyleForAltText(Element&, PassRefPtr<ComputedStyle> newStyle);
};

} // namespace blink

#endif // HTMLImageFallbackHelper_h
