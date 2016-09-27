// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLRTElement_h
#define HTMLRTElement_h

#include "core/html/HTMLElement.h"

namespace blink {

// <rt> is an HTMLElement in script, but we use a separate interface here
// so HTMLElement's createLayoutObject doesn't need to know about it.
class HTMLRTElement final : public HTMLElement {
public:
    DECLARE_NODE_FACTORY(HTMLRTElement);

private:
    explicit HTMLRTElement(Document&);

    LayoutObject* createLayoutObject(const ComputedStyle&) override;
};

} // namespace

#endif
