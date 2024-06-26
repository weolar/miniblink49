// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_CFX_FOLDERFONTINFO_H_
#define CORE_FXGE_CFX_FOLDERFONTINFO_H_

#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/cfx_fontmapper.h"
#include "core/fxge/systemfontinfo_iface.h"

class CFX_FolderFontInfo : public SystemFontInfoIface {
 public:
  CFX_FolderFontInfo();
  ~CFX_FolderFontInfo() override;

  void AddPath(const ByteString& path);

  // IFX_SytemFontInfo:
  bool EnumFontList(CFX_FontMapper* pMapper) override;
  void* MapFont(int weight,
                bool bItalic,
                int charset,
                int pitch_family,
                const char* face) override;
#ifdef PDF_ENABLE_XFA
  void* MapFontByUnicode(uint32_t dwUnicode,
                         int weight,
                         bool bItalic,
                         int pitch_family) override;
#endif  // PDF_ENABLE_XFA
  void* GetFont(const char* face) override;
  uint32_t GetFontData(void* hFont,
                       uint32_t table,
                       uint8_t* buffer,
                       uint32_t size) override;
  void DeleteFont(void* hFont) override;
  bool GetFaceName(void* hFont, ByteString* name) override;
  bool GetFontCharset(void* hFont, int* charset) override;

 protected:
  class FontFaceInfo {
   public:
    FontFaceInfo(ByteString filePath,
                 ByteString faceName,
                 ByteString fontTables,
                 uint32_t fontOffset,
                 uint32_t fileSize);

    const ByteString m_FilePath;
    const ByteString m_FaceName;
    const ByteString m_FontTables;
    const uint32_t m_FontOffset;
    const uint32_t m_FileSize;
    uint32_t m_Styles;
    uint32_t m_Charsets;
  };

  void ScanPath(const ByteString& path);
  void ScanFile(const ByteString& path);
  void ReportFace(const ByteString& path,
                  FILE* pFile,
                  uint32_t filesize,
                  uint32_t offset);
  void* GetSubstFont(const ByteString& face);
  void* FindFont(int weight,
                 bool bItalic,
                 int charset,
                 int pitch_family,
                 const char* family,
                 bool bMatchName);

  std::map<ByteString, std::unique_ptr<FontFaceInfo>> m_FontList;
  std::vector<ByteString> m_PathList;
  UnownedPtr<CFX_FontMapper> m_pMapper;
};

#endif  // CORE_FXGE_CFX_FOLDERFONTINFO_H_
