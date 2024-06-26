// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/property_tree.h"

#include "cc/test/geometry_test_utils.h"
#include "cc/trees/draw_property_utils.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

TEST(PropertyTreeTest, ComputeTransformRoot)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.local.Translate(2, 2);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    gfx::Transform expected;
    gfx::Transform transform;
    bool success = tree.ComputeTransform(0, 0, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.Translate(2, 2);
    success = tree.ComputeTransform(0, -1, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.MakeIdentity();
    expected.Translate(-2, -2);
    success = tree.ComputeTransform(-1, 0, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);
}

TEST(PropertyTreeTest, ComputeTransformChild)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.local.Translate(2, 2);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode child;
    child.data.local.Translate(3, 3);
    child.data.target_id = 0;
    child.data.source_node_id = 0;

    tree.Insert(child, 0);
    tree.UpdateTransforms(1);

    gfx::Transform expected;
    gfx::Transform transform;

    expected.Translate(3, 3);
    bool success = tree.ComputeTransform(1, 0, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.MakeIdentity();
    expected.Translate(-3, -3);
    success = tree.ComputeTransform(0, 1, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.MakeIdentity();
    expected.Translate(5, 5);
    success = tree.ComputeTransform(1, -1, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.MakeIdentity();
    expected.Translate(-5, -5);
    success = tree.ComputeTransform(-1, 1, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);
}

TEST(PropertyTreeTest, ComputeTransformSibling)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.local.Translate(2, 2);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode child;
    child.data.local.Translate(3, 3);
    child.data.source_node_id = 0;
    child.data.target_id = 0;

    TransformNode sibling;
    sibling.data.local.Translate(7, 7);
    sibling.data.source_node_id = 0;
    sibling.data.target_id = 0;

    tree.Insert(child, 0);
    tree.Insert(sibling, 0);

    tree.UpdateTransforms(1);
    tree.UpdateTransforms(2);

    gfx::Transform expected;
    gfx::Transform transform;

    expected.Translate(4, 4);
    bool success = tree.ComputeTransform(2, 1, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.MakeIdentity();
    expected.Translate(-4, -4);
    success = tree.ComputeTransform(1, 2, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);
}

TEST(PropertyTreeTest, ComputeTransformSiblingSingularAncestor)
{
    // In this test, we have the following tree:
    // root
    //   + singular
    //     + child
    //     + sibling
    // Since the lowest common ancestor of |child| and |sibling| has a singular
    // transform, we cannot use screen space transforms to compute change of basis
    // transforms between these nodes.
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.local.Translate(2, 2);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode singular;
    singular.data.local.matrix().set(2, 2, 0.0);
    singular.data.source_node_id = 0;
    singular.data.target_id = 0;

    TransformNode child;
    child.data.local.Translate(3, 3);
    child.data.source_node_id = 1;
    child.data.target_id = 0;

    TransformNode sibling;
    sibling.data.local.Translate(7, 7);
    sibling.data.source_node_id = 1;
    sibling.data.target_id = 0;

    tree.Insert(singular, 0);
    tree.Insert(child, 1);
    tree.Insert(sibling, 1);

    tree.UpdateTransforms(1);
    tree.UpdateTransforms(2);
    tree.UpdateTransforms(3);

    gfx::Transform expected;
    gfx::Transform transform;

    expected.Translate(4, 4);
    bool success = tree.ComputeTransform(3, 2, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    transform.MakeIdentity();
    expected.MakeIdentity();
    expected.Translate(-4, -4);
    success = tree.ComputeTransform(2, 3, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);
}

TEST(PropertyTreeTest, TransformsWithFlattening)
{
    TransformTree tree;

    int grand_parent = tree.Insert(TransformNode(), 0);
    tree.Node(grand_parent)->data.content_target_id = grand_parent;
    tree.Node(grand_parent)->data.target_id = grand_parent;
    tree.Node(grand_parent)->data.source_node_id = 0;

    gfx::Transform rotation_about_x;
    rotation_about_x.RotateAboutXAxis(15);

    int parent = tree.Insert(TransformNode(), grand_parent);
    tree.Node(parent)->data.needs_sublayer_scale = true;
    tree.Node(parent)->data.target_id = grand_parent;
    tree.Node(parent)->data.content_target_id = parent;
    tree.Node(parent)->data.source_node_id = grand_parent;
    tree.Node(parent)->data.local = rotation_about_x;

    int child = tree.Insert(TransformNode(), parent);
    tree.Node(child)->data.target_id = parent;
    tree.Node(child)->data.content_target_id = parent;
    tree.Node(child)->data.source_node_id = parent;
    tree.Node(child)->data.flattens_inherited_transform = true;
    tree.Node(child)->data.local = rotation_about_x;

    int grand_child = tree.Insert(TransformNode(), child);
    tree.Node(grand_child)->data.target_id = parent;
    tree.Node(grand_child)->data.content_target_id = parent;
    tree.Node(grand_child)->data.source_node_id = child;
    tree.Node(grand_child)->data.flattens_inherited_transform = true;
    tree.Node(grand_child)->data.local = rotation_about_x;

    tree.set_needs_update(true);
    ComputeTransforms(&tree);

    gfx::Transform flattened_rotation_about_x = rotation_about_x;
    flattened_rotation_about_x.FlattenTo2d();

    EXPECT_TRANSFORMATION_MATRIX_EQ(rotation_about_x,
        tree.Node(child)->data.to_target);

    EXPECT_TRANSFORMATION_MATRIX_EQ(flattened_rotation_about_x * rotation_about_x,
        tree.Node(child)->data.to_screen);

    EXPECT_TRANSFORMATION_MATRIX_EQ(flattened_rotation_about_x * rotation_about_x,
        tree.Node(grand_child)->data.to_target);

    EXPECT_TRANSFORMATION_MATRIX_EQ(flattened_rotation_about_x * flattened_rotation_about_x * rotation_about_x,
        tree.Node(grand_child)->data.to_screen);

    gfx::Transform grand_child_to_child;
    bool success = tree.ComputeTransform(grand_child, child, &grand_child_to_child);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(rotation_about_x, grand_child_to_child);

    // Remove flattening at grand_child, and recompute transforms.
    tree.Node(grand_child)->data.flattens_inherited_transform = false;
    tree.set_needs_update(true);
    ComputeTransforms(&tree);

    EXPECT_TRANSFORMATION_MATRIX_EQ(rotation_about_x * rotation_about_x,
        tree.Node(grand_child)->data.to_target);

    EXPECT_TRANSFORMATION_MATRIX_EQ(
        flattened_rotation_about_x * rotation_about_x * rotation_about_x,
        tree.Node(grand_child)->data.to_screen);

    success = tree.ComputeTransform(grand_child, child, &grand_child_to_child);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(rotation_about_x, grand_child_to_child);
}

TEST(PropertyTreeTest, MultiplicationOrder)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.local.Translate(2, 2);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode child;
    child.data.local.Scale(2, 2);
    child.data.target_id = 0;
    child.data.source_node_id = 0;

    tree.Insert(child, 0);
    tree.UpdateTransforms(1);

    gfx::Transform expected;
    expected.Translate(2, 2);
    expected.Scale(2, 2);

    gfx::Transform transform;
    gfx::Transform inverse;

    bool success = tree.ComputeTransform(1, -1, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    success = tree.ComputeTransform(-1, 1, &inverse);
    EXPECT_TRUE(success);

    transform = transform * inverse;
    expected.MakeIdentity();
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);
}

TEST(PropertyTreeTest, ComputeTransformWithUninvertibleTransform)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode child;
    child.data.local.Scale(0, 0);
    child.data.target_id = 0;
    child.data.source_node_id = 0;

    tree.Insert(child, 0);
    tree.UpdateTransforms(1);

    gfx::Transform expected;
    expected.Scale(0, 0);

    gfx::Transform transform;
    gfx::Transform inverse;

    bool success = tree.ComputeTransform(1, 0, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected, transform);

    // To compute this would require inverting the 0 matrix, so we cannot
    // succeed.
    success = tree.ComputeTransform(0, 1, &inverse);
    EXPECT_FALSE(success);
}

TEST(PropertyTreeTest, ComputeTransformWithSublayerScale)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode grand_parent;
    grand_parent.data.local.Scale(2.f, 2.f);
    grand_parent.data.target_id = 0;
    grand_parent.data.source_node_id = 0;
    grand_parent.data.needs_sublayer_scale = true;
    int grand_parent_id = tree.Insert(grand_parent, 0);
    tree.UpdateTransforms(grand_parent_id);

    TransformNode parent;
    parent.data.local.Translate(15.f, 15.f);
    parent.data.target_id = grand_parent_id;
    parent.data.source_node_id = grand_parent_id;
    int parent_id = tree.Insert(parent, grand_parent_id);
    tree.UpdateTransforms(parent_id);

    TransformNode child;
    child.data.local.Scale(3.f, 3.f);
    child.data.target_id = grand_parent_id;
    child.data.source_node_id = parent_id;
    int child_id = tree.Insert(child, parent_id);
    tree.UpdateTransforms(child_id);

    TransformNode grand_child;
    grand_child.data.local.Scale(5.f, 5.f);
    grand_child.data.target_id = grand_parent_id;
    grand_child.data.source_node_id = child_id;
    grand_child.data.needs_sublayer_scale = true;
    int grand_child_id = tree.Insert(grand_child, child_id);
    tree.UpdateTransforms(grand_child_id);

    EXPECT_EQ(gfx::Vector2dF(2.f, 2.f),
        tree.Node(grand_parent_id)->data.sublayer_scale);
    EXPECT_EQ(gfx::Vector2dF(30.f, 30.f),
        tree.Node(grand_child_id)->data.sublayer_scale);

    // Compute transform from grand_parent to grand_child.
    gfx::Transform expected_transform_without_sublayer_scale;
    expected_transform_without_sublayer_scale.Scale(1.f / 15.f, 1.f / 15.f);
    expected_transform_without_sublayer_scale.Translate(-15.f, -15.f);

    gfx::Transform expected_transform_with_dest_sublayer_scale;
    expected_transform_with_dest_sublayer_scale.Scale(30.f, 30.f);
    expected_transform_with_dest_sublayer_scale.Scale(1.f / 15.f, 1.f / 15.f);
    expected_transform_with_dest_sublayer_scale.Translate(-15.f, -15.f);

    gfx::Transform expected_transform_with_source_sublayer_scale;
    expected_transform_with_source_sublayer_scale.Scale(1.f / 15.f, 1.f / 15.f);
    expected_transform_with_source_sublayer_scale.Translate(-15.f, -15.f);
    expected_transform_with_source_sublayer_scale.Scale(0.5f, 0.5f);

    gfx::Transform transform;
    bool success = tree.ComputeTransform(grand_parent_id, grand_child_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform_without_sublayer_scale,
        transform);

    success = tree.ComputeTransformWithDestinationSublayerScale(
        grand_parent_id, grand_child_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform_with_dest_sublayer_scale,
        transform);

    success = tree.ComputeTransformWithSourceSublayerScale(
        grand_parent_id, grand_child_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform_with_source_sublayer_scale,
        transform);

    // Now compute transform from grand_child to grand_parent.
    expected_transform_without_sublayer_scale.MakeIdentity();
    expected_transform_without_sublayer_scale.Translate(15.f, 15.f);
    expected_transform_without_sublayer_scale.Scale(15.f, 15.f);

    expected_transform_with_dest_sublayer_scale.MakeIdentity();
    expected_transform_with_dest_sublayer_scale.Scale(2.f, 2.f);
    expected_transform_with_dest_sublayer_scale.Translate(15.f, 15.f);
    expected_transform_with_dest_sublayer_scale.Scale(15.f, 15.f);

    expected_transform_with_source_sublayer_scale.MakeIdentity();
    expected_transform_with_source_sublayer_scale.Translate(15.f, 15.f);
    expected_transform_with_source_sublayer_scale.Scale(15.f, 15.f);
    expected_transform_with_source_sublayer_scale.Scale(1.f / 30.f, 1.f / 30.f);

    success = tree.ComputeTransform(grand_child_id, grand_parent_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform_without_sublayer_scale,
        transform);

    success = tree.ComputeTransformWithDestinationSublayerScale(
        grand_child_id, grand_parent_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform_with_dest_sublayer_scale,
        transform);

    success = tree.ComputeTransformWithSourceSublayerScale(
        grand_child_id, grand_parent_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform_with_source_sublayer_scale,
        transform);
}

TEST(PropertyTreeTest, ComputeTransformToTargetWithZeroSublayerScale)
{
    TransformTree tree;
    TransformNode& root = *tree.Node(0);
    root.data.target_id = 0;
    tree.UpdateTransforms(0);

    TransformNode grand_parent;
    grand_parent.data.local.Scale(2.f, 0.f);
    grand_parent.data.target_id = 0;
    grand_parent.data.source_node_id = 0;
    grand_parent.data.needs_sublayer_scale = true;
    int grand_parent_id = tree.Insert(grand_parent, 0);
    tree.Node(grand_parent_id)->data.content_target_id = grand_parent_id;
    tree.UpdateTransforms(grand_parent_id);

    TransformNode parent;
    parent.data.local.Translate(1.f, 1.f);
    parent.data.target_id = grand_parent_id;
    parent.data.content_target_id = grand_parent_id;
    parent.data.source_node_id = grand_parent_id;
    int parent_id = tree.Insert(parent, grand_parent_id);
    tree.UpdateTransforms(parent_id);

    TransformNode child;
    child.data.local.Translate(3.f, 4.f);
    child.data.target_id = grand_parent_id;
    child.data.content_target_id = grand_parent_id;
    child.data.source_node_id = parent_id;
    int child_id = tree.Insert(child, parent_id);
    tree.UpdateTransforms(child_id);

    gfx::Transform expected_transform;
    expected_transform.Translate(4.f, 5.f);

    gfx::Transform transform;
    bool success = tree.ComputeTransform(child_id, grand_parent_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform, transform);

    tree.Node(grand_parent_id)->data.local.MakeIdentity();
    tree.Node(grand_parent_id)->data.local.Scale(0.f, 2.f);
    tree.Node(grand_parent_id)->data.needs_local_transform_update = true;
    tree.set_needs_update(true);

    ComputeTransforms(&tree);

    success = tree.ComputeTransform(child_id, grand_parent_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform, transform);

    tree.Node(grand_parent_id)->data.local.MakeIdentity();
    tree.Node(grand_parent_id)->data.local.Scale(0.f, 0.f);
    tree.Node(grand_parent_id)->data.needs_local_transform_update = true;
    tree.set_needs_update(true);

    ComputeTransforms(&tree);

    success = tree.ComputeTransform(child_id, grand_parent_id, &transform);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(expected_transform, transform);
}

TEST(PropertyTreeTest, FlatteningWhenDestinationHasOnlyFlatAncestors)
{
    // This tests that flattening is performed correctly when
    // destination and its ancestors are flat, but there are 3d transforms
    // and flattening between the source and destination.
    TransformTree tree;

    int parent = tree.Insert(TransformNode(), 0);
    tree.Node(parent)->data.content_target_id = parent;
    tree.Node(parent)->data.target_id = parent;
    tree.Node(parent)->data.source_node_id = 0;
    tree.Node(parent)->data.local.Translate(2, 2);

    gfx::Transform rotation_about_x;
    rotation_about_x.RotateAboutXAxis(15);

    int child = tree.Insert(TransformNode(), parent);
    tree.Node(child)->data.content_target_id = child;
    tree.Node(child)->data.target_id = child;
    tree.Node(child)->data.source_node_id = parent;
    tree.Node(child)->data.local = rotation_about_x;

    int grand_child = tree.Insert(TransformNode(), child);
    tree.Node(grand_child)->data.content_target_id = grand_child;
    tree.Node(grand_child)->data.target_id = grand_child;
    tree.Node(grand_child)->data.source_node_id = child;
    tree.Node(grand_child)->data.flattens_inherited_transform = true;

    tree.set_needs_update(true);
    ComputeTransforms(&tree);

    gfx::Transform flattened_rotation_about_x = rotation_about_x;
    flattened_rotation_about_x.FlattenTo2d();

    gfx::Transform grand_child_to_parent;
    bool success = tree.ComputeTransform(grand_child, parent, &grand_child_to_parent);
    EXPECT_TRUE(success);
    EXPECT_TRANSFORMATION_MATRIX_EQ(flattened_rotation_about_x,
        grand_child_to_parent);
}

TEST(PropertyTreeTest, ScreenSpaceOpacityUpdateTest)
{
    // This tests that screen space opacity is updated for the subtree when
    // opacity of a node changes.
    EffectTree tree;

    int parent = tree.Insert(EffectNode(), 0);
    int child = tree.Insert(EffectNode(), parent);

    EXPECT_EQ(tree.Node(child)->data.screen_space_opacity, 1.f);
    tree.Node(parent)->data.opacity = 0.5f;
    tree.set_needs_update(true);
    ComputeOpacities(&tree);
    EXPECT_EQ(tree.Node(child)->data.screen_space_opacity, 0.5f);

    tree.Node(child)->data.opacity = 0.5f;
    tree.set_needs_update(true);
    ComputeOpacities(&tree);
    EXPECT_EQ(tree.Node(child)->data.screen_space_opacity, 0.25f);
}

TEST(PropertyTreeTest, NonIntegerTranslationTest)
{
    // This tests that when a node has non-integer translation, the information
    // is propagated to the subtree.
    TransformTree tree;

    int parent = tree.Insert(TransformNode(), 0);
    tree.Node(parent)->data.target_id = parent;
    tree.Node(parent)->data.local.Translate(1.5f, 1.5f);

    int child = tree.Insert(TransformNode(), parent);
    tree.Node(child)->data.target_id = parent;
    tree.Node(child)->data.local.Translate(1, 1);
    tree.set_needs_update(true);
    ComputeTransforms(&tree);
    EXPECT_FALSE(
        tree.Node(parent)->data.node_and_ancestors_have_only_integer_translation);
    EXPECT_FALSE(
        tree.Node(child)->data.node_and_ancestors_have_only_integer_translation);

    tree.Node(parent)->data.local.Translate(0.5f, 0.5f);
    tree.Node(child)->data.local.Translate(0.5f, 0.5f);
    tree.set_needs_update(true);
    ComputeTransforms(&tree);
    EXPECT_TRUE(
        tree.Node(parent)->data.node_and_ancestors_have_only_integer_translation);
    EXPECT_FALSE(
        tree.Node(child)->data.node_and_ancestors_have_only_integer_translation);

    tree.Node(child)->data.local.Translate(0.5f, 0.5f);
    tree.Node(child)->data.target_id = child;
    tree.set_needs_update(true);
    ComputeTransforms(&tree);
    EXPECT_TRUE(
        tree.Node(parent)->data.node_and_ancestors_have_only_integer_translation);
    EXPECT_TRUE(
        tree.Node(child)->data.node_and_ancestors_have_only_integer_translation);
}

} // namespace cc
