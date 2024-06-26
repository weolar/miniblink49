// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/trees/LayerSorter.h"

#include <algorithm>
#include <limits>
#include "third_party/WebKit/Source/wtf/Deque.h"
#include "mc/base/MathUtil.h"
#include "mc/trees/DrawProperties.h"

using namespace blink;

namespace mc {

// This epsilon is used to determine if two layers are too close to each other
// to be able to tell which is in front of the other.  It's a relative epsilon
// so it is robust to changes in scene scale.  This value was chosen by picking
// a value near machine epsilon and then increasing it until the flickering on
// the test scene went away.
const float k_layer_epsilon = 1e-4f;

inline static float PerpProduct(const FloatPoint& u, const FloatPoint& v) {
    return u.x() * v.y() - u.y() * v.x();
}

// Tests if two edges defined by their endpoints (a,b) and (c,d) intersect.
// Returns true and the point of intersection if they do and false otherwise.
static bool EdgeEdgeTest(const FloatPoint& a,
    const FloatPoint& b,
    const FloatPoint& c,
    const FloatPoint& d,
    FloatPoint* r) {
    FloatPoint u = FloatPoint(b - a);
    FloatPoint v = FloatPoint(d - c);
    FloatPoint w = FloatPoint(a - c);

    float denom = PerpProduct(u, v);

    // If denom == 0 then the edges are parallel. While they could be overlapping
    // we don't bother to check here as the we'll find their intersections from
    // the corner to quad tests.
    if (!denom)
        return false;

    float s = PerpProduct(v, w) / denom;
    if (s < 0.f || s > 1.f)
        return false;

    float t = PerpProduct(u, w) / denom;
    if (t < 0.f || t > 1.f)
        return false;

    u.scale(s, s);
    *r = a + u;
    return true;
}

GraphNode::GraphNode(mc_blink::WebLayerImpl* layer_impl)
    : layer(layer_impl),
    incoming_edge_weight(0.f) {}

GraphNode::~GraphNode() {}

LayerSorter::LayerSorter()
    : z_range_(0.f) {}

LayerSorter::~LayerSorter() {}

static float CheckFloatingPointNumericAmcuracy(float a, float b) {
    float abs_dif = std::abs(b - a);
    float abs_max = std::max(std::abs(b), std::abs(a));
    // Check to see if we've got a result with a reasonable amount of error.
    return abs_dif / abs_max;
}

// Checks whether layer "a" draws on top of layer "b". The weight value returned
// is an indication of the maximum z-depth difference between the layers or zero
// if the layers are found to be intesecting (some features are in front and
// some are behind).
LayerSorter::ABCompareResult LayerSorter::CheckOverlap(LayerShape* a,
    LayerShape* b,
    float z_threshold,
    float* weight) {
    *weight = 0.f;

    // Early out if the projected bounds don't overlap.
    if (!a->projected_bounds.intersects(b->projected_bounds))
        return None;

    blink::FloatPoint aPoints[4] = { a->projected_quad.p1(),
        a->projected_quad.p2(),
        a->projected_quad.p3(),
        a->projected_quad.p4() };
    blink::FloatPoint bPoints[4] = { b->projected_quad.p1(),
        b->projected_quad.p2(),
        b->projected_quad.p3(),
        b->projected_quad.p4() };

    // Make a list of points that inside both layer quad projections.
    Vector<blink::FloatPoint> overlap_points;

    // Check all four corners of one layer against the other layer's quad.
    for (int i = 0; i < 4; ++i) {
        if (a->projected_quad.containsPoint(bPoints[i]))
            overlap_points.append(bPoints[i]);
        if (b->projected_quad.containsPoint(aPoints[i]))
            overlap_points.append(aPoints[i]);
    }

    // Check all the edges of one layer for intersection with the other layer's
    // edges.
    blink::FloatPoint r;
    for (int ea = 0; ea < 4; ++ea)
        for (int eb = 0; eb < 4; ++eb)
            if (EdgeEdgeTest(aPoints[ea], aPoints[(ea + 1) % 4],
                bPoints[eb], bPoints[(eb + 1) % 4],
                &r))
                overlap_points.append(r);

    if (overlap_points.isEmpty())
        return None;

    // Check the corresponding layer depth value for all overlap points to
    // determine which layer is in front.
    float max_positive = 0.f;
    float max_negative = 0.f;

    // This flag tracks the existance of a numerically amcurate seperation
    // between two layers.  If there is no amcurate seperation, the layers
    // cannot be effectively sorted.
    bool amcurate = false;

    for (size_t o = 0; o < overlap_points.size(); o++) {
        float za = a->LayerZFromProjectedPoint(overlap_points[o]);
        float zb = b->LayerZFromProjectedPoint(overlap_points[o]);

        // Here we attempt to avoid numeric issues with layers that are too
        // close together.  If we have 2-sided quads that are very close
        // together then we will draw them in document order to avoid
        // flickering.  The correct solution is for the content maker to turn
        // on back-face culling or move the quads apart (if they're not two
        // sides of one object).
        if (CheckFloatingPointNumericAmcuracy(za, zb) > k_layer_epsilon)
            amcurate = true;

        float diff = za - zb;
        if (diff > max_positive)
            max_positive = diff;
        if (diff < max_negative)
            max_negative = diff;
    }

    // If we can't tell which should come first, we use document order.
    if (!amcurate)
        return ABeforeB;

    float max_diff = std::abs(max_positive) > std::abs(max_negative) ?
    max_positive : max_negative;

    // If the results are inconsistent (and the z difference substantial to rule
    // out numerical errors) then the layers are intersecting. We will still
    // return an order based on the maximum depth difference but with an edge
    // weight of zero these layers will get priority if a graph cycle is present
    // and needs to be broken.
    if (max_positive > z_threshold && max_negative < -z_threshold)
        *weight = 0.f;
    else
        *weight = std::abs(max_diff);

    // Maintain relative order if the layers have the same depth at all
    // intersection points.
    if (max_diff <= 0.f)
        return ABeforeB;

    return BBeforeA;
}

LayerShape::LayerShape() {}

LayerShape::LayerShape(float width, float height, const SkMatrix44& draw_transform) {
    FloatQuad layer_quad(FloatRect(0.f, 0.f, width, height));

    // Compute the projection of the layer quad onto the z = 0 plane.

    blink::FloatPoint clipped_quad[8];
    int num_vertices_in_clipped_quad;
    MathUtil::MapClippedQuad(draw_transform,
        layer_quad,
        clipped_quad,
        &num_vertices_in_clipped_quad);

    if (num_vertices_in_clipped_quad < 3) {
        projected_bounds = FloatRect();
        return;
    }

    projected_bounds = MathUtil::ComputeEnclosingRectOfVertices(clipped_quad, num_vertices_in_clipped_quad);

    // NOTE: it will require very significant refactoring and overhead to deal
    // with generalized polygons or multiple quads per layer here. For the sake of
    // layer sorting it is equally correct to take a subsection of the polygon
    // that can be made into a quad. This will only be incorrect in the case of
    // intersecting layers, which are not supported yet anyway.
    projected_quad.setP1(clipped_quad[0]);
    projected_quad.setP2(clipped_quad[1]);
    projected_quad.setP3(clipped_quad[2]);
    if (num_vertices_in_clipped_quad >= 4) {
        projected_quad.setP4(clipped_quad[3]);
    }
    else {
        // This will be a degenerate quad that is actually a triangle.
        projected_quad.setP4(clipped_quad[2]);
    }

    // Compute the normal of the layer's plane.
    bool clipped = false;
    blink::FloatPoint3D c1 = MathUtil::MapPoint(draw_transform, blink::FloatPoint3D(0.f, 0.f, 0.f), &clipped);
    blink::FloatPoint3D c2 = MathUtil::MapPoint(draw_transform, blink::FloatPoint3D(0.f, 1.f, 0.f), &clipped);
    blink::FloatPoint3D c3 = MathUtil::MapPoint(draw_transform, blink::FloatPoint3D(1.f, 0.f, 0.f), &clipped);
    // TODO(shawnsingh): Deal with clipping.
    blink::FloatPoint3D c12 = c2 - c1;
    blink::FloatPoint3D c13 = c3 - c1;
    layer_normal = MathUtil::CrossProduct(c13, c12);

    transform_origin = c1;
}

LayerShape::~LayerShape() {}

// Returns the Z coordinate of a point on the layer that projects
// to point p which lies on the z = 0 plane. It does it by computing the
// intersection of a line starting from p along the Z axis and the plane
// of the layer.
float LayerShape::LayerZFromProjectedPoint(const blink::FloatPoint& p) const {
    blink::FloatPoint3D z_axis(0.f, 0.f, 1.f);
    blink::FloatPoint3D w = blink::FloatPoint3D(p) - transform_origin;

    float d = MathUtil::DotProduct(layer_normal, z_axis);
    float n = -MathUtil::DotProduct(layer_normal, w);

    // Check if layer is parallel to the z = 0 axis which will make it
    // invisible and hence returning zero is fine.
    if (!d)
        return 0.f;

    // The intersection point would be given by:
    // p + (n / d) * u  but since we are only interested in the
    // z coordinate and p's z coord is zero, all we need is the value of n/d.
    return n / d;
}

void LayerSorter::CreateGraphNodes(WebLayerImplList::iterator first, WebLayerImplList::iterator last) {
    //DVLOG(2) << "Creating graph nodes:";
    float min_z = FLT_MAX;
    float max_z = -FLT_MAX;
    //OutputDebugStringA("\n-----------\n");
    for (WebLayerImplList::const_iterator it = first; it < last; it++) {
        nodes_.append(GraphNode(*it));
        GraphNode& node = nodes_.at(nodes_.size() - 1);
        if (!node.layer->drawsContent())
            continue;

//         DVLOG(2) << "Layer " << node.layer->id() <<
//             " (" << node.layer->bounds().width() <<
//             " x " << node.layer->bounds().height() << ")";

        SkMatrix44 draw_transform;
        float layer_width, layer_height;
        draw_transform = node.layer->drawTransform();
        layer_width = node.layer->bounds().width;
        layer_height = node.layer->bounds().height;
        //////////////////////////////////////////////////////////////////////////
//         Vector<char> output; // weolar
//         output.resize(101);
//         float allTrans = 0;
//         float allTrans2 = 0;
//         for (int i = 0; i < 4; ++i) {
//             for (int j = 0; j < 4; ++j) {
//                 allTrans += draw_transform.get(i, j);
//                 allTrans2 += node.layer->drawProperties()->currentTransform.get(i, j);
//             }
//         }
// 
//         sprintf_s(output.data(), 100, "LayerSorter::CreateGraphNodes %f %f, %f %f\n", allTrans, allTrans2, layer_width, layer_height);
//         OutputDebugStringA(output.data());
        //////////////////////////////////////////////////////////////////////////
        node.shape = LayerShape(layer_width, layer_height, draw_transform);

        max_z = std::max(max_z, node.shape.transform_origin.z());
        min_z = std::min(min_z, node.shape.transform_origin.z());
    }
    //OutputDebugStringA("-----------\n");

    z_range_ = std::abs(max_z - min_z);
}

void LayerSorter::CreateGraphEdges() {
    //DVLOG(2) << "Edges:";
    // Fraction of the total z_range below which z differences
    // are not considered reliable.
    const float z_threshold_factor = 0.01f;
    float z_threshold = z_range_ * z_threshold_factor;

    for (size_t na = 0; na < nodes_.size(); na++) {
        GraphNode& node_a = nodes_[na];
        if (!node_a.layer->drawsContent())
            continue;
        for (size_t nb = na + 1; nb < nodes_.size(); nb++) {
            GraphNode& node_b = nodes_[nb];
            if (!node_b.layer->drawsContent())
                continue;
            float weight = 0.f;
            ABCompareResult overlap_result = CheckOverlap(&node_a.shape,
                &node_b.shape,
                z_threshold,
                &weight);
            GraphNode* start_node = NULL;
            GraphNode* end_node = NULL;
            if (overlap_result == ABeforeB) {
                start_node = &node_a;
                end_node = &node_b;
            }
            else if (overlap_result == BBeforeA) {
                start_node = &node_b;
                end_node = &node_a;
            }

            if (start_node) {
                //DVLOG(2) << start_node->layer->id() << " -> " << end_node->layer->id();
                edges_.append(GraphEdge(start_node, end_node, weight));
            }
        }
    }

    for (size_t i = 0; i < edges_.size(); i++) {
        GraphEdge& edge = edges_[i];
        //active_edges_[&edge] = &edge;
        active_edges_.set(&edge, &edge);
        edge.from->outgoing.append(&edge);
        edge.to->incoming.append(&edge);
        edge.to->incoming_edge_weight += edge.weight;
    }
}

// Finds and removes an edge from the list by doing a swap with the
// last element of the list.
void LayerSorter::RemoveEdgeFromList(GraphEdge* edge, Vector<GraphEdge*>* list)
{
    size_t iter = list->find(edge);
    ASSERT(iter != WTF::kNotFound);
    list->remove(iter);
}

// Sorts the given list of layers such that they can be painted in a
// back-to-front order. Sorting produces correct results for non-intersecting
// layers that don't have cyclical order dependencies. Cycles and intersections
// are broken (somewhat) aribtrarily. Sorting of layers is done via a
// topological sort of a directed graph whose nodes are the layers themselves.
// An edge from node A to node B signifies that layer A needs to be drawn before
// layer B. If A and B have no dependency between each other, then we preserve
// the ordering of those layers as they were in the original list.
//
// The draw order between two layers is determined by projecting the two
// triangles making up each layer quad to the Z = 0 plane, finding points of
// intersection between the triangles and backprojecting those points to the
// plane of the layer to determine the corresponding Z coordinate. The layer
// with the lower Z coordinate (farther from the eye) needs to be rendered
// first.
//
// If the layer projections don't intersect, then no edges (dependencies) are
// created between them in the graph. HOWEVER, in this case we still need to
// preserve the ordering of the original list of layers, since that list should
// already have proper z-index ordering of layers.
//
void LayerSorter::Sort(WebLayerImplList::iterator first, WebLayerImplList::iterator last) {
    //////////////////////////////////////////////////////////////////////////
//     Vector<char> output; // weolar
//     output.resize(101);
//     OutputDebugStringA("LayerSorter::Sort ");
//     for (WebLayerImplList::iterator it = first; it < last; it++) {
//         sprintf_s(output.data(), 100, " %d ", (*it)->id());
//         OutputDebugStringA(output.data());
//     }
//     OutputDebugStringA("\n");
    //////////////////////////////////////////////////////////////////////////

    //DVLOG(2) << "Sorting start ----";
    CreateGraphNodes(first, last);

    CreateGraphEdges();

    Vector<GraphNode*> sorted_list;
    Deque<GraphNode*> no_incoming_edge_node_list;

    // Find all the nodes that don't have incoming edges.
    for (NodeList::iterator la = nodes_.begin(); la < nodes_.end(); la++) {
        if (!la->incoming.size())
            no_incoming_edge_node_list.append(&(*la));
    }

    //DVLOG(2) << "Sorted list: ";
    while (active_edges_.size() || no_incoming_edge_node_list.size()) {
        while (no_incoming_edge_node_list.size()) {
            // It is necessary to preserve the existing ordering of layers, when there
            // are no explicit dependencies (because this existing ordering has
            // correct z-index/layout ordering). To preserve this ordering, we process
            // Nodes in the same order that they were added to the list.
            GraphNode* from_node = no_incoming_edge_node_list.first();
            no_incoming_edge_node_list.removeFirst();

            // Add it to the final list.
            sorted_list.append(from_node);

            //DVLOG(2) << from_node->layer->id() << ", ";

            // Remove all its outgoing edges from the graph.
            for (size_t i = 0; i < from_node->outgoing.size(); i++) {
                GraphEdge* outgoing_edge = from_node->outgoing[i];

                active_edges_.remove(outgoing_edge);
                RemoveEdgeFromList(outgoing_edge, &outgoing_edge->to->incoming);
                outgoing_edge->to->incoming_edge_weight -= outgoing_edge->weight;

                if (!outgoing_edge->to->incoming.size())
                    no_incoming_edge_node_list.append(outgoing_edge->to);
            }
            from_node->outgoing.clear();
        }

        if (!active_edges_.size())
            break;

        // If there are still active edges but the list of nodes without incoming
        // edges is empty then we have run into a cycle. Break the cycle by finding
        // the node with the smallest overall incoming edge weight and use it. This
        // will favor nodes that have zero-weight incoming edges i.e. layers that
        // are being omcluded by a layer that intersects them.
        float min_incoming_edge_weight = FLT_MAX;
        GraphNode* next_node = NULL;
        for (size_t i = 0; i < nodes_.size(); i++) {
            if (nodes_[i].incoming.size() &&
                nodes_[i].incoming_edge_weight < min_incoming_edge_weight) {
                min_incoming_edge_weight = nodes_[i].incoming_edge_weight;
                next_node = &nodes_[i];
            }
        }
        ASSERT(next_node);
        // Remove all its incoming edges.
        for (size_t e = 0; e < next_node->incoming.size(); e++) {
            GraphEdge* incoming_edge = next_node->incoming[e];

            active_edges_.remove(incoming_edge);
            RemoveEdgeFromList(incoming_edge, &incoming_edge->from->outgoing);
        }
        next_node->incoming.clear();
        next_node->incoming_edge_weight = 0.f;
        no_incoming_edge_node_list.append(next_node);
//         DVLOG(2) << "Breaking cycle by cleaning up incoming edges from " <<
//             next_node->layer->id() <<
//             " (weight = " << min_incoming_edge_weight << ")";
    }

    // Note: The original elements of the list are in no danger of having their
    // ref count go to zero here as they are all nodes of the layer hierarchy and
    // are kept alive by their parent nodes.
    int count = 0;
    for (WebLayerImplList::iterator it = first; it < last; it++)
        *it = sorted_list[count++]->layer;
    //////////////////////////////////////////////////////////////////////////
//     output.resize(101);
//     OutputDebugStringA("LayerSorter::Sort ");
//     for (WebLayerImplList::iterator it = first; it < last; it++) {
//         sprintf_s(output.data(), 100, " %d ", (*it)->id());
//         OutputDebugStringA(output.data());
//     }
//     OutputDebugStringA("\n");
    //////////////////////////////////////////////////////////////////////////

    // DVLOG(2) << "Sorting end ----";

    nodes_.clear();
    edges_.clear();
    active_edges_.clear();
}

}  // namespace mc
