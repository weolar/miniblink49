// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_crypto_handler.h"

#include <time.h>

#include <algorithm>
#include <stack>
#include <utility>

#include "core/fdrm/fx_crypt.h"
#include "core/fpdfapi/edit/cpdf_encryptor.h"
#include "core/fpdfapi/edit/cpdf_flateencoder.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_object_walker.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "core/fpdfapi/parser/cpdf_security_handler.h"
#include "core/fpdfapi/parser/cpdf_simple_parser.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_string.h"

namespace {

constexpr char kContentsKey[] = "Contents";
constexpr char kTypeKey[] = "Type";
constexpr char kFTKey[] = "FT";
constexpr char kSignTypeValue[] = "Sig";

}  // namespace

// static
bool CPDF_CryptoHandler::IsSignatureDictionary(
    const CPDF_Dictionary* dictionary) {
  if (!dictionary)
    return false;
  const CPDF_Object* type_obj = dictionary->GetDirectObjectFor(kTypeKey);
  if (!type_obj)
    type_obj = dictionary->GetDirectObjectFor(kFTKey);
  return type_obj && type_obj->GetString() == kSignTypeValue;
}

void CPDF_CryptoHandler::CryptBlock(bool bEncrypt,
                                    uint32_t objnum,
                                    uint32_t gennum,
                                    pdfium::span<const uint8_t> source,
                                    uint8_t* dest_buf,
                                    uint32_t& dest_size) {
  if (m_Cipher == FXCIPHER_NONE) {
    memcpy(dest_buf, source.data(), source.size());
    return;
  }
  uint8_t realkey[16];
  int realkeylen = 16;
  if (m_Cipher != FXCIPHER_AES || m_KeyLen != 32) {
    uint8_t key1[32];
    PopulateKey(objnum, gennum, key1);

    if (m_Cipher == FXCIPHER_AES) {
      memcpy(key1 + m_KeyLen + 5, "sAlT", 4);
    }
    CRYPT_MD5Generate(
        key1, m_Cipher == FXCIPHER_AES ? m_KeyLen + 9 : m_KeyLen + 5, realkey);
    realkeylen = m_KeyLen + 5;
    if (realkeylen > 16) {
      realkeylen = 16;
    }
  }
  if (m_Cipher == FXCIPHER_AES) {
    CRYPT_AESSetKey(m_pAESContext.get(),
                    m_KeyLen == 32 ? m_EncryptKey : realkey, m_KeyLen,
                    bEncrypt);
    if (bEncrypt) {
      uint8_t iv[16];
      for (int i = 0; i < 16; i++) {
        iv[i] = (uint8_t)rand();
      }
      CRYPT_AESSetIV(m_pAESContext.get(), iv);
      memcpy(dest_buf, iv, 16);
      int nblocks = source.size() / 16;
      CRYPT_AESEncrypt(m_pAESContext.get(), dest_buf + 16, source.data(),
                       nblocks * 16);
      uint8_t padding[16];
      memcpy(padding, source.data() + nblocks * 16, source.size() % 16);
      memset(padding + source.size() % 16, 16 - source.size() % 16,
             16 - source.size() % 16);
      CRYPT_AESEncrypt(m_pAESContext.get(), dest_buf + nblocks * 16 + 16,
                       padding, 16);
      dest_size = 32 + nblocks * 16;
    } else {
      CRYPT_AESSetIV(m_pAESContext.get(), source.data());
      CRYPT_AESDecrypt(m_pAESContext.get(), dest_buf, source.data() + 16,
                       source.size() - 16);
      dest_size = source.size() - 16;
      dest_size -= dest_buf[dest_size - 1];
    }
  } else {
    ASSERT(dest_size == source.size());
    if (dest_buf != source.data()) {
      memcpy(dest_buf, source.data(), source.size());
    }
    CRYPT_ArcFourCryptBlock(dest_buf, dest_size, realkey, realkeylen);
  }
}

struct AESCryptContext {
  bool m_bIV;
  uint32_t m_BlockOffset;
  CRYPT_aes_context m_Context;
  uint8_t m_Block[16];
};

void* CPDF_CryptoHandler::CryptStart(uint32_t objnum,
                                     uint32_t gennum,
                                     bool bEncrypt) {
  if (m_Cipher == FXCIPHER_NONE) {
    return this;
  }
  if (m_Cipher == FXCIPHER_AES && m_KeyLen == 32) {
    AESCryptContext* pContext = FX_Alloc(AESCryptContext, 1);
    pContext->m_bIV = true;
    pContext->m_BlockOffset = 0;
    CRYPT_AESSetKey(&pContext->m_Context, m_EncryptKey, 32, bEncrypt);
    if (bEncrypt) {
      for (int i = 0; i < 16; i++) {
        pContext->m_Block[i] = (uint8_t)rand();
      }
      CRYPT_AESSetIV(&pContext->m_Context, pContext->m_Block);
    }
    return pContext;
  }
  uint8_t key1[48];
  PopulateKey(objnum, gennum, key1);

  if (m_Cipher == FXCIPHER_AES) {
    memcpy(key1 + m_KeyLen + 5, "sAlT", 4);
  }
  uint8_t realkey[16];
  CRYPT_MD5Generate(
      key1, m_Cipher == FXCIPHER_AES ? m_KeyLen + 9 : m_KeyLen + 5, realkey);
  int realkeylen = m_KeyLen + 5;
  if (realkeylen > 16) {
    realkeylen = 16;
  }
  if (m_Cipher == FXCIPHER_AES) {
    AESCryptContext* pContext = FX_Alloc(AESCryptContext, 1);
    pContext->m_bIV = true;
    pContext->m_BlockOffset = 0;
    CRYPT_AESSetKey(&pContext->m_Context, realkey, 16, bEncrypt);
    if (bEncrypt) {
      for (int i = 0; i < 16; i++) {
        pContext->m_Block[i] = (uint8_t)rand();
      }
      CRYPT_AESSetIV(&pContext->m_Context, pContext->m_Block);
    }
    return pContext;
  }
  CRYPT_rc4_context* pContext = FX_Alloc(CRYPT_rc4_context, 1);
  CRYPT_ArcFourSetup(pContext, realkey, realkeylen);
  return pContext;
}

bool CPDF_CryptoHandler::CryptStream(void* context,
                                     pdfium::span<const uint8_t> source,
                                     CFX_BinaryBuf& dest_buf,
                                     bool bEncrypt) {
  if (!context)
    return false;

  if (m_Cipher == FXCIPHER_NONE) {
    dest_buf.AppendBlock(source.data(), source.size());
    return true;
  }
  if (m_Cipher == FXCIPHER_RC4) {
    int old_size = dest_buf.GetSize();
    dest_buf.AppendBlock(source.data(), source.size());
    CRYPT_ArcFourCrypt(static_cast<CRYPT_rc4_context*>(context),
                       dest_buf.GetBuffer() + old_size, source.size());
    return true;
  }
  AESCryptContext* pContext = static_cast<AESCryptContext*>(context);
  if (pContext->m_bIV && bEncrypt) {
    dest_buf.AppendBlock(pContext->m_Block, 16);
    pContext->m_bIV = false;
  }
  uint32_t src_off = 0;
  uint32_t src_left = source.size();
  while (1) {
    uint32_t copy_size = 16 - pContext->m_BlockOffset;
    if (copy_size > src_left) {
      copy_size = src_left;
    }
    memcpy(pContext->m_Block + pContext->m_BlockOffset, source.data() + src_off,
           copy_size);
    src_off += copy_size;
    src_left -= copy_size;
    pContext->m_BlockOffset += copy_size;
    if (pContext->m_BlockOffset == 16) {
      if (!bEncrypt && pContext->m_bIV) {
        CRYPT_AESSetIV(&pContext->m_Context, pContext->m_Block);
        pContext->m_bIV = false;
        pContext->m_BlockOffset = 0;
      } else if (src_off < source.size()) {
        uint8_t block_buf[16];
        if (bEncrypt) {
          CRYPT_AESEncrypt(&pContext->m_Context, block_buf, pContext->m_Block,
                           16);
        } else {
          CRYPT_AESDecrypt(&pContext->m_Context, block_buf, pContext->m_Block,
                           16);
        }
        dest_buf.AppendBlock(block_buf, 16);
        pContext->m_BlockOffset = 0;
      }
    }
    if (!src_left) {
      break;
    }
  }
  return true;
}
bool CPDF_CryptoHandler::CryptFinish(void* context,
                                     CFX_BinaryBuf& dest_buf,
                                     bool bEncrypt) {
  if (!context) {
    return false;
  }
  if (m_Cipher == FXCIPHER_NONE) {
    return true;
  }
  if (m_Cipher == FXCIPHER_RC4) {
    FX_Free(context);
    return true;
  }
  auto* pContext = static_cast<AESCryptContext*>(context);
  if (bEncrypt) {
    uint8_t block_buf[16];
    if (pContext->m_BlockOffset == 16) {
      CRYPT_AESEncrypt(&pContext->m_Context, block_buf, pContext->m_Block, 16);
      dest_buf.AppendBlock(block_buf, 16);
      pContext->m_BlockOffset = 0;
    }
    memset(pContext->m_Block + pContext->m_BlockOffset,
           (uint8_t)(16 - pContext->m_BlockOffset),
           16 - pContext->m_BlockOffset);
    CRYPT_AESEncrypt(&pContext->m_Context, block_buf, pContext->m_Block, 16);
    dest_buf.AppendBlock(block_buf, 16);
  } else if (pContext->m_BlockOffset == 16) {
    uint8_t block_buf[16];
    CRYPT_AESDecrypt(&pContext->m_Context, block_buf, pContext->m_Block, 16);
    if (block_buf[15] <= 16) {
      dest_buf.AppendBlock(block_buf, 16 - block_buf[15]);
    }
  }
  FX_Free(pContext);
  return true;
}

ByteString CPDF_CryptoHandler::Decrypt(uint32_t objnum,
                                       uint32_t gennum,
                                       const ByteString& str) {
  CFX_BinaryBuf dest_buf;
  void* context = DecryptStart(objnum, gennum);
  DecryptStream(context, str.AsRawSpan(), dest_buf);
  DecryptFinish(context, dest_buf);
  return ByteString(dest_buf.GetBuffer(), dest_buf.GetSize());
}

void* CPDF_CryptoHandler::DecryptStart(uint32_t objnum, uint32_t gennum) {
  return CryptStart(objnum, gennum, false);
}
uint32_t CPDF_CryptoHandler::DecryptGetSize(uint32_t src_size) {
  return m_Cipher == FXCIPHER_AES ? src_size - 16 : src_size;
}

bool CPDF_CryptoHandler::IsCipherAES() const {
  return m_Cipher == FXCIPHER_AES;
}

std::unique_ptr<CPDF_Object> CPDF_CryptoHandler::DecryptObjectTree(
    std::unique_ptr<CPDF_Object> object) {
  if (!object)
    return nullptr;

  struct MayBeSignature {
    const CPDF_Dictionary* parent;
    CPDF_Object* contents;
  };

  std::stack<MayBeSignature> may_be_sign_dictionaries;
  const uint32_t obj_num = object->GetObjNum();
  const uint32_t gen_num = object->GetGenNum();

  CPDF_Object* object_to_decrypt = object.get();
  while (object_to_decrypt) {
    CPDF_NonConstObjectWalker walker(object_to_decrypt);
    object_to_decrypt = nullptr;
    while (CPDF_Object* child = walker.GetNext()) {
      const CPDF_Dictionary* parent_dict =
          walker.GetParent() ? walker.GetParent()->GetDict() : nullptr;
      if (walker.dictionary_key() == kContentsKey &&
          (parent_dict->KeyExist(kTypeKey) || parent_dict->KeyExist(kFTKey))) {
        // This object may be contents of signature dictionary.
        // But now values of 'Type' and 'FT' of dictionary keys are encrypted,
        // and we can not check this.
        // Temporary skip it, to prevent signature corruption.
        // It will be decrypted on next interations, if this is not contents of
        // signature dictionary.
        may_be_sign_dictionaries.push(MayBeSignature({parent_dict, child}));
        walker.SkipWalkIntoCurrentObject();
        continue;
      }
      // Strings decryption.
      if (child->IsString()) {
        // TODO(art-snake): Move decryption into the CPDF_String class.
        CPDF_String* str = child->AsString();
        str->SetString(Decrypt(obj_num, gen_num, str->GetString()));
      }
      // Stream decryption.
      if (child->IsStream()) {
        // TODO(art-snake): Move decryption into the CPDF_Stream class.
        CPDF_Stream* stream = child->AsStream();
        auto stream_access = pdfium::MakeRetain<CPDF_StreamAcc>(stream);
        stream_access->LoadAllDataRaw();

        if (IsCipherAES() && stream_access->GetSize() < 16) {
          stream->SetData({});
          continue;
        }

        CFX_BinaryBuf decrypted_buf;
        decrypted_buf.EstimateSize(DecryptGetSize(stream_access->GetSize()));

        void* context = DecryptStart(obj_num, gen_num);
        bool decrypt_result =
            DecryptStream(context,
                          pdfium::make_span(stream_access->GetData(),
                                            stream_access->GetSize()),
                          decrypted_buf);
        decrypt_result &= DecryptFinish(context, decrypted_buf);
        if (decrypt_result) {
          const uint32_t decrypted_size = decrypted_buf.GetSize();
          stream->SetData(decrypted_buf.DetachBuffer(), decrypted_size);
        } else {
          // Decryption failed, set the stream to empty
          stream->SetData({});
        }
      }
    }
    // Signature dictionaries check.
    while (!may_be_sign_dictionaries.empty()) {
      auto dict_and_contents = std::move(may_be_sign_dictionaries.top());
      may_be_sign_dictionaries.pop();
      if (!IsSignatureDictionary(dict_and_contents.parent)) {
        // This is not signature dictionary. Do decrypt its contents.
        object_to_decrypt = dict_and_contents.contents;
        break;
      }
    }
  }
  return object;
}

bool CPDF_CryptoHandler::DecryptStream(void* context,
                                       pdfium::span<const uint8_t> source,
                                       CFX_BinaryBuf& dest_buf) {
  return CryptStream(context, source, dest_buf, false);
}

bool CPDF_CryptoHandler::DecryptFinish(void* context, CFX_BinaryBuf& dest_buf) {
  return CryptFinish(context, dest_buf, false);
}

size_t CPDF_CryptoHandler::EncryptGetSize(
    pdfium::span<const uint8_t> source) const {
  return m_Cipher == FXCIPHER_AES ? source.size() + 32 : source.size();
}

bool CPDF_CryptoHandler::EncryptContent(uint32_t objnum,
                                        uint32_t gennum,
                                        pdfium::span<const uint8_t> source,
                                        uint8_t* dest_buf,
                                        uint32_t& dest_size) {
  CryptBlock(true, objnum, gennum, source, dest_buf, dest_size);
  return true;
}

CPDF_CryptoHandler::CPDF_CryptoHandler(int cipher,
                                       const uint8_t* key,
                                       int keylen)
    : m_KeyLen(std::min(keylen, 32)), m_Cipher(cipher) {
  ASSERT(cipher != FXCIPHER_AES || keylen == 16 || keylen == 24 ||
         keylen == 32);
  ASSERT(cipher != FXCIPHER_AES2 || keylen == 32);
  ASSERT(cipher != FXCIPHER_RC4 || (keylen >= 5 && keylen <= 16));

  if (m_Cipher != FXCIPHER_NONE)
    memcpy(m_EncryptKey, key, m_KeyLen);

  if (m_Cipher == FXCIPHER_AES)
    m_pAESContext.reset(FX_Alloc(CRYPT_aes_context, 1));
}

CPDF_CryptoHandler::~CPDF_CryptoHandler() {}

void CPDF_CryptoHandler::PopulateKey(uint32_t objnum,
                                     uint32_t gennum,
                                     uint8_t* key) {
  memcpy(key, m_EncryptKey, m_KeyLen);
  key[m_KeyLen + 0] = (uint8_t)objnum;
  key[m_KeyLen + 1] = (uint8_t)(objnum >> 8);
  key[m_KeyLen + 2] = (uint8_t)(objnum >> 16);
  key[m_KeyLen + 3] = (uint8_t)gennum;
  key[m_KeyLen + 4] = (uint8_t)(gennum >> 8);
}
