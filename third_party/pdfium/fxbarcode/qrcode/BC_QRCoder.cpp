// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2008 ZXing authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <utility>

#include "fxbarcode/common/BC_CommonByteMatrix.h"
#include "fxbarcode/qrcode/BC_QRCoder.h"
#include "fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "fxbarcode/qrcode/BC_QRCoderMode.h"

CBC_QRCoder::CBC_QRCoder() = default;

CBC_QRCoder::~CBC_QRCoder() = default;

const CBC_QRCoderErrorCorrectionLevel* CBC_QRCoder::GetECLevel() const {
  return m_ecLevel.Get();
}

int32_t CBC_QRCoder::GetVersion() const {
  return m_version;
}

int32_t CBC_QRCoder::GetMatrixWidth() const {
  return m_matrixWidth;
}

int32_t CBC_QRCoder::GetMaskPattern() const {
  return m_maskPattern;
}

int32_t CBC_QRCoder::GetNumTotalBytes() const {
  return m_numTotalBytes;
}

int32_t CBC_QRCoder::GetNumDataBytes() const {
  return m_numDataBytes;
}

int32_t CBC_QRCoder::GetNumECBytes() const {
  return m_numECBytes;
}

int32_t CBC_QRCoder::GetNumRSBlocks() const {
  return m_numRSBlocks;
}

const CBC_CommonByteMatrix* CBC_QRCoder::GetMatrix() const {
  return m_matrix.get();
}

bool CBC_QRCoder::IsValid() const {
  return m_ecLevel && m_version != -1 && m_matrixWidth != -1 &&
         m_maskPattern != -1 && m_numTotalBytes != -1 && m_numDataBytes != -1 &&
         m_numECBytes != -1 && m_numRSBlocks != -1 &&
         IsValidMaskPattern(m_maskPattern) &&
         m_numTotalBytes == m_numDataBytes + m_numECBytes && m_matrix &&
         m_matrixWidth == m_matrix->GetWidth() &&
         m_matrix->GetWidth() == m_matrix->GetHeight();
}

void CBC_QRCoder::SetECLevel(const CBC_QRCoderErrorCorrectionLevel* ecLevel) {
  m_ecLevel = ecLevel;
}

void CBC_QRCoder::SetVersion(int32_t version) {
  m_version = version;
}

void CBC_QRCoder::SetMatrixWidth(int32_t width) {
  m_matrixWidth = width;
}

void CBC_QRCoder::SetMaskPattern(int32_t pattern) {
  m_maskPattern = pattern;
}

void CBC_QRCoder::SetNumDataBytes(int32_t bytes) {
  m_numDataBytes = bytes;
}

void CBC_QRCoder::SetNumTotalBytes(int32_t value) {
  m_numTotalBytes = value;
}

void CBC_QRCoder::SetNumRSBlocks(int32_t block) {
  m_numRSBlocks = block;
}

void CBC_QRCoder::SetNumECBytes(int32_t value) {
  m_numECBytes = value;
}

bool CBC_QRCoder::IsValidMaskPattern(int32_t maskPattern) {
  return maskPattern >= 0 && maskPattern < kNumMaskPatterns;
}

void CBC_QRCoder::SetMatrix(std::unique_ptr<CBC_CommonByteMatrix> pMatrix) {
  m_matrix = std::move(pMatrix);
}
