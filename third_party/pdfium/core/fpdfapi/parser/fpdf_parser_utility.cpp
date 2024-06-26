// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/fpdf_parser_utility.h"

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_stream.h"
#include "third_party/base/logging.h"

// Indexed by 8-bit character code, contains either:
//   'W' - for whitespace: NUL, TAB, CR, LF, FF, SPACE, 0x80, 0xff
//   'N' - for numeric: 0123456789+-.
//   'D' - for delimiter: %()/<>[]{}
//   'R' - otherwise.
const char PDF_CharType[256] = {
    // NUL  SOH  STX  ETX  EOT  ENQ  ACK  BEL  BS   HT   LF   VT   FF   CR   SO
    // SI
    'W', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'W', 'W', 'R', 'W', 'W', 'R',
    'R',

    // DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN  EM   SUB  ESC  FS   GS   RS
    // US
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R',

    // SP    !    "    #    $    %    &    ´    (    )    *    +    ,    -    .
    // /
    'W', 'R', 'R', 'R', 'R', 'D', 'R', 'R', 'D', 'D', 'R', 'N', 'R', 'N', 'N',
    'D',

    // 0    1    2    3    4    5    6    7    8    9    :    ;    <    =    > ?
    'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'R', 'R', 'D', 'R', 'D',
    'R',

    // @    A    B    C    D    E    F    G    H    I    J    K    L    M    N O
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R',

    // P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^ _
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'D', 'R', 'D', 'R',
    'R',

    // `    a    b    c    d    e    f    g    h    i    j    k    l    m    n o
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R',

    // p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~
    // DEL
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'D', 'R', 'D', 'R',
    'R',

    'W', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
    'R', 'R', 'R', 'R', 'R', 'R', 'R', 'W'};

Optional<FX_FILESIZE> GetHeaderOffset(
    const RetainPtr<IFX_SeekableReadStream>& pFile) {
  static constexpr size_t kBufSize = 4;
  uint8_t buf[kBufSize];
  for (FX_FILESIZE offset = 0; offset <= 1024; ++offset) {
    if (!pFile->ReadBlockAtOffset(buf, offset, kBufSize))
      return {};

    if (memcmp(buf, "%PDF", 4) == 0)
      return offset;
  }
  return {};
}

int32_t GetDirectInteger(const CPDF_Dictionary* pDict, const ByteString& key) {
  const CPDF_Number* pObj = ToNumber(pDict->GetObjectFor(key));
  return pObj ? pObj->GetInteger() : 0;
}

ByteString PDF_NameDecode(ByteStringView bstr) {
  if (!bstr.Contains('#'))
    return ByteString(bstr);

  size_t src_size = bstr.GetLength();
  size_t out_index = 0;
  ByteString result;
  {
    // Span's lifetime must end before ReleaseBuffer() below.
    pdfium::span<char> pDest = result.GetBuffer(src_size);
    for (size_t i = 0; i < src_size; i++) {
      if (bstr[i] == '#' && i + 2 < src_size) {
        pDest[out_index++] = FXSYS_HexCharToInt(bstr[i + 1]) * 16 +
                             FXSYS_HexCharToInt(bstr[i + 2]);
        i += 2;
      } else {
        pDest[out_index++] = bstr[i];
      }
    }
  }
  result.ReleaseBuffer(out_index);
  return result;
}

ByteString PDF_NameEncode(const ByteString& orig) {
  const uint8_t* src_buf = reinterpret_cast<const uint8_t*>(orig.c_str());
  int src_len = orig.GetLength();
  int dest_len = 0;
  int i;
  for (i = 0; i < src_len; i++) {
    uint8_t ch = src_buf[i];
    if (ch >= 0x80 || PDFCharIsWhitespace(ch) || ch == '#' ||
        PDFCharIsDelimiter(ch)) {
      dest_len += 3;
    } else {
      dest_len++;
    }
  }
  if (dest_len == src_len)
    return orig;

  ByteString res;
  {
    // Span's lifetime must end before ReleaseBuffer() below.
    pdfium::span<char> dest_buf = res.GetBuffer(dest_len);
    dest_len = 0;
    for (i = 0; i < src_len; i++) {
      uint8_t ch = src_buf[i];
      if (ch >= 0x80 || PDFCharIsWhitespace(ch) || ch == '#' ||
          PDFCharIsDelimiter(ch)) {
        dest_buf[dest_len++] = '#';
        FXSYS_IntToTwoHexChars(ch, &dest_buf[dest_len]);
        dest_len += 2;
        continue;
      }
      dest_buf[dest_len++] = ch;
    }
  }
  res.ReleaseBuffer(dest_len);
  return res;
}

std::ostream& operator<<(std::ostream& buf, const CPDF_Object* pObj) {
  if (!pObj) {
    buf << " null";
    return buf;
  }
  switch (pObj->GetType()) {
    case CPDF_Object::kNullobj:
      buf << " null";
      break;
    case CPDF_Object::kBoolean:
    case CPDF_Object::kNumber:
      buf << " " << pObj->GetString();
      break;
    case CPDF_Object::kString:
      buf << PDF_EncodeString(pObj->GetString(), pObj->AsString()->IsHex());
      break;
    case CPDF_Object::kName: {
      ByteString str = pObj->GetString();
      buf << "/" << PDF_NameEncode(str);
      break;
    }
    case CPDF_Object::kReference: {
      buf << " " << pObj->AsReference()->GetRefObjNum() << " 0 R ";
      break;
    }
    case CPDF_Object::kArray: {
      const CPDF_Array* p = pObj->AsArray();
      buf << "[";
      for (size_t i = 0; i < p->size(); i++) {
        const CPDF_Object* pElement = p->GetObjectAt(i);
        if (pElement && !pElement->IsInline()) {
          buf << " " << pElement->GetObjNum() << " 0 R";
        } else {
          buf << pElement;
        }
      }
      buf << "]";
      break;
    }
    case CPDF_Object::kDictionary: {
      CPDF_DictionaryLocker locker(pObj->AsDictionary());
      buf << "<<";
      for (const auto& it : locker) {
        const ByteString& key = it.first;
        CPDF_Object* pValue = it.second.get();
        buf << "/" << PDF_NameEncode(key);
        if (pValue && !pValue->IsInline()) {
          buf << " " << pValue->GetObjNum() << " 0 R ";
        } else {
          buf << pValue;
        }
      }
      buf << ">>";
      break;
    }
    case CPDF_Object::kStream: {
      const CPDF_Stream* p = pObj->AsStream();
      buf << p->GetDict() << "stream\r\n";
      auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(p);
      pAcc->LoadAllDataRaw();
      buf.write(reinterpret_cast<const char*>(pAcc->GetData()),
                pAcc->GetSize());
      buf << "\r\nendstream";
      break;
    }
    default:
      NOTREACHED();
      break;
  }
  return buf;
}
