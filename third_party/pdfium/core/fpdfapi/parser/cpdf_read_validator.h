// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_PARSER_CPDF_READ_VALIDATOR_H_
#define CORE_FPDFAPI_PARSER_CPDF_READ_VALIDATOR_H_

#include "core/fpdfapi/parser/cpdf_data_avail.h"
#include "core/fxcrt/fx_stream.h"

class CPDF_ReadValidator : public IFX_SeekableReadStream {
 public:
  class Session {
   public:
    explicit Session(CPDF_ReadValidator* validator);
    ~Session();

   private:
    UnownedPtr<CPDF_ReadValidator> validator_;
    bool saved_read_error_;
    bool saved_has_unavailable_data_;
  };

  void SetDownloadHints(CPDF_DataAvail::DownloadHints* hints) {
    hints_ = hints;
  }

  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  bool read_error() const { return read_error_; }
  bool has_unavailable_data() const { return has_unavailable_data_; }

  bool has_read_problems() const {
    return read_error() || has_unavailable_data();
  }

  void ResetErrors();

  bool IsWholeFileAvailable();

  bool CheckDataRangeAndRequestIfUnavailable(FX_FILESIZE offset, size_t size);
  bool CheckWholeFileAndRequestIfUnavailable();

  // IFX_SeekableReadStream overrides:
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override;
  FX_FILESIZE GetSize() override;

 protected:
  CPDF_ReadValidator(const RetainPtr<IFX_SeekableReadStream>& file_read,
                     CPDF_DataAvail::FileAvail* file_avail);
  ~CPDF_ReadValidator() override;

 private:
  void ScheduleDownload(FX_FILESIZE offset, size_t size);
  bool IsDataRangeAvailable(FX_FILESIZE offset, size_t size) const;

  RetainPtr<IFX_SeekableReadStream> file_read_;
  UnownedPtr<CPDF_DataAvail::FileAvail> file_avail_;

  UnownedPtr<CPDF_DataAvail::DownloadHints> hints_;

  bool read_error_;
  bool has_unavailable_data_;
  bool whole_file_already_available_;
  const FX_FILESIZE file_size_;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_READ_VALIDATOR_H_
