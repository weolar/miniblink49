#include "node.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"
#include "v8.h"

namespace node {

using v8::Array;
using v8::ArrayBuffer;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::HeapSpaceStatistics;
using v8::HeapStatistics;
using v8::Isolate;
using v8::Local;
using v8::NewStringType;
using v8::Object;
using v8::String;
using v8::Uint32;
using v8::V8;
using v8::Value;

#define HEAP_STATISTICS_PROPERTIES(V)                                         \
  V(0, total_heap_size, kTotalHeapSizeIndex)                                  \
  V(1, total_heap_size_executable, kTotalHeapSizeExecutableIndex)             \
  V(2, total_physical_size, kTotalPhysicalSizeIndex)                          \
  V(3, total_available_size, kTotalAvailableSize)                             \
  V(4, used_heap_size, kUsedHeapSizeIndex)                                    \
  V(5, heap_size_limit, kHeapSizeLimitIndex)

#define V(a, b, c) +1
static const size_t kHeapStatisticsPropertiesCount =
    HEAP_STATISTICS_PROPERTIES(V);
#undef V

#define HEAP_SPACE_STATISTICS_PROPERTIES(V)                                   \
  V(0, space_size, kSpaceSizeIndex)                                           \
  V(1, space_used_size, kSpaceUsedSizeIndex)                                  \
  V(2, space_available_size, kSpaceAvailableSizeIndex)                        \
  V(3, physical_space_size, kPhysicalSpaceSizeIndex)

#define V(a, b, c) +1
static const size_t kHeapSpaceStatisticsPropertiesCount =
    HEAP_SPACE_STATISTICS_PROPERTIES(V);
#undef V

// Will be populated in InitializeV8Bindings.
static size_t number_of_heap_spaces = 0;


void UpdateHeapStatisticsArrayBuffer(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  HeapStatistics s;
  env->isolate()->GetHeapStatistics(&s);
  uint32_t* const buffer = env->heap_statistics_buffer();
#define V(index, name, _) buffer[index] = static_cast<uint32_t>(s.name());
  HEAP_STATISTICS_PROPERTIES(V)
#undef V
}


void UpdateHeapSpaceStatisticsBuffer(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  HeapSpaceStatistics s;
  Isolate* const isolate = env->isolate();
  uint32_t* buffer = env->heap_space_statistics_buffer();

  for (size_t i = 0; i < number_of_heap_spaces; i++) {
    isolate->GetHeapSpaceStatistics(&s, i);
    size_t const property_offset = i * kHeapSpaceStatisticsPropertiesCount;
#define V(index, name, _) buffer[property_offset + index] = \
                              static_cast<uint32_t>(s.name());
      HEAP_SPACE_STATISTICS_PROPERTIES(V)
#undef V
  }
}


void SetFlagsFromString(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (args.Length() < 1)
    return env->ThrowTypeError("v8 flag is required");
  if (!args[0]->IsString())
    return env->ThrowTypeError("v8 flag must be a string");

  String::Utf8Value flags(args[0]);
  V8::SetFlagsFromString(*flags, flags.length());
}


void InitializeV8Bindings(Local<Object> target,
                          Local<Value> unused,
                          Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  env->SetMethod(target,
                 "updateHeapStatisticsArrayBuffer",
                 UpdateHeapStatisticsArrayBuffer);

  env->set_heap_statistics_buffer(new uint32_t[kHeapStatisticsPropertiesCount]);

  const size_t heap_statistics_buffer_byte_length =
      sizeof(*env->heap_statistics_buffer()) * kHeapStatisticsPropertiesCount;

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(),
                                    "heapStatisticsArrayBuffer"),
              ArrayBuffer::New(env->isolate(),
                               env->heap_statistics_buffer(),
                               heap_statistics_buffer_byte_length));

#define V(i, _, name)                                                         \
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), #name),                   \
              Uint32::NewFromUnsigned(env->isolate(), i));

  HEAP_STATISTICS_PROPERTIES(V)
#undef V

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(),
                                    "kHeapSpaceStatisticsPropertiesCount"),
              Uint32::NewFromUnsigned(env->isolate(),
                                      kHeapSpaceStatisticsPropertiesCount));

  number_of_heap_spaces = env->isolate()->NumberOfHeapSpaces();

  // Heap space names are extracted once and exposed to JavaScript to
  // avoid excessive creation of heap space name Strings.
  HeapSpaceStatistics s;
  const Local<Array> heap_spaces = Array::New(env->isolate(),
                                              number_of_heap_spaces);
  for (size_t i = 0; i < number_of_heap_spaces; i++) {
    env->isolate()->GetHeapSpaceStatistics(&s, i);
    Local<String> heap_space_name = String::NewFromUtf8(env->isolate(),
                                                        s.space_name(),
                                                        NewStringType::kNormal)
                                        .ToLocalChecked();
    heap_spaces->Set(i, heap_space_name);
  }
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "kHeapSpaces"),
              heap_spaces);

  env->SetMethod(target,
                 "updateHeapSpaceStatisticsArrayBuffer",
                 UpdateHeapSpaceStatisticsBuffer);

  env->set_heap_space_statistics_buffer(
    new uint32_t[kHeapSpaceStatisticsPropertiesCount * number_of_heap_spaces]);

  const size_t heap_space_statistics_buffer_byte_length =
      sizeof(*env->heap_space_statistics_buffer()) *
      kHeapSpaceStatisticsPropertiesCount *
      number_of_heap_spaces;

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(),
                                    "heapSpaceStatisticsArrayBuffer"),
              ArrayBuffer::New(env->isolate(),
                               env->heap_space_statistics_buffer(),
                               heap_space_statistics_buffer_byte_length));

#define V(i, _, name)                                                         \
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), #name),                   \
              Uint32::NewFromUnsigned(env->isolate(), i));

  HEAP_SPACE_STATISTICS_PROPERTIES(V)
#undef V

  env->SetMethod(target, "setFlagsFromString", SetFlagsFromString);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(v8, node::InitializeV8Bindings)
