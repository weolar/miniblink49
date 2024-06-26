// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_object_walker.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "third_party/base/ptr_util.h"

namespace {

class StreamIterator final : public CPDF_ObjectWalker::SubobjectIterator {
 public:
  explicit StreamIterator(const CPDF_Stream* stream)
      : SubobjectIterator(stream) {}
  ~StreamIterator() override {}

  bool IsFinished() const override { return IsStarted() && is_finished_; }

  const CPDF_Object* IncrementImpl() override {
    ASSERT(IsStarted());
    ASSERT(!IsFinished());
    is_finished_ = true;
    return object()->GetDict();
  }

  void Start() override {}

 private:
  bool is_finished_ = false;
};

class DictionaryIterator final : public CPDF_ObjectWalker::SubobjectIterator {
 public:
  explicit DictionaryIterator(const CPDF_Dictionary* dictionary)
      : SubobjectIterator(dictionary), locker_(dictionary) {}
  ~DictionaryIterator() override {}

  bool IsFinished() const override {
    return IsStarted() && dict_iterator_ == locker_.end();
  }

  const CPDF_Object* IncrementImpl() override {
    ASSERT(IsStarted());
    ASSERT(!IsFinished());
    const CPDF_Object* result = dict_iterator_->second.get();
    dict_key_ = dict_iterator_->first;
    ++dict_iterator_;
    return result;
  }

  void Start() override {
    ASSERT(!IsStarted());
    dict_iterator_ = locker_.begin();
  }

  ByteString dict_key() const { return dict_key_; }

 private:
  CPDF_Dictionary::const_iterator dict_iterator_;
  CPDF_DictionaryLocker locker_;
  ByteString dict_key_;
};

class ArrayIterator final : public CPDF_ObjectWalker::SubobjectIterator {
 public:
  explicit ArrayIterator(const CPDF_Array* array)
      : SubobjectIterator(array), locker_(array) {}

  ~ArrayIterator() override {}

  bool IsFinished() const override {
    return IsStarted() && arr_iterator_ == locker_.end();
  }

  const CPDF_Object* IncrementImpl() override {
    ASSERT(IsStarted());
    ASSERT(!IsFinished());
    const CPDF_Object* result = arr_iterator_->get();
    ++arr_iterator_;
    return result;
  }

  void Start() override { arr_iterator_ = locker_.begin(); }

 public:
  CPDF_Array::const_iterator arr_iterator_;
  CPDF_ArrayLocker locker_;
};

}  // namespace

CPDF_ObjectWalker::SubobjectIterator::~SubobjectIterator() {}

const CPDF_Object* CPDF_ObjectWalker::SubobjectIterator::Increment() {
  if (!IsStarted()) {
    Start();
    is_started_ = true;
  }
  while (!IsFinished()) {
    const CPDF_Object* result = IncrementImpl();
    if (result)
      return result;
  }
  return nullptr;
}

CPDF_ObjectWalker::SubobjectIterator::SubobjectIterator(
    const CPDF_Object* object)
    : object_(object) {
  ASSERT(object_);
}

// static
std::unique_ptr<CPDF_ObjectWalker::SubobjectIterator>
CPDF_ObjectWalker::MakeIterator(const CPDF_Object* object) {
  if (object->IsStream())
    return pdfium::MakeUnique<StreamIterator>(object->AsStream());
  if (object->IsDictionary())
    return pdfium::MakeUnique<DictionaryIterator>(object->AsDictionary());
  if (object->IsArray())
    return pdfium::MakeUnique<ArrayIterator>(object->AsArray());
  return nullptr;
}

CPDF_ObjectWalker::CPDF_ObjectWalker(const CPDF_Object* root)
    : next_object_(root) {}

CPDF_ObjectWalker::~CPDF_ObjectWalker() = default;

const CPDF_Object* CPDF_ObjectWalker::GetNext() {
  while (!stack_.empty() || next_object_) {
    if (next_object_) {
      auto new_iterator = MakeIterator(next_object_.Get());
      if (new_iterator) {
        // Schedule walk within composite objects.
        stack_.push(std::move(new_iterator));
      }
      auto* result = next_object_.Get();
      next_object_ = nullptr;
      return result;
    }

    SubobjectIterator* it = stack_.top().get();
    if (it->IsFinished()) {
      stack_.pop();
    } else {
      next_object_ = it->Increment();
      parent_object_ = it->object();
      dict_key_ = parent_object_->IsDictionary()
                      ? static_cast<DictionaryIterator*>(it)->dict_key()
                      : ByteString();
      current_depth_ = stack_.size();
    }
  }
  dict_key_ = ByteString();
  current_depth_ = 0;
  return nullptr;
}

void CPDF_ObjectWalker::SkipWalkIntoCurrentObject() {
  if (stack_.empty() || stack_.top()->IsStarted())
    return;
  stack_.pop();
}
