// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_PROGRESSIVEDECODER_H_
#define CORE_FXCODEC_CODEC_CCODEC_PROGRESSIVEDECODER_H_

#include <memory>
#include <utility>
#include <vector>

#include "core/fxcodec/codec/ccodec_jpegmodule.h"
#include "core/fxcodec/fx_codec_def.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxcrt/unowned_ptr.h"
#include "core/fxge/fx_dib.h"

#ifdef PDF_ENABLE_XFA_BMP
#include "core/fxcodec/codec/ccodec_bmpmodule.h"
#endif  // PDF_ENABLE_XFA_BMP

#ifdef PDF_ENABLE_XFA_GIF
#include "core/fxcodec/codec/ccodec_gifmodule.h"
#endif  // PDF_ENABLE_XFA_GIF

#ifdef PDF_ENABLE_XFA_PNG
#include "core/fxcodec/codec/ccodec_pngmodule.h"
#endif  // PDF_ENABLE_XFA_PNG

#ifdef PDF_ENABLE_XFA_TIFF
#include "core/fxcodec/codec/ccodec_tiffmodule.h"
#endif  // PDF_ENABLE_XFA_TIFF

class CCodec_ModuleMgr;
class CFX_DIBAttribute;
class CFX_DIBitmap;
class IFX_SeekableReadStream;

class CCodec_Dummy {};  // Placeholder to work around C++ syntax issues

class CCodec_ProgressiveDecoder :
#ifdef PDF_ENABLE_XFA_BMP
    public CCodec_BmpModule::Delegate,
#endif  // PDF_ENABLE_XFA_BMP
#ifdef PDF_ENABLE_XFA_GIF
    public CCodec_GifModule::Delegate,
#endif  // PDF_ENABLE_XFA_GIF
#ifdef PDF_ENABLE_XFA_PNG
    public CCodec_PngModule::Delegate,
#endif  // PDF_ENABLE_XFA_PNG
    public CCodec_Dummy {
 public:
  enum FXCodec_Format {
    FXCodec_Invalid = 0,
    FXCodec_1bppGray = 0x101,
    FXCodec_1bppRgb = 0x001,
    FXCodec_8bppGray = 0x108,
    FXCodec_8bppRgb = 0x008,
    FXCodec_Rgb = 0x018,
    FXCodec_Rgb32 = 0x020,
    FXCodec_Argb = 0x220,
    FXCodec_Cmyk = 0x120
  };

  explicit CCodec_ProgressiveDecoder(CCodec_ModuleMgr* pCodecMgr);
  virtual ~CCodec_ProgressiveDecoder();

  FXCODEC_STATUS LoadImageInfo(const RetainPtr<IFX_SeekableReadStream>& pFile,
                               FXCODEC_IMAGE_TYPE imageType,
                               CFX_DIBAttribute* pAttribute,
                               bool bSkipImageTypeCheck);

  FXCODEC_IMAGE_TYPE GetType() const { return m_imageType; }
  int32_t GetWidth() const { return m_SrcWidth; }
  int32_t GetHeight() const { return m_SrcHeight; }
  int32_t GetNumComponents() const { return m_SrcComponents; }
  int32_t GetBPC() const { return m_SrcBPC; }
  void SetClipBox(FX_RECT* clip);

  std::pair<FXCODEC_STATUS, size_t> GetFrames();
  FXCODEC_STATUS StartDecode(const RetainPtr<CFX_DIBitmap>& pDIBitmap,
                             int start_x,
                             int start_y,
                             int size_x,
                             int size_y);

  FXCODEC_STATUS ContinueDecode();

  struct PixelWeight {
    int m_SrcStart;
    int m_SrcEnd;
    int m_Weights[1];
  };

  class CFXCODEC_WeightTable {
   public:
    CFXCODEC_WeightTable();
    ~CFXCODEC_WeightTable();

    void Calc(int dest_len,
              int dest_min,
              int dest_max,
              int src_len,
              int src_min,
              int src_max);
    PixelWeight* GetPixelWeight(int pixel) {
      return reinterpret_cast<PixelWeight*>(m_pWeightTables.data() +
                                            (pixel - m_DestMin) * m_ItemSize);
    }

    int m_DestMin;
    int m_ItemSize;
    std::vector<uint8_t> m_pWeightTables;
  };

  class CFXCODEC_HorzTable {
   public:
    CFXCODEC_HorzTable();
    ~CFXCODEC_HorzTable();

    void Calc(int dest_len, int src_len);
    PixelWeight* GetPixelWeight(int pixel) {
      return reinterpret_cast<PixelWeight*>(m_pWeightTables.data() +
                                            pixel * m_ItemSize);
    }

    int m_ItemSize;
    std::vector<uint8_t> m_pWeightTables;
  };

  class CFXCODEC_VertTable {
   public:
    CFXCODEC_VertTable();
    ~CFXCODEC_VertTable();

    void Calc(int dest_len, int src_len);
    PixelWeight* GetPixelWeight(int pixel) {
      return reinterpret_cast<PixelWeight*>(m_pWeightTables.data() +
                                            pixel * m_ItemSize);
    }
    int m_ItemSize;
    std::vector<uint8_t> m_pWeightTables;
  };

#ifdef PDF_ENABLE_XFA_PNG
  // CCodec_PngModule::Delegate
  bool PngReadHeader(int width,
                     int height,
                     int bpc,
                     int pass,
                     int* color_type,
                     double* gamma) override;
  bool PngAskScanlineBuf(int line, uint8_t** pSrcBuf) override;
  void PngFillScanlineBufCompleted(int pass, int line) override;
#endif  // PDF_ENABLE_XFA_PNG

#ifdef PDF_ENABLE_XFA_GIF
  // CCodec_GifModule::Delegate
  void GifRecordCurrentPosition(uint32_t& cur_pos) override;
  bool GifInputRecordPositionBuf(uint32_t rcd_pos,
                                 const FX_RECT& img_rc,
                                 int32_t pal_num,
                                 CFX_GifPalette* pal_ptr,
                                 int32_t delay_time,
                                 bool user_input,
                                 int32_t trans_index,
                                 int32_t disposal_method,
                                 bool interlace) override;
  void GifReadScanline(int32_t row_num, uint8_t* row_buf) override;
#endif  // PDF_ENABLE_XFA_GIF

#ifdef PDF_ENABLE_XFA_BMP
  // CCodec_BmpModule::Delegate
  bool BmpInputImagePositionBuf(uint32_t rcd_pos) override;
  void BmpReadScanline(uint32_t row_num,
                       const std::vector<uint8_t>& row_buf) override;
#endif  // PDF_ENABLE_XFA_BMP

 private:
#ifdef PDF_ENABLE_XFA_BMP
  bool BmpReadMoreData(CCodec_BmpModule* pBmpModule,
                       CodecModuleIface::Context* pBmpContext,
                       FXCODEC_STATUS& err_status);
  bool BmpDetectImageTypeInBuffer(CFX_DIBAttribute* pAttribute);
  FXCODEC_STATUS BmpStartDecode(const RetainPtr<CFX_DIBitmap>& pDIBitmap);
  FXCODEC_STATUS BmpContinueDecode();
#endif  // PDF_ENABLE_XFA_BMP

#ifdef PDF_ENABLE_XFA_GIF
  bool GifReadMoreData(CCodec_GifModule* pGifModule,
                       FXCODEC_STATUS& err_status);
  bool GifDetectImageTypeInBuffer(CFX_DIBAttribute* pAttribute);
  FXCODEC_STATUS GifStartDecode(const RetainPtr<CFX_DIBitmap>& pDIBitmap);
  FXCODEC_STATUS GifContinueDecode();
  void GifDoubleLineResampleVert(const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
                                 double scale_y,
                                 int dest_row);
#endif  // PDF_ENABLE_XFA_GIF

#ifdef PDF_ENABLE_XFA_PNG
  void PngOneOneMapResampleHorz(const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
                                int32_t dest_line,
                                uint8_t* src_scan,
                                FXCodec_Format src_format);
  bool PngDetectImageTypeInBuffer(CFX_DIBAttribute* pAttribute);
  FXCODEC_STATUS PngStartDecode(const RetainPtr<CFX_DIBitmap>& pDIBitmap);
  FXCODEC_STATUS PngContinueDecode();
#endif  // PDF_ENABLE_XFA_PNG

#ifdef PDF_ENABLE_XFA_TIFF
  bool TiffDetectImageTypeFromFile(CFX_DIBAttribute* pAttribute);
  FXCODEC_STATUS TiffContinueDecode();
#endif  // PDF_ENABLE_XFA_TIFF

  bool JpegReadMoreData(CCodec_JpegModule* pJpegModule,
                        FXCODEC_STATUS& err_status);
  bool JpegDetectImageTypeInBuffer(CFX_DIBAttribute* pAttribute);
  FXCODEC_STATUS JpegStartDecode(const RetainPtr<CFX_DIBitmap>& pDIBitmap);
  FXCODEC_STATUS JpegContinueDecode();

  bool DetectImageType(FXCODEC_IMAGE_TYPE imageType,
                       CFX_DIBAttribute* pAttribute);
  bool ReadMoreData(CodecModuleIface* pModule,
                    CodecModuleIface::Context* pContext,
                    bool invalidate_buffer,
                    FXCODEC_STATUS& err_status);

  void GetDownScale(int& down_scale);
  void GetTransMethod(FXDIB_Format dest_format, FXCodec_Format src_format);

  void ReSampleScanline(const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
                        int32_t dest_line,
                        uint8_t* src_scan,
                        FXCodec_Format src_format);
  void Resample(const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
                int32_t src_line,
                uint8_t* src_scan,
                FXCodec_Format src_format);
  void ResampleVert(const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
                    double scale_y,
                    int dest_row);
  void ResampleVertBT(const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
                      double scale_y,
                      int dest_row);

  FXCODEC_STATUS m_status = FXCODEC_STATUS_DECODE_FINISH;
  FXCODEC_IMAGE_TYPE m_imageType = FXCODEC_IMAGE_UNKNOWN;
  RetainPtr<IFX_SeekableReadStream> m_pFile;
  RetainPtr<CFX_DIBitmap> m_pDeviceBitmap;
  UnownedPtr<CCodec_ModuleMgr> m_pCodecMgr;
  RetainPtr<CFX_CodecMemory> m_pCodecMemory;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pDecodeBuf;
  std::unique_ptr<FX_ARGB, FxFreeDeleter> m_pSrcPalette;
  std::unique_ptr<CodecModuleIface::Context> m_pJpegContext;
#ifdef PDF_ENABLE_XFA_BMP
  std::unique_ptr<CodecModuleIface::Context> m_pBmpContext;
#endif  // PDF_ENABLE_XFA_BMP
#ifdef PDF_ENABLE_XFA_GIF
  std::unique_ptr<CodecModuleIface::Context> m_pGifContext;
#endif  // PDF_ENABLE_XFA_GIF
#ifdef PDF_ENABLE_XFA_PNG
  std::unique_ptr<CodecModuleIface::Context> m_pPngContext;
#endif  // PDF_ENABLE_XFA_PNG
#ifdef PDF_ENABLE_XFA_TIFF
  std::unique_ptr<CodecModuleIface::Context> m_pTiffContext;
#endif  // PDF_ENABLE_XFA_TIFF
  uint32_t m_offSet = 0;
  int m_ScanlineSize = 0;
  CFXCODEC_WeightTable m_WeightHorz;
  CFXCODEC_VertTable m_WeightVert;
  CFXCODEC_HorzTable m_WeightHorzOO;
  int m_SrcWidth = 0;
  int m_SrcHeight = 0;
  int m_SrcComponents = 0;
  int m_SrcBPC = 0;
  FX_RECT m_clipBox;
  int m_startX = 0;
  int m_startY = 0;
  int m_sizeX = 0;
  int m_sizeY = 0;
  int m_TransMethod = -1;
  int m_SrcPaletteNumber = 0;
  int m_SrcRow = 0;
  FXCodec_Format m_SrcFormat = FXCodec_Invalid;
  int m_SrcPassNumber = 0;
  size_t m_FrameNumber = 0;
  size_t m_FrameCur = 0;
#ifdef PDF_ENABLE_XFA_GIF
  int m_GifBgIndex = 0;
  CFX_GifPalette* m_pGifPalette = nullptr;
  int32_t m_GifPltNumber = 0;
  int m_GifTransIndex = -1;
  FX_RECT m_GifFrameRect;
  bool m_InvalidateGifBuffer = true;
#endif  // PDF_ENABLE_XFA_GIF
#ifdef PDF_ENABLE_XFA_BMP
  bool m_BmpIsTopBottom = false;
#endif  // PDF_ENABLE_XFA_BMP
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_PROGRESSIVEDECODER_H_
