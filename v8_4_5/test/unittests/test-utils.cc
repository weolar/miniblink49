// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "test/unittests/test-utils.h"

#include "src/base/platform/time.h"
#include "src/debug.h"
#include "src/flags.h"
#include "src/isolate.h"

namespace v8 {

class ArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
 public:
  virtual void* Allocate(size_t length) {
    void* data = AllocateUninitialized(length);
    return data == NULL ? data : memset(data, 0, length);
  }
  virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
  virtual void Free(void* data, size_t) { free(data); }
};


// static
ArrayBufferAllocator* TestWithIsolate::array_buffer_allocator_ = NULL;

// static
Isolate* TestWithIsolate::isolate_ = NULL;


TestWithIsolate::TestWithIsolate()
    : isolate_scope_(isolate()), handle_scope_(isolate()) {}


TestWithIsolate::~TestWithIsolate() {}


// static
void TestWithIsolate::SetUpTestCase() {
  Test::SetUpTestCase();
  EXPECT_EQ(NULL, isolate_);
  v8::Isolate::CreateParams create_params;
  array_buffer_allocator_ = new ArrayBufferAllocator;
  create_params.array_buffer_allocator = array_buffer_allocator_;
  isolate_ = v8::Isolate::New(create_params);
  EXPECT_TRUE(isolate_ != NULL);
}


// static
void TestWithIsolate::TearDownTestCase() {
  ASSERT_TRUE(isolate_ != NULL);
  isolate_->Dispose();
  isolate_ = NULL;
  delete array_buffer_allocator_;
  Test::TearDownTestCase();
}


TestWithContext::TestWithContext()
    : context_(Context::New(isolate())), context_scope_(context_) {}


TestWithContext::~TestWithContext() {}


namespace base {
namespace {

inline int64_t GetRandomSeedFromFlag(int random_seed) {
  return random_seed ? random_seed : TimeTicks::Now().ToInternalValue();
}

}  // namespace

TestWithRandomNumberGenerator::TestWithRandomNumberGenerator()
    : rng_(GetRandomSeedFromFlag(internal::FLAG_random_seed)) {}


TestWithRandomNumberGenerator::~TestWithRandomNumberGenerator() {}

}  // namespace base


namespace internal {

TestWithIsolate::~TestWithIsolate() {}

TestWithIsolateAndZone::~TestWithIsolateAndZone() {}

Factory* TestWithIsolate::factory() const { return isolate()->factory(); }


base::RandomNumberGenerator* TestWithIsolate::random_number_generator() const {
  return isolate()->random_number_generator();
}


TestWithZone::~TestWithZone() {}

}  // namespace internal
}  // namespace v8
