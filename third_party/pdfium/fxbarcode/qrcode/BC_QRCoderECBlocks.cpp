// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com
// Original code is licensed as follows:
/*
 * Copyright 2007 ZXing authors
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

#include "fxbarcode/qrcode/BC_QRCoderECBlocks.h"

#include "fxbarcode/qrcode/BC_QRCoderECBlocksData.h"

CBC_QRCoderECBlocks::CBC_QRCoderECBlocks(const CBC_QRCoderECBlockData& data)
    : m_data(data) {}

CBC_QRCoderECBlocks::~CBC_QRCoderECBlocks() {}

int32_t CBC_QRCoderECBlocks::GetECCodeWordsPerBlock() const {
  return m_data.ecCodeWordsPerBlock;
}

int32_t CBC_QRCoderECBlocks::GetNumBlocks() const {
  return m_data.count1 + m_data.count2;
}

int32_t CBC_QRCoderECBlocks::GetTotalECCodeWords() const {
  return GetECCodeWordsPerBlock() * GetNumBlocks();
}

int32_t CBC_QRCoderECBlocks::GetTotalDataCodeWords() const {
  return m_data.count1 * (m_data.dataCodeWords1 + GetECCodeWordsPerBlock()) +
         m_data.count2 * (m_data.dataCodeWords2 + GetECCodeWordsPerBlock());
}
