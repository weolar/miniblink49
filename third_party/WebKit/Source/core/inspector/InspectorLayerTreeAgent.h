/*
 * Copyright (C) 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef InspectorLayerTreeAgent_h
#define InspectorLayerTreeAgent_h

#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/InspectorTypeBuilder.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "platform/Timer.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class GraphicsContext;
class GraphicsLayer;
class InspectorPageAgent;
class LayoutObject;
class LayoutRect;
class PictureSnapshot;
class DeprecatedPaintLayer;
class DeprecatedPaintLayerCompositor;

typedef String ErrorString;

class CORE_EXPORT InspectorLayerTreeAgent final : public InspectorBaseAgent<InspectorLayerTreeAgent, InspectorFrontend::LayerTree>, public InspectorBackendDispatcher::LayerTreeCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorLayerTreeAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorLayerTreeAgent> create(InspectorPageAgent* pageAgent)
    {
        return adoptPtrWillBeNoop(new InspectorLayerTreeAgent(pageAgent));
    }
    virtual ~InspectorLayerTreeAgent();
    DECLARE_VIRTUAL_TRACE();

    void disable(ErrorString*) override;
    void restore() override;

    // Called from InspectorController
    void willAddPageOverlay(const GraphicsLayer*);
    void didRemovePageOverlay(const GraphicsLayer*);

    // Called from InspectorInstrumentation
    void layerTreeDidChange();
    void didPaint(LayoutObject*, const GraphicsLayer*, GraphicsContext*, const LayoutRect&);

    // Called from the front-end.
    virtual void enable(ErrorString*) override;
    virtual void compositingReasons(ErrorString*, const String& layerId, RefPtr<TypeBuilder::Array<String> >&) override;
    virtual void makeSnapshot(ErrorString*, const String& layerId, String* snapshotId) override;
    virtual void loadSnapshot(ErrorString*, const RefPtr<JSONArray>& tiles, String* snapshotId) override;
    virtual void releaseSnapshot(ErrorString*, const String& snapshotId) override;
    virtual void replaySnapshot(ErrorString*, const String& snapshotId, const int* fromStep, const int* toStep, const double* scale, String* dataURL) override;
    virtual void profileSnapshot(ErrorString*, const String& snapshotId, const int* minRepeatCount, const double* minDuration, const RefPtr<JSONObject>* clipRect, RefPtr<TypeBuilder::Array<TypeBuilder::Array<double> > >&) override;
    virtual void snapshotCommandLog(ErrorString*, const String& snapshotId, RefPtr<TypeBuilder::Array<JSONObject> >&) override;

    // Called by other agents.
    PassRefPtr<TypeBuilder::Array<TypeBuilder::LayerTree::Layer> > buildLayerTree();

private:
    static unsigned s_lastSnapshotId;

    explicit InspectorLayerTreeAgent(InspectorPageAgent*);

    GraphicsLayer* rootGraphicsLayer();

    DeprecatedPaintLayerCompositor* deprecatedPaintLayerCompositor();
    GraphicsLayer* layerById(ErrorString*, const String& layerId);
    const PictureSnapshot* snapshotById(ErrorString*, const String& snapshotId);

    typedef HashMap<int, int> LayerIdToNodeIdMap;
    void buildLayerIdToNodeIdMap(DeprecatedPaintLayer*, LayerIdToNodeIdMap&);
    void gatherGraphicsLayers(GraphicsLayer*, HashMap<int, int>& layerIdToNodeIdMap, RefPtr<TypeBuilder::Array<TypeBuilder::LayerTree::Layer> >&);
    int idForNode(Node*);

    RawPtrWillBeMember<InspectorPageAgent> m_pageAgent;
    Vector<int, 2> m_pageOverlayLayerIds;

    typedef HashMap<String, RefPtr<PictureSnapshot> > SnapshotById;
    SnapshotById m_snapshotById;
};

} // namespace blink


#endif // !defined(InspectorLayerTreeAgent_h)
