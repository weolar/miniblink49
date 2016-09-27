// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdlib.h>

#include "base/logging.h"
#include "gin/array_buffer.h"
#ifdef MINIBLINK_NOT_IMPLEMENTED
#include "gin/per_isolate_data.h"
#endif // MINIBLINK_NOT_IMPLEMENTED
#include "third_party/WebKit/Source/wtf/ArrayBufferContents.h"

namespace gin {

#ifdef MINIBLINK_NOT_IMPLEMENTED
namespace {

gin::WrapperInfo g_array_buffer_wrapper_info = {gin::kEmbedderNativeGin};

}  // namespace

static_assert(V8_ARRAY_BUFFER_INTERNAL_FIELD_COUNT == 2,
              "array buffers must have two internal fields");
#endif // MINIBLINK_NOT_IMPLEMENTED

// ArrayBufferAllocator -------------------------------------------------------

void* ArrayBufferAllocator::Allocate(size_t length) {
  //return calloc(1, length);
    void* data = nullptr;
    WTF::ArrayBufferContents::allocateMemory(length, WTF::ArrayBufferContents::ZeroInitialize, data);
    return data;
}

void* ArrayBufferAllocator::AllocateUninitialized(size_t length) {
  //return malloc(length);
    void* data = nullptr;
    WTF::ArrayBufferContents::allocateMemory(length, WTF::ArrayBufferContents::DontInitialize, data);
    return data;
}

void ArrayBufferAllocator::Free(void* data, size_t length) {
  //free(data);
    WTF::ArrayBufferContents::freeMemory(data, length);
}

ArrayBufferAllocator* ArrayBufferAllocator::SharedInstance() {
  static ArrayBufferAllocator* instance = new ArrayBufferAllocator();
  return instance;
}

#ifdef MINIBLINK_NOT_IMPLEMENTED

// ArrayBuffer::Private -------------------------------------------------------

// This class exists to solve a tricky lifetime problem. The V8 API doesn't
// want to expose a direct view into the memory behind an array buffer because
// V8 might deallocate that memory during garbage collection. Instead, the V8
// API forces us to externalize the buffer and take ownership of the memory.
// In order to know when to free the memory, we need to figure out both when
// we're done with it and when V8 is done with it.
//
// To determine whether we're done with the memory, every view we have into
// the array buffer takes a reference to the ArrayBuffer::Private object that
// actually owns the memory. To determine when V8 is done with the memory, we
// open a weak handle to the ArrayBuffer object. When we receive the weak
// callback, we know the object is about to be garbage collected and we can
// drop V8's implied reference to the memory.
//
// The final subtlety is that we need every ArrayBuffer into the same array
// buffer to AddRef the same ArrayBuffer::Private. To make that work, we store
// a pointer to the ArrayBuffer::Private object in an internal field of the
// ArrayBuffer object.
//
class ArrayBuffer::Private : public base::RefCounted<ArrayBuffer::Private> {
 public:
  static scoped_refptr<Private> From(v8::Isolate* isolate,
                                     v8::Local<v8::ArrayBuffer> array);

  void* buffer() const { return buffer_; }
  size_t length() const { return length_; }

 private:
  friend class base::RefCounted<Private>;

  Private(v8::Isolate* isolate, v8::Local<v8::ArrayBuffer> array);
  ~Private();

  static void FirstWeakCallback(const v8::WeakCallbackInfo<Private>& data);
  static void SecondWeakCallback(const v8::WeakCallbackInfo<Private>& data);

  v8::Global<v8::ArrayBuffer> array_buffer_;
  scoped_refptr<Private> self_reference_;
  v8::Isolate* isolate_;
  void* buffer_;
  size_t length_;
};

scoped_refptr<ArrayBuffer::Private> ArrayBuffer::Private::From(
    v8::Isolate* isolate, v8::Local<v8::ArrayBuffer> array) {
  if (array->IsExternal()) {
    CHECK_EQ(WrapperInfo::From(v8::Local<v8::Object>::Cast(array)),
             &g_array_buffer_wrapper_info)
        << "Cannot mix blink and gin ArrayBuffers";
    return make_scoped_refptr(static_cast<Private*>(
        array->GetAlignedPointerFromInternalField(kEncodedValueIndex)));
  }
  return make_scoped_refptr(new Private(isolate, array));
}

ArrayBuffer::Private::Private(v8::Isolate* isolate,
                              v8::Local<v8::ArrayBuffer> array)
    : array_buffer_(isolate, array), isolate_(isolate) {
  // Take ownership of the array buffer.
  CHECK(!array->IsExternal());
  v8::ArrayBuffer::Contents contents = array->Externalize();
  buffer_ = contents.Data();
  length_ = contents.ByteLength();

  array->SetAlignedPointerInInternalField(kWrapperInfoIndex,
                                          &g_array_buffer_wrapper_info);
  array->SetAlignedPointerInInternalField(kEncodedValueIndex, this);

  self_reference_ = this;  // Cleared in SecondWeakCallback.
  array_buffer_.SetWeak(this, FirstWeakCallback,
                        v8::WeakCallbackType::kParameter);
}

ArrayBuffer::Private::~Private() {
  PerIsolateData::From(isolate_)->allocator()->Free(buffer_, length_);
}

void ArrayBuffer::Private::FirstWeakCallback(
    const v8::WeakCallbackInfo<Private>& data) {
  Private* parameter = data.GetParameter();
  parameter->array_buffer_.Reset();
  data.SetSecondPassCallback(SecondWeakCallback);
}

void ArrayBuffer::Private::SecondWeakCallback(
    const v8::WeakCallbackInfo<Private>& data) {
  Private* parameter = data.GetParameter();
  parameter->self_reference_ = NULL;
}

// ArrayBuffer ----------------------------------------------------------------

ArrayBuffer::ArrayBuffer()
    : bytes_(0),
      num_bytes_(0) {
}

ArrayBuffer::ArrayBuffer(v8::Isolate* isolate,
                         v8::Local<v8::ArrayBuffer> array) {
  private_ = ArrayBuffer::Private::From(isolate, array);
  bytes_ = private_->buffer();
  num_bytes_ = private_->length();
}

ArrayBuffer::~ArrayBuffer() {
}

ArrayBuffer& ArrayBuffer::operator=(const ArrayBuffer& other) {
  private_ = other.private_;
  bytes_ = other.bytes_;
  num_bytes_ = other.num_bytes_;
  return *this;
}

// Converter<ArrayBuffer> -----------------------------------------------------

bool Converter<ArrayBuffer>::FromV8(v8::Isolate* isolate,
                                    v8::Local<v8::Value> val,
                                    ArrayBuffer* out) {
  if (!val->IsArrayBuffer())
    return false;
  *out = ArrayBuffer(isolate, v8::Local<v8::ArrayBuffer>::Cast(val));
  return true;
}

// ArrayBufferView ------------------------------------------------------------

ArrayBufferView::ArrayBufferView()
    : offset_(0),
      num_bytes_(0) {
}

ArrayBufferView::ArrayBufferView(v8::Isolate* isolate,
                                 v8::Local<v8::ArrayBufferView> view)
    : array_buffer_(isolate, view->Buffer()),
      offset_(view->ByteOffset()),
      num_bytes_(view->ByteLength()) {
}

ArrayBufferView::~ArrayBufferView() {
}

ArrayBufferView& ArrayBufferView::operator=(const ArrayBufferView& other) {
  array_buffer_ = other.array_buffer_;
  offset_ = other.offset_;
  num_bytes_ = other.num_bytes_;
  return *this;
}


// Converter<ArrayBufferView> -------------------------------------------------

bool Converter<ArrayBufferView>::FromV8(v8::Isolate* isolate,
                                        v8::Local<v8::Value> val,
                                        ArrayBufferView* out) {
  if (!val->IsArrayBufferView())
    return false;
  *out = ArrayBufferView(isolate, v8::Local<v8::ArrayBufferView>::Cast(val));
  return true;
}
#endif // MINIBLINK_NOT_IMPLEMENTED

}  // namespace gin
