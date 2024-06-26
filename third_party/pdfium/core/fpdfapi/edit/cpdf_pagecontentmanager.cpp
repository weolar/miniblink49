// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/edit/cpdf_pagecontentmanager.h"

#include <map>
#include <numeric>
#include <vector>

#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"

CPDF_PageContentManager::CPDF_PageContentManager(
    const CPDF_PageObjectHolder* obj_holder)
    : obj_holder_(obj_holder), doc_(obj_holder_->GetDocument()) {
  CPDF_Dictionary* page_dict = obj_holder_->GetDict();
  CPDF_Object* contents_obj = page_dict->GetObjectFor("Contents");
  CPDF_Array* contents_array = ToArray(contents_obj);
  if (contents_array) {
    contents_array_ = contents_array;
    return;
  }

  CPDF_Reference* contents_reference = ToReference(contents_obj);
  if (contents_reference) {
    CPDF_Object* indirect_obj = contents_reference->GetDirect();
    if (!indirect_obj)
      return;

    contents_array = indirect_obj->AsArray();
    if (contents_array)
      contents_array_ = contents_array;
    else if (indirect_obj->IsStream())
      contents_stream_ = indirect_obj->AsStream();
  }
}

CPDF_PageContentManager::~CPDF_PageContentManager() = default;

CPDF_Stream* CPDF_PageContentManager::GetStreamByIndex(size_t stream_index) {
  if (contents_stream_)
    return stream_index == 0 ? contents_stream_.Get() : nullptr;

  if (contents_array_) {
    CPDF_Reference* stream_reference =
        ToReference(contents_array_->GetObjectAt(stream_index));
    if (!stream_reference)
      return nullptr;

    return stream_reference->GetDirect()->AsStream();
  }

  return nullptr;
}

size_t CPDF_PageContentManager::AddStream(std::ostringstream* buf) {
  CPDF_Stream* new_stream = doc_->NewIndirect<CPDF_Stream>();
  new_stream->SetDataFromStringstream(buf);

  // If there is one Content stream (not in an array), now there will be two, so
  // create an array with the old and the new one. The new one's index is 1.
  if (contents_stream_) {
    CPDF_Array* new_contents_array = doc_->NewIndirect<CPDF_Array>();
    new_contents_array->Add(contents_stream_->MakeReference(doc_.Get()));
    new_contents_array->Add(new_stream->MakeReference(doc_.Get()));

    CPDF_Dictionary* page_dict = obj_holder_->GetDict();
    page_dict->SetFor("Contents",
                      new_contents_array->MakeReference(doc_.Get()));
    contents_array_ = new_contents_array;
    contents_stream_ = nullptr;
    return 1;
  }

  // If there is an array, just add the new stream to it, at the last position.
  if (contents_array_) {
    contents_array_->Add(new_stream->MakeReference(doc_.Get()));
    return contents_array_->size() - 1;
  }

  // There were no Contents, so add the new stream as the single Content stream.
  // Its index is 0.
  CPDF_Dictionary* page_dict = obj_holder_->GetDict();
  page_dict->SetFor("Contents", new_stream->MakeReference(doc_.Get()));
  contents_stream_ = new_stream;
  return 0;
}

void CPDF_PageContentManager::ScheduleRemoveStreamByIndex(size_t stream_index) {
  streams_to_remove_.insert(stream_index);
}

void CPDF_PageContentManager::ExecuteScheduledRemovals() {
  // This method assumes there are no dirty streams in the
  // CPDF_PageObjectHolder. If there were any, their indexes would need to be
  // updated.
  // Since this is only called by CPDF_PageContentGenerator::GenerateContent(),
  // which cleans up the dirty streams first, this should always be true.
  ASSERT(obj_holder_->GetDirtyStreams().empty());

  if (contents_stream_) {
    // Only stream that can be removed is 0.
    if (streams_to_remove_.find(0) != streams_to_remove_.end()) {
      CPDF_Dictionary* page_dict = obj_holder_->GetDict();
      page_dict->RemoveFor("Contents");
      contents_stream_ = nullptr;
    }
  } else if (contents_array_) {
    // Initialize a vector with the old stream indexes. This will be used to
    // build a map from the old to the new indexes.
    std::vector<size_t> streams_left(contents_array_->size());
    std::iota(streams_left.begin(), streams_left.end(), 0);

    // In reverse order so as to not change the indexes in the middle of the
    // loop, remove the streams.
    for (auto it = streams_to_remove_.rbegin(); it != streams_to_remove_.rend();
         ++it) {
      size_t stream_index = *it;
      contents_array_->RemoveAt(stream_index);
      streams_left.erase(streams_left.begin() + stream_index);
    }

    // Create a mapping from the old to the new stream indexes, shifted due to
    // the deletion of the |streams_to_remove_|.
    std::map<int32_t, size_t> stream_index_mapping;
    for (size_t i = 0; i < streams_left.size(); ++i)
      stream_index_mapping[streams_left[i]] = i;

    // Update the page objects' content stream indexes.
    for (const auto& obj : *obj_holder_->GetPageObjectList()) {
      int32_t old_stream_index = obj->GetContentStream();
      size_t new_stream_index = stream_index_mapping[old_stream_index];
      obj->SetContentStream(new_stream_index);
    }

    // Even if there is a single content stream now, keep the array with a
    // single element. It's valid, a second stream might be added soon, and the
    // complexity of removing it is not worth it.
  }

  streams_to_remove_.clear();
}
