// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fpdfsdk/cpdfsdk_customaccess.h"

CPDFSDK_CustomAccess::CPDFSDK_CustomAccess(FPDF_FILEACCESS* pFileAccess)
    : m_pFileAccess(pFileAccess) {
  ASSERT(m_pFileAccess);
}

CPDFSDK_CustomAccess::~CPDFSDK_CustomAccess() = default;

FX_FILESIZE CPDFSDK_CustomAccess::GetSize() {
  return m_pFileAccess->m_FileLen;
}

bool CPDFSDK_CustomAccess::ReadBlockAtOffset(void* buffer,
                                             FX_FILESIZE offset,
                                             size_t size) {
  if (!buffer || offset < 0 || !size)
    return false;

  FX_SAFE_FILESIZE new_pos = pdfium::base::checked_cast<FX_FILESIZE>(size);
  new_pos += offset;
  return new_pos.IsValid() && new_pos.ValueOrDie() <= GetSize() &&
         m_pFileAccess->m_GetBlock(m_pFileAccess->m_Param, offset,
                                   static_cast<uint8_t*>(buffer), size);
}
