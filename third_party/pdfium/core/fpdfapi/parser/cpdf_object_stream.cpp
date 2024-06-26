// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_object_stream.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

// static
bool CPDF_ObjectStream::IsObjectsStreamObject(const CPDF_Object* object) {
  const CPDF_Stream* stream = ToStream(object);
  if (!stream)
    return false;

  const CPDF_Dictionary* stream_dict = stream->GetDict();
  if (!stream_dict)
    return false;

  if (stream_dict->GetStringFor("Type") != "ObjStm")
    return false;

  const CPDF_Number* number_of_objects =
      ToNumber(stream_dict->GetObjectFor("N"));
  if (!number_of_objects || !number_of_objects->IsInteger() ||
      number_of_objects->GetInteger() < 0 ||
      number_of_objects->GetInteger() >=
          static_cast<int>(CPDF_Parser::kMaxObjectNumber)) {
    return false;
  }

  const CPDF_Number* first_object_offset =
      ToNumber(stream_dict->GetObjectFor("First"));
  if (!first_object_offset || !first_object_offset->IsInteger() ||
      first_object_offset->GetInteger() < 0) {
    return false;
  }

  return true;
}

//  static
std::unique_ptr<CPDF_ObjectStream> CPDF_ObjectStream::Create(
    const CPDF_Stream* stream) {
  if (!IsObjectsStreamObject(stream))
    return nullptr;
  // The ctor of CPDF_ObjectStream is protected. Use WrapUnique instead
  // MakeUnique.
  return pdfium::WrapUnique(new CPDF_ObjectStream(stream));
}

CPDF_ObjectStream::CPDF_ObjectStream(const CPDF_Stream* obj_stream)
    : obj_num_(obj_stream->GetObjNum()),
      first_object_offset_(obj_stream->GetDict()->GetIntegerFor("First")) {
  ASSERT(IsObjectsStreamObject(obj_stream));
  if (const auto* extends_ref =
          ToReference(obj_stream->GetDict()->GetObjectFor("Extends"))) {
    extends_obj_num_ = extends_ref->GetRefObjNum();
  }
  Init(obj_stream);
}

CPDF_ObjectStream::~CPDF_ObjectStream() = default;

bool CPDF_ObjectStream::HasObject(uint32_t obj_number) const {
  return pdfium::ContainsKey(objects_offsets_, obj_number);
}

std::unique_ptr<CPDF_Object> CPDF_ObjectStream::ParseObject(
    CPDF_IndirectObjectHolder* pObjList,
    uint32_t obj_number) const {
  const auto it = objects_offsets_.find(obj_number);
  if (it == objects_offsets_.end())
    return nullptr;

  std::unique_ptr<CPDF_Object> result =
      ParseObjectAtOffset(pObjList, it->second);
  if (!result)
    return nullptr;

  result->SetObjNum(obj_number);
  return result;
}

void CPDF_ObjectStream::Init(const CPDF_Stream* stream) {
  {
    auto stream_acc = pdfium::MakeRetain<CPDF_StreamAcc>(stream);
    stream_acc->LoadAllDataFiltered();
    const uint32_t data_size = stream_acc->GetSize();
    data_stream_ = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        stream_acc->DetachData(), data_size);
  }

  CPDF_SyntaxParser syntax(data_stream_);
  const int object_count = stream->GetDict()->GetIntegerFor("N");
  for (int32_t i = object_count; i > 0; --i) {
    if (syntax.GetPos() >= data_stream_->GetSize())
      break;

    const uint32_t obj_num = syntax.GetDirectNum();
    const uint32_t obj_offset = syntax.GetDirectNum();
    if (!obj_num)
      continue;

    objects_offsets_[obj_num] = obj_offset;
  }
}

std::unique_ptr<CPDF_Object> CPDF_ObjectStream::ParseObjectAtOffset(
    CPDF_IndirectObjectHolder* pObjList,
    uint32_t object_offset) const {
  FX_SAFE_FILESIZE offset_in_stream = first_object_offset_;
  offset_in_stream += object_offset;

  if (!offset_in_stream.IsValid())
    return nullptr;

  if (offset_in_stream.ValueOrDie() >= data_stream_->GetSize())
    return nullptr;

  CPDF_SyntaxParser syntax(data_stream_);
  syntax.SetPos(offset_in_stream.ValueOrDie());
  return syntax.GetObjectBody(pObjList);
}
