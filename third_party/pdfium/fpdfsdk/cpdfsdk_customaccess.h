// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_CPDFSDK_CUSTOMACCESS_H_
#define FPDFSDK_CPDFSDK_CUSTOMACCESS_H_

#include "core/fxcrt/fx_stream.h"
#include "public/fpdfview.h"

class CPDFSDK_CustomAccess final : public IFX_SeekableReadStream {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  // IFX_SeekableReadStream
  FX_FILESIZE GetSize() override;
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override;

 private:
  explicit CPDFSDK_CustomAccess(FPDF_FILEACCESS* pFileAccess);
  ~CPDFSDK_CustomAccess() override;

  UnownedPtr<FPDF_FILEACCESS> const m_pFileAccess;
};

#endif  // FPDFSDK_CPDFSDK_CUSTOMACCESS_H_
