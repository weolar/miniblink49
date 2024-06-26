// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "gpu/command_buffer/service/gles2_cmd_decoder_mock.h"
#include "gpu/command_buffer/service/gpu_service_test.h"
#include "gpu/command_buffer/service/gpu_tracer.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_context.h"
#include "ui/gl/gl_mock.h"
#include "ui/gl/gpu_timing.h"
#include "ui/gl/gpu_timing_fake.h"

namespace gpu {
namespace gles2 {
    namespace {

        using ::testing::_;
        using ::testing::AtMost;
        using ::testing::Exactly;
        using ::testing::Invoke;
        using ::testing::Return;

        int64 g_fakeCPUTime = 0;
        int64 FakeCpuTime()
        {
            return g_fakeCPUTime;
        }

        class MockOutputter : public Outputter {
        public:
            MockOutputter() { }
            MOCK_METHOD5(TraceDevice,
                void(GpuTracerSource source,
                    const std::string& category, const std::string& name,
                    int64 start_time, int64 end_time));

            MOCK_METHOD3(TraceServiceBegin,
                void(GpuTracerSource source,
                    const std::string& category, const std::string& name));

            MOCK_METHOD3(TraceServiceEnd,
                void(GpuTracerSource source,
                    const std::string& category, const std::string& name));

        protected:
            ~MockOutputter() { }
        };

        class GPUTracerTester : public GPUTracer {
        public:
            explicit GPUTracerTester(gles2::GLES2Decoder* decoder)
                : GPUTracer(decoder)
                , tracing_enabled_(0)
            {
                gpu_timing_client_->SetCpuTimeForTesting(base::Bind(&FakeCpuTime));

                // Force tracing to be dependent on our mock variable here.
                gpu_trace_srv_category = &tracing_enabled_;
                gpu_trace_dev_category = &tracing_enabled_;
            }

            ~GPUTracerTester() override { }

            void SetTracingEnabled(bool enabled)
            {
                tracing_enabled_ = enabled ? 1 : 0;
            }

            void SetOutputter(scoped_refptr<Outputter> outputter)
            {
                set_outputter_ = outputter;
            }

        protected:
            scoped_refptr<Outputter> CreateOutputter(const std::string& name) override
            {
                if (set_outputter_.get()) {
                    return set_outputter_;
                }
                return new MockOutputter();
            }

            unsigned char tracing_enabled_;
            scoped_refptr<Outputter> set_outputter_;
        };

        class BaseGpuTest : public GpuServiceTest {
        public:
            explicit BaseGpuTest(gfx::GPUTiming::TimerType test_timer_type)
                : test_timer_type_(test_timer_type)
            {
            }

        protected:
            void SetUp() override
            {
                g_fakeCPUTime = 0;
                const char* gl_version = "3.2";
                const char* extensions = "";
                if (GetTimerType() == gfx::GPUTiming::kTimerTypeEXT) {
                    gl_version = "opengl 2.1";
                    extensions = "GL_EXT_timer_query";
                } else if (GetTimerType() == gfx::GPUTiming::kTimerTypeDisjoint) {
                    gl_version = "opengl es 3.0";
                    extensions = "GL_EXT_disjoint_timer_query";
                } else if (GetTimerType() == gfx::GPUTiming::kTimerTypeARB) {
                    // TODO(sievers): The tracer should not depend on ARB_occlusion_query.
                    // Try merge Query APIs (core, ARB, EXT) into a single binding each.
                    extensions = "GL_ARB_timer_query GL_ARB_occlusion_query";
                }
                GpuServiceTest::SetUpWithGLVersion(gl_version, extensions);

                // Disjoint check should only be called by kTracerTypeDisjointTimer type.
                if (GetTimerType() == gfx::GPUTiming::kTimerTypeDisjoint)
                    gl_fake_queries_.ExpectDisjointCalls(*gl_);
                else
                    gl_fake_queries_.ExpectNoDisjointCalls(*gl_);

                gpu_timing_client_ = GetGLContext()->CreateGPUTimingClient();
                gpu_timing_client_->SetCpuTimeForTesting(base::Bind(&FakeCpuTime));
                gl_fake_queries_.Reset();

                outputter_ref_ = new MockOutputter();
            }

            void TearDown() override
            {
                outputter_ref_ = NULL;
                gpu_timing_client_ = NULL;

                gl_fake_queries_.Reset();
                GpuServiceTest::TearDown();
            }

            void ExpectTraceQueryMocks()
            {
                if (gpu_timing_client_->IsAvailable()) {
                    // Delegate query APIs used by GPUTrace to a GlFakeQueries
                    const bool elapsed = (GetTimerType() == gfx::GPUTiming::kTimerTypeEXT);
                    gl_fake_queries_.ExpectGPUTimerQuery(*gl_, elapsed);
                }
            }

            void ExpectOutputterBeginMocks(MockOutputter* outputter,
                GpuTracerSource source,
                const std::string& category,
                const std::string& name)
            {
                EXPECT_CALL(*outputter,
                    TraceServiceBegin(source, category, name));
            }

            void ExpectOutputterEndMocks(MockOutputter* outputter,
                GpuTracerSource source,
                const std::string& category,
                const std::string& name, int64 expect_start_time,
                int64 expect_end_time,
                bool trace_service,
                bool trace_device)
            {
                if (trace_service) {
                    EXPECT_CALL(*outputter,
                        TraceServiceEnd(source, category, name));
                }

                if (trace_device) {
                    EXPECT_CALL(*outputter,
                        TraceDevice(source, category, name,
                            expect_start_time, expect_end_time))
                        .Times(Exactly(1));
                } else {
                    EXPECT_CALL(*outputter, TraceDevice(source, category, name, expect_start_time, expect_end_time))
                        .Times(Exactly(0));
                }
            }

            void ExpectDisjointOutputMocks(MockOutputter* outputter,
                int64 expect_start_time,
                int64 expect_end_time)
            {
                EXPECT_CALL(*outputter,
                    TraceDevice(kTraceDisjoint, "DisjointEvent", _,
                        expect_start_time, expect_end_time))
                    .Times(Exactly(1));
            }

            void ExpectNoDisjointOutputMocks(MockOutputter* outputter)
            {
                EXPECT_CALL(*outputter,
                    TraceDevice(kTraceDisjoint, "DisjointEvent", _, _, _))
                    .Times(Exactly(0));
            }

            void ExpectOutputterMocks(MockOutputter* outputter,
                bool tracing_service,
                bool tracing_device,
                GpuTracerSource source,
                const std::string& category,
                const std::string& name, int64 expect_start_time,
                int64 expect_end_time)
            {
                if (tracing_service)
                    ExpectOutputterBeginMocks(outputter, source, category, name);
                const bool valid_timer = tracing_device && gpu_timing_client_->IsAvailable();
                ExpectOutputterEndMocks(outputter, source, category, name,
                    expect_start_time, expect_end_time,
                    tracing_service, valid_timer);
            }

            void ExpectTracerOffsetQueryMocks()
            {
                if (GetTimerType() != gfx::GPUTiming::kTimerTypeARB) {
                    gl_fake_queries_.ExpectNoOffsetCalculationQuery(*gl_);
                } else {
                    gl_fake_queries_.ExpectOffsetCalculationQuery(*gl_);
                }
            }

            gfx::GPUTiming::TimerType GetTimerType() { return test_timer_type_; }

            gfx::GPUTiming::TimerType test_timer_type_;
            gfx::GPUTimingFake gl_fake_queries_;

            scoped_refptr<gfx::GPUTimingClient> gpu_timing_client_;
            scoped_refptr<MockOutputter> outputter_ref_;
        };

        // Test GPUTrace calls all the correct gl calls.
        class BaseGpuTraceTest : public BaseGpuTest {
        public:
            explicit BaseGpuTraceTest(gfx::GPUTiming::TimerType test_timer_type)
                : BaseGpuTest(test_timer_type)
            {
            }

            void DoTraceTest(bool tracing_service, bool tracing_device)
            {
                // Expected results
                const GpuTracerSource tracer_source = kTraceCHROMIUM;
                const std::string category_name("trace_category");
                const std::string trace_name("trace_test");
                const int64 offset_time = 3231;
                const GLint64 start_timestamp = 7 * base::Time::kNanosecondsPerMicrosecond;
                const GLint64 end_timestamp = 32 * base::Time::kNanosecondsPerMicrosecond;
                const int64 expect_start_time = (start_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;
                const int64 expect_end_time = (end_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;

                ExpectOutputterMocks(outputter_ref_.get(), tracing_service, tracing_device,
                    tracer_source, category_name, trace_name,
                    expect_start_time, expect_end_time);

                if (tracing_device)
                    ExpectTraceQueryMocks();

                scoped_refptr<GPUTrace> trace = new GPUTrace(
                    outputter_ref_, gpu_timing_client_.get(), tracer_source,
                    category_name, trace_name, tracing_service, tracing_device);

                gl_fake_queries_.SetCurrentGLTime(start_timestamp);
                g_fakeCPUTime = expect_start_time;
                trace->Start();

                // Shouldn't be available before End() call
                gl_fake_queries_.SetCurrentGLTime(end_timestamp);
                g_fakeCPUTime = expect_end_time;
                if (tracing_device)
                    EXPECT_FALSE(trace->IsAvailable());

                trace->End();

                // Shouldn't be available until the queries complete
                gl_fake_queries_.SetCurrentGLTime(end_timestamp - base::Time::kNanosecondsPerMicrosecond);
                g_fakeCPUTime = expect_end_time - 1;
                if (tracing_device)
                    EXPECT_FALSE(trace->IsAvailable());

                // Now it should be available
                gl_fake_queries_.SetCurrentGLTime(end_timestamp);
                g_fakeCPUTime = expect_end_time;
                EXPECT_TRUE(trace->IsAvailable());

                // Proces should output expected Trace results to MockOutputter
                trace->Process();

                // Destroy trace after we are done.
                trace->Destroy(true);

                outputter_ref_ = NULL;
            }
        };

        class GpuARBTimerTraceTest : public BaseGpuTraceTest {
        public:
            GpuARBTimerTraceTest()
                : BaseGpuTraceTest(gfx::GPUTiming::kTimerTypeARB)
            {
            }
        };

        class GpuDisjointTimerTraceTest : public BaseGpuTraceTest {
        public:
            GpuDisjointTimerTraceTest()
                : BaseGpuTraceTest(gfx::GPUTiming::kTimerTypeDisjoint)
            {
            }
        };

        TEST_F(GpuARBTimerTraceTest, ARBTimerTraceTestOff)
        {
            DoTraceTest(false, false);
        }

        TEST_F(GpuARBTimerTraceTest, ARBTimerTraceTestServiceOnly)
        {
            DoTraceTest(true, false);
        }

        TEST_F(GpuARBTimerTraceTest, ARBTimerTraceTestDeviceOnly)
        {
            DoTraceTest(false, true);
        }

        TEST_F(GpuARBTimerTraceTest, ARBTimerTraceTestBothOn)
        {
            DoTraceTest(true, true);
        }

        TEST_F(GpuDisjointTimerTraceTest, DisjointTimerTraceTestOff)
        {
            DoTraceTest(false, false);
        }

        TEST_F(GpuDisjointTimerTraceTest, DisjointTimerTraceTestServiceOnly)
        {
            DoTraceTest(true, false);
        }

        TEST_F(GpuDisjointTimerTraceTest, DisjointTimerTraceTestDeviceOnly)
        {
            DoTraceTest(false, true);
        }

        TEST_F(GpuDisjointTimerTraceTest, DisjointTimerTraceTestBothOn)
        {
            DoTraceTest(true, true);
        }

        // Test GPUTracer calls all the correct gl calls.
        class BaseGpuTracerTest : public BaseGpuTest {
        public:
            explicit BaseGpuTracerTest(gfx::GPUTiming::TimerType test_timer_type)
                : BaseGpuTest(test_timer_type)
            {
            }

            void DoBasicTracerTest()
            {
                ExpectTracerOffsetQueryMocks();

                MockGLES2Decoder decoder;
                EXPECT_CALL(decoder, GetGLContext()).WillOnce(Return(GetGLContext()));
                GPUTracerTester tracer(&decoder);
                tracer.SetTracingEnabled(true);

                tracer.SetOutputter(outputter_ref_);

                ASSERT_TRUE(tracer.BeginDecoding());
                ASSERT_TRUE(tracer.EndDecoding());

                outputter_ref_ = NULL;
            }

            void DoDisabledTracingTest()
            {
                ExpectTracerOffsetQueryMocks();

                const GpuTracerSource source = static_cast<GpuTracerSource>(0);

                MockGLES2Decoder decoder;
                EXPECT_CALL(decoder, GetGLContext()).WillOnce(Return(GetGLContext()));
                GPUTracerTester tracer(&decoder);
                tracer.SetTracingEnabled(false);
                tracer.SetOutputter(outputter_ref_);

                ASSERT_TRUE(tracer.BeginDecoding());
                ASSERT_TRUE(tracer.Begin("disabled_category", "disabled_name", source));
                ASSERT_TRUE(tracer.End(source));
                ASSERT_TRUE(tracer.EndDecoding());
            }

            void DoTracerMarkersTest()
            {
                ExpectTracerOffsetQueryMocks();
                gl_fake_queries_.ExpectGetErrorCalls(*gl_);

                const std::string category_name("trace_category");
                const std::string trace_name("trace_test");
                const int64 offset_time = 3231;
                const GLint64 start_timestamp = 7 * base::Time::kNanosecondsPerMicrosecond;
                const GLint64 end_timestamp = 32 * base::Time::kNanosecondsPerMicrosecond;
                const int64 expect_start_time = (start_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;
                const int64 expect_end_time = (end_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;

                MockGLES2Decoder decoder;
                EXPECT_CALL(decoder, GetGLContext()).WillOnce(Return(GetGLContext()));
                GPUTracerTester tracer(&decoder);
                tracer.SetTracingEnabled(true);

                tracer.SetOutputter(outputter_ref_);

                gl_fake_queries_.SetCurrentGLTime(start_timestamp);
                g_fakeCPUTime = expect_start_time;

                ASSERT_TRUE(tracer.BeginDecoding());

                ExpectTraceQueryMocks();

                // This will test multiple marker sources which overlap one another.
                for (int i = 0; i < NUM_TRACER_SOURCES; ++i) {
                    // Set times so each source has a different time.
                    gl_fake_queries_.SetCurrentGLTime(
                        start_timestamp + (i * base::Time::kNanosecondsPerMicrosecond));
                    g_fakeCPUTime = expect_start_time + i;

                    // Each trace name should be different to differentiate.
                    const char num_char = static_cast<char>('0' + i);
                    std::string source_category = category_name + num_char;
                    std::string source_trace_name = trace_name + num_char;

                    const GpuTracerSource source = static_cast<GpuTracerSource>(i);
                    ExpectOutputterBeginMocks(outputter_ref_.get(), source,
                        source_category, source_trace_name);
                    ASSERT_TRUE(tracer.Begin(source_category, source_trace_name, source));
                }
                for (int i = 0; i < NUM_TRACER_SOURCES; ++i) {
                    // Set times so each source has a different time.
                    gl_fake_queries_.SetCurrentGLTime(
                        end_timestamp + (i * base::Time::kNanosecondsPerMicrosecond));
                    g_fakeCPUTime = expect_end_time + i;

                    // Each trace name should be different to differentiate.
                    const char num_char = static_cast<char>('0' + i);
                    std::string source_category = category_name + num_char;
                    std::string source_trace_name = trace_name + num_char;

                    const bool valid_timer = gpu_timing_client_->IsAvailable();
                    const GpuTracerSource source = static_cast<GpuTracerSource>(i);
                    ExpectOutputterEndMocks(outputter_ref_.get(), source, source_category,
                        source_trace_name, expect_start_time + i,
                        expect_end_time + i, true, valid_timer);
                    // Check if the current category/name are correct for this source.
                    ASSERT_EQ(source_category, tracer.CurrentCategory(source));
                    ASSERT_EQ(source_trace_name, tracer.CurrentName(source));

                    ASSERT_TRUE(tracer.End(source));
                }
                ASSERT_TRUE(tracer.EndDecoding());
                tracer.ProcessTraces();
                outputter_ref_ = NULL;
            }

            void DoOngoingTracerMarkerTest()
            {
                ExpectTracerOffsetQueryMocks();
                gl_fake_queries_.ExpectGetErrorCalls(*gl_);

                const std::string category_name("trace_category");
                const std::string trace_name("trace_test");
                const GpuTracerSource source = static_cast<GpuTracerSource>(0);
                const int64 offset_time = 3231;
                const GLint64 start_timestamp = 7 * base::Time::kNanosecondsPerMicrosecond;
                const int64 expect_start_time = (start_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;
                const bool valid_timer = gpu_timing_client_->IsAvailable();

                MockGLES2Decoder decoder;
                EXPECT_CALL(decoder, GetGLContext()).WillOnce(Return(GetGLContext()));
                GPUTracerTester tracer(&decoder);
                tracer.SetOutputter(outputter_ref_);

                // Create trace marker while traces are disabled.
                gl_fake_queries_.SetCurrentGLTime(start_timestamp);
                g_fakeCPUTime = expect_start_time;

                tracer.SetTracingEnabled(false);
                ASSERT_TRUE(tracer.BeginDecoding());
                ASSERT_TRUE(tracer.Begin(category_name, trace_name, source));
                ASSERT_TRUE(tracer.EndDecoding());

                // Enable traces now.
                tracer.SetTracingEnabled(true);
                ExpectTraceQueryMocks();

                // trace should happen when decoding begins, at time start+1.
                gl_fake_queries_.SetCurrentGLTime(
                    start_timestamp + (1 * base::Time::kNanosecondsPerMicrosecond));
                g_fakeCPUTime = expect_start_time + 1;
                ASSERT_TRUE(tracer.BeginDecoding());

                // End decoding at time start+2.
                ExpectOutputterEndMocks(outputter_ref_.get(), source, category_name,
                    trace_name, expect_start_time + 1,
                    expect_start_time + 2, true, valid_timer);
                gl_fake_queries_.SetCurrentGLTime(
                    start_timestamp + (2 * base::Time::kNanosecondsPerMicrosecond));
                g_fakeCPUTime = expect_start_time + 2;
                ASSERT_TRUE(tracer.EndDecoding());

                // Begin decoding again at time start+3.
                gl_fake_queries_.SetCurrentGLTime(
                    start_timestamp + (3 * base::Time::kNanosecondsPerMicrosecond));
                g_fakeCPUTime = expect_start_time + 3;
                ASSERT_TRUE(tracer.BeginDecoding());

                // End trace at time start+4
                gl_fake_queries_.SetCurrentGLTime(
                    start_timestamp + (4 * base::Time::kNanosecondsPerMicrosecond));
                g_fakeCPUTime = expect_start_time + 4;
                ExpectOutputterEndMocks(outputter_ref_.get(), source, category_name,
                    trace_name, expect_start_time + 3,
                    expect_start_time + 4, true, valid_timer);
                ASSERT_TRUE(tracer.End(source));

                // Increment time before we end decoding to test trace does not stop here.
                gl_fake_queries_.SetCurrentGLTime(
                    start_timestamp + (5 * base::Time::kNanosecondsPerMicrosecond));
                g_fakeCPUTime = expect_start_time + 5;
                ASSERT_TRUE(tracer.EndDecoding());
                tracer.ProcessTraces();
            }

            void DoDisjointTest()
            {
                // Cause a disjoint in a middle of a trace and expect no output calls.
                ExpectTracerOffsetQueryMocks();
                gl_fake_queries_.ExpectGetErrorCalls(*gl_);

                const std::string category_name("trace_category");
                const std::string trace_name("trace_test");
                const GpuTracerSource source = static_cast<GpuTracerSource>(0);
                const int64 offset_time = 3231;
                const GLint64 start_timestamp = 7 * base::Time::kNanosecondsPerMicrosecond;
                const GLint64 end_timestamp = 32 * base::Time::kNanosecondsPerMicrosecond;
                const int64 expect_start_time = (start_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;
                const int64 expect_end_time = (end_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;

                MockGLES2Decoder decoder;
                EXPECT_CALL(decoder, GetGLContext()).WillOnce(Return(GetGLContext()));
                GPUTracerTester tracer(&decoder);
                tracer.SetTracingEnabled(true);

                tracer.SetOutputter(outputter_ref_);

                gl_fake_queries_.SetCurrentGLTime(start_timestamp);
                g_fakeCPUTime = expect_start_time;

                ASSERT_TRUE(tracer.BeginDecoding());

                ExpectTraceQueryMocks();

                ExpectOutputterBeginMocks(outputter_ref_.get(), source,
                    category_name, trace_name);
                ASSERT_TRUE(tracer.Begin(category_name, trace_name, source));

                gl_fake_queries_.SetCurrentGLTime(end_timestamp);
                g_fakeCPUTime = expect_end_time;

                // Create GPUTimingClient to make sure disjoint value is correct. This
                // should not interfere with the tracer's disjoint value.
                scoped_refptr<gfx::GPUTimingClient> disjoint_client = GetGLContext()->CreateGPUTimingClient();

                // We assert here based on the disjoint_client because if disjoints are not
                // working properly there is no point testing the tracer output.
                ASSERT_FALSE(disjoint_client->CheckAndResetTimerErrors());
                gl_fake_queries_.SetDisjoint();
                ASSERT_TRUE(disjoint_client->CheckAndResetTimerErrors());

                ExpectDisjointOutputMocks(outputter_ref_.get(),
                    expect_start_time, expect_end_time);

                ExpectOutputterEndMocks(outputter_ref_.get(), source,
                    category_name, trace_name,
                    expect_start_time, expect_end_time, true, false);

                ASSERT_TRUE(tracer.End(source));
                ASSERT_TRUE(tracer.EndDecoding());
                tracer.ProcessTraces();

                outputter_ref_ = NULL;
            }

            void DoOutsideDisjointTest()
            {
                ExpectTracerOffsetQueryMocks();
                gl_fake_queries_.ExpectGetErrorCalls(*gl_);

                const std::string category_name("trace_category");
                const std::string trace_name("trace_test");
                const GpuTracerSource source = static_cast<GpuTracerSource>(0);
                const int64 offset_time = 3231;
                const GLint64 start_timestamp = 7 * base::Time::kNanosecondsPerMicrosecond;
                const GLint64 end_timestamp = 32 * base::Time::kNanosecondsPerMicrosecond;
                const int64 expect_start_time = (start_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;
                const int64 expect_end_time = (end_timestamp / base::Time::kNanosecondsPerMicrosecond) + offset_time;

                MockGLES2Decoder decoder;
                EXPECT_CALL(decoder, GetGLContext()).WillOnce(Return(GetGLContext()));
                EXPECT_CALL(decoder, MakeCurrent()).WillRepeatedly(Return(true));
                GPUTracerTester tracer(&decoder);
                tracer.SetOutputter(outputter_ref_);

                // Start a trace before tracing is enabled.
                tracer.SetTracingEnabled(false);
                ASSERT_TRUE(tracer.BeginDecoding());
                ASSERT_TRUE(tracer.Begin(category_name, trace_name, source));
                ASSERT_TRUE(tracer.EndDecoding());

                // Enabling traces now, trace should be ongoing.
                tracer.SetTracingEnabled(true);
                gl_fake_queries_.SetCurrentGLTime(start_timestamp);
                g_fakeCPUTime = expect_start_time;

                // Disjoints before we start tracing anything should not do anything.
                ExpectNoDisjointOutputMocks(outputter_ref_.get());
                gl_fake_queries_.SetDisjoint();

                ExpectTraceQueryMocks();
                ExpectOutputterBeginMocks(outputter_ref_.get(), source,
                    category_name, trace_name);
                ASSERT_TRUE(tracer.BeginDecoding());

                // Set times so each source has a different time.
                gl_fake_queries_.SetCurrentGLTime(end_timestamp);
                g_fakeCPUTime = expect_end_time;

                ExpectOutputterEndMocks(outputter_ref_.get(), source, category_name,
                    trace_name, expect_start_time,
                    expect_end_time, true, true);

                ASSERT_TRUE(tracer.End(source));
                ASSERT_TRUE(tracer.EndDecoding());
                tracer.ProcessTraces();
            }
        };

        class InvalidTimerTracerTest : public BaseGpuTracerTest {
        public:
            InvalidTimerTracerTest()
                : BaseGpuTracerTest(gfx::GPUTiming::kTimerTypeInvalid)
            {
            }
        };

        class GpuEXTTimerTracerTest : public BaseGpuTracerTest {
        public:
            GpuEXTTimerTracerTest()
                : BaseGpuTracerTest(gfx::GPUTiming::kTimerTypeEXT)
            {
            }
        };

        class GpuARBTimerTracerTest : public BaseGpuTracerTest {
        public:
            GpuARBTimerTracerTest()
                : BaseGpuTracerTest(gfx::GPUTiming::kTimerTypeARB)
            {
            }
        };

        class GpuDisjointTimerTracerTest : public BaseGpuTracerTest {
        public:
            GpuDisjointTimerTracerTest()
                : BaseGpuTracerTest(gfx::GPUTiming::kTimerTypeDisjoint)
            {
            }
        };

        TEST_F(InvalidTimerTracerTest, InvalidTimerBasicTracerTest)
        {
            DoBasicTracerTest();
        }

        TEST_F(GpuEXTTimerTracerTest, EXTTimerBasicTracerTest)
        {
            DoBasicTracerTest();
        }

        TEST_F(GpuARBTimerTracerTest, ARBTimerBasicTracerTest)
        {
            DoBasicTracerTest();
        }

        TEST_F(GpuDisjointTimerTracerTest, DisjointTimerBasicTracerTest)
        {
            DoBasicTracerTest();
        }

        TEST_F(InvalidTimerTracerTest, InvalidTimerDisabledTest)
        {
            DoDisabledTracingTest();
        }

        TEST_F(GpuEXTTimerTracerTest, EXTTimerDisabledTest)
        {
            DoDisabledTracingTest();
        }

        TEST_F(GpuARBTimerTracerTest, ARBTimerDisabledTest)
        {
            DoDisabledTracingTest();
        }

        TEST_F(GpuDisjointTimerTracerTest, DisjointTimerDisabledTest)
        {
            DoDisabledTracingTest();
        }

        TEST_F(InvalidTimerTracerTest, InvalidTimerTracerMarkersTest)
        {
            DoTracerMarkersTest();
        }

        TEST_F(GpuEXTTimerTracerTest, EXTTimerTracerMarkersTest)
        {
            DoTracerMarkersTest();
        }

        TEST_F(GpuARBTimerTracerTest, ARBTimerTracerMarkersTest)
        {
            DoTracerMarkersTest();
        }

        TEST_F(GpuDisjointTimerTracerTest, DisjointTimerBasicTracerMarkersTest)
        {
            DoTracerMarkersTest();
        }

        TEST_F(InvalidTimerTracerTest, InvalidTimerOngoingTracerMarkersTest)
        {
            DoOngoingTracerMarkerTest();
        }

        TEST_F(GpuEXTTimerTracerTest, EXTTimerOngoingTracerMarkersTest)
        {
            DoOngoingTracerMarkerTest();
        }

        TEST_F(GpuARBTimerTracerTest, ARBTimerBasicOngoingTracerMarkersTest)
        {
            DoOngoingTracerMarkerTest();
        }

        TEST_F(GpuDisjointTimerTracerTest, DisjointTimerOngoingTracerMarkersTest)
        {
            DoOngoingTracerMarkerTest();
        }

        TEST_F(GpuDisjointTimerTracerTest, DisjointTimerDisjointTraceTest)
        {
            DoDisjointTest();
        }

        TEST_F(GpuDisjointTimerTracerTest, NonrelevantDisjointTraceTest)
        {
            DoOutsideDisjointTest();
        }

        class GPUTracerTest : public GpuServiceTest {
        protected:
            void SetUp() override
            {
                g_fakeCPUTime = 0;
                GpuServiceTest::SetUpWithGLVersion("3.2", "");
                decoder_.reset(new MockGLES2Decoder());
                EXPECT_CALL(*decoder_, GetGLContext())
                    .Times(AtMost(1))
                    .WillRepeatedly(Return(GetGLContext()));
                tracer_tester_.reset(new GPUTracerTester(decoder_.get()));
            }

            void TearDown() override
            {
                tracer_tester_ = nullptr;
                decoder_ = nullptr;
                GpuServiceTest::TearDown();
            }
            scoped_ptr<MockGLES2Decoder> decoder_;
            scoped_ptr<GPUTracerTester> tracer_tester_;
        };

        TEST_F(GPUTracerTest, IsTracingTest)
        {
            EXPECT_FALSE(tracer_tester_->IsTracing());
            tracer_tester_->SetTracingEnabled(true);
            EXPECT_TRUE(tracer_tester_->IsTracing());
        }
        // Test basic functionality of the GPUTracerTester.
        TEST_F(GPUTracerTest, DecodeTest)
        {
            ASSERT_TRUE(tracer_tester_->BeginDecoding());
            EXPECT_FALSE(tracer_tester_->BeginDecoding());
            ASSERT_TRUE(tracer_tester_->EndDecoding());
            EXPECT_FALSE(tracer_tester_->EndDecoding());
        }

        TEST_F(GPUTracerTest, TraceDuringDecodeTest)
        {
            const std::string category_name("trace_category");
            const std::string trace_name("trace_test");

            EXPECT_FALSE(
                tracer_tester_->Begin(category_name, trace_name, kTraceCHROMIUM));

            ASSERT_TRUE(tracer_tester_->BeginDecoding());
            EXPECT_TRUE(
                tracer_tester_->Begin(category_name, trace_name, kTraceCHROMIUM));
            ASSERT_TRUE(tracer_tester_->EndDecoding());
        }

        TEST_F(GpuDisjointTimerTracerTest, MultipleClientsDisjointTest)
        {
            scoped_refptr<gfx::GPUTimingClient> client1 = GetGLContext()->CreateGPUTimingClient();
            scoped_refptr<gfx::GPUTimingClient> client2 = GetGLContext()->CreateGPUTimingClient();

            // Test both clients are initialized as no errors.
            ASSERT_FALSE(client1->CheckAndResetTimerErrors());
            ASSERT_FALSE(client2->CheckAndResetTimerErrors());

            // Issue a disjoint.
            gl_fake_queries_.SetDisjoint();

            ASSERT_TRUE(client1->CheckAndResetTimerErrors());
            ASSERT_TRUE(client2->CheckAndResetTimerErrors());

            // Test both are now reset.
            ASSERT_FALSE(client1->CheckAndResetTimerErrors());
            ASSERT_FALSE(client2->CheckAndResetTimerErrors());

            // Issue a disjoint.
            gl_fake_queries_.SetDisjoint();

            // Test new client disjoint value is cleared.
            scoped_refptr<gfx::GPUTimingClient> client3 = GetGLContext()->CreateGPUTimingClient();
            ASSERT_TRUE(client1->CheckAndResetTimerErrors());
            ASSERT_TRUE(client2->CheckAndResetTimerErrors());
            ASSERT_FALSE(client3->CheckAndResetTimerErrors());
        }

    } // namespace
} // namespace gles2
} // namespace gpu
