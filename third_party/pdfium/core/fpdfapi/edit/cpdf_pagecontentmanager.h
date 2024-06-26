// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_EDIT_CPDF_PAGECONTENTMANAGER_H_
#define CORE_FPDFAPI_EDIT_CPDF_PAGECONTENTMANAGER_H_

#include <set>
#include <sstream>

#include "core/fxcrt/unowned_ptr.h"

class CPDF_Array;
class CPDF_Document;
class CPDF_Object;
class CPDF_Stream;
class CPDF_PageObjectHolder;

class CPDF_PageContentManager {
 public:
  explicit CPDF_PageContentManager(const CPDF_PageObjectHolder* pObjHolder);
  ~CPDF_PageContentManager();

  // Gets the Content stream at a given index. If Contents is a single stream
  // rather than an array, it is considered to be at index 0.
  CPDF_Stream* GetStreamByIndex(size_t stream_index);

  // Adds a new Content stream. Its index in the array will be returned, or 0
  // if Contents is not an array, but only a single stream.
  size_t AddStream(std::ostringstream* buf);

  // Schedule the removal of the Content stream at a given index. It will be
  // removed when ExecuteScheduledRemovals() is called.
  void ScheduleRemoveStreamByIndex(size_t stream_index);

  // Remove all Content streams for which ScheduleRemoveStreamByIndex() was
  // called. Update the content stream of all page objects with the shifted
  // indexes.
  void ExecuteScheduledRemovals();

 private:
  UnownedPtr<const CPDF_PageObjectHolder> const obj_holder_;
  UnownedPtr<CPDF_Document> const doc_;
  UnownedPtr<CPDF_Array> contents_array_;
  UnownedPtr<CPDF_Stream> contents_stream_;
  std::set<size_t> streams_to_remove_;
};

#endif  // CORE_FPDFAPI_EDIT_CPDF_PAGECONTENTMANAGER_H_
