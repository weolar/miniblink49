// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_EDIT_CPDF_CREATOR_H_
#define CORE_FPDFAPI_EDIT_CPDF_CREATOR_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Array;
class CPDF_CryptoHandler;
class CPDF_SecurityHandler;
class CPDF_Dictionary;
class CPDF_Document;
class CPDF_Object;
class CPDF_Parser;

#define FPDFCREATE_INCREMENTAL 1
#define FPDFCREATE_NO_ORIGINAL 2

class CPDF_Creator {
 public:
  CPDF_Creator(CPDF_Document* pDoc,
               const RetainPtr<IFX_RetainableWriteStream>& archive);
  ~CPDF_Creator();

  void RemoveSecurity();
  bool Create(uint32_t flags);
  bool SetFileVersion(int32_t fileVersion);

 private:
  enum class Stage {
    kInvalid = -1,
    kInit0 = 0,
    kWriteHeader10 = 10,
    kWriteIncremental15 = 15,
    kInitWriteObjs20 = 20,
    kWriteOldObjs21 = 21,
    kInitWriteNewObjs25 = 25,
    kWriteNewObjs26 = 26,
    kWriteEncryptDict27 = 27,
    kInitWriteXRefs80 = 80,
    kWriteXrefsNotIncremental81 = 81,
    kWriteXrefsIncremental82 = 82,
    kWriteTrailerAndFinish90 = 90,
    kComplete100 = 100,
  };

  bool Continue();
  void Clear();

  void InitNewObjNumOffsets();
  void InitID();

  CPDF_Creator::Stage WriteDoc_Stage1();
  CPDF_Creator::Stage WriteDoc_Stage2();
  CPDF_Creator::Stage WriteDoc_Stage3();
  CPDF_Creator::Stage WriteDoc_Stage4();

  bool WriteOldIndirectObject(uint32_t objnum);
  bool WriteOldObjs();
  bool WriteNewObjs();
  bool WriteIndirectObj(uint32_t objnum, const CPDF_Object* pObj);

  CPDF_CryptoHandler* GetCryptoHandler();

  UnownedPtr<CPDF_Document> const m_pDocument;
  UnownedPtr<const CPDF_Parser> const m_pParser;
  UnownedPtr<const CPDF_Dictionary> m_pEncryptDict;
  std::unique_ptr<CPDF_Dictionary> m_pNewEncryptDict;
  fxcrt::MaybeOwned<CPDF_SecurityHandler> m_pSecurityHandler;
  UnownedPtr<const CPDF_Object> m_pMetadata;
  uint32_t m_dwLastObjNum;
  std::unique_ptr<IFX_ArchiveStream> m_Archive;
  FX_FILESIZE m_SavedOffset = 0;
  Stage m_iStage = Stage::kInvalid;
  uint32_t m_CurObjNum = 0;
  FX_FILESIZE m_XrefStart = 0;
  std::map<uint32_t, FX_FILESIZE> m_ObjectOffsets;
  std::vector<uint32_t> m_NewObjNumArray;  // Sorted, ascending.
  std::unique_ptr<CPDF_Array> m_pIDArray;
  int32_t m_FileVersion = 0;
  bool m_bSecurityChanged = false;
  bool m_IsIncremental = false;
  bool m_IsOriginal = false;
};

#endif  // CORE_FPDFAPI_EDIT_CPDF_CREATOR_H_
