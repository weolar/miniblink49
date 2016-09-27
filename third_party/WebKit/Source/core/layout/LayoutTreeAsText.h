/*
 * Copyright (C) 2003, 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef LayoutTreeAsText_h
#define LayoutTreeAsText_h

#include "core/CoreExport.h"
#include "platform/text/TextStream.h"
#include "wtf/Forward.h"

namespace blink {

class Color;
class DeprecatedPaintLayer;
class Element;
class LayoutRect;
class LocalFrame;
class Node;
class LayoutBlockFlow;
class LayoutObject;
class TextStream;

enum LayoutAsTextBehaviorFlags {
    LayoutAsTextBehaviorNormal = 0,
    LayoutAsTextShowAllLayers = 1 << 0, // Dump all layers, not just those that would paint.
    LayoutAsTextShowLayerNesting = 1 << 1, // Annotate the layer lists.
    LayoutAsTextShowCompositedLayers = 1 << 2, // Show which layers are composited.
    LayoutAsTextShowAddresses = 1 << 3, // Show layer and layoutObject addresses.
    LayoutAsTextShowIDAndClass = 1 << 4, // Show id and class attributes
    LayoutAsTextPrintingMode = 1 << 5, // Dump the tree in printing mode.
    LayoutAsTextDontUpdateLayout = 1 << 6, // Don't update layout, to make it safe to call showLayerTree() from the debugger inside layout or painting code.
    LayoutAsTextShowLayoutState = 1 << 7, // Print the various 'needs layout' bits on layoutObjects.
    LayoutAsTextShowLineTrees = 1 << 8 // Dump the line trees for each LayoutBlockFlow.
};
typedef unsigned LayoutAsTextBehavior;

// You don't need pageWidthInPixels if you don't specify LayoutAsTextInPrintingMode.
CORE_EXPORT String externalRepresentation(LocalFrame*, LayoutAsTextBehavior = LayoutAsTextBehaviorNormal);
CORE_EXPORT String externalRepresentation(Element*, LayoutAsTextBehavior = LayoutAsTextBehaviorNormal);
void write(TextStream&, const LayoutObject&, int indent = 0, LayoutAsTextBehavior = LayoutAsTextBehaviorNormal);

class LayoutTreeAsText {
// FIXME: This is a cheesy hack to allow easy access to ComputedStyle colors.  It won't be needed if we convert
// it to use visitedDependentColor instead. (This just involves rebaselining many results though, so for now it's
// not being done).
public:
static void writeLayoutObject(TextStream&, const LayoutObject&, LayoutAsTextBehavior);
static void writeLayers(TextStream&, const DeprecatedPaintLayer* rootLayer, DeprecatedPaintLayer*, const LayoutRect& paintDirtyRect, int indent = 0, LayoutAsTextBehavior = LayoutAsTextBehaviorNormal);
static void writeLineBoxTree(TextStream&, const LayoutBlockFlow&, int indent = 0);
};

// Helper function shared with SVGLayoutTreeAsText
String quoteAndEscapeNonPrintables(const String&);

CORE_EXPORT String counterValueForElement(Element*);

CORE_EXPORT String markerTextForListItem(Element*);

CORE_EXPORT String nodePositionAsStringForTesting(Node*);

TextStream& operator<<(TextStream&, const Color&);

} // namespace blink

#endif // LayoutTreeAsText_h
