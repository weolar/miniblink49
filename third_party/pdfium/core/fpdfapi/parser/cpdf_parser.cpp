// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_parser.h"

#include <algorithm>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_crypto_handler.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_linearized_header.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_object_stream.h"
#include "core/fpdfapi/parser/cpdf_read_validator.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_security_handler.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxcrt/autorestorer.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_safe_types.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

namespace {

// A limit on the size of the xref table. Theoretical limits are higher, but
// this may be large enough in practice.
const int32_t kMaxXRefSize = 1048576;

// "%PDF-1.7\n"
constexpr FX_FILESIZE kPDFHeaderSize = 9;

uint32_t GetVarInt(const uint8_t* p, int32_t n) {
  uint32_t result = 0;
  for (int32_t i = 0; i < n; ++i)
    result = result * 256 + p[i];
  return result;
}

class ObjectsHolderStub final : public CPDF_Parser::ParsedObjectsHolder {
 public:
  ObjectsHolderStub() = default;
  ~ObjectsHolderStub() override = default;
  bool TryInit() override { return true; }
};

}  // namespace

CPDF_Parser::CPDF_Parser(ParsedObjectsHolder* holder)
    : m_pObjectsHolder(holder),
      m_CrossRefTable(pdfium::MakeUnique<CPDF_CrossRefTable>()) {
  if (!holder) {
    m_pOwnedObjectsHolder = pdfium::MakeUnique<ObjectsHolderStub>();
    m_pObjectsHolder = m_pOwnedObjectsHolder.get();
  }
}

CPDF_Parser::CPDF_Parser() : CPDF_Parser(nullptr) {}

CPDF_Parser::~CPDF_Parser() {
  ReleaseEncryptHandler();
}

uint32_t CPDF_Parser::GetLastObjNum() const {
  return m_CrossRefTable->objects_info().empty()
             ? 0
             : m_CrossRefTable->objects_info().rbegin()->first;
}

bool CPDF_Parser::IsValidObjectNumber(uint32_t objnum) const {
  return objnum <= GetLastObjNum();
}

FX_FILESIZE CPDF_Parser::GetObjectPositionOrZero(uint32_t objnum) const {
  const auto* info = m_CrossRefTable->GetObjectInfo(objnum);
  return (info && info->type == ObjectType::kNormal) ? info->pos : 0;
}

CPDF_Parser::ObjectType CPDF_Parser::GetObjectType(uint32_t objnum) const {
  ASSERT(IsValidObjectNumber(objnum));
  const auto* info = m_CrossRefTable->GetObjectInfo(objnum);
  return info ? info->type : ObjectType::kFree;
}

uint16_t CPDF_Parser::GetObjectGenNum(uint32_t objnum) const {
  ASSERT(IsValidObjectNumber(objnum));
  const auto* info = m_CrossRefTable->GetObjectInfo(objnum);
  return (info && info->type == ObjectType::kNormal) ? info->gennum : 0;
}

bool CPDF_Parser::IsObjectFreeOrNull(uint32_t objnum) const {
  switch (GetObjectType(objnum)) {
    case ObjectType::kFree:
    case ObjectType::kNull:
      return true;
    case ObjectType::kNotCompressed:
    case ObjectType::kCompressed:
      return false;
  }
  NOTREACHED();
  return false;
}

bool CPDF_Parser::IsObjectFree(uint32_t objnum) const {
  return GetObjectType(objnum) == ObjectType::kFree;
}

void CPDF_Parser::ShrinkObjectMap(uint32_t objnum) {
  m_CrossRefTable->ShrinkObjectMap(objnum);
}

bool CPDF_Parser::InitSyntaxParser(
    const RetainPtr<CPDF_ReadValidator>& validator) {
  const Optional<FX_FILESIZE> header_offset = GetHeaderOffset(validator);
  if (!header_offset)
    return false;
  if (validator->GetSize() < *header_offset + kPDFHeaderSize)
    return false;

  m_pSyntax = pdfium::MakeUnique<CPDF_SyntaxParser>(validator, *header_offset);
  return ParseFileVersion();
}

bool CPDF_Parser::ParseFileVersion() {
  m_FileVersion = 0;
  uint8_t ch;
  if (!m_pSyntax->GetCharAt(5, ch))
    return false;

  if (std::isdigit(ch))
    m_FileVersion = FXSYS_DecimalCharToInt(static_cast<wchar_t>(ch)) * 10;

  if (!m_pSyntax->GetCharAt(7, ch))
    return false;

  if (std::isdigit(ch))
    m_FileVersion += FXSYS_DecimalCharToInt(static_cast<wchar_t>(ch));
  return true;
}

CPDF_Parser::Error CPDF_Parser::StartParse(
    const RetainPtr<IFX_SeekableReadStream>& pFileAccess,
    const char* password) {
  if (!InitSyntaxParser(
          pdfium::MakeRetain<CPDF_ReadValidator>(pFileAccess, nullptr)))
    return FORMAT_ERROR;
  SetPassword(password);
  return StartParseInternal();
}

CPDF_Parser::Error CPDF_Parser::StartParseInternal() {
  ASSERT(!m_bHasParsed);
  ASSERT(!m_bXRefTableRebuilt);
  m_bHasParsed = true;
  m_bXRefStream = false;

  m_LastXRefOffset = ParseStartXRef();
  if (m_LastXRefOffset >= kPDFHeaderSize) {
    if (!LoadAllCrossRefV4(m_LastXRefOffset) &&
        !LoadAllCrossRefV5(m_LastXRefOffset)) {
      if (!RebuildCrossRef())
        return FORMAT_ERROR;

      m_bXRefTableRebuilt = true;
      m_LastXRefOffset = 0;
    }
  } else {
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    m_bXRefTableRebuilt = true;
  }
  Error eRet = SetEncryptHandler();
  if (eRet != SUCCESS)
    return eRet;

  if (!GetRoot() || !m_pObjectsHolder->TryInit()) {
    if (m_bXRefTableRebuilt)
      return FORMAT_ERROR;

    ReleaseEncryptHandler();
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;

    m_pObjectsHolder->TryInit();
    if (!GetRoot())
      return FORMAT_ERROR;
  }
  if (GetRootObjNum() == CPDF_Object::kInvalidObjNum) {
    ReleaseEncryptHandler();
    if (!RebuildCrossRef() || GetRootObjNum() == CPDF_Object::kInvalidObjNum)
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;
  }
  if (m_pSecurityHandler && !m_pSecurityHandler->IsMetadataEncrypted()) {
    CPDF_Reference* pMetadata =
        ToReference(GetRoot()->GetObjectFor("Metadata"));
    if (pMetadata)
      m_MetadataObjnum = pMetadata->GetRefObjNum();
  }
  return SUCCESS;
}

FX_FILESIZE CPDF_Parser::ParseStartXRef() {
  static constexpr char kStartXRefKeyword[] = "startxref";
  m_pSyntax->SetPos(m_pSyntax->GetDocumentSize() - strlen(kStartXRefKeyword));
  if (!m_pSyntax->BackwardsSearchToWord(kStartXRefKeyword, 4096))
    return 0;

  // Skip "startxref" keyword.
  m_pSyntax->GetKeyword();

  // Read XRef offset.
  bool bNumber;
  const ByteString xrefpos_str = m_pSyntax->GetNextWord(&bNumber);
  if (!bNumber || xrefpos_str.IsEmpty())
    return 0;

  const FX_SAFE_FILESIZE result = FXSYS_atoi64(xrefpos_str.c_str());
  if (!result.IsValid() || result.ValueOrDie() >= m_pSyntax->GetDocumentSize())
    return 0;

  return result.ValueOrDie();
}

CPDF_Parser::Error CPDF_Parser::SetEncryptHandler() {
  ReleaseEncryptHandler();
  if (!GetTrailer())
    return FORMAT_ERROR;

  const CPDF_Dictionary* pEncryptDict = GetEncryptDict();
  if (!pEncryptDict)
    return SUCCESS;

  if (pEncryptDict->GetStringFor("Filter") != "Standard")
    return HANDLER_ERROR;

  std::unique_ptr<CPDF_SecurityHandler> pSecurityHandler =
      pdfium::MakeUnique<CPDF_SecurityHandler>();
  if (!pSecurityHandler->OnInit(pEncryptDict, GetIDArray(), m_Password))
    return PASSWORD_ERROR;

  m_pSecurityHandler = std::move(pSecurityHandler);
  return SUCCESS;
}

void CPDF_Parser::ReleaseEncryptHandler() {
  m_pSecurityHandler.reset();
}

// Ideally, all the cross reference entries should be verified.
// In reality, we rarely see well-formed cross references don't match
// with the objects. crbug/602650 showed a case where object numbers
// in the cross reference table are all off by one.
bool CPDF_Parser::VerifyCrossRefV4() {
  for (const auto& it : m_CrossRefTable->objects_info()) {
    if (it.second.pos == 0)
      continue;
    // Find the first non-zero position.
    FX_FILESIZE SavedPos = m_pSyntax->GetPos();
    m_pSyntax->SetPos(it.second.pos);
    bool is_num = false;
    ByteString num_str = m_pSyntax->GetNextWord(&is_num);
    m_pSyntax->SetPos(SavedPos);
    if (!is_num || num_str.IsEmpty() ||
        FXSYS_atoui(num_str.c_str()) != it.first) {
      // If the object number read doesn't match the one stored,
      // something is wrong with the cross reference table.
      return false;
    }
    break;
  }
  return true;
}

bool CPDF_Parser::LoadAllCrossRefV4(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV4(xrefpos, true))
    return false;

  std::unique_ptr<CPDF_Dictionary> trailer = LoadTrailerV4();
  if (!trailer)
    return false;

  m_CrossRefTable->SetTrailer(std::move(trailer));
  int32_t xrefsize = GetDirectInteger(GetTrailer(), "Size");
  if (xrefsize > 0 && xrefsize <= kMaxXRefSize)
    ShrinkObjectMap(xrefsize);

  std::vector<FX_FILESIZE> CrossRefList;
  std::vector<FX_FILESIZE> XRefStreamList;
  std::set<FX_FILESIZE> seen_xrefpos;

  CrossRefList.push_back(xrefpos);
  XRefStreamList.push_back(GetDirectInteger(GetTrailer(), "XRefStm"));
  seen_xrefpos.insert(xrefpos);

  // When the trailer doesn't have Prev entry or Prev entry value is not
  // numerical, GetDirectInteger() returns 0. Loading will end.
  xrefpos = GetDirectInteger(GetTrailer(), "Prev");
  while (xrefpos) {
    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return false;

    seen_xrefpos.insert(xrefpos);

    // SLOW ...
    CrossRefList.insert(CrossRefList.begin(), xrefpos);
    LoadCrossRefV4(xrefpos, true);

    std::unique_ptr<CPDF_Dictionary> pDict(LoadTrailerV4());
    if (!pDict)
      return false;

    xrefpos = GetDirectInteger(pDict.get(), "Prev");

    // SLOW ...
    XRefStreamList.insert(XRefStreamList.begin(),
                          pDict->GetIntegerFor("XRefStm"));

    m_CrossRefTable = CPDF_CrossRefTable::MergeUp(
        pdfium::MakeUnique<CPDF_CrossRefTable>(std::move(pDict)),
        std::move(m_CrossRefTable));
  }

  for (size_t i = 0; i < CrossRefList.size(); ++i) {
    if (!LoadCrossRefV4(CrossRefList[i], false))
      return false;

    if (XRefStreamList[i] && !LoadCrossRefV5(&XRefStreamList[i], false))
      return false;

    if (i == 0 && !VerifyCrossRefV4())
      return false;
  }
  return true;
}

bool CPDF_Parser::LoadLinearizedAllCrossRefV4(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV4(xrefpos, false))
    return false;

  std::unique_ptr<CPDF_Dictionary> trailer = LoadTrailerV4();
  if (!trailer)
    return false;

  m_CrossRefTable = CPDF_CrossRefTable::MergeUp(
      pdfium::MakeUnique<CPDF_CrossRefTable>(std::move(trailer)),
      std::move(m_CrossRefTable));

  int32_t xrefsize = GetDirectInteger(GetTrailer(), "Size");
  if (xrefsize == 0)
    return false;

  std::vector<FX_FILESIZE> CrossRefList;
  std::vector<FX_FILESIZE> XRefStreamList;
  std::set<FX_FILESIZE> seen_xrefpos;

  CrossRefList.push_back(xrefpos);
  XRefStreamList.push_back(GetDirectInteger(GetTrailer(), "XRefStm"));
  seen_xrefpos.insert(xrefpos);

  xrefpos = GetDirectInteger(GetTrailer(), "Prev");
  while (xrefpos) {
    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return false;

    seen_xrefpos.insert(xrefpos);

    // SLOW ...
    CrossRefList.insert(CrossRefList.begin(), xrefpos);
    LoadCrossRefV4(xrefpos, true);

    std::unique_ptr<CPDF_Dictionary> pDict(LoadTrailerV4());
    if (!pDict)
      return false;

    xrefpos = GetDirectInteger(pDict.get(), "Prev");

    // SLOW ...
    XRefStreamList.insert(XRefStreamList.begin(),
                          pDict->GetIntegerFor("XRefStm"));

    m_CrossRefTable = CPDF_CrossRefTable::MergeUp(
        pdfium::MakeUnique<CPDF_CrossRefTable>(std::move(pDict)),
        std::move(m_CrossRefTable));
  }

  for (size_t i = 1; i < CrossRefList.size(); ++i) {
    if (!LoadCrossRefV4(CrossRefList[i], false))
      return false;

    if (XRefStreamList[i] && !LoadCrossRefV5(&XRefStreamList[i], false))
      return false;
  }
  return true;
}

bool CPDF_Parser::ParseAndAppendCrossRefSubsectionData(
    uint32_t start_objnum,
    uint32_t count,
    std::vector<CrossRefObjData>* out_objects) {
  if (!count)
    return true;

  // Each entry shall be exactly 20 byte.
  // A sample entry looks like:
  // "0000000000 00007 f\r\n"
  static constexpr int32_t kEntryConstSize = 20;

  if (!out_objects) {
    FX_SAFE_FILESIZE pos = count;
    pos *= kEntryConstSize;
    pos += m_pSyntax->GetPos();
    if (!pos.IsValid())
      return false;
    m_pSyntax->SetPos(pos.ValueOrDie());
    return true;
  }
  const size_t start_obj_index = out_objects->size();
  FX_SAFE_SIZE_T new_size = start_obj_index;
  new_size += count;
  if (!new_size.IsValid())
    return false;

  if (new_size.ValueOrDie() > kMaxXRefSize)
    return false;

  const size_t max_entries_in_file =
      m_pSyntax->GetDocumentSize() / kEntryConstSize;
  if (new_size.ValueOrDie() > max_entries_in_file)
    return false;

  out_objects->resize(new_size.ValueOrDie());

  std::vector<char> buf(1024 * kEntryConstSize + 1);
  buf.back() = '\0';

  int32_t nBlocks = count / 1024 + 1;
  for (int32_t block = 0; block < nBlocks; block++) {
    int32_t block_size = block == nBlocks - 1 ? count % 1024 : 1024;
    if (!m_pSyntax->ReadBlock(reinterpret_cast<uint8_t*>(buf.data()),
                              block_size * kEntryConstSize)) {
      return false;
    }

    for (int32_t i = 0; i < block_size; i++) {
      CrossRefObjData& obj_data =
          (*out_objects)[start_obj_index + block * 1024 + i];

      const uint32_t objnum = start_objnum + block * 1024 + i;

      obj_data.obj_num = objnum;

      ObjectInfo& info = obj_data.info;

      char* pEntry = &buf[i * kEntryConstSize];
      if (pEntry[17] == 'f') {
        info.pos = 0;
        info.type = ObjectType::kFree;
      } else {
        const FX_SAFE_FILESIZE offset = FXSYS_atoi64(pEntry);
        if (!offset.IsValid())
          return false;

        if (offset.ValueOrDie() == 0) {
          for (int32_t c = 0; c < 10; c++) {
            if (!std::isdigit(pEntry[c]))
              return false;
          }
        }

        info.pos = offset.ValueOrDie();

        // TODO(art-snake): The info.gennum is uint16_t, but version may be
        // greated than max<uint16_t>. Needs solve this issue.
        const int32_t version = FXSYS_atoi(pEntry + 11);
        info.gennum = version;
        info.type = ObjectType::kNotCompressed;
      }
    }
  }
  return true;
}

bool CPDF_Parser::ParseCrossRefV4(std::vector<CrossRefObjData>* out_objects) {
  if (out_objects)
    out_objects->clear();

  if (m_pSyntax->GetKeyword() != "xref")
    return false;
  std::vector<CrossRefObjData> result_objects;
  while (1) {
    FX_FILESIZE saved_pos = m_pSyntax->GetPos();
    bool bIsNumber;
    ByteString word = m_pSyntax->GetNextWord(&bIsNumber);
    if (word.IsEmpty())
      return false;

    if (!bIsNumber) {
      m_pSyntax->SetPos(saved_pos);
      break;
    }

    uint32_t start_objnum = FXSYS_atoui(word.c_str());
    if (start_objnum >= kMaxObjectNumber)
      return false;

    uint32_t count = m_pSyntax->GetDirectNum();
    m_pSyntax->ToNextWord();

    if (!ParseAndAppendCrossRefSubsectionData(
            start_objnum, count, out_objects ? &result_objects : nullptr)) {
      return false;
    }
  }
  if (out_objects)
    *out_objects = std::move(result_objects);
  return true;
}

bool CPDF_Parser::LoadCrossRefV4(FX_FILESIZE pos, bool bSkip) {
  m_pSyntax->SetPos(pos);
  std::vector<CrossRefObjData> objects;
  if (!ParseCrossRefV4(bSkip ? nullptr : &objects))
    return false;

  MergeCrossRefObjectsData(objects);
  return true;
}

void CPDF_Parser::MergeCrossRefObjectsData(
    const std::vector<CrossRefObjData>& objects) {
  for (const auto& obj : objects) {
    switch (obj.info.type) {
      case ObjectType::kFree:
        if (obj.info.gennum > 0)
          m_CrossRefTable->SetFree(obj.obj_num);
        break;
      case ObjectType::kNormal:
      case ObjectType::kObjStream:
        m_CrossRefTable->AddNormal(obj.obj_num, obj.info.gennum, obj.info.pos);
        break;
      case ObjectType::kCompressed:
        m_CrossRefTable->AddCompressed(obj.obj_num, obj.info.archive_obj_num);
        break;
      default:
        NOTREACHED();
    }
  }
}

bool CPDF_Parser::LoadAllCrossRefV5(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV5(&xrefpos, true))
    return false;

  std::set<FX_FILESIZE> seen_xrefpos;
  while (xrefpos) {
    seen_xrefpos.insert(xrefpos);
    if (!LoadCrossRefV5(&xrefpos, false))
      return false;

    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return false;
  }
  m_ObjectStreamMap.clear();
  m_bXRefStream = true;
  return true;
}

bool CPDF_Parser::RebuildCrossRef() {
  auto cross_ref_table = pdfium::MakeUnique<CPDF_CrossRefTable>();

  const uint32_t kBufferSize = 4096;
  m_pSyntax->SetReadBufferSize(kBufferSize);
  m_pSyntax->SetPos(0);

  bool bIsNumber;
  std::vector<std::pair<uint32_t, FX_FILESIZE>> numbers;
  for (ByteString word = m_pSyntax->GetNextWord(&bIsNumber); !word.IsEmpty();
       word = m_pSyntax->GetNextWord(&bIsNumber)) {
    if (bIsNumber) {
      numbers.emplace_back(FXSYS_atoui(word.c_str()),
                           m_pSyntax->GetPos() - word.GetLength());
      if (numbers.size() > 2u)
        numbers.erase(numbers.begin());
      continue;
    }

    if (word == "(") {
      m_pSyntax->ReadString();
    } else if (word == "<") {
      m_pSyntax->ReadHexString();
    } else if (word == "trailer") {
      std::unique_ptr<CPDF_Object> pTrailer = m_pSyntax->GetObjectBody(nullptr);
      if (pTrailer) {
        cross_ref_table = CPDF_CrossRefTable::MergeUp(
            std::move(cross_ref_table),
            pdfium::MakeUnique<CPDF_CrossRefTable>(ToDictionary(
                pTrailer->IsStream() ? pTrailer->AsStream()->GetDict()->Clone()
                                     : std::move(pTrailer))));
      }
    } else if (word == "obj" && numbers.size() == 2u) {
      const FX_FILESIZE obj_pos = numbers[0].second;
      const uint32_t obj_num = numbers[0].first;
      const uint32_t gen_num = numbers[1].first;

      m_pSyntax->SetPos(obj_pos);
      const std::unique_ptr<CPDF_Stream> pStream =
          ToStream(m_pSyntax->GetIndirectObject(
              nullptr, CPDF_SyntaxParser::ParseType::kStrict));

      if (pStream && pStream->GetDict()->GetStringFor("Type") == "XRef") {
        cross_ref_table = CPDF_CrossRefTable::MergeUp(
            std::move(cross_ref_table),
            pdfium::MakeUnique<CPDF_CrossRefTable>(
                ToDictionary(pStream->GetDict()->Clone())));
      }

      if (obj_num < kMaxObjectNumber) {
        cross_ref_table->AddNormal(obj_num, gen_num, obj_pos);
        if (const auto object_stream =
                CPDF_ObjectStream::Create(pStream.get())) {
          for (const auto& it : object_stream->objects_offsets()) {
            if (it.first < kMaxObjectNumber)
              cross_ref_table->AddCompressed(it.first, obj_num);
          }
        }
      }
    }
    numbers.clear();
  }

  m_CrossRefTable = CPDF_CrossRefTable::MergeUp(std::move(m_CrossRefTable),
                                                std::move(cross_ref_table));
  // Resore default buffer size.
  m_pSyntax->SetReadBufferSize(CPDF_ModuleMgr::kFileBufSize);

  return GetTrailer() && !m_CrossRefTable->objects_info().empty();
}

bool CPDF_Parser::LoadCrossRefV5(FX_FILESIZE* pos, bool bMainXRef) {
  std::unique_ptr<CPDF_Object> pObject(ParseIndirectObjectAt(*pos, 0));
  if (!pObject || !pObject->GetObjNum())
    return false;

  CPDF_Stream* pStream = pObject->AsStream();
  if (!pStream)
    return false;

  CPDF_Dictionary* pDict = pStream->GetDict();
  *pos = pDict->GetIntegerFor("Prev");
  int32_t size = pDict->GetIntegerFor("Size");
  if (size < 0)
    return false;

  std::unique_ptr<CPDF_Dictionary> pNewTrailer = ToDictionary(pDict->Clone());
  if (bMainXRef) {
    m_CrossRefTable =
        pdfium::MakeUnique<CPDF_CrossRefTable>(std::move(pNewTrailer));
    m_CrossRefTable->ShrinkObjectMap(size);
  } else {
    m_CrossRefTable = CPDF_CrossRefTable::MergeUp(
        pdfium::MakeUnique<CPDF_CrossRefTable>(std::move(pNewTrailer)),
        std::move(m_CrossRefTable));
  }

  std::vector<std::pair<int32_t, int32_t>> arrIndex;
  CPDF_Array* pArray = pDict->GetArrayFor("Index");
  if (pArray) {
    for (size_t i = 0; i < pArray->size() / 2; i++) {
      CPDF_Object* pStartNumObj = pArray->GetObjectAt(i * 2);
      CPDF_Object* pCountObj = pArray->GetObjectAt(i * 2 + 1);

      if (ToNumber(pStartNumObj) && ToNumber(pCountObj)) {
        int nStartNum = pStartNumObj->GetInteger();
        int nCount = pCountObj->GetInteger();
        if (nStartNum >= 0 && nCount > 0)
          arrIndex.push_back(std::make_pair(nStartNum, nCount));
      }
    }
  }

  if (arrIndex.empty())
    arrIndex.push_back(std::make_pair(0, size));

  pArray = pDict->GetArrayFor("W");
  if (!pArray)
    return false;

  std::vector<uint32_t> WidthArray;
  FX_SAFE_UINT32 dwAccWidth = 0;
  for (size_t i = 0; i < pArray->size(); ++i) {
    WidthArray.push_back(pArray->GetIntegerAt(i));
    dwAccWidth += WidthArray[i];
  }

  if (!dwAccWidth.IsValid() || WidthArray.size() < 3)
    return false;

  uint32_t totalWidth = dwAccWidth.ValueOrDie();
  auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
  pAcc->LoadAllDataFiltered();

  const uint8_t* pData = pAcc->GetData();
  uint32_t dwTotalSize = pAcc->GetSize();
  uint32_t segindex = 0;
  for (const auto& index : arrIndex) {
    const int32_t startnum = index.first;
    if (startnum < 0)
      continue;

    uint32_t count = pdfium::base::checked_cast<uint32_t>(index.second);
    FX_SAFE_UINT32 dwCaculatedSize = segindex;
    dwCaculatedSize += count;
    dwCaculatedSize *= totalWidth;
    if (!dwCaculatedSize.IsValid() ||
        dwCaculatedSize.ValueOrDie() > dwTotalSize) {
      continue;
    }

    const uint8_t* segstart = pData + segindex * totalWidth;
    FX_SAFE_UINT32 dwMaxObjNum = startnum;
    dwMaxObjNum += count;
    uint32_t dwV5Size =
        m_CrossRefTable->objects_info().empty() ? 0 : GetLastObjNum() + 1;
    if (!dwMaxObjNum.IsValid() || dwMaxObjNum.ValueOrDie() > dwV5Size)
      continue;

    for (uint32_t i = 0; i < count; i++) {
      ObjectType type = ObjectType::kNotCompressed;
      const uint8_t* entrystart = segstart + i * totalWidth;
      if (WidthArray[0]) {
        const uint32_t cross_ref_stream_obj_type =
            GetVarInt(entrystart, WidthArray[0]);
        type = GetObjectTypeFromCrossRefStreamType(cross_ref_stream_obj_type);
        if (type == ObjectType::kNull)
          continue;
      }

      const uint32_t objnum = startnum + i;
      if (objnum >= CPDF_Parser::kMaxObjectNumber)
        continue;

      const ObjectType existing_type = GetObjectType(objnum);
      if (existing_type == ObjectType::kNull) {
        uint32_t offset = GetVarInt(entrystart + WidthArray[0], WidthArray[1]);
        if (pdfium::base::IsValueInRangeForNumericType<FX_FILESIZE>(offset))
          m_CrossRefTable->AddNormal(objnum, 0, offset);
        continue;
      }

      if (existing_type != ObjectType::kFree)
        continue;

      if (type == ObjectType::kFree) {
        m_CrossRefTable->SetFree(objnum);
        continue;
      }

      const uint32_t entry_value =
          GetVarInt(entrystart + WidthArray[0], WidthArray[1]);
      if (type == ObjectType::kNotCompressed) {
        const uint32_t offset = entry_value;
        if (pdfium::base::IsValueInRangeForNumericType<FX_FILESIZE>(offset))
          m_CrossRefTable->AddNormal(objnum, 0, offset);
        continue;
      }

      ASSERT(type == ObjectType::kCompressed);
      const uint32_t archive_obj_num = entry_value;
      if (!IsValidObjectNumber(archive_obj_num))
        return false;

      m_CrossRefTable->AddCompressed(objnum, archive_obj_num);
    }
    segindex += count;
  }
  return true;
}

const CPDF_Array* CPDF_Parser::GetIDArray() const {
  return GetTrailer() ? GetTrailer()->GetArrayFor("ID") : nullptr;
}

CPDF_Dictionary* CPDF_Parser::GetRoot() const {
  CPDF_Object* obj =
      m_pObjectsHolder->GetOrParseIndirectObject(GetRootObjNum());
  return obj ? obj->GetDict() : nullptr;
}

const CPDF_Dictionary* CPDF_Parser::GetEncryptDict() const {
  if (!GetTrailer())
    return nullptr;

  const CPDF_Object* pEncryptObj = GetTrailer()->GetObjectFor("Encrypt");
  if (!pEncryptObj)
    return nullptr;

  if (pEncryptObj->IsDictionary())
    return ToDictionary(pEncryptObj);

  if (pEncryptObj->IsReference()) {
    return ToDictionary(m_pObjectsHolder->GetOrParseIndirectObject(
        pEncryptObj->AsReference()->GetRefObjNum()));
  }
  return nullptr;
}

const CPDF_Dictionary* CPDF_Parser::GetTrailer() const {
  return m_CrossRefTable->trailer();
}

std::unique_ptr<CPDF_Dictionary> CPDF_Parser::GetCombinedTrailer() const {
  return m_CrossRefTable->trailer()
             ? ToDictionary(m_CrossRefTable->trailer()->Clone())
             : std::unique_ptr<CPDF_Dictionary>();
}

uint32_t CPDF_Parser::GetInfoObjNum() const {
  const CPDF_Reference* pRef =
      ToReference(m_CrossRefTable->trailer()
                      ? m_CrossRefTable->trailer()->GetObjectFor("Info")
                      : nullptr);
  return pRef ? pRef->GetRefObjNum() : CPDF_Object::kInvalidObjNum;
}

uint32_t CPDF_Parser::GetRootObjNum() const {
  const CPDF_Reference* pRef =
      ToReference(m_CrossRefTable->trailer()
                      ? m_CrossRefTable->trailer()->GetObjectFor("Root")
                      : nullptr);
  return pRef ? pRef->GetRefObjNum() : CPDF_Object::kInvalidObjNum;
}

std::unique_ptr<CPDF_Object> CPDF_Parser::ParseIndirectObject(
    uint32_t objnum) {
  if (!IsValidObjectNumber(objnum))
    return nullptr;

  // Prevent circular parsing the same object.
  if (pdfium::ContainsKey(m_ParsingObjNums, objnum))
    return nullptr;

  pdfium::ScopedSetInsertion<uint32_t> local_insert(&m_ParsingObjNums, objnum);
  if (GetObjectType(objnum) == ObjectType::kNotCompressed) {
    FX_FILESIZE pos = GetObjectPositionOrZero(objnum);
    if (pos <= 0)
      return nullptr;
    return ParseIndirectObjectAt(pos, objnum);
  }
  if (GetObjectType(objnum) != ObjectType::kCompressed)
    return nullptr;

  const CPDF_ObjectStream* pObjStream =
      GetObjectStream(m_CrossRefTable->GetObjectInfo(objnum)->archive_obj_num);
  if (!pObjStream)
    return nullptr;

  return pObjStream->ParseObject(m_pObjectsHolder.Get(), objnum);
}

const CPDF_ObjectStream* CPDF_Parser::GetObjectStream(uint32_t object_number) {
  // Prevent circular parsing the same object.
  if (pdfium::ContainsKey(m_ParsingObjNums, object_number))
    return nullptr;

  pdfium::ScopedSetInsertion<uint32_t> local_insert(&m_ParsingObjNums,
                                                    object_number);

  auto it = m_ObjectStreamMap.find(object_number);
  if (it != m_ObjectStreamMap.end())
    return it->second.get();

  const auto* info = m_CrossRefTable->GetObjectInfo(object_number);
  if (!info || info->type != ObjectType::kObjStream)
    return nullptr;

  const FX_FILESIZE object_pos = info->pos;
  if (object_pos <= 0)
    return nullptr;

  std::unique_ptr<CPDF_Object> object =
      ParseIndirectObjectAt(object_pos, object_number);
  if (!object)
    return nullptr;

  std::unique_ptr<CPDF_ObjectStream> objs_stream =
      CPDF_ObjectStream::Create(ToStream(object.get()));
  const CPDF_ObjectStream* result = objs_stream.get();
  m_ObjectStreamMap[object_number] = std::move(objs_stream);

  return result;
}

std::unique_ptr<CPDF_Object> CPDF_Parser::ParseIndirectObjectAt(
    FX_FILESIZE pos,
    uint32_t objnum) {
  const FX_FILESIZE saved_pos = m_pSyntax->GetPos();
  m_pSyntax->SetPos(pos);
  auto result = m_pSyntax->GetIndirectObject(
      m_pObjectsHolder.Get(), CPDF_SyntaxParser::ParseType::kLoose);

  m_pSyntax->SetPos(saved_pos);

  if (result && objnum && result->GetObjNum() != objnum)
    return nullptr;

  const bool should_decrypt = m_pSecurityHandler &&
                              m_pSecurityHandler->GetCryptoHandler() &&
                              objnum != m_MetadataObjnum;
  if (should_decrypt)
    result = m_pSecurityHandler->GetCryptoHandler()->DecryptObjectTree(
        std::move(result));

  return result;
}

uint32_t CPDF_Parser::GetFirstPageNo() const {
  return m_pLinearized ? m_pLinearized->GetFirstPageNo() : 0;
}

void CPDF_Parser::SetLinearizedHeader(
    std::unique_ptr<CPDF_LinearizedHeader> pLinearized) {
  m_pLinearized = std::move(pLinearized);
}

std::unique_ptr<CPDF_Dictionary> CPDF_Parser::LoadTrailerV4() {
  if (m_pSyntax->GetKeyword() != "trailer")
    return nullptr;

  return ToDictionary(m_pSyntax->GetObjectBody(m_pObjectsHolder.Get()));
}

uint32_t CPDF_Parser::GetPermissions() const {
  return m_pSecurityHandler ? m_pSecurityHandler->GetPermissions() : 0xFFFFFFFF;
}

std::unique_ptr<CPDF_LinearizedHeader> CPDF_Parser::ParseLinearizedHeader() {
  return CPDF_LinearizedHeader::Parse(m_pSyntax.get());
}

CPDF_Parser::Error CPDF_Parser::StartLinearizedParse(
    const RetainPtr<CPDF_ReadValidator>& validator,
    const char* password) {
  ASSERT(!m_bHasParsed);
  ASSERT(!m_bXRefTableRebuilt);
  SetPassword(password);
  m_bXRefStream = false;
  m_LastXRefOffset = 0;

  if (!InitSyntaxParser(validator))
    return FORMAT_ERROR;

  m_pLinearized = ParseLinearizedHeader();
  if (!m_pLinearized)
    return StartParseInternal();

  m_bHasParsed = true;

  m_LastXRefOffset = m_pLinearized->GetLastXRefOffset();
  FX_FILESIZE dwFirstXRefOffset = m_LastXRefOffset;
  bool bLoadV4 = LoadCrossRefV4(dwFirstXRefOffset, false);
  if (!bLoadV4 && !LoadCrossRefV5(&dwFirstXRefOffset, true)) {
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    m_bXRefTableRebuilt = true;
    m_LastXRefOffset = 0;
  }
  if (bLoadV4) {
    std::unique_ptr<CPDF_Dictionary> trailer = LoadTrailerV4();
    if (!trailer)
      return SUCCESS;

    m_CrossRefTable->SetTrailer(std::move(trailer));
    int32_t xrefsize = GetDirectInteger(GetTrailer(), "Size");
    if (xrefsize > 0)
      ShrinkObjectMap(xrefsize);
  }

  Error eRet = SetEncryptHandler();
  if (eRet != SUCCESS)
    return eRet;

  if (!GetRoot() || !m_pObjectsHolder->TryInit()) {
    if (m_bXRefTableRebuilt)
      return FORMAT_ERROR;

    ReleaseEncryptHandler();
    if (!RebuildCrossRef())
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;

    m_pObjectsHolder->TryInit();
    if (!GetRoot())
      return FORMAT_ERROR;
  }

  if (GetRootObjNum() == CPDF_Object::kInvalidObjNum) {
    ReleaseEncryptHandler();
    if (!RebuildCrossRef() || GetRootObjNum() == CPDF_Object::kInvalidObjNum)
      return FORMAT_ERROR;

    eRet = SetEncryptHandler();
    if (eRet != SUCCESS)
      return eRet;
  }

  if (m_pSecurityHandler && m_pSecurityHandler->IsMetadataEncrypted()) {
    if (CPDF_Reference* pMetadata =
            ToReference(GetRoot()->GetObjectFor("Metadata")))
      m_MetadataObjnum = pMetadata->GetRefObjNum();
  }
  return SUCCESS;
}

bool CPDF_Parser::LoadLinearizedAllCrossRefV5(FX_FILESIZE xrefpos) {
  if (!LoadCrossRefV5(&xrefpos, false))
    return false;

  std::set<FX_FILESIZE> seen_xrefpos;
  while (xrefpos) {
    seen_xrefpos.insert(xrefpos);
    if (!LoadCrossRefV5(&xrefpos, false))
      return false;

    // Check for circular references.
    if (pdfium::ContainsKey(seen_xrefpos, xrefpos))
      return false;
  }
  m_ObjectStreamMap.clear();
  m_bXRefStream = true;
  return true;
}

CPDF_Parser::Error CPDF_Parser::LoadLinearizedMainXRefTable() {
  const FX_SAFE_FILESIZE main_xref_offset = GetTrailer()->GetIntegerFor("Prev");
  if (!main_xref_offset.IsValid())
    return FORMAT_ERROR;

  if (main_xref_offset.ValueOrDie() == 0)
    return SUCCESS;

  const AutoRestorer<uint32_t> save_metadata_objnum(&m_MetadataObjnum);
  m_MetadataObjnum = 0;
  m_ObjectStreamMap.clear();

  if (!LoadLinearizedAllCrossRefV4(main_xref_offset.ValueOrDie()) &&
      !LoadLinearizedAllCrossRefV5(main_xref_offset.ValueOrDie())) {
    m_LastXRefOffset = 0;
    return FORMAT_ERROR;
  }

  return SUCCESS;
}

CPDF_Parser::ObjectType CPDF_Parser::GetObjectTypeFromCrossRefStreamType(
    uint32_t cross_ref_stream_type) const {
  switch (cross_ref_stream_type) {
    case 0:
      return CPDF_Parser::ObjectType::kFree;
    case 1:
      return CPDF_Parser::ObjectType::kNotCompressed;
    case 2:
      return CPDF_Parser::ObjectType::kCompressed;
    default:
      return CPDF_Parser::ObjectType::kNull;
  }
}
