// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_SECURITY_HANDLER_H_
#define CORE_FPDFAPI_PARSER_CPDF_SECURITY_HANDLER_H_

#include <memory>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

#define FXCIPHER_NONE 0
#define FXCIPHER_RC4 1
#define FXCIPHER_AES 2
#define FXCIPHER_AES2 3

class CPDF_Array;
class CPDF_CryptoHandler;
class CPDF_Dictionary;
class CPDF_Parser;

class CPDF_SecurityHandler {
 public:
  CPDF_SecurityHandler();
  ~CPDF_SecurityHandler();

  bool OnInit(const CPDF_Dictionary* pEncryptDict,
              const CPDF_Array* pIdArray,
              const ByteString& password);
  void OnCreate(CPDF_Dictionary* pEncryptDict,
                const CPDF_Array* pIdArray,
                const ByteString& user_password,
                const ByteString& owner_password);
  void OnCreate(CPDF_Dictionary* pEncryptDict,
                const CPDF_Array* pIdArray,
                const ByteString& user_password);

  uint32_t GetPermissions() const;
  bool IsMetadataEncrypted() const;

  CPDF_CryptoHandler* GetCryptoHandler() const {
    return m_pCryptoHandler.get();
  }

 private:
  bool LoadDict(const CPDF_Dictionary* pEncryptDict);
  bool LoadDict(const CPDF_Dictionary* pEncryptDict,
                int& cipher,
                int& key_len);

  ByteString GetUserPassword(const ByteString& owner_password) const;
  bool CheckPassword(const ByteString& user_password, bool bOwner);
  bool CheckPasswordImpl(const ByteString& password, bool bOwner);
  bool CheckUserPassword(const ByteString& password, bool bIgnoreEncryptMeta);
  bool CheckOwnerPassword(const ByteString& password);
  bool AES256_CheckPassword(const ByteString& password, bool bOwner);
  void AES256_SetPassword(CPDF_Dictionary* pEncryptDict,
                          const ByteString& password,
                          bool bOwner,
                          const uint8_t* key);
  void AES256_SetPerms(CPDF_Dictionary* pEncryptDict,
                       uint32_t permission,
                       bool bEncryptMetadata,
                       const uint8_t* key);
  void OnCreateInternal(CPDF_Dictionary* pEncryptDict,
                        const CPDF_Array* pIdArray,
                        const ByteString& user_password,
                        const ByteString& owner_password,
                        bool bDefault);
  bool CheckSecurity(const ByteString& password);

  void InitCryptoHandler();

  int m_Version;
  int m_Revision;
  ByteString m_FileId;
  uint32_t m_Permissions;
  int m_Cipher;
  int m_KeyLen;
  bool m_bOwnerUnlocked;
  UnownedPtr<const CPDF_Dictionary> m_pEncryptDict;
  std::unique_ptr<CPDF_CryptoHandler> m_pCryptoHandler;
  uint8_t m_EncryptKey[32];
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_SECURITY_HANDLER_H_
