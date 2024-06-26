// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_QRCODE_BC_QRCODERVERSION_H_
#define FXBARCODE_QRCODE_BC_QRCODERVERSION_H_

#include <memory>

#include "fxbarcode/qrcode/BC_QRCoderECBlocks.h"

class CBC_QRCoderErrorCorrectionLevel;

class CBC_QRCoderVersion {
 public:
  static constexpr int32_t kMaxVersion = 40;

  CBC_QRCoderVersion(int32_t versionNumber,
                     const CBC_QRCoderECBlockData data[4]);
  ~CBC_QRCoderVersion();

  static void Initialize();
  static void Finalize();

  static const CBC_QRCoderVersion* GetVersionForNumber(int32_t versionNumber);

  int32_t GetVersionNumber() const;
  int32_t GetTotalCodeWords() const;
  int32_t GetDimensionForVersion() const;
  const CBC_QRCoderECBlocks* GetECBlocksForLevel(
      const CBC_QRCoderErrorCorrectionLevel& ecLevel) const;

 private:
  const int32_t m_versionNumber;
  int32_t m_totalCodeWords;
  std::unique_ptr<CBC_QRCoderECBlocks> m_ecBlocksArray[4];
};

#endif  // FXBARCODE_QRCODE_BC_QRCODERVERSION_H_
