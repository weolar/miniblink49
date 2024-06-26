// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/scoped_ptr_deque.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/output/bsp_tree.h"
#include "cc/output/bsp_walk_action.h"
#include "cc/quads/draw_polygon.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

#define EXPECT_SORTED_LISTS_EQ(polygon_list, compare_list)              \
    do {                                                                \
        EXPECT_EQ(polygon_list.size(), compare_list.size());            \
        for (unsigned int i = 0; i < polygon_list.size(); i++) {        \
            EXPECT_EQ(polygon_list[i]->order_index(), compare_list[i]); \
        }                                                               \
    } while (false);

#define INT_VECTOR_FROM_ARRAY(array) \
    std::vector<int>(array, array + arraysize(array))

#define CREATE_DRAW_POLYGON(vertex_vector, normal, polygon_id) \
    new DrawPolygon(NULL, vertex_vector, normal, polygon_id)

    class BspTreeTest {
    public:
        static void RunTest(ScopedPtrDeque<DrawPolygon>* test_polygons,
            const std::vector<int>& compare_list)
        {
            BspTree bsp_tree(test_polygons);

            std::vector<DrawPolygon*> sorted_list;
            BspWalkActionToVector action_handler(&sorted_list);
            bsp_tree.TraverseWithActionHandler(&action_handler);

            EXPECT_SORTED_LISTS_EQ(sorted_list, compare_list);
            EXPECT_TRUE(VerifySidedness(bsp_tree.root()));
        }

        static bool VerifySidedness(const scoped_ptr<BspNode>& node)
        {
            // We check if both the front and back child nodes have geometry that is
            // completely on the expected side of the current node.
            bool front_ok = true;
            bool back_ok = true;
            if (node->back_child) {
                // Make sure the back child lies entirely behind this node.
                BspCompareResult result = DrawPolygon::SideCompare(
                    *(node->back_child->node_data), *(node->node_data));
                if (result != BSP_BACK) {
                    return false;
                }
                back_ok = VerifySidedness(node->back_child);
            }
            // Make sure the front child lies entirely in front of this node.
            if (node->front_child) {
                BspCompareResult result = DrawPolygon::SideCompare(
                    *(node->front_child->node_data), *(node->node_data));
                if (result != BSP_FRONT) {
                    return false;
                }
                front_ok = VerifySidedness(node->front_child);
            }
            if (!back_ok || !front_ok) {
                return false;
            }

            // Now we need to make sure our coplanar geometry is all actually coplanar.
            for (size_t i = 0; i < node->coplanars_front.size(); i++) {
                BspCompareResult result = DrawPolygon::SideCompare(
                    *(node->coplanars_front[i]), *(node->node_data));
                if (result != BSP_COPLANAR_FRONT) {
                    return false;
                }
            }
            for (size_t i = 0; i < node->coplanars_back.size(); i++) {
                BspCompareResult result = DrawPolygon::SideCompare(
                    *(node->coplanars_back[i]), *(node->node_data));
                if (result != BSP_COPLANAR_BACK) {
                    return false;
                }
            }
            return true;
        }
    };

    // Simple standing quads all parallel with each other, causing no splits.
    TEST(BspTreeTest, NoSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(0.0f, 10.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(0.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 0.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(10.0f, 10.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(0.0f, 10.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 0.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(10.0f, 0.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(10.0f, 10.0f, -5.0f));
        std::vector<gfx::Point3F> vertices_c;
        vertices_c.push_back(gfx::Point3F(0.0f, 10.0f, 5.0f));
        vertices_c.push_back(gfx::Point3F(0.0f, 0.0f, 5.0f));
        vertices_c.push_back(gfx::Point3F(10.0f, 0.0f, 5.0f));
        vertices_c.push_back(gfx::Point3F(10.0f, 10.0f, 5.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 1));
        scoped_ptr<DrawPolygon> polygon_c(
            CREATE_DRAW_POLYGON(vertices_c, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 2));

        ScopedPtrDeque<DrawPolygon> polygon_list;
        polygon_list.push_back(polygon_a.Pass());
        polygon_list.push_back(polygon_b.Pass());
        polygon_list.push_back(polygon_c.Pass());

        int compare_ids[] = { 1, 0, 2 };
        std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
        BspTreeTest::RunTest(&polygon_list, compare_list);
    }

    // Basic two polygon split, can be viewed as a + from above.
    TEST(BspTreeTest, BasicSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(-5.0f, -5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(-5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, -5.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, 5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, 5.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1));

        ScopedPtrDeque<DrawPolygon> polygon_list;
        polygon_list.push_back(polygon_a.Pass());
        polygon_list.push_back(polygon_b.Pass());

        int compare_ids[] = { 1, 0, 1 };
        std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
        BspTreeTest::RunTest(&polygon_list, compare_list);
    }

    // Same as above with the second quad offset so it doesn't intersect. One quad
    // should be very clearly on one side of the other, and no splitting should
    // occur.
    TEST(BspTreeTest, QuadOffset)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(-5.0f, -5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(-5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, -5.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, -15.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, -15.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, -10.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, -10.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1));

        ScopedPtrDeque<DrawPolygon> polygon_list;
        polygon_list.push_back(polygon_a.Pass());
        polygon_list.push_back(polygon_b.Pass());

        int compare_ids[] = { 1, 0 };
        std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
        BspTreeTest::RunTest(&polygon_list, compare_list);
    }

    // Same as above, but this time we change the order in which the quads are
    // inserted into the tree, causing one to actually cross the plane of the other
    // and cause a split.
    TEST(BspTreeTest, QuadOffsetSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(-5.0f, -5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(-5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, -5.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, -15.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, -15.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, -10.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, -10.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1));

        ScopedPtrDeque<DrawPolygon> polygon_list;
        polygon_list.push_back(polygon_b.Pass());
        polygon_list.push_back(polygon_a.Pass());

        int compare_ids[] = { 0, 1, 0 };
        std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
        BspTreeTest::RunTest(&polygon_list, compare_list);
    }

    // In addition to what can be viewed as a + from above, another piece of
    // geometry is inserted to cut these pieces right in the middle, viewed as
    // a quad from overhead.
    TEST(BspTreeTest, ThreeWaySplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(-5.0f, -5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(-5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, -5.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, -5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, 5.0f, 5.0f));
        vertices_b.push_back(gfx::Point3F(0.0f, -5.0f, 5.0f));
        std::vector<gfx::Point3F> vertices_c;
        vertices_c.push_back(gfx::Point3F(-5.0f, 0.0f, -5.0f));
        vertices_c.push_back(gfx::Point3F(-5.0f, 0.0f, 5.0f));
        vertices_c.push_back(gfx::Point3F(5.0f, 0.0f, 5.0f));
        vertices_c.push_back(gfx::Point3F(5.0f, 0.0f, -5.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 1));
        scoped_ptr<DrawPolygon> polygon_c(
            CREATE_DRAW_POLYGON(vertices_c, gfx::Vector3dF(0.0f, 1.0f, 0.0f), 2));

        ScopedPtrDeque<DrawPolygon> polygon_list;
        polygon_list.push_back(polygon_a.Pass());
        polygon_list.push_back(polygon_b.Pass());
        polygon_list.push_back(polygon_c.Pass());

        int compare_ids[] = { 2, 1, 2, 0, 2, 1, 2 };
        std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
        BspTreeTest::RunTest(&polygon_list, compare_list);
    }

    // This test checks whether coplanar geometry, when inserted into the tree in
    // order, comes back in the same order as it should.
    TEST(BspTreeTest, Coplanar)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(-5.0f, -5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(-5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, -5.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(-4.0f, -4.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(-4.0f, 4.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(4.0f, 4.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(4.0f, -4.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_c;
        vertices_c.push_back(gfx::Point3F(-3.0f, -3.0f, 0.0f));
        vertices_c.push_back(gfx::Point3F(-3.0f, 3.0f, 0.0f));
        vertices_c.push_back(gfx::Point3F(3.0f, 3.0f, 0.0f));
        vertices_c.push_back(gfx::Point3F(3.0f, -3.0f, 0.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 1));
        scoped_ptr<DrawPolygon> polygon_c(
            CREATE_DRAW_POLYGON(vertices_c, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 2));

        scoped_ptr<DrawPolygon> polygon_d = polygon_a->CreateCopy();
        scoped_ptr<DrawPolygon> polygon_e = polygon_b->CreateCopy();
        scoped_ptr<DrawPolygon> polygon_f = polygon_c->CreateCopy();

        {
            ScopedPtrDeque<DrawPolygon> polygon_list;
            polygon_list.push_back(polygon_a.Pass());
            polygon_list.push_back(polygon_b.Pass());
            polygon_list.push_back(polygon_c.Pass());

            int compare_ids[] = { 0, 1, 2 };
            std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
            BspTreeTest::RunTest(&polygon_list, compare_list);
        }

        // Now check a different order and ensure we get that back as well
        {
            ScopedPtrDeque<DrawPolygon> polygon_list;
            polygon_list.push_back(polygon_f.Pass());
            polygon_list.push_back(polygon_d.Pass());
            polygon_list.push_back(polygon_e.Pass());

            int compare_ids[] = { 0, 1, 2 };
            std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
            BspTreeTest::RunTest(&polygon_list, compare_list);
        }
    }

    // A bunch of coplanar geometry should end up sharing a single node, and
    // result in the final piece of geometry splitting into just two pieces on
    // either side of the shared plane.
    TEST(BspTreeTest, CoplanarSplit)
    {
        std::vector<gfx::Point3F> vertices_a;
        vertices_a.push_back(gfx::Point3F(-5.0f, -5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(-5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, 5.0f, 0.0f));
        vertices_a.push_back(gfx::Point3F(5.0f, -5.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_b;
        vertices_b.push_back(gfx::Point3F(-4.0f, -4.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(-4.0f, 4.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(4.0f, 4.0f, 0.0f));
        vertices_b.push_back(gfx::Point3F(4.0f, -4.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_c;
        vertices_c.push_back(gfx::Point3F(-3.0f, -3.0f, 0.0f));
        vertices_c.push_back(gfx::Point3F(-3.0f, 3.0f, 0.0f));
        vertices_c.push_back(gfx::Point3F(3.0f, 3.0f, 0.0f));
        vertices_c.push_back(gfx::Point3F(3.0f, -3.0f, 0.0f));
        std::vector<gfx::Point3F> vertices_d;
        vertices_d.push_back(gfx::Point3F(0.0f, -15.0f, -15.0f));
        vertices_d.push_back(gfx::Point3F(0.0f, 15.0f, -15.0f));
        vertices_d.push_back(gfx::Point3F(0.0f, 15.0f, 15.0f));
        vertices_d.push_back(gfx::Point3F(0.0f, -15.0f, 15.0f));

        scoped_ptr<DrawPolygon> polygon_a(
            CREATE_DRAW_POLYGON(vertices_a, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 0));
        scoped_ptr<DrawPolygon> polygon_b(
            CREATE_DRAW_POLYGON(vertices_b, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 1));
        scoped_ptr<DrawPolygon> polygon_c(
            CREATE_DRAW_POLYGON(vertices_c, gfx::Vector3dF(0.0f, 0.0f, 1.0f), 2));
        scoped_ptr<DrawPolygon> polygon_d(
            CREATE_DRAW_POLYGON(vertices_d, gfx::Vector3dF(-1.0f, 0.0f, 0.0f), 3));

        ScopedPtrDeque<DrawPolygon> polygon_list;
        polygon_list.push_back(polygon_a.Pass());
        polygon_list.push_back(polygon_b.Pass());
        polygon_list.push_back(polygon_c.Pass());
        polygon_list.push_back(polygon_d.Pass());

        int compare_ids[] = { 3, 0, 1, 2, 3 };
        std::vector<int> compare_list = INT_VECTOR_FROM_ARRAY(compare_ids);
        BspTreeTest::RunTest(&polygon_list, compare_list);
    }

} // namespace
} // namespace cc
