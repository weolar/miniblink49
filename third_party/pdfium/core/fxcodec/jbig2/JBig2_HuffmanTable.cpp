// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_HuffmanTable.h"

#include <limits>
#include <vector>

#include "core/fxcodec/jbig2/JBig2_BitStream.h"
#include "core/fxcodec/jbig2/JBig2_Context.h"
#include "core/fxcrt/fx_memory.h"
#include "third_party/base/numerics/safe_math.h"

namespace {

struct JBig2TableLine {
  uint8_t PREFLEN;
  uint8_t RANDELEN;
  int32_t RANGELOW;
};

struct HuffmanTable {
  bool HTOOB;
  const JBig2TableLine* lines;
  size_t size;
};

constexpr JBig2TableLine kTableLine1[] = {{1, 4, 0},
                                          {2, 8, 16},
                                          {3, 16, 272},
                                          {0, 32, -1},
                                          {3, 32, 65808}};

constexpr JBig2TableLine kTableLine2[] = {{1, 0, 0},   {2, 0, 1},  {3, 0, 2},
                                          {4, 3, 3},   {5, 6, 11}, {0, 32, -1},
                                          {6, 32, 75}, {6, 0, 0}};

constexpr JBig2TableLine kTableLine3[] = {
    {8, 8, -256}, {1, 0, 0},     {2, 0, 1},   {3, 0, 2}, {4, 3, 3},
    {5, 6, 11},   {8, 32, -257}, {7, 32, 75}, {6, 0, 0}};

constexpr JBig2TableLine kTableLine4[] = {{1, 0, 1},  {2, 0, 2},  {3, 0, 3},
                                          {4, 3, 4},  {5, 6, 12}, {0, 32, -1},
                                          {5, 32, 76}};

constexpr JBig2TableLine kTableLine5[] = {{7, 8, -255},  {1, 0, 1},  {2, 0, 2},
                                          {3, 0, 3},     {4, 3, 4},  {5, 6, 12},
                                          {7, 32, -256}, {6, 32, 76}};

constexpr JBig2TableLine kTableLine6[] = {
    {5, 10, -2048}, {4, 9, -1024}, {4, 8, -512},   {4, 7, -256}, {5, 6, -128},
    {5, 5, -64},    {4, 5, -32},   {2, 7, 0},      {3, 7, 128},  {3, 8, 256},
    {4, 9, 512},    {4, 10, 1024}, {6, 32, -2049}, {6, 32, 2048}};

constexpr JBig2TableLine kTableLine7[] = {
    {4, 9, -1024}, {3, 8, -512}, {4, 7, -256},  {5, 6, -128},   {5, 5, -64},
    {4, 5, -32},   {4, 5, 0},    {5, 5, 32},    {5, 6, 64},     {4, 7, 128},
    {3, 8, 256},   {3, 9, 512},  {3, 10, 1024}, {5, 32, -1025}, {5, 32, 2048}};

constexpr JBig2TableLine kTableLine8[] = {
    {8, 3, -15}, {9, 1, -7},  {8, 1, -5},   {9, 0, -3},   {7, 0, -2},
    {4, 0, -1},  {2, 1, 0},   {5, 0, 2},    {6, 0, 3},    {3, 4, 4},
    {6, 1, 20},  {4, 4, 22},  {4, 5, 38},   {5, 6, 70},   {5, 7, 134},
    {6, 7, 262}, {7, 8, 390}, {6, 10, 646}, {9, 32, -16}, {9, 32, 1670},
    {2, 0, 0}};

constexpr JBig2TableLine kTableLine9[] = {
    {8, 4, -31},   {9, 2, -15}, {8, 2, -11}, {9, 1, -7},    {7, 1, -5},
    {4, 1, -3},    {3, 1, -1},  {3, 1, 1},   {5, 1, 3},     {6, 1, 5},
    {3, 5, 7},     {6, 2, 39},  {4, 5, 43},  {4, 6, 75},    {5, 7, 139},
    {5, 8, 267},   {6, 8, 523}, {7, 9, 779}, {6, 11, 1291}, {9, 32, -32},
    {9, 32, 3339}, {2, 0, 0}};

constexpr JBig2TableLine kTableLine10[] = {
    {7, 4, -21}, {8, 0, -5},    {7, 0, -4},    {5, 0, -3},   {2, 2, -2},
    {5, 0, 2},   {6, 0, 3},     {7, 0, 4},     {8, 0, 5},    {2, 6, 6},
    {5, 5, 70},  {6, 5, 102},   {6, 6, 134},   {6, 7, 198},  {6, 8, 326},
    {6, 9, 582}, {6, 10, 1094}, {7, 11, 2118}, {8, 32, -22}, {8, 32, 4166},
    {2, 0, 0}};

constexpr JBig2TableLine kTableLine11[] = {
    {1, 0, 1},  {2, 1, 2},  {4, 0, 4},  {4, 1, 5},   {5, 1, 7},
    {5, 2, 9},  {6, 2, 13}, {7, 2, 17}, {7, 3, 21},  {7, 4, 29},
    {7, 5, 45}, {7, 6, 77}, {0, 32, 0}, {7, 32, 141}};

constexpr JBig2TableLine kTableLine12[] = {
    {1, 0, 1},  {2, 0, 2},  {3, 1, 3},  {5, 0, 5},  {5, 1, 6},
    {6, 1, 8},  {7, 0, 10}, {7, 1, 11}, {7, 2, 13}, {7, 3, 17},
    {7, 4, 25}, {8, 5, 41}, {0, 32, 0}, {8, 32, 73}};

constexpr JBig2TableLine kTableLine13[] = {
    {1, 0, 1},  {3, 0, 2},  {4, 0, 3},  {5, 0, 4},   {4, 1, 5},
    {3, 3, 7},  {6, 1, 15}, {6, 2, 17}, {6, 3, 21},  {6, 4, 29},
    {6, 5, 45}, {7, 6, 77}, {0, 32, 0}, {7, 32, 141}};

constexpr JBig2TableLine kTableLine14[] = {{3, 0, -2}, {3, 0, -1}, {1, 0, 0},
                                           {3, 0, 1},  {3, 0, 2},  {0, 32, -3},
                                           {0, 32, 3}};

constexpr JBig2TableLine kTableLine15[] = {
    {7, 4, -24}, {6, 2, -8},   {5, 1, -4}, {4, 0, -2}, {3, 0, -1},
    {1, 0, 0},   {3, 0, 1},    {4, 0, 2},  {5, 1, 3},  {6, 2, 5},
    {7, 4, 9},   {7, 32, -25}, {7, 32, 25}};

/*constexpr*/ HuffmanTable kHuffmanTables[16] = {
    {false, nullptr, 0},  // Zero dummy to preserve indexing.
    {false, kTableLine1, FX_ArraySize(kTableLine1)/*5*/},
    {true, kTableLine2, FX_ArraySize(kTableLine2)/*8*/},
    {true, kTableLine3, FX_ArraySize(kTableLine3)/*9*/},
    {false, kTableLine4, FX_ArraySize(kTableLine4)  /*7*/},
    {false, kTableLine5, FX_ArraySize(kTableLine5)  /*8*/},
    {false, kTableLine6, FX_ArraySize(kTableLine6)  /*14*/},
    {false, kTableLine7, FX_ArraySize(kTableLine7)  /*15*/},
    {true, kTableLine8, FX_ArraySize(kTableLine8)/*21*/},
    {true, kTableLine9, FX_ArraySize(kTableLine9)/*22*/},
    {true, kTableLine10, FX_ArraySize(kTableLine10)/*21*/},
    {false, kTableLine11, FX_ArraySize(kTableLine11)  /*14*/},
    {false, kTableLine12, FX_ArraySize(kTableLine12)  /*14*/},
    {false, kTableLine13, FX_ArraySize(kTableLine13)  /*14*/},
    {false, kTableLine14, FX_ArraySize(kTableLine14)  /*7*/},
    {false, kTableLine15, FX_ArraySize(kTableLine15)  /*13*/}
};

static_assert(CJBig2_HuffmanTable::kNumHuffmanTables ==
                  FX_ArraySize(kHuffmanTables),
              "kNumHuffmanTables must be equal to the size of kHuffmanTables");

}  // namespace

CJBig2_HuffmanTable::CJBig2_HuffmanTable(size_t idx) {
  ASSERT(idx > 0);
  ASSERT(idx < kNumHuffmanTables);
  const HuffmanTable& table = kHuffmanTables[idx];
  HTOOB = table.HTOOB;
  NTEMP = table.size;
  m_bOK = ParseFromStandardTable(idx);
  ASSERT(m_bOK);
}

CJBig2_HuffmanTable::CJBig2_HuffmanTable(CJBig2_BitStream* pStream)
    : HTOOB(false), NTEMP(0) {
  m_bOK = ParseFromCodedBuffer(pStream);
}

CJBig2_HuffmanTable::~CJBig2_HuffmanTable() {}

bool CJBig2_HuffmanTable::ParseFromStandardTable(size_t idx) {
  const JBig2TableLine* pTable = kHuffmanTables[idx].lines;
  CODES.resize(NTEMP);
  RANGELEN.resize(NTEMP);
  RANGELOW.resize(NTEMP);
  for (uint32_t i = 0; i < NTEMP; ++i) {
    CODES[i].codelen = pTable[i].PREFLEN;
    RANGELEN[i] = pTable[i].RANDELEN;
    RANGELOW[i] = pTable[i].RANGELOW;
  }
  return CJBig2_Context::HuffmanAssignCode(CODES.data(), NTEMP);
}

bool CJBig2_HuffmanTable::ParseFromCodedBuffer(CJBig2_BitStream* pStream) {
  unsigned char cTemp;
  if (pStream->read1Byte(&cTemp) == -1)
    return false;

  HTOOB = !!(cTemp & 0x01);
  unsigned char HTPS = ((cTemp >> 1) & 0x07) + 1;
  unsigned char HTRS = ((cTemp >> 4) & 0x07) + 1;
  uint32_t HTLOW;
  uint32_t HTHIGH;
  if (pStream->readInteger(&HTLOW) == -1 ||
      pStream->readInteger(&HTHIGH) == -1) {
    return false;
  }

  const int low = static_cast<int>(HTLOW);
  const int high = static_cast<int>(HTHIGH);
  if (low > high)
    return false;

  ExtendBuffers(false);
  pdfium::base::CheckedNumeric<int> cur_low = low;
  do {
    if ((pStream->readNBits(HTPS, &CODES[NTEMP].codelen) == -1) ||
        (pStream->readNBits(HTRS, &RANGELEN[NTEMP]) == -1) ||
        (static_cast<size_t>(RANGELEN[NTEMP]) >= 8 * sizeof(cur_low))) {
      return false;
    }
    RANGELOW[NTEMP] = cur_low.ValueOrDie();

    if (RANGELEN[NTEMP] >= 32)
      return false;

    cur_low += (1 << RANGELEN[NTEMP]);
    if (!cur_low.IsValid())
      return false;
    ExtendBuffers(true);
  } while (cur_low.ValueOrDie() < high);

  if (pStream->readNBits(HTPS, &CODES[NTEMP].codelen) == -1)
    return false;

  RANGELEN[NTEMP] = 32;
  if (low == std::numeric_limits<int>::min())
    return false;

  RANGELOW[NTEMP] = low - 1;
  ExtendBuffers(true);

  if (pStream->readNBits(HTPS, &CODES[NTEMP].codelen) == -1)
    return false;

  RANGELEN[NTEMP] = 32;
  RANGELOW[NTEMP] = high;
  ExtendBuffers(true);

  if (HTOOB) {
    if (pStream->readNBits(HTPS, &CODES[NTEMP].codelen) == -1)
      return false;

    ++NTEMP;
  }

  return CJBig2_Context::HuffmanAssignCode(CODES.data(), NTEMP);
}

void CJBig2_HuffmanTable::ExtendBuffers(bool increment) {
  if (increment)
    ++NTEMP;

  size_t size = CODES.size();
  if (NTEMP < size)
    return;

  size += 16;
  ASSERT(NTEMP < size);
  CODES.resize(size);
  RANGELEN.resize(size);
  RANGELOW.resize(size);
}
