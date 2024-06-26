// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_CPDF_MODULEMGR_H_
#define CORE_FPDFAPI_CPDF_MODULEMGR_H_

#include <memory>
#include <utility>

class CCodec_FaxModule;
class CCodec_FlateModule;
class CCodec_IccModule;
class CCodec_Jbig2Module;
class CCodec_JpegModule;
class CCodec_JpxModule;
class CCodec_ModuleMgr;
class CPDF_PageModule;

class CFSDK_UnsupportInfo_Adapter {
 public:
  explicit CFSDK_UnsupportInfo_Adapter(void* unsp_info)
      : m_unsp_info(unsp_info) {}

  void* GetUnspInfo() const { return m_unsp_info; }

 private:
  void* const m_unsp_info;
};

class CPDF_ModuleMgr {
 public:
  static CPDF_ModuleMgr* Get();
  static void Destroy();
  static const int kFileBufSize = 512;

  void Init();

  void SetUnsupportInfoAdapter(
      std::unique_ptr<CFSDK_UnsupportInfo_Adapter> pAdapter) {
    m_pUnsupportInfoAdapter = std::move(pAdapter);
  }
  CFSDK_UnsupportInfo_Adapter* GetUnsupportInfoAdapter() const {
    return m_pUnsupportInfoAdapter.get();
  }

  CCodec_ModuleMgr* GetCodecModule() const { return m_pCodecModule.get(); }
  CPDF_PageModule* GetPageModule() const { return m_pPageModule.get(); }

  CCodec_FaxModule* GetFaxModule();
  CCodec_JpegModule* GetJpegModule();
  CCodec_JpxModule* GetJpxModule();
  CCodec_Jbig2Module* GetJbig2Module();
  CCodec_IccModule* GetIccModule();
  CCodec_FlateModule* GetFlateModule();

 private:
  CPDF_ModuleMgr();
  ~CPDF_ModuleMgr();

  void InitPageModule();
  void InitCodecModule();
  void LoadCodecModules();
  void LoadEmbeddedMaps();
  void LoadEmbeddedGB1CMaps();
  void LoadEmbeddedCNS1CMaps();
  void LoadEmbeddedJapan1CMaps();
  void LoadEmbeddedKorea1CMaps();

  std::unique_ptr<CCodec_ModuleMgr> m_pCodecModule;
  std::unique_ptr<CPDF_PageModule> m_pPageModule;
  std::unique_ptr<CFSDK_UnsupportInfo_Adapter> m_pUnsupportInfoAdapter;
};

#endif  // CORE_FPDFAPI_CPDF_MODULEMGR_H_
