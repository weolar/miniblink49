// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_PARSER_CPDF_OBJECT_WALKER_H_
#define CORE_FPDFAPI_PARSER_CPDF_OBJECT_WALKER_H_

#include <memory>
#include <stack>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Object;

// Walk on all non-null sub-objects in an object in depth, include itself,
// like in flat list.
class CPDF_ObjectWalker {
 public:
  class SubobjectIterator {
   public:
    virtual ~SubobjectIterator();
    virtual bool IsFinished() const = 0;
    bool IsStarted() const { return is_started_; }
    const CPDF_Object* Increment();
    const CPDF_Object* object() const { return object_.Get(); }

   protected:
    explicit SubobjectIterator(const CPDF_Object* object);

    virtual const CPDF_Object* IncrementImpl() = 0;
    virtual void Start() = 0;

   private:
    UnownedPtr<const CPDF_Object> object_;
    bool is_started_ = false;
  };

  explicit CPDF_ObjectWalker(const CPDF_Object* root);
  ~CPDF_ObjectWalker();

  const CPDF_Object* GetNext();
  void SkipWalkIntoCurrentObject();

  size_t current_depth() const { return current_depth_; }
  const CPDF_Object* GetParent() const { return parent_object_.Get(); }
  const ByteString& dictionary_key() const { return dict_key_; }

 private:
  static std::unique_ptr<SubobjectIterator> MakeIterator(
      const CPDF_Object* object);

  UnownedPtr<const CPDF_Object> next_object_;
  UnownedPtr<const CPDF_Object> parent_object_;
  ByteString dict_key_;
  size_t current_depth_ = 0;
  std::stack<std::unique_ptr<SubobjectIterator>> stack_;
};

class CPDF_NonConstObjectWalker final : public CPDF_ObjectWalker {
 public:
  explicit CPDF_NonConstObjectWalker(CPDF_Object* root)
      : CPDF_ObjectWalker(root) {}

  CPDF_Object* GetNext() {
    return const_cast<CPDF_Object*>(CPDF_ObjectWalker::GetNext());
  }
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_OBJECT_WALKER_H_
