// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CFX_CTTGSUBTABLE_H_
#define CORE_FPDFAPI_FONT_CFX_CTTGSUBTABLE_H_

#include <stdint.h>

#include <memory>
#include <set>
#include <vector>

#include "core/fxge/fx_freetype.h"

class CFX_CTTGSUBTable {
 public:
  explicit CFX_CTTGSUBTable(FT_Bytes gsub);
  ~CFX_CTTGSUBTable();

  uint32_t GetVerticalGlyph(uint32_t glyphnum) const;

 private:
  struct TLangSysRecord {
    TLangSysRecord();
    ~TLangSysRecord();

    uint32_t LangSysTag;
    uint16_t LookupOrder;
    uint16_t ReqFeatureIndex;
    std::vector<uint16_t> FeatureIndices;
  };

  struct TScriptRecord {
    TScriptRecord();
    ~TScriptRecord();

    uint32_t ScriptTag;
    uint16_t DefaultLangSys;
    std::vector<TLangSysRecord> LangSysRecords;
  };

  struct TFeatureRecord {
    TFeatureRecord();
    ~TFeatureRecord();

    uint32_t FeatureTag;
    uint16_t FeatureParams;
    std::vector<uint16_t> LookupListIndices;
  };

  struct TRangeRecord {
    TRangeRecord();

    uint16_t Start;
    uint16_t End;
    uint16_t StartCoverageIndex;
  };

  struct TCoverageFormatBase {
    virtual ~TCoverageFormatBase() = default;
    uint16_t CoverageFormat;
  };

  struct TCoverageFormat1 final : public TCoverageFormatBase {
    TCoverageFormat1();
    ~TCoverageFormat1() override;

    std::vector<uint16_t> GlyphArray;
  };

  struct TCoverageFormat2 final : public TCoverageFormatBase {
    TCoverageFormat2();
    ~TCoverageFormat2() override;

    std::vector<TRangeRecord> RangeRecords;
  };

  struct TDevice {
    TDevice() : StartSize(0), EndSize(0), DeltaFormat(0) {}

    uint16_t StartSize;
    uint16_t EndSize;
    uint16_t DeltaFormat;
  };

  struct TSubTableBase {
    TSubTableBase();
    virtual ~TSubTableBase();

    std::unique_ptr<TCoverageFormatBase> Coverage;
    uint16_t SubstFormat;
  };

  struct TSubTable1 final : public TSubTableBase {
    TSubTable1();
    ~TSubTable1() override;

    int16_t DeltaGlyphID;
  };

  struct TSubTable2 final : public TSubTableBase {
    TSubTable2();
    ~TSubTable2() override;

    std::vector<uint16_t> Substitutes;
  };

  struct TLookup {
    TLookup();
    ~TLookup();

    uint16_t LookupType;
    uint16_t LookupFlag;
    std::vector<std::unique_ptr<TSubTableBase>> SubTables;
  };

  bool LoadGSUBTable(FT_Bytes gsub);
  bool Parse(FT_Bytes scriptlist, FT_Bytes featurelist, FT_Bytes lookuplist);
  void ParseScriptList(FT_Bytes raw);
  void ParseScript(FT_Bytes raw, TScriptRecord* rec);
  void ParseLangSys(FT_Bytes raw, TLangSysRecord* rec);
  void ParseFeatureList(FT_Bytes raw);
  void ParseFeature(FT_Bytes raw, TFeatureRecord* rec);
  void ParseLookupList(FT_Bytes raw);
  void ParseLookup(FT_Bytes raw, TLookup* rec);
  std::unique_ptr<TCoverageFormatBase> ParseCoverage(FT_Bytes raw);
  void ParseCoverageFormat1(FT_Bytes raw, TCoverageFormat1* rec);
  void ParseCoverageFormat2(FT_Bytes raw, TCoverageFormat2* rec);
  void ParseSingleSubst(FT_Bytes raw, std::unique_ptr<TSubTableBase>* rec);
  void ParseSingleSubstFormat1(FT_Bytes raw, TSubTable1* rec);
  void ParseSingleSubstFormat2(FT_Bytes raw, TSubTable2* rec);

  bool GetVerticalGlyphSub(const TFeatureRecord& feature,
                           uint32_t glyphnum,
                           uint32_t* vglyphnum) const;
  bool GetVerticalGlyphSub2(const TLookup& lookup,
                            uint32_t glyphnum,
                            uint32_t* vglyphnum) const;
  int GetCoverageIndex(TCoverageFormatBase* Coverage, uint32_t g) const;

  uint8_t GetUInt8(FT_Bytes& p) const;
  int16_t GetInt16(FT_Bytes& p) const;
  uint16_t GetUInt16(FT_Bytes& p) const;
  int32_t GetInt32(FT_Bytes& p) const;
  uint32_t GetUInt32(FT_Bytes& p) const;

  std::set<uint32_t> m_featureSet;
  std::vector<TScriptRecord> ScriptList;
  std::vector<TFeatureRecord> FeatureList;
  std::vector<TLookup> LookupList;
};

#endif  // CORE_FPDFAPI_FONT_CFX_CTTGSUBTABLE_H_
