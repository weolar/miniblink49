// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/memory/scoped_ptr.h"
#include "base/strings/string_split.h"
#include "gpu/config/gpu_info.h"
#include "gpu/config/gpu_info_collector.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_implementation.h"
#include "ui/gl/gl_mock.h"
#include "ui/gl/test/gl_surface_test_support.h"

using ::gfx::MockGLInterface;
using ::testing::_;
using ::testing::Return;
using ::testing::SetArgPointee;

namespace {

// Allows testing of all configurations on all operating systems.
enum MockedOperatingSystemKind {
    kMockedAndroid,
    kMockedLinux,
    kMockedMacOSX,
    kMockedWindows
};

} // anonymous namespace

namespace gpu {

static const MockedOperatingSystemKind kMockedOperatingSystemKinds[] = {
    kMockedAndroid,
    kMockedLinux,
    kMockedMacOSX,
    kMockedWindows
};

class GPUInfoCollectorTest
    : public testing::Test,
      public ::testing::WithParamInterface<MockedOperatingSystemKind> {
public:
    GPUInfoCollectorTest() { }
    ~GPUInfoCollectorTest() override { }

    void SetUp() override
    {
        testing::Test::SetUp();
        gfx::SetGLGetProcAddressProc(gfx::MockGLInterface::GetGLProcAddress);
        gfx::GLSurfaceTestSupport::InitializeOneOffWithMockBindings();
        gl_.reset(new ::testing::StrictMock<::gfx::MockGLInterface>());
        ::gfx::MockGLInterface::SetGLInterface(gl_.get());
        switch (GetParam()) {
        case kMockedAndroid: {
            test_values_.gpu.vendor_id = 0; // not implemented
            test_values_.gpu.device_id = 0; // not implemented
            test_values_.driver_vendor = ""; // not implemented
            test_values_.driver_version = "14.0";
            test_values_.pixel_shader_version = "1.00";
            test_values_.vertex_shader_version = "1.00";
            test_values_.gl_renderer = "Adreno (TM) 320";
            test_values_.gl_vendor = "Qualcomm";
            test_values_.gl_version = "OpenGL ES 2.0 V@14.0 AU@04.02 (CL@3206)";
            test_values_.gl_extensions = "GL_OES_packed_depth_stencil GL_EXT_texture_format_BGRA8888 "
                                         "GL_EXT_read_format_bgra";
            gl_shading_language_version_ = "1.00";
            break;
        }
        case kMockedLinux: {
            test_values_.gpu.vendor_id = 0x10de;
            test_values_.gpu.device_id = 0x0658;
            test_values_.driver_vendor = "NVIDIA";
            test_values_.driver_version = "195.36.24";
            test_values_.pixel_shader_version = "1.50";
            test_values_.vertex_shader_version = "1.50";
            test_values_.gl_renderer = "Quadro FX 380/PCI/SSE2";
            test_values_.gl_vendor = "NVIDIA Corporation";
            test_values_.gl_version = "3.2.0 NVIDIA 195.36.24";
            test_values_.gl_extensions = "GL_OES_packed_depth_stencil GL_EXT_texture_format_BGRA8888 "
                                         "GL_EXT_read_format_bgra";
            gl_shading_language_version_ = "1.50 NVIDIA via Cg compiler";
            break;
        }
        case kMockedMacOSX: {
            test_values_.gpu.vendor_id = 0x10de;
            test_values_.gpu.device_id = 0x0640;
            test_values_.driver_vendor = ""; // not implemented
            test_values_.driver_version = "1.6.18";
            test_values_.pixel_shader_version = "1.20";
            test_values_.vertex_shader_version = "1.20";
            test_values_.gl_renderer = "NVIDIA GeForce GT 120 OpenGL Engine";
            test_values_.gl_vendor = "NVIDIA Corporation";
            test_values_.gl_version = "2.1 NVIDIA-1.6.18";
            test_values_.gl_extensions = "GL_OES_packed_depth_stencil GL_EXT_texture_format_BGRA8888 "
                                         "GL_EXT_read_format_bgra";
            gl_shading_language_version_ = "1.20 ";
            break;
        }
        case kMockedWindows: {
            test_values_.gpu.vendor_id = 0x10de;
            test_values_.gpu.device_id = 0x0658;
            test_values_.driver_vendor = ""; // not implemented
            test_values_.driver_version = "";
            test_values_.pixel_shader_version = "1.40";
            test_values_.vertex_shader_version = "1.40";
            test_values_.gl_renderer = "Quadro FX 380/PCI/SSE2";
            test_values_.gl_vendor = "NVIDIA Corporation";
            test_values_.gl_version = "3.1.0";
            test_values_.gl_extensions = "GL_OES_packed_depth_stencil GL_EXT_texture_format_BGRA8888 "
                                         "GL_EXT_read_format_bgra";
            gl_shading_language_version_ = "1.40 NVIDIA via Cg compiler";
            break;
        }
        default: {
            NOTREACHED();
            break;
        }
        }

        test_values_.can_lose_context = false;

        EXPECT_CALL(*gl_, GetString(GL_VERSION))
            .WillRepeatedly(Return(reinterpret_cast<const GLubyte*>(
                test_values_.gl_version.c_str())));

        // Now that that expectation is set up, we can call this helper function.
        if (gfx::WillUseGLGetStringForExtensions()) {
            EXPECT_CALL(*gl_, GetString(GL_EXTENSIONS))
                .WillRepeatedly(Return(reinterpret_cast<const GLubyte*>(
                    test_values_.gl_extensions.c_str())));
        } else {
            split_extensions_.clear();
            split_extensions_ = base::SplitString(
                test_values_.gl_extensions, " ",
                base::KEEP_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
            EXPECT_CALL(*gl_, GetIntegerv(GL_NUM_EXTENSIONS, _))
                .WillRepeatedly(SetArgPointee<1>(split_extensions_.size()));
            for (size_t ii = 0; ii < split_extensions_.size(); ++ii) {
                EXPECT_CALL(*gl_, GetStringi(GL_EXTENSIONS, ii))
                    .WillRepeatedly(Return(reinterpret_cast<const uint8*>(
                        split_extensions_[ii].c_str())));
            }
        }
        EXPECT_CALL(*gl_, GetString(GL_SHADING_LANGUAGE_VERSION))
            .WillRepeatedly(Return(reinterpret_cast<const GLubyte*>(
                gl_shading_language_version_)));
        EXPECT_CALL(*gl_, GetString(GL_VENDOR))
            .WillRepeatedly(Return(reinterpret_cast<const GLubyte*>(
                test_values_.gl_vendor.c_str())));
        EXPECT_CALL(*gl_, GetString(GL_RENDERER))
            .WillRepeatedly(Return(reinterpret_cast<const GLubyte*>(
                test_values_.gl_renderer.c_str())));
        EXPECT_CALL(*gl_, GetIntegerv(GL_MAX_SAMPLES, _))
            .WillOnce(SetArgPointee<1>(8))
            .RetiresOnSaturation();
    }

    void TearDown() override
    {
        ::gfx::MockGLInterface::SetGLInterface(NULL);
        gl_.reset();
        gfx::ClearGLBindings();

        testing::Test::TearDown();
    }

public:
    // Use StrictMock to make 100% sure we know how GL will be called.
    scoped_ptr<::testing::StrictMock<::gfx::MockGLInterface>> gl_;
    GPUInfo test_values_;
    const char* gl_shading_language_version_ = nullptr;

    // Persistent storage is needed for the split extension string.
    std::vector<std::string> split_extensions_;
};

INSTANTIATE_TEST_CASE_P(GPUConfig,
    GPUInfoCollectorTest,
    ::testing::ValuesIn(kMockedOperatingSystemKinds));

// TODO(rlp): Test the vendor and device id collection if deemed necessary as
//            it involves several complicated mocks for each platform.

// TODO(kbr): This test still has platform-dependent behavior because
// CollectDriverInfoGL behaves differently per platform. This should
// be fixed.
TEST_P(GPUInfoCollectorTest, CollectGraphicsInfoGL)
{
    GPUInfo gpu_info;
    CollectGraphicsInfoGL(&gpu_info);
#if defined(OS_WIN)
    if (GetParam() == kMockedWindows) {
        EXPECT_EQ(test_values_.driver_vendor,
            gpu_info.driver_vendor);
        // Skip testing the driver version on Windows because it's
        // obtained from the bot's registry.
    }
#elif defined(OS_MACOSX)
    if (GetParam() == kMockedMacOSX) {
        EXPECT_EQ(test_values_.driver_vendor,
            gpu_info.driver_vendor);
        EXPECT_EQ(test_values_.driver_version,
            gpu_info.driver_version);
    }
#elif defined(OS_ANDROID)
    if (GetParam() == kMockedAndroid) {
        EXPECT_EQ(test_values_.driver_vendor,
            gpu_info.driver_vendor);
        EXPECT_EQ(test_values_.driver_version,
            gpu_info.driver_version);
    }
#else // defined (OS_LINUX)
    if (GetParam() == kMockedLinux) {
        EXPECT_EQ(test_values_.driver_vendor,
            gpu_info.driver_vendor);
        EXPECT_EQ(test_values_.driver_version,
            gpu_info.driver_version);
    }
#endif

    EXPECT_EQ(test_values_.pixel_shader_version,
        gpu_info.pixel_shader_version);
    EXPECT_EQ(test_values_.vertex_shader_version,
        gpu_info.vertex_shader_version);
    EXPECT_EQ(test_values_.gl_version, gpu_info.gl_version);
    EXPECT_EQ(test_values_.gl_renderer, gpu_info.gl_renderer);
    EXPECT_EQ(test_values_.gl_vendor, gpu_info.gl_vendor);
    EXPECT_EQ(test_values_.gl_extensions, gpu_info.gl_extensions);
}

class CollectDriverInfoGLTest : public testing::Test {
public:
    CollectDriverInfoGLTest() { }
    ~CollectDriverInfoGLTest() override { }

    void SetUp() override { }
    void TearDown() override { }
};

TEST_F(CollectDriverInfoGLTest, CollectDriverInfoGL)
{
    const struct {
        const char* gl_renderer;
        const char* gl_vendor;
        const char* gl_version;
        const char* expected_driver_version;
    } kTestStrings[] = {
#if defined(OS_ANDROID)
        { "Adreno (TM) 320",
            "Qualcomm",
            "OpenGL ES 2.0 V@14.0 AU@04.02 (CL@3206)",
            "14.0" },
        { "Adreno (TM) 420", "Qualcomm", "OpenGL ES 3.0 V@84.0 AU@ (CL@)", "84.0" },
        { "PowerVR Rogue G6430",
            "Imagination Technologies",
            "OpenGL ES 3.1 build 1.4@3283119",
            "1.4" },
        { "Mali-T604", "ARM", "OpenGL ES 3.1", "0" },
        { "NVIDIA Tegra",
            "NVIDIA Corporation",
            "OpenGL ES 3.1 NVIDIA 343.00",
            "343.00" },
        { "NVIDIA Tegra 3",
            "NVIDIA Corporation",
            "OpenGL ES 2.0 14.01003",
            "14.01003" },
        { "random GPU",
            "random vendor",
            "OpenGL ES 2.0 with_long_version_string=1.2.3.4",
            "1.2" },
        { "random GPU",
            "random vendor",
            "OpenGL ES 2.0 with_short_version_string=1",
            "0" },
        { "random GPU",
            "random vendor",
            "OpenGL ES 2.0 with_no_version_string",
            "0" },
#elif defined(OS_MACOSX)
        { "Intel Iris Pro OpenGL Engine",
            "Intel Inc.",
            "2.1 INTEL-10.6.20",
            "10.6.20" },
#elif defined(OS_LINUX)
        { "Quadro K2000/PCIe/SSE2",
            "NVIDIA Corporation",
            "4.4.0 NVIDIA 331.79",
            "331.79" },
#endif
        { NULL, NULL, NULL, NULL }
    };

    GPUInfo gpu_info;
    for (int i = 0; kTestStrings[i].gl_renderer != NULL; ++i) {
        gpu_info.gl_renderer = kTestStrings[i].gl_renderer;
        gpu_info.gl_vendor = kTestStrings[i].gl_vendor;
        gpu_info.gl_version = kTestStrings[i].gl_version;
        EXPECT_EQ(CollectDriverInfoGL(&gpu_info), kCollectInfoSuccess);
        EXPECT_EQ(gpu_info.driver_version, kTestStrings[i].expected_driver_version);
    }
}

} // namespace gpu
