// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gpu/command_buffer/service/path_manager.h"

#include "base/memory/scoped_ptr.h"
#include "gpu/command_buffer/service/gpu_service_test.h"
#include "gpu/command_buffer/service/mocks.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_mock.h"

namespace gpu {
namespace gles2 {

    class PathManagerTest : public GpuServiceTest {
    public:
        PathManagerTest() { }

    protected:
        void SetUp() override
        {
            SetUpWithGLVersion("3.0", "GL_NV_path_rendering");
            manager_.reset(new PathManager());
        }

        void TearDown() override
        {
            manager_->Destroy(true);
            manager_.reset();
            GpuServiceTest::TearDown();
        }

        scoped_ptr<PathManager> manager_;
    };

    TEST_F(PathManagerTest, Basic)
    {
        const GLuint kClient1Id = 1;
        const GLuint kService1Id = 11;
        const GLuint kClient2Id = 2;
        GLuint service_id = 0;
        manager_->CreatePathRange(kClient1Id, kClient1Id, kService1Id);
        ASSERT_TRUE(manager_->HasPathsInRange(kClient1Id, kClient1Id));
        EXPECT_TRUE(manager_->GetPath(kClient1Id, &service_id));
        EXPECT_EQ(kService1Id, service_id);

        // Check we get nothing for a non-existent path.
        service_id = 123u;
        ASSERT_FALSE(manager_->HasPathsInRange(kClient2Id, kClient2Id));
        EXPECT_FALSE(manager_->GetPath(kClient2Id, &service_id));
        EXPECT_EQ(123u, service_id);

        // Check trying to remove non-existent paths does not crash.
        manager_->RemovePaths(kClient2Id, kClient2Id);

        // Check that it gets deleted when the last reference is released.
        EXPECT_CALL(*gl_, DeletePathsNV(kService1Id, 1))
            .Times(1)
            .RetiresOnSaturation();

        // Check we can't get the path after we remove it.
        manager_->RemovePaths(kClient1Id, kClient1Id);
        ASSERT_FALSE(manager_->HasPathsInRange(kClient1Id, kClient1Id));
        EXPECT_FALSE(manager_->GetPath(kClient1Id, &service_id));
    }

    // Tests that path manager does not merge ranges that contain service ids that
    //  prevent the merging. Path ranges A and B can be merged if
    // * client ids of B start immediately after the last client id of A
    // * service ids of B start immediately after the last service id of A
    // and similarly for the 'before' case.
    TEST_F(PathManagerTest, NonContiguousServiceIds)
    {
        const GLuint kMergeCheckRange = 54;

        const struct {
            GLuint first_client_id;
            GLuint last_client_id;
            GLuint first_service_id;
        } kIdRanges[] = { { 500, 1000, 900 }, { 1001, 1155, 1 }, { 200, 499, 4888 } };
        for (auto& range : kIdRanges) {
            manager_->CreatePathRange(range.first_client_id, range.last_client_id,
                range.first_service_id);
            ASSERT_TRUE(manager_->HasPathsInRange(range.first_client_id,
                range.first_client_id));
            ASSERT_TRUE(
                manager_->HasPathsInRange(range.last_client_id, range.last_client_id));
            ASSERT_TRUE(
                manager_->HasPathsInRange(range.first_client_id, range.last_client_id));
            GLuint service_id = 0u;
            EXPECT_TRUE(manager_->GetPath(range.first_client_id + 5u, &service_id));
            EXPECT_EQ(range.first_service_id + 5u, service_id);
        }

        // Insert a mergeable range last, to check that merges
        // work. Otherwise the test could succeed because merges were not
        // working.
        auto& merge_candidate = kIdRanges[1];
        GLuint merge_candidate_range = merge_candidate.last_client_id - merge_candidate.first_client_id + 1;
        manager_->CreatePathRange(
            merge_candidate.last_client_id + 1,
            merge_candidate.last_client_id + kMergeCheckRange,
            merge_candidate.first_service_id + merge_candidate_range);

        // We detect that ranges were not merged accidentally by detecting individual
        // deletes.
        for (auto& range : kIdRanges) {
            if (&range == &merge_candidate)
                continue;
            GLsizei range_amount = range.last_client_id - range.first_client_id + 1;
            EXPECT_CALL(*gl_, DeletePathsNV(range.first_service_id, range_amount))
                .Times(1)
                .RetiresOnSaturation();
        }

        // Just a check that merges work.
        EXPECT_CALL(*gl_, DeletePathsNV(merge_candidate.first_service_id, merge_candidate_range + kMergeCheckRange))
            .Times(1)
            .RetiresOnSaturation();

        // Remove all ids. This should cause the expected amount of DeletePathsNV
        // calls.
        manager_->RemovePaths(1, std::numeric_limits<GLsizei>::max());

        for (auto& range : kIdRanges) {
            ASSERT_FALSE(
                manager_->HasPathsInRange(range.first_client_id, range.last_client_id));
        }
    }

    TEST_F(PathManagerTest, DeleteBigRange)
    {
        // Allocates two ranges which in path manager end up merging as one
        // big range. The range will be too big to fit in one DeletePaths
        // call.  Test that the range is deleted correctly with two calls.
        const GLuint kFirstClientId1 = 1;
        const GLsizei kRange1 = std::numeric_limits<GLsizei>::max() - 3;
        const GLuint kLastClientId1 = kFirstClientId1 + kRange1 - 1;
        const GLuint kFirstServiceId1 = 77;
        const GLuint kLastServiceId1 = kFirstServiceId1 + kRange1 - 1;

        const GLuint kFirstClientId2 = kLastClientId1 + 1;
        const GLsizei kRange2 = 15;
        const GLuint kLastClientId2 = kFirstClientId2 + kRange2 - 1;
        const GLuint kFirstServiceId2 = kLastServiceId1 + 1;

        const GLsizei kFirstDeleteRange = std::numeric_limits<GLsizei>::max();
        const GLsizei kSecondDeleteRange = kRange2 - (kFirstDeleteRange - kRange1);
        const GLuint kSecondDeleteFirstServiceId = kFirstServiceId1 + kFirstDeleteRange;

        EXPECT_CALL(*gl_, DeletePathsNV(kFirstServiceId1, std::numeric_limits<GLsizei>::max()))
            .RetiresOnSaturation();

        EXPECT_CALL(*gl_, DeletePathsNV(kSecondDeleteFirstServiceId, kSecondDeleteRange)).RetiresOnSaturation();

        manager_->CreatePathRange(kFirstClientId1, kLastClientId1, kFirstServiceId1);
        manager_->CreatePathRange(kFirstClientId2, kLastClientId2, kFirstServiceId2);
        manager_->RemovePaths(0, std::numeric_limits<GLuint>::max());
    }

} // namespace gles2

} // namespace gpu
