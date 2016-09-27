// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <map>
#include <set>

#include "gpu/command_buffer/service/gpu_service_test.h"
#include "gpu/command_buffer/service/gpu_tracer.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_mock.h"

namespace gpu {
namespace gles2 {

using ::testing::InvokeWithoutArgs;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::ReturnPointee;
using ::testing::NotNull;
using ::testing::ElementsAreArray;
using ::testing::ElementsAre;
using ::testing::SetArrayArgument;
using ::testing::AtLeast;
using ::testing::SetArgPointee;
using ::testing::Pointee;
using ::testing::Unused;
using ::testing::Invoke;
using ::testing::_;

class MockOutputter : public Outputter {
 public:
  MockOutputter() {}
  MOCK_METHOD3(Trace,
               void(const std::string& name, int64 start_time, int64 end_time));

 protected:
  ~MockOutputter() {}
};

class GlFakeQueries {
 public:
  GlFakeQueries() {}

  void Reset() {
    current_time_ = 0;
    next_query_id_ = 23;
    alloced_queries_.clear();
    query_timestamp_.clear();
  }

  void SetCurrentGLTime(GLint64 current_time) { current_time_ = current_time; }

  void GenQueries(GLsizei n, GLuint* ids) {
    for (GLsizei i = 0; i < n; i++) {
      ids[i] = next_query_id_++;
      alloced_queries_.insert(ids[i]);
    }
  }

  void DeleteQueries(GLsizei n, const GLuint* ids) {
    for (GLsizei i = 0; i < n; i++) {
      alloced_queries_.erase(ids[i]);
      query_timestamp_.erase(ids[i]);
    }
  }

  void GetQueryObjectiv(GLuint id, GLenum pname, GLint* params) {
    switch (pname) {
      case GL_QUERY_RESULT_AVAILABLE: {
        std::map<GLuint, GLint64>::iterator it = query_timestamp_.find(id);
        if (it != query_timestamp_.end() && it->second <= current_time_)
          *params = 1;
        else
          *params = 0;
        break;
      }
      default:
        ASSERT_TRUE(false);
    }
  }

  void QueryCounter(GLuint id, GLenum target) {
    switch (target) {
      case GL_TIMESTAMP:
        ASSERT_TRUE(alloced_queries_.find(id) != alloced_queries_.end());
        query_timestamp_[id] = current_time_;
        break;
      default:
        ASSERT_TRUE(false);
    }
  }

  void GetQueryObjectui64v(GLuint id, GLenum pname, GLuint64* params) {
    switch (pname) {
      case GL_QUERY_RESULT:
        ASSERT_TRUE(query_timestamp_.find(id) != query_timestamp_.end());
        *params = query_timestamp_.find(id)->second;
        break;
      default:
        ASSERT_TRUE(false);
    }
  }

 protected:
  GLint64 current_time_;
  GLuint next_query_id_;
  std::set<GLuint> alloced_queries_;
  std::map<GLuint, GLint64> query_timestamp_;
};

class GpuTracerTest : public GpuServiceTest {
 public:
  GpuTracerTest() {}

  ///////////////////////////////////////////////////////////////////////////

 protected:
  virtual void SetUp() {
    GpuServiceTest::SetUp();
    gl_fake_queries_.Reset();
  }

  virtual void TearDown() {
    gl_.reset();
    gl_fake_queries_.Reset();
    GpuServiceTest::TearDown();
  }

  void SetupTimerQueryMocks() {
    // Delegate query APIs used by GLARBTimerTrace to a GlFakeQueries
    EXPECT_CALL(*gl_, GenQueries(_, NotNull())).Times(AtLeast(1)).WillOnce(
        Invoke(&gl_fake_queries_, &GlFakeQueries::GenQueries));

    EXPECT_CALL(*gl_, GetQueryObjectiv(_, GL_QUERY_RESULT_AVAILABLE, NotNull()))
        .Times(AtLeast(2))
        .WillRepeatedly(
             Invoke(&gl_fake_queries_, &GlFakeQueries::GetQueryObjectiv));

    EXPECT_CALL(*gl_, QueryCounter(_, GL_TIMESTAMP))
        .Times(AtLeast(2))
        .WillRepeatedly(
             Invoke(&gl_fake_queries_, &GlFakeQueries::QueryCounter));

    EXPECT_CALL(*gl_, GetQueryObjectui64v(_, GL_QUERY_RESULT, NotNull()))
        .Times(AtLeast(2))
        .WillRepeatedly(
             Invoke(&gl_fake_queries_, &GlFakeQueries::GetQueryObjectui64v));

    EXPECT_CALL(*gl_, DeleteQueries(2, NotNull()))
        .Times(AtLeast(1))
        .WillRepeatedly(
             Invoke(&gl_fake_queries_, &GlFakeQueries::DeleteQueries));
  }

  GlFakeQueries gl_fake_queries_;
};

TEST_F(GpuTracerTest, GLARBTimerTrace) {
  // Test basic timer query functionality
  {
    MockOutputter* outputter = new MockOutputter();
    scoped_refptr<Outputter> outputter_ref = outputter;

    SetupTimerQueryMocks();

    // Expected results
    const std::string trace_name("trace_test");
    const int64 offset_time = 3231;
    const GLint64 start_timestamp = 7 * base::Time::kNanosecondsPerMicrosecond;
    const GLint64 end_timestamp = 32 * base::Time::kNanosecondsPerMicrosecond;
    const int64 expect_start_time =
        (start_timestamp / base::Time::kNanosecondsPerMicrosecond) +
        offset_time;
    const int64 expect_end_time =
        (end_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;

    // Expected Outputter::Trace call
    EXPECT_CALL(*outputter,
                Trace(trace_name, expect_start_time, expect_end_time));

    scoped_refptr<GLARBTimerTrace> trace =
        new GLARBTimerTrace(outputter_ref, trace_name, offset_time);

    gl_fake_queries_.SetCurrentGLTime(start_timestamp);
    trace->Start();

    // Shouldn't be available before End() call
    gl_fake_queries_.SetCurrentGLTime(end_timestamp);
    EXPECT_FALSE(trace->IsAvailable());

    trace->End();

    // Shouldn't be available until the queries complete
    gl_fake_queries_.SetCurrentGLTime(end_timestamp -
                                      base::Time::kNanosecondsPerMicrosecond);
    EXPECT_FALSE(trace->IsAvailable());

    // Now it should be available
    gl_fake_queries_.SetCurrentGLTime(end_timestamp);
    EXPECT_TRUE(trace->IsAvailable());

    // Proces should output expected Trace results to MockOutputter
    trace->Process();
  }
}

}  // namespace gles2
}  // namespace gpu
