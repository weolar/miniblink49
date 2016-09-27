// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <GLES2/gl2.h>
#include <GLES2/gl2chromium.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2extchromium.h>

#include "base/bind.h"
#include "base/memory/ref_counted.h"
#include "base/process/process_handle.h"
#include "gpu/command_buffer/client/gles2_implementation.h"
#include "gpu/command_buffer/client/gpu_memory_buffer_factory.h"
#include "gpu/command_buffer/service/command_buffer_service.h"
#include "gpu/command_buffer/service/image_manager.h"
#include "gpu/command_buffer/tests/gl_manager.h"
#include "gpu/command_buffer/tests/gl_test_utils.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/gl/gl_image.h"

using testing::_;
using testing::IgnoreResult;
using testing::InvokeWithoutArgs;
using testing::Invoke;
using testing::Return;
using testing::SetArgPointee;
using testing::StrictMock;

namespace gpu {
namespace gles2 {

static const int kImageWidth = 32;
static const int kImageHeight = 32;
static const int kImageBytesPerPixel = 4;

class MockGpuMemoryBuffer : public gfx::GpuMemoryBuffer {
 public:
  MockGpuMemoryBuffer(int width, int height) {}
  virtual ~MockGpuMemoryBuffer() {
    Die();
  }

  MOCK_METHOD0(Map, void*());
  MOCK_METHOD0(Unmap, void());
  MOCK_CONST_METHOD0(IsMapped, bool());
  MOCK_CONST_METHOD0(GetStride, uint32());
  MOCK_CONST_METHOD0(GetHandle, gfx::GpuMemoryBufferHandle());
  MOCK_METHOD0(Die, void());

 private:
  DISALLOW_COPY_AND_ASSIGN(MockGpuMemoryBuffer);
};

class MockGpuMemoryBufferFactory : public GpuMemoryBufferFactory {
 public:
  MockGpuMemoryBufferFactory() {}
  virtual ~MockGpuMemoryBufferFactory() {}

  MOCK_METHOD4(CreateGpuMemoryBuffer,
               gfx::GpuMemoryBuffer*(size_t, size_t, unsigned, unsigned));

 private:
  DISALLOW_COPY_AND_ASSIGN(MockGpuMemoryBufferFactory);
};

class MockGpuMemoryBufferTest : public testing::Test {
 protected:
  virtual void SetUp() {
    GLManager::Options options;
    image_manager_ = new ImageManager;
    gpu_memory_buffer_factory_.reset(new MockGpuMemoryBufferFactory);
    options.image_manager = image_manager_.get();
    options.gpu_memory_buffer_factory = gpu_memory_buffer_factory_.get();

    gl_.Initialize(options);
    gl_.MakeCurrent();

    glGenTextures(2, texture_ids_);
    glBindTexture(GL_TEXTURE_2D, texture_ids_[1]);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &framebuffer_id_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           texture_ids_[1],
                           0);
  }

  virtual void TearDown() {
    glDeleteTextures(2, texture_ids_);
    glDeleteFramebuffers(1, &framebuffer_id_);

    gl_.Destroy();
  }

  scoped_refptr<ImageManager> image_manager_;
  scoped_ptr<MockGpuMemoryBufferFactory> gpu_memory_buffer_factory_;
  GLManager gl_;
  GLuint texture_ids_[2];
  GLuint framebuffer_id_;
};

// An end to end test that tests the whole GpuMemoryBuffer lifecycle.
TEST_F(MockGpuMemoryBufferTest, Lifecycle) {
  size_t bytes = kImageWidth * kImageHeight * kImageBytesPerPixel;
  uint8 pixels[1 * 4] = { 255u, 0u, 0u, 255u };

  // Buffer is owned and freed by GpuMemoryBufferTracker.
  StrictMock<MockGpuMemoryBuffer>* gpu_memory_buffer =
      new StrictMock<MockGpuMemoryBuffer>(kImageWidth, kImageHeight);
  base::SharedMemory shared_memory;
  shared_memory.CreateAnonymous(bytes);

  base::SharedMemoryHandle duped_shared_memory_handle;
  shared_memory.ShareToProcess(base::GetCurrentProcessHandle(),
                               &duped_shared_memory_handle);
  gfx::GpuMemoryBufferHandle handle;
  handle.type = gfx::SHARED_MEMORY_BUFFER;
  handle.handle = duped_shared_memory_handle;

  EXPECT_CALL(
      *gpu_memory_buffer_factory_.get(),
      CreateGpuMemoryBuffer(
          kImageWidth, kImageHeight, GL_RGBA8_OES, GL_IMAGE_MAP_CHROMIUM))
      .Times(1)
      .WillOnce(Return(gpu_memory_buffer))
      .RetiresOnSaturation();
  EXPECT_CALL(*gpu_memory_buffer, GetHandle())
      .Times(1)
      .WillOnce(Return(handle))
      .RetiresOnSaturation();

  // Create the image. This should add the image ID to the ImageManager.
  GLuint image_id = glCreateImageCHROMIUM(
      kImageWidth, kImageHeight, GL_RGBA8_OES, GL_IMAGE_MAP_CHROMIUM);
  EXPECT_NE(0u, image_id);
  EXPECT_TRUE(image_manager_->LookupImage(image_id) != NULL);

  EXPECT_CALL(*gpu_memory_buffer, IsMapped())
      .WillOnce(Return(false))
      .RetiresOnSaturation();

  shared_memory.Map(bytes);
  EXPECT_TRUE(shared_memory.memory());

  EXPECT_CALL(*gpu_memory_buffer, Map())
      .Times(1)
      .WillOnce(Return(shared_memory.memory()))
      .RetiresOnSaturation();
  uint8* mapped_buffer = static_cast<uint8*>(glMapImageCHROMIUM(image_id));
  ASSERT_TRUE(mapped_buffer != NULL);

  // Assign a value to each pixel.
  int stride = kImageWidth * kImageBytesPerPixel;
  for (int x = 0; x < kImageWidth; ++x) {
    for (int y = 0; y < kImageHeight; ++y) {
      mapped_buffer[y * stride + x * kImageBytesPerPixel + 0] = pixels[0];
      mapped_buffer[y * stride + x * kImageBytesPerPixel + 1] = pixels[1];
      mapped_buffer[y * stride + x * kImageBytesPerPixel + 2] = pixels[2];
      mapped_buffer[y * stride + x * kImageBytesPerPixel + 3] = pixels[3];
    }
  }

  EXPECT_CALL(*gpu_memory_buffer, IsMapped())
      .WillOnce(Return(true))
      .RetiresOnSaturation();

  // Unmap the image.
  EXPECT_CALL(*gpu_memory_buffer, Unmap())
      .Times(1)
      .RetiresOnSaturation();
  glUnmapImageCHROMIUM(image_id);

  // Bind the texture and the image.
  glBindTexture(GL_TEXTURE_2D, texture_ids_[0]);
  glBindTexImage2DCHROMIUM(GL_TEXTURE_2D, image_id);

  // Copy texture so we can verify result using CheckPixels.
  glCopyTextureCHROMIUM(GL_TEXTURE_2D,
                        texture_ids_[0],
                        texture_ids_[1],
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE);
  EXPECT_TRUE(glGetError() == GL_NO_ERROR);

  // Check if pixels match the values that were assigned to the mapped buffer.
  GLTestHelper::CheckPixels(0, 0, kImageWidth, kImageHeight, 0, pixels);
  EXPECT_TRUE(GL_NO_ERROR == glGetError());

  // Release the image.
  glReleaseTexImage2DCHROMIUM(GL_TEXTURE_2D, image_id);

  // Destroy the image.
  EXPECT_CALL(*gpu_memory_buffer, Die())
      .Times(1)
      .RetiresOnSaturation();
  glDestroyImageCHROMIUM(image_id);
}

}  // namespace gles2
}  // namespace gpu
