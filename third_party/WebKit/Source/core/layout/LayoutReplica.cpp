/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutReplica.h"

#include "core/paint/ReplicaPainter.h"

namespace blink {

LayoutReplica::LayoutReplica()
    : LayoutBox(nullptr)
{
    // This is a hack. Replicas are synthetic, and don't pick up the attributes of the
    // layoutObjects being replicated, so they always report that they are inline, non-replaced.
    // However, we need transforms to be applied to replicas for reflections, so have to pass
    // the if (!isInline() || isReplaced()) check before setHasTransform().
    setReplaced(true);
}

LayoutReplica* LayoutReplica::createAnonymous(Document* document)
{
    LayoutReplica* layoutObject = new LayoutReplica();
    layoutObject->setDocumentForAnonymous(document);
    return layoutObject;
}

LayoutReplica::~LayoutReplica()
{
}

void LayoutReplica::layout()
{
    setFrameRect(parentBox()->borderBoxRect());
    addVisualOverflow(parentBox()->visualOverflowRect());
    updateLayerTransformAfterLayout();
    clearNeedsLayout();
}

void LayoutReplica::computePreferredLogicalWidths()
{
    m_minPreferredLogicalWidth = parentBox()->size().width();
    m_maxPreferredLogicalWidth = m_minPreferredLogicalWidth;
    clearPreferredLogicalWidthsDirty();
}

void LayoutReplica::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    ReplicaPainter(*this).paint(paintInfo, paintOffset);
}

} // namespace blink
