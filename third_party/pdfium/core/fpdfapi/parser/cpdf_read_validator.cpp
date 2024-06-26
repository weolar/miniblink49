// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_read_validator.h"

#include <algorithm>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "third_party/base/logging.h"

namespace {

constexpr FX_FILESIZE kAlignBlockValue = CPDF_ModuleMgr::kFileBufSize;

FX_FILESIZE AlignDown(FX_FILESIZE offset) {
  return offset > 0 ? (offset - offset % kAlignBlockValue) : 0;
}

FX_FILESIZE AlignUp(FX_FILESIZE offset) {
  FX_SAFE_FILESIZE safe_result = AlignDown(offset);
  safe_result += kAlignBlockValue;
  if (safe_result.IsValid())
    return safe_result.ValueOrDie();
  return offset;
}

}  // namespace

CPDF_ReadValidator::Session::Session(CPDF_ReadValidator* validator)
    : validator_(validator) {
  ASSERT(validator_);
  saved_read_error_ = validator_->read_error_;
  saved_has_unavailable_data_ = validator_->has_unavailable_data_;
  validator_->ResetErrors();
}

CPDF_ReadValidator::Session::~Session() {
  validator_->read_error_ |= saved_read_error_;
  validator_->has_unavailable_data_ |= saved_has_unavailable_data_;
}

CPDF_ReadValidator::CPDF_ReadValidator(
    const RetainPtr<IFX_SeekableReadStream>& file_read,
    CPDF_DataAvail::FileAvail* file_avail)
    : file_read_(file_read),
      file_avail_(file_avail),
      read_error_(false),
      has_unavailable_data_(false),
      whole_file_already_available_(false),
      file_size_(file_read->GetSize()) {}

CPDF_ReadValidator::~CPDF_ReadValidator() {}

void CPDF_ReadValidator::ResetErrors() {
  read_error_ = false;
  has_unavailable_data_ = false;
}

bool CPDF_ReadValidator::ReadBlockAtOffset(void* buffer,
                                           FX_FILESIZE offset,
                                           size_t size) {
  FX_SAFE_FILESIZE end_offset = offset;
  end_offset += size;
  if (!end_offset.IsValid() || end_offset.ValueOrDie() > file_size_)
    return false;

  if (!IsDataRangeAvailable(offset, size)) {
    ScheduleDownload(offset, size);
    return false;
  }

  if (file_read_->ReadBlockAtOffset(buffer, offset, size))
    return true;

  read_error_ = true;
  ScheduleDownload(offset, size);
  return false;
}

FX_FILESIZE CPDF_ReadValidator::GetSize() {
  return file_size_;
}

void CPDF_ReadValidator::ScheduleDownload(FX_FILESIZE offset, size_t size) {
  has_unavailable_data_ = true;
  if (!hints_ || size == 0)
    return;

  const FX_FILESIZE start_segment_offset = AlignDown(offset);
  FX_SAFE_FILESIZE end_segment_offset = offset;
  end_segment_offset += size;
  if (!end_segment_offset.IsValid()) {
    NOTREACHED();
    return;
  }
  end_segment_offset =
      std::min(file_size_, AlignUp(end_segment_offset.ValueOrDie()));

  FX_SAFE_SIZE_T segment_size = end_segment_offset;
  segment_size -= start_segment_offset;
  if (!segment_size.IsValid()) {
    NOTREACHED();
    return;
  }
  hints_->AddSegment(start_segment_offset, segment_size.ValueOrDie());
}

bool CPDF_ReadValidator::IsDataRangeAvailable(FX_FILESIZE offset,
                                              size_t size) const {
  return whole_file_already_available_ || !file_avail_ ||
         file_avail_->IsDataAvail(offset, size);
}

bool CPDF_ReadValidator::IsWholeFileAvailable() {
  const FX_SAFE_SIZE_T safe_size = file_size_;
  whole_file_already_available_ =
      whole_file_already_available_ ||
      (safe_size.IsValid() ? IsDataRangeAvailable(0, safe_size.ValueOrDie())
                           : false);

  return whole_file_already_available_;
}

bool CPDF_ReadValidator::CheckDataRangeAndRequestIfUnavailable(
    FX_FILESIZE offset,
    size_t size) {
  if (offset > file_size_)
    return true;

  FX_SAFE_FILESIZE end_segment_offset = offset;
  end_segment_offset += size;
  // Increase checked range to allow CPDF_SyntaxParser read whole buffer.
  end_segment_offset += CPDF_ModuleMgr::kFileBufSize;
  if (!end_segment_offset.IsValid()) {
    NOTREACHED();
    return false;
  }
  end_segment_offset = std::min(
      file_size_, static_cast<FX_FILESIZE>(end_segment_offset.ValueOrDie()));
  FX_SAFE_SIZE_T segment_size = end_segment_offset;
  segment_size -= offset;
  if (!segment_size.IsValid()) {
    NOTREACHED();
    return false;
  }

  if (IsDataRangeAvailable(offset, segment_size.ValueOrDie()))
    return true;

  ScheduleDownload(offset, segment_size.ValueOrDie());
  return false;
}

bool CPDF_ReadValidator::CheckWholeFileAndRequestIfUnavailable() {
  if (IsWholeFileAvailable())
    return true;

  const FX_SAFE_SIZE_T safe_size = file_size_;
  if (safe_size.IsValid())
    ScheduleDownload(0, safe_size.ValueOrDie());

  return false;
}
