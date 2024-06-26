// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_PARSER_H_
#define CORE_FPDFAPI_PARSER_CPDF_PARSER_H_

#include <limits>
#include <map>
#include <memory>
#include <set>
#include <vector>

#include "core/fpdfapi/parser/cpdf_cross_ref_table.h"
#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Array;
class CPDF_CryptoHandler;
class CPDF_Dictionary;
class CPDF_LinearizedHeader;
class CPDF_Object;
class CPDF_ObjectStream;
class CPDF_ReadValidator;
class CPDF_SecurityHandler;
class CPDF_SyntaxParser;
class IFX_SeekableReadStream;

class CPDF_Parser {
 public:
  class ParsedObjectsHolder : public CPDF_IndirectObjectHolder {
   public:
    virtual bool TryInit() = 0;
  };

  enum Error {
    SUCCESS = 0,
    FILE_ERROR,
    FORMAT_ERROR,
    PASSWORD_ERROR,
    HANDLER_ERROR
  };

  // A limit on the maximum object number in the xref table. Theoretical limits
  // are higher, but this may be large enough in practice.
  // Note: This was 1M, but https://crbug.com/910009 encountered a PDF with
  // object numbers in the 1.7M range. The PDF only has 10K objects, but they
  // are non-consecutive.
  static constexpr uint32_t kMaxObjectNumber = 4 * 1024 * 1024;

  static const size_t kInvalidPos = std::numeric_limits<size_t>::max();

  explicit CPDF_Parser(ParsedObjectsHolder* holder);
  CPDF_Parser();
  ~CPDF_Parser();

  Error StartParse(const RetainPtr<IFX_SeekableReadStream>& pFile,
                   const char* password);
  Error StartLinearizedParse(const RetainPtr<CPDF_ReadValidator>& validator,
                             const char* password);

  void SetPassword(const char* password) { m_Password = password; }
  ByteString GetPassword() const { return m_Password; }

  const CPDF_Dictionary* GetTrailer() const;

  // Returns a new trailer which combines the last read trailer with the /Root
  // and /Info from previous ones.
  std::unique_ptr<CPDF_Dictionary> GetCombinedTrailer() const;

  FX_FILESIZE GetLastXRefOffset() const { return m_LastXRefOffset; }

  uint32_t GetPermissions() const;
  uint32_t GetRootObjNum() const;
  uint32_t GetInfoObjNum() const;
  const CPDF_Array* GetIDArray() const;
  CPDF_Dictionary* GetRoot() const;

  const CPDF_Dictionary* GetEncryptDict() const;

  std::unique_ptr<CPDF_Object> ParseIndirectObject(uint32_t objnum);

  uint32_t GetLastObjNum() const;
  bool IsValidObjectNumber(uint32_t objnum) const;
  FX_FILESIZE GetObjectPositionOrZero(uint32_t objnum) const;
  uint16_t GetObjectGenNum(uint32_t objnum) const;
  bool IsObjectFreeOrNull(uint32_t objnum) const;
  CPDF_SecurityHandler* GetSecurityHandler() const {
    return m_pSecurityHandler.get();
  }
  bool IsObjectFree(uint32_t objnum) const;

  int GetFileVersion() const { return m_FileVersion; }
  bool IsXRefStream() const { return m_bXRefStream; }

  std::unique_ptr<CPDF_Object> ParseIndirectObjectAt(
      FX_FILESIZE pos,
      uint32_t objnum);

  uint32_t GetFirstPageNo() const;
  const CPDF_LinearizedHeader* GetLinearizedHeader() const {
    return m_pLinearized.get();
  }

  const CPDF_CrossRefTable* GetCrossRefTable() const {
    return m_CrossRefTable.get();
  }

  bool xref_table_rebuilt() const { return m_bXRefTableRebuilt; }

  CPDF_SyntaxParser* GetSyntax() const { return m_pSyntax.get(); }

  void SetLinearizedHeader(std::unique_ptr<CPDF_LinearizedHeader> pLinearized);

 protected:
  using ObjectType = CPDF_CrossRefTable::ObjectType;
  using ObjectInfo = CPDF_CrossRefTable::ObjectInfo;

  bool LoadCrossRefV4(FX_FILESIZE pos, bool bSkip);
  bool RebuildCrossRef();

  std::unique_ptr<CPDF_SyntaxParser> m_pSyntax;

 private:
  friend class cpdf_parser_BadStartXrefShouldNotBuildCrossRefTable_Test;
  friend class cpdf_parser_ParseStartXRefWithHeaderOffset_Test;
  friend class cpdf_parser_ParseStartXRef_Test;
  friend class cpdf_parser_ParseLinearizedWithHeaderOffset_Test;
  friend class CPDF_DataAvail;

  struct CrossRefObjData {
    uint32_t obj_num = 0;
    ObjectInfo info;
  };

  Error StartParseInternal();
  FX_FILESIZE ParseStartXRef();
  bool LoadAllCrossRefV4(FX_FILESIZE pos);
  bool LoadAllCrossRefV5(FX_FILESIZE pos);
  bool LoadCrossRefV5(FX_FILESIZE* pos, bool bMainXRef);
  std::unique_ptr<CPDF_Dictionary> LoadTrailerV4();
  Error SetEncryptHandler();
  void ReleaseEncryptHandler();
  bool LoadLinearizedAllCrossRefV4(FX_FILESIZE pos);
  bool LoadLinearizedAllCrossRefV5(FX_FILESIZE pos);
  Error LoadLinearizedMainXRefTable();
  const CPDF_ObjectStream* GetObjectStream(uint32_t object_number);
  std::unique_ptr<CPDF_LinearizedHeader> ParseLinearizedHeader();
  void ShrinkObjectMap(uint32_t size);
  // A simple check whether the cross reference table matches with
  // the objects.
  bool VerifyCrossRefV4();

  // If out_objects is null, the parser position will be moved to end subsection
  // without additional validation.
  bool ParseAndAppendCrossRefSubsectionData(
      uint32_t start_objnum,
      uint32_t count,
      std::vector<CrossRefObjData>* out_objects);
  bool ParseCrossRefV4(std::vector<CrossRefObjData>* out_objects);
  void MergeCrossRefObjectsData(const std::vector<CrossRefObjData>& objects);

  bool InitSyntaxParser(const RetainPtr<CPDF_ReadValidator>& validator);
  bool ParseFileVersion();

  ObjectType GetObjectType(uint32_t objnum) const;
  ObjectType GetObjectTypeFromCrossRefStreamType(
      uint32_t cross_ref_stream_type) const;

  std::unique_ptr<ParsedObjectsHolder> m_pOwnedObjectsHolder;
  UnownedPtr<ParsedObjectsHolder> m_pObjectsHolder;

  bool m_bHasParsed = false;
  bool m_bXRefStream = false;
  bool m_bXRefTableRebuilt = false;
  int m_FileVersion = 0;
  // m_CrossRefTable must be destroyed after m_pSecurityHandler due to the
  // ownership of the ID array data.
  std::unique_ptr<CPDF_CrossRefTable> m_CrossRefTable;
  FX_FILESIZE m_LastXRefOffset;
  std::unique_ptr<CPDF_SecurityHandler> m_pSecurityHandler;
  ByteString m_Password;
  std::unique_ptr<CPDF_LinearizedHeader> m_pLinearized;

  // A map of object numbers to indirect streams.
  std::map<uint32_t, std::unique_ptr<CPDF_ObjectStream>> m_ObjectStreamMap;

  // All indirect object numbers that are being parsed.
  std::set<uint32_t> m_ParsingObjNums;

  uint32_t m_MetadataObjnum = 0;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_PARSER_H_
