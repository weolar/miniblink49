// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_SORTER_H_
#define CC_TREES_LAYER_SORTER_H_

#include "third_party/WebKit/Source/wtf/Vector.h"

#include "cc/blink/WebLayerImpl.h"
#include "third_party/WebKit/Source/platform/geometry/FloatRect.h"
#include "third_party/WebKit/Source/platform/geometry/FloatQuad.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint3D.h"

#if defined(COMPILER_GCC)
namespace cc { struct GraphEdge; }

namespace BASE_HASH_NAMESPACE {
template <>
struct hash<cc::GraphEdge*> {
  size_t operator()(cc::GraphEdge* ptr) const {
    return hash<size_t>()(reinterpret_cast<size_t>(ptr));
  }
};
}  // namespace BASE_HASH_NAMESPACE
#endif  // COMPILER

using cc_blink::WebLayerImplList;

namespace cc {
struct GraphEdge;

// Holds various useful properties derived from a layer's 3D outline.
struct LayerShape {
    LayerShape();
    LayerShape(float width, float height, const SkMatrix44& draw_transform);
    ~LayerShape();

    float LayerZFromProjectedPoint(const blink::FloatPoint& p) const;

    blink::FloatPoint3D layer_normal;
    blink::FloatPoint3D transform_origin;
    blink::FloatQuad projected_quad;
    blink::FloatRect projected_bounds;
};

struct GraphNode {
    explicit GraphNode(cc_blink::WebLayerImpl* layer_impl);
    ~GraphNode();

    cc_blink::WebLayerImpl* layer;
    LayerShape shape;
    Vector<GraphEdge*> incoming;
    Vector<GraphEdge*> outgoing;
    float incoming_edge_weight;
};

struct GraphEdge {
    GraphEdge(GraphNode* from_node, GraphNode* to_node, float weight)
        : from(from_node),
        to(to_node),
        weight(weight) {}

    GraphNode* from;
    GraphNode* to;
    float weight;
};


class LayerSorter {
public:
    LayerSorter();
    ~LayerSorter();

    void Sort(WebLayerImplList::iterator first, WebLayerImplList::iterator last);

    enum ABCompareResult {
        ABeforeB,
        BBeforeA,
        None
    };

    static ABCompareResult CheckOverlap(LayerShape* a,
        LayerShape* b,
        float z_threshold,
        float* weight);

private:
    typedef Vector<GraphNode> NodeList;
    typedef Vector<GraphEdge> EdgeList;
    NodeList nodes_;
    EdgeList edges_;
    float z_range_;

    typedef HashMap<GraphEdge*, GraphEdge*> EdgeMap;
    EdgeMap active_edges_;

    void CreateGraphNodes(WebLayerImplList::iterator first, WebLayerImplList::iterator last);
    void CreateGraphEdges();
    void RemoveEdgeFromList(GraphEdge* graph, Vector<GraphEdge*>* list);

    DISALLOW_COPY_AND_ASSIGN(LayerSorter);
};

}  // namespace cc
#endif  // CC_TREES_LAYER_SORTER_H_
