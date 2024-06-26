// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cfx_cttgsubtable.h"

#include <utility>

#include "core/fxge/fx_freetype.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

constexpr uint32_t MakeTag(char c1, char c2, char c3, char c4) {
  return static_cast<uint8_t>(c1) << 24 | static_cast<uint8_t>(c2) << 16 |
         static_cast<uint8_t>(c3) << 8 | static_cast<uint8_t>(c4);
}

bool IsVerticalFeatureTag(uint32_t tag) {
  static constexpr uint32_t kTags[] = {
      MakeTag('v', 'r', 't', '2'), MakeTag('v', 'e', 'r', 't'),
  };
  return tag == kTags[0] || tag == kTags[1];
}

}  // namespace

CFX_CTTGSUBTable::CFX_CTTGSUBTable(FT_Bytes gsub) {
  if (!LoadGSUBTable(gsub))
    return;

  for (const TScriptRecord& script : ScriptList) {
    for (const auto& record : script.LangSysRecords) {
      for (uint16_t index : record.FeatureIndices) {
        if (IsVerticalFeatureTag(FeatureList[index].FeatureTag))
          m_featureSet.insert(index);
      }
    }
  }
  if (!m_featureSet.empty())
    return;

  int i = 0;
  for (const TFeatureRecord& feature : FeatureList) {
    if (IsVerticalFeatureTag(feature.FeatureTag))
      m_featureSet.insert(i);
    ++i;
  }
}

CFX_CTTGSUBTable::~CFX_CTTGSUBTable() = default;

bool CFX_CTTGSUBTable::LoadGSUBTable(FT_Bytes gsub) {
  if ((gsub[0] << 24u | gsub[1] << 16u | gsub[2] << 8u | gsub[3]) != 0x00010000)
    return false;

  return Parse(&gsub[gsub[4] << 8 | gsub[5]], &gsub[gsub[6] << 8 | gsub[7]],
               &gsub[gsub[8] << 8 | gsub[9]]);
}

uint32_t CFX_CTTGSUBTable::GetVerticalGlyph(uint32_t glyphnum) const {
  uint32_t vglyphnum = 0;
  for (uint32_t item : m_featureSet) {
    if (GetVerticalGlyphSub(FeatureList[item], glyphnum, &vglyphnum))
      break;
  }
  return vglyphnum;
}

bool CFX_CTTGSUBTable::GetVerticalGlyphSub(const TFeatureRecord& feature,
                                           uint32_t glyphnum,
                                           uint32_t* vglyphnum) const {
  for (int index : feature.LookupListIndices) {
    if (!pdfium::IndexInBounds(LookupList, index))
      continue;
    if (LookupList[index].LookupType == 1 &&
        GetVerticalGlyphSub2(LookupList[index], glyphnum, vglyphnum)) {
      return true;
    }
  }
  return false;
}

bool CFX_CTTGSUBTable::GetVerticalGlyphSub2(const TLookup& lookup,
                                            uint32_t glyphnum,
                                            uint32_t* vglyphnum) const {
  for (const auto& subTable : lookup.SubTables) {
    switch (subTable->SubstFormat) {
      case 1: {
        auto* tbl1 = static_cast<TSubTable1*>(subTable.get());
        if (GetCoverageIndex(tbl1->Coverage.get(), glyphnum) >= 0) {
          *vglyphnum = glyphnum + tbl1->DeltaGlyphID;
          return true;
        }
        break;
      }
      case 2: {
        auto* tbl2 = static_cast<TSubTable2*>(subTable.get());
        int index = GetCoverageIndex(tbl2->Coverage.get(), glyphnum);
        if (pdfium::IndexInBounds(tbl2->Substitutes, index)) {
          *vglyphnum = tbl2->Substitutes[index];
          return true;
        }
        break;
      }
    }
  }
  return false;
}

int CFX_CTTGSUBTable::GetCoverageIndex(TCoverageFormatBase* Coverage,
                                       uint32_t g) const {
  if (!Coverage)
    return -1;

  switch (Coverage->CoverageFormat) {
    case 1: {
      int i = 0;
      TCoverageFormat1* c1 = static_cast<TCoverageFormat1*>(Coverage);
      for (const auto& glyph : c1->GlyphArray) {
        if (static_cast<uint32_t>(glyph) == g)
          return i;
        ++i;
      }
      return -1;
    }
    case 2: {
      TCoverageFormat2* c2 = static_cast<TCoverageFormat2*>(Coverage);
      for (const auto& rangeRec : c2->RangeRecords) {
        uint32_t s = rangeRec.Start;
        uint32_t e = rangeRec.End;
        uint32_t si = rangeRec.StartCoverageIndex;
        if (s <= g && g <= e)
          return si + g - s;
      }
      return -1;
    }
  }
  return -1;
}

uint8_t CFX_CTTGSUBTable::GetUInt8(FT_Bytes& p) const {
  uint8_t ret = p[0];
  p += 1;
  return ret;
}

int16_t CFX_CTTGSUBTable::GetInt16(FT_Bytes& p) const {
  uint16_t ret = p[0] << 8 | p[1];
  p += 2;
  return *(int16_t*)&ret;
}

uint16_t CFX_CTTGSUBTable::GetUInt16(FT_Bytes& p) const {
  uint16_t ret = p[0] << 8 | p[1];
  p += 2;
  return ret;
}

int32_t CFX_CTTGSUBTable::GetInt32(FT_Bytes& p) const {
  uint32_t ret = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
  p += 4;
  return *(int32_t*)&ret;
}

uint32_t CFX_CTTGSUBTable::GetUInt32(FT_Bytes& p) const {
  uint32_t ret = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
  p += 4;
  return ret;
}

bool CFX_CTTGSUBTable::Parse(FT_Bytes scriptlist,
                             FT_Bytes featurelist,
                             FT_Bytes lookuplist) {
  ParseScriptList(scriptlist);
  ParseFeatureList(featurelist);
  ParseLookupList(lookuplist);
  return true;
}

void CFX_CTTGSUBTable::ParseScriptList(FT_Bytes raw) {
  FT_Bytes sp = raw;
  ScriptList = std::vector<TScriptRecord>(GetUInt16(sp));
  for (auto& scriptRec : ScriptList) {
    scriptRec.ScriptTag = GetUInt32(sp);
    ParseScript(&raw[GetUInt16(sp)], &scriptRec);
  }
}

void CFX_CTTGSUBTable::ParseScript(FT_Bytes raw, TScriptRecord* rec) {
  FT_Bytes sp = raw;
  rec->DefaultLangSys = GetUInt16(sp);
  rec->LangSysRecords = std::vector<TLangSysRecord>(GetUInt16(sp));
  for (auto& sysRecord : rec->LangSysRecords) {
    sysRecord.LangSysTag = GetUInt32(sp);
    ParseLangSys(&raw[GetUInt16(sp)], &sysRecord);
  }
}

void CFX_CTTGSUBTable::ParseLangSys(FT_Bytes raw, TLangSysRecord* rec) {
  FT_Bytes sp = raw;
  rec->LookupOrder = GetUInt16(sp);
  rec->ReqFeatureIndex = GetUInt16(sp);
  rec->FeatureIndices = std::vector<uint16_t>(GetUInt16(sp));
  for (auto& element : rec->FeatureIndices)
    element = GetUInt16(sp);
}

void CFX_CTTGSUBTable::ParseFeatureList(FT_Bytes raw) {
  FT_Bytes sp = raw;
  FeatureList = std::vector<TFeatureRecord>(GetUInt16(sp));
  for (auto& featureRec : FeatureList) {
    featureRec.FeatureTag = GetUInt32(sp);
    ParseFeature(&raw[GetUInt16(sp)], &featureRec);
  }
}

void CFX_CTTGSUBTable::ParseFeature(FT_Bytes raw, TFeatureRecord* rec) {
  FT_Bytes sp = raw;
  rec->FeatureParams = GetUInt16(sp);
  rec->LookupListIndices = std::vector<uint16_t>(GetUInt16(sp));
  for (auto& listIndex : rec->LookupListIndices)
    listIndex = GetUInt16(sp);
}

void CFX_CTTGSUBTable::ParseLookupList(FT_Bytes raw) {
  FT_Bytes sp = raw;
  LookupList = std::vector<TLookup>(GetUInt16(sp));
  for (auto& lookup : LookupList)
    ParseLookup(&raw[GetUInt16(sp)], &lookup);
}

void CFX_CTTGSUBTable::ParseLookup(FT_Bytes raw, TLookup* rec) {
  FT_Bytes sp = raw;
  rec->LookupType = GetUInt16(sp);
  rec->LookupFlag = GetUInt16(sp);
  rec->SubTables = std::vector<std::unique_ptr<TSubTableBase>>(GetUInt16(sp));
  if (rec->LookupType != 1)
    return;

  for (auto& subTable : rec->SubTables)
    ParseSingleSubst(&raw[GetUInt16(sp)], &subTable);
}

std::unique_ptr<CFX_CTTGSUBTable::TCoverageFormatBase>
CFX_CTTGSUBTable::ParseCoverage(FT_Bytes raw) {
  FT_Bytes sp = raw;
  uint16_t format = GetUInt16(sp);
  if (format == 1) {
    auto rec = pdfium::MakeUnique<TCoverageFormat1>();
    ParseCoverageFormat1(raw, rec.get());
    return std::move(rec);
  }
  if (format == 2) {
    auto rec = pdfium::MakeUnique<TCoverageFormat2>();
    ParseCoverageFormat2(raw, rec.get());
    return std::move(rec);
  }
  return nullptr;
}

void CFX_CTTGSUBTable::ParseCoverageFormat1(FT_Bytes raw,
                                            TCoverageFormat1* rec) {
  FT_Bytes sp = raw;
  (void)GetUInt16(sp);
  rec->GlyphArray = std::vector<uint16_t>(GetUInt16(sp));
  for (auto& glyph : rec->GlyphArray)
    glyph = GetUInt16(sp);
}

void CFX_CTTGSUBTable::ParseCoverageFormat2(FT_Bytes raw,
                                            TCoverageFormat2* rec) {
  FT_Bytes sp = raw;
  (void)GetUInt16(sp);
  rec->RangeRecords = std::vector<TRangeRecord>(GetUInt16(sp));
  for (auto& rangeRec : rec->RangeRecords) {
    rangeRec.Start = GetUInt16(sp);
    rangeRec.End = GetUInt16(sp);
    rangeRec.StartCoverageIndex = GetUInt16(sp);
  }
}

void CFX_CTTGSUBTable::ParseSingleSubst(FT_Bytes raw,
                                        std::unique_ptr<TSubTableBase>* rec) {
  FT_Bytes sp = raw;
  uint16_t Format = GetUInt16(sp);
  switch (Format) {
    case 1:
      *rec = pdfium::MakeUnique<TSubTable1>();
      ParseSingleSubstFormat1(raw, static_cast<TSubTable1*>(rec->get()));
      break;
    case 2:
      *rec = pdfium::MakeUnique<TSubTable2>();
      ParseSingleSubstFormat2(raw, static_cast<TSubTable2*>(rec->get()));
      break;
  }
}

void CFX_CTTGSUBTable::ParseSingleSubstFormat1(FT_Bytes raw, TSubTable1* rec) {
  FT_Bytes sp = raw;
  GetUInt16(sp);
  uint16_t offset = GetUInt16(sp);
  rec->Coverage = ParseCoverage(&raw[offset]);
  rec->DeltaGlyphID = GetInt16(sp);
}

void CFX_CTTGSUBTable::ParseSingleSubstFormat2(FT_Bytes raw, TSubTable2* rec) {
  FT_Bytes sp = raw;
  (void)GetUInt16(sp);
  uint16_t offset = GetUInt16(sp);
  rec->Coverage = ParseCoverage(&raw[offset]);
  rec->Substitutes = std::vector<uint16_t>(GetUInt16(sp));
  for (auto& substitute : rec->Substitutes)
    substitute = GetUInt16(sp);
}

CFX_CTTGSUBTable::TLangSysRecord::TLangSysRecord()
    : LangSysTag(0), LookupOrder(0), ReqFeatureIndex(0) {}

CFX_CTTGSUBTable::TLangSysRecord::~TLangSysRecord() {}

CFX_CTTGSUBTable::TScriptRecord::TScriptRecord()
    : ScriptTag(0), DefaultLangSys(0) {}

CFX_CTTGSUBTable::TScriptRecord::~TScriptRecord() {}

CFX_CTTGSUBTable::TFeatureRecord::TFeatureRecord()
    : FeatureTag(0), FeatureParams(0) {}

CFX_CTTGSUBTable::TFeatureRecord::~TFeatureRecord() {}

CFX_CTTGSUBTable::TRangeRecord::TRangeRecord()
    : Start(0), End(0), StartCoverageIndex(0) {}

CFX_CTTGSUBTable::TCoverageFormat1::TCoverageFormat1() {
  CoverageFormat = 1;
}

CFX_CTTGSUBTable::TCoverageFormat1::~TCoverageFormat1() {}

CFX_CTTGSUBTable::TCoverageFormat2::TCoverageFormat2() {
  CoverageFormat = 2;
}

CFX_CTTGSUBTable::TCoverageFormat2::~TCoverageFormat2() {}

CFX_CTTGSUBTable::TSubTableBase::TSubTableBase() {}

CFX_CTTGSUBTable::TSubTableBase::~TSubTableBase() {}

CFX_CTTGSUBTable::TSubTable1::TSubTable1() {
  SubstFormat = 1;
}

CFX_CTTGSUBTable::TSubTable1::~TSubTable1() {}

CFX_CTTGSUBTable::TSubTable2::TSubTable2() {
  SubstFormat = 2;
}

CFX_CTTGSUBTable::TSubTable2::~TSubTable2() {}

CFX_CTTGSUBTable::TLookup::TLookup() : LookupType(0), LookupFlag(0) {}

CFX_CTTGSUBTable::TLookup::~TLookup() {}
