/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LayoutIFrame_h
#define LayoutIFrame_h

#include "core/layout/LayoutPart.h"

namespace blink {

class LayoutIFrame final : public LayoutPart {
public:
    explicit LayoutIFrame(Element*);

    virtual const char* name() const override { return "LayoutIFrame"; }

private:
    virtual bool shouldComputeSizeAsReplaced() const override;
    virtual bool isInlineBlockOrInlineTable() const override;

    virtual void layout() override;

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectLayoutIFrame || LayoutPart::isOfType(type); }

    virtual DeprecatedPaintLayerType layerTypeRequired() const override;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutIFrame, isLayoutIFrame());

} // namespace blink

#endif // LayoutIFrame_h
