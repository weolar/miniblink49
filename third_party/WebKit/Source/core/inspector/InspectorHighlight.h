// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorHighlight_h
#define InspectorHighlight_h

#include "core/CoreExport.h"
#include "core/InspectorTypeBuilder.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/Color.h"
#include "platform/heap/Handle.h"

namespace blink {

class Color;
class JSONValue;

struct CORE_EXPORT InspectorHighlightConfig {
    WTF_MAKE_FAST_ALLOCATED(InspectorHighlightConfig);
public:
    InspectorHighlightConfig();

    Color content;
    Color contentOutline;
    Color padding;
    Color border;
    Color margin;
    Color eventTarget;
    Color shape;
    Color shapeMargin;

    bool showInfo;
    bool showRulers;
    bool showExtensionLines;
    bool showLayoutEditor;
};

class CORE_EXPORT InspectorHighlight {
    STACK_ALLOCATED();
public:
    InspectorHighlight(Node*, const InspectorHighlightConfig&, bool appendElementInfo);
    InspectorHighlight();
    ~InspectorHighlight();

    static bool getBoxModel(Node*, RefPtr<TypeBuilder::DOM::BoxModel>&);
    static InspectorHighlightConfig defaultConfig();

    void appendPath(PassRefPtr<JSONArrayBase> path, const Color& fillColor, const Color& outlineColor);
    void appendQuad(const FloatQuad&, const Color& fillColor, const Color& outlineColor = Color::transparent);
    void appendEventTargetQuads(Node* eventTargetNode, const InspectorHighlightConfig&);
    PassRefPtr<JSONObject> asJSONObject() const;

private:
    void appendNodeHighlight(Node*, const InspectorHighlightConfig&);
    void appendPathsForShapeOutside(Node*, const InspectorHighlightConfig&);

    RefPtr<JSONObject> m_elementInfo;
    RefPtr<JSONArray> m_highlightPaths;
    bool m_showRulers;
    bool m_showExtensionLines;
};

} // namespace blink

#endif // InspectorHighlight_h
