// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_SEEKABLEMULTISTREAM_H_
#define CORE_FXCRT_CFX_SEEKABLEMULTISTREAM_H_

#include <vector>

#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/retain_ptr.h"

class CPDF_Stream;
class CPDF_StreamAcc;

class CFX_SeekableMultiStream final : public IFX_SeekableStream {
 public:
  explicit CFX_SeekableMultiStream(
      const std::vector<const CPDF_Stream*>& streams);
  ~CFX_SeekableMultiStream() override;

  // IFX_SeekableReadStream
  FX_FILESIZE GetPosition() override;
  FX_FILESIZE GetSize() override;
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override;
  size_t ReadBlock(void* buffer, size_t size) override;
  bool IsEOF() override;
  bool Flush() override;
  bool WriteBlockAtOffset(const void* pData,
                          FX_FILESIZE offset,
                          size_t size) override;

 private:
  std::vector<RetainPtr<CPDF_StreamAcc>> m_Data;
};

#endif  // CORE_FXCRT_CFX_SEEKABLEMULTISTREAM_H_
