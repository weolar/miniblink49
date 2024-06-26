// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/win32/cfx_psrenderer.h"

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fxcodec/codec/ccodec_basicmodule.h"
#include "core/fxcodec/codec/ccodec_faxmodule.h"
#include "core/fxcodec/codec/ccodec_flatemodule.h"
#include "core/fxcodec/codec/ccodec_jpegmodule.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxge/cfx_facecache.h"
#include "core/fxge/cfx_fontcache.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/dib/cfx_dibextractor.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_dib.h"
#include "core/fxge/win32/cpsoutput.h"
#include "third_party/base/ptr_util.h"

namespace {

bool FaxCompressData(std::unique_ptr<uint8_t, FxFreeDeleter> src_buf,
                     int width,
                     int height,
                     std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                     uint32_t* dest_size) {
  if (width * height <= 128) {
    *dest_buf = std::move(src_buf);
    *dest_size = (width + 7) / 8 * height;
    return false;
  }

  CCodec_FaxModule::FaxEncode(src_buf.get(), width, height, (width + 7) / 8,
                              dest_buf, dest_size);
  return true;
}

void PSCompressData(int PSLevel,
                    uint8_t* src_buf,
                    uint32_t src_size,
                    uint8_t** output_buf,
                    uint32_t* output_size,
                    const char** filter) {
  *output_buf = src_buf;
  *output_size = src_size;
  *filter = "";
  if (src_size < 1024)
    return;

  CCodec_ModuleMgr* pEncoders = CPDF_ModuleMgr::Get()->GetCodecModule();
  uint8_t* dest_buf = nullptr;
  uint32_t dest_size = src_size;
  if (PSLevel >= 3) {
    std::unique_ptr<uint8_t, FxFreeDeleter> dest_buf_unique;
    if (pEncoders->GetFlateModule()->Encode(src_buf, src_size, &dest_buf_unique,
                                            &dest_size)) {
      dest_buf = dest_buf_unique.release();
      *filter = "/FlateDecode filter ";
    }
  } else {
    std::unique_ptr<uint8_t, FxFreeDeleter> dest_buf_unique;
    if (pEncoders->GetBasicModule()->RunLengthEncode(
            {src_buf, src_size}, &dest_buf_unique, &dest_size)) {
      dest_buf = dest_buf_unique.release();
      *filter = "/RunLengthDecode filter ";
    }
  }
  if (dest_size < src_size) {
    *output_buf = dest_buf;
    *output_size = dest_size;
  } else {
    *filter = nullptr;
    FX_Free(dest_buf);
  }
}

}  // namespace

struct PSGlyph {
  UnownedPtr<CFX_Font> m_pFont;
  uint32_t m_GlyphIndex;
  bool m_bGlyphAdjust;
  float m_AdjustMatrix[4];
};

class CPSFont {
 public:
  int m_nGlyphs;
  PSGlyph m_Glyphs[256];
};

CFX_PSRenderer::CFX_PSRenderer()
    : m_pStream(nullptr),
      m_bGraphStateSet(false),
      m_bColorSet(false),
      m_bInited(false) {}

CFX_PSRenderer::~CFX_PSRenderer() {}

void CFX_PSRenderer::Init(const RetainPtr<IFX_RetainableWriteStream>& pStream,
                          int pslevel,
                          int width,
                          int height,
                          bool bCmykOutput) {
  m_PSLevel = pslevel;
  m_pStream = pStream;
  m_ClipBox.left = 0;
  m_ClipBox.top = 0;
  m_ClipBox.right = width;
  m_ClipBox.bottom = height;
  m_bCmykOutput = bCmykOutput;
}

bool CFX_PSRenderer::StartRendering() {
  if (m_bInited)
    return true;

  static const char init_str[] =
      "\nsave\n/im/initmatrix load def\n"
      "/n/newpath load def/m/moveto load def/l/lineto load def/c/curveto load "
      "def/h/closepath load def\n"
      "/f/fill load def/F/eofill load def/s/stroke load def/W/clip load "
      "def/W*/eoclip load def\n"
      "/rg/setrgbcolor load def/k/setcmykcolor load def\n"
      "/J/setlinecap load def/j/setlinejoin load def/w/setlinewidth load "
      "def/M/setmiterlimit load def/d/setdash load def\n"
      "/q/gsave load def/Q/grestore load def/iM/imagemask load def\n"
      "/Tj/show load def/Ff/findfont load def/Fs/scalefont load def/Sf/setfont "
      "load def\n"
      "/cm/concat load def/Cm/currentmatrix load def/mx/matrix load "
      "def/sm/setmatrix load def\n";
  m_pStream->WriteString(init_str);
  m_bInited = true;
  return true;
}

void CFX_PSRenderer::EndRendering() {
  if (!m_bInited)
    return;

  m_pStream->WriteString("\nrestore\n");
  m_bInited = false;
}

void CFX_PSRenderer::SaveState() {
  StartRendering();
  m_pStream->WriteString("q\n");
  m_ClipBoxStack.push_back(m_ClipBox);
}

void CFX_PSRenderer::RestoreState(bool bKeepSaved) {
  StartRendering();
  m_pStream->WriteString("Q\n");
  if (bKeepSaved)
    m_pStream->WriteString("q\n");

  m_bColorSet = false;
  m_bGraphStateSet = false;
  if (m_ClipBoxStack.empty())
    return;

  m_ClipBox = m_ClipBoxStack.back();
  if (!bKeepSaved)
    m_ClipBoxStack.pop_back();
}

void CFX_PSRenderer::OutputPath(const CFX_PathData* pPathData,
                                const CFX_Matrix* pObject2Device) {
  std::ostringstream buf;
  size_t size = pPathData->GetPoints().size();

  for (size_t i = 0; i < size; i++) {
    FXPT_TYPE type = pPathData->GetType(i);
    bool closing = pPathData->IsClosingFigure(i);
    CFX_PointF pos = pPathData->GetPoint(i);
    if (pObject2Device)
      pos = pObject2Device->Transform(pos);

    buf << pos.x << " " << pos.y;
    switch (type) {
      case FXPT_TYPE::MoveTo:
        buf << " m ";
        break;
      case FXPT_TYPE::LineTo:
        buf << " l ";
        if (closing)
          buf << "h ";
        break;
      case FXPT_TYPE::BezierTo: {
        CFX_PointF pos1 = pPathData->GetPoint(i + 1);
        CFX_PointF pos2 = pPathData->GetPoint(i + 2);
        if (pObject2Device) {
          pos1 = pObject2Device->Transform(pos1);
          pos2 = pObject2Device->Transform(pos2);
        }
        buf << " " << pos1.x << " " << pos1.y << " " << pos2.x << " " << pos2.y
            << " c";
        if (closing)
          buf << " h";
        buf << "\n";
        i += 2;
        break;
      }
    }
  }
  WriteToStream(&buf);
}

void CFX_PSRenderer::SetClip_PathFill(const CFX_PathData* pPathData,
                                      const CFX_Matrix* pObject2Device,
                                      int fill_mode) {
  StartRendering();
  OutputPath(pPathData, pObject2Device);
  CFX_FloatRect rect = pPathData->GetBoundingBox();
  if (pObject2Device)
    rect = pObject2Device->TransformRect(rect);

  m_ClipBox.left = static_cast<int>(rect.left);
  m_ClipBox.right = static_cast<int>(rect.left + rect.right);
  m_ClipBox.top = static_cast<int>(rect.top + rect.bottom);
  m_ClipBox.bottom = static_cast<int>(rect.bottom);

  m_pStream->WriteString("W");
  if ((fill_mode & 3) != FXFILL_WINDING)
    m_pStream->WriteString("*");
  m_pStream->WriteString(" n\n");
}

void CFX_PSRenderer::SetClip_PathStroke(const CFX_PathData* pPathData,
                                        const CFX_Matrix* pObject2Device,
                                        const CFX_GraphStateData* pGraphState) {
  StartRendering();
  SetGraphState(pGraphState);

  std::ostringstream buf;
  buf << "mx Cm [" << pObject2Device->a << " " << pObject2Device->b << " "
      << pObject2Device->c << " " << pObject2Device->d << " "
      << pObject2Device->e << " " << pObject2Device->f << "]cm ";
  WriteToStream(&buf);

  OutputPath(pPathData, nullptr);
  CFX_FloatRect rect = pPathData->GetBoundingBox(pGraphState->m_LineWidth,
                                                 pGraphState->m_MiterLimit);
  m_ClipBox.Intersect(pObject2Device->TransformRect(rect).GetOuterRect());

  m_pStream->WriteString("strokepath W n sm\n");
}

bool CFX_PSRenderer::DrawPath(const CFX_PathData* pPathData,
                              const CFX_Matrix* pObject2Device,
                              const CFX_GraphStateData* pGraphState,
                              uint32_t fill_color,
                              uint32_t stroke_color,
                              int fill_mode) {
  StartRendering();
  int fill_alpha = FXARGB_A(fill_color);
  int stroke_alpha = FXARGB_A(stroke_color);
  if (fill_alpha && fill_alpha < 255)
    return false;
  if (stroke_alpha && stroke_alpha < 255)
    return false;
  if (fill_alpha == 0 && stroke_alpha == 0)
    return false;

  if (stroke_alpha) {
    SetGraphState(pGraphState);
    if (pObject2Device) {
      std::ostringstream buf;
      buf << "mx Cm [" << pObject2Device->a << " " << pObject2Device->b << " "
          << pObject2Device->c << " " << pObject2Device->d << " "
          << pObject2Device->e << " " << pObject2Device->f << "]cm ";
      WriteToStream(&buf);
    }
  }

  OutputPath(pPathData, stroke_alpha ? nullptr : pObject2Device);
  if (fill_mode && fill_alpha) {
    SetColor(fill_color);
    if ((fill_mode & 3) == FXFILL_WINDING) {
      if (stroke_alpha)
        m_pStream->WriteString("q f Q ");
      else
        m_pStream->WriteString("f");
    } else if ((fill_mode & 3) == FXFILL_ALTERNATE) {
      if (stroke_alpha)
        m_pStream->WriteString("q F Q ");
      else
        m_pStream->WriteString("F");
    }
  }

  if (stroke_alpha) {
    SetColor(stroke_color);
    m_pStream->WriteString("s");
    if (pObject2Device)
      m_pStream->WriteString(" sm");
  }

  m_pStream->WriteString("\n");
  return true;
}

void CFX_PSRenderer::SetGraphState(const CFX_GraphStateData* pGraphState) {
  std::ostringstream buf;
  if (!m_bGraphStateSet ||
      m_CurGraphState.m_LineCap != pGraphState->m_LineCap) {
    buf << pGraphState->m_LineCap << " J\n";
  }
  if (!m_bGraphStateSet ||
      m_CurGraphState.m_DashArray != pGraphState->m_DashArray) {
    buf << "[";
    for (const auto& dash : pGraphState->m_DashArray)
      buf << dash << " ";
    buf << "]" << pGraphState->m_DashPhase << " d\n";
  }
  if (!m_bGraphStateSet ||
      m_CurGraphState.m_LineJoin != pGraphState->m_LineJoin) {
    buf << pGraphState->m_LineJoin << " j\n";
  }
  if (!m_bGraphStateSet ||
      m_CurGraphState.m_LineWidth != pGraphState->m_LineWidth) {
    buf << pGraphState->m_LineWidth << " w\n";
  }
  if (!m_bGraphStateSet ||
      m_CurGraphState.m_MiterLimit != pGraphState->m_MiterLimit) {
    buf << pGraphState->m_MiterLimit << " M\n";
  }
  m_CurGraphState = *pGraphState;
  m_bGraphStateSet = true;
  WriteToStream(&buf);
}

bool CFX_PSRenderer::SetDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                               uint32_t color,
                               int left,
                               int top) {
  StartRendering();
  CFX_Matrix matrix = CFX_RenderDevice::GetFlipMatrix(
      pSource->GetWidth(), pSource->GetHeight(), left, top);
  return DrawDIBits(pSource, color, matrix, FXDIB_ResampleOptions());
}

bool CFX_PSRenderer::StretchDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                                   uint32_t color,
                                   int dest_left,
                                   int dest_top,
                                   int dest_width,
                                   int dest_height,
                                   const FXDIB_ResampleOptions& options) {
  StartRendering();
  CFX_Matrix matrix = CFX_RenderDevice::GetFlipMatrix(dest_width, dest_height,
                                                      dest_left, dest_top);
  return DrawDIBits(pSource, color, matrix, options);
}

bool CFX_PSRenderer::DrawDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                                uint32_t color,
                                const CFX_Matrix& matrix,
                                const FXDIB_ResampleOptions& options) {
  StartRendering();
  if ((matrix.a == 0 && matrix.b == 0) || (matrix.c == 0 && matrix.d == 0))
    return true;

  if (pSource->HasAlpha())
    return false;

  int alpha = FXARGB_A(color);
  if (pSource->IsAlphaMask() && (alpha < 255 || pSource->GetBPP() != 1))
    return false;

  m_pStream->WriteString("q\n");

  std::ostringstream buf;
  buf << "[" << matrix.a << " " << matrix.b << " " << matrix.c << " "
      << matrix.d << " " << matrix.e << " " << matrix.f << "]cm ";

  int width = pSource->GetWidth();
  int height = pSource->GetHeight();
  buf << width << " " << height;

  if (pSource->GetBPP() == 1 && !pSource->GetPalette()) {
    int pitch = (width + 7) / 8;
    uint32_t src_size = height * pitch;
    std::unique_ptr<uint8_t, FxFreeDeleter> src_buf(
        FX_Alloc(uint8_t, src_size));
    for (int row = 0; row < height; row++) {
      const uint8_t* src_scan = pSource->GetScanline(row);
      memcpy(src_buf.get() + row * pitch, src_scan, pitch);
    }

    std::unique_ptr<uint8_t, FxFreeDeleter> output_buf;
    uint32_t output_size;
    bool compressed = FaxCompressData(std::move(src_buf), width, height,
                                      &output_buf, &output_size);
    if (pSource->IsAlphaMask()) {
      SetColor(color);
      m_bColorSet = false;
      buf << " true[";
    } else {
      buf << " 1[";
    }
    buf << width << " 0 0 -" << height << " 0 " << height
        << "]currentfile/ASCII85Decode filter ";

    if (compressed) {
      buf << "<</K -1/EndOfBlock false/Columns " << width << "/Rows " << height
          << ">>/CCITTFaxDecode filter ";
    }
    if (pSource->IsAlphaMask())
      buf << "iM\n";
    else
      buf << "false 1 colorimage\n";

    WriteToStream(&buf);
    WritePSBinary(output_buf.get(), output_size);
  } else {
    CFX_DIBExtractor source_extractor(pSource);
    RetainPtr<CFX_DIBBase> pConverted = source_extractor.GetBitmap();
    if (!pConverted)
      return false;
    switch (pSource->GetFormat()) {
      case FXDIB_1bppRgb:
      case FXDIB_Rgb32:
        pConverted = pConverted->CloneConvert(FXDIB_Rgb);
        break;
      case FXDIB_8bppRgb:
        if (pSource->GetPalette()) {
          pConverted = pConverted->CloneConvert(FXDIB_Rgb);
        }
        break;
      case FXDIB_1bppCmyk:
        pConverted = pConverted->CloneConvert(FXDIB_Cmyk);
        break;
      case FXDIB_8bppCmyk:
        if (pSource->GetPalette()) {
          pConverted = pConverted->CloneConvert(FXDIB_Cmyk);
        }
        break;
      default:
        break;
    }
    if (!pConverted) {
      m_pStream->WriteString("\nQ\n");
      return false;
    }

    int bpp = pConverted->GetBPP() / 8;
    uint8_t* output_buf = nullptr;
    size_t output_size = 0;
    const char* filter = nullptr;
    if ((m_PSLevel == 2 || options.bLossy) &&
        CCodec_JpegModule::JpegEncode(pConverted, &output_buf, &output_size)) {
      filter = "/DCTDecode filter ";
    }
    if (!filter) {
      int src_pitch = width * bpp;
      output_size = height * src_pitch;
      output_buf = FX_Alloc(uint8_t, output_size);
      for (int row = 0; row < height; row++) {
        const uint8_t* src_scan = pConverted->GetScanline(row);
        uint8_t* dest_scan = output_buf + row * src_pitch;
        if (bpp == 3) {
          for (int col = 0; col < width; col++) {
            *dest_scan++ = src_scan[2];
            *dest_scan++ = src_scan[1];
            *dest_scan++ = *src_scan;
            src_scan += 3;
          }
        } else {
          memcpy(dest_scan, src_scan, src_pitch);
        }
      }
      uint8_t* compressed_buf;
      uint32_t compressed_size;
      PSCompressData(m_PSLevel, output_buf, output_size, &compressed_buf,
                     &compressed_size, &filter);
      if (output_buf != compressed_buf)
        FX_Free(output_buf);

      output_buf = compressed_buf;
      output_size = compressed_size;
    }
    buf << " 8[";
    buf << width << " 0 0 -" << height << " 0 " << height << "]";
    buf << "currentfile/ASCII85Decode filter ";
    if (filter)
      buf << filter;

    buf << "false " << bpp;
    buf << " colorimage\n";
    WriteToStream(&buf);

    WritePSBinary(output_buf, output_size);
    FX_Free(output_buf);
  }
  m_pStream->WriteString("\nQ\n");
  return true;
}

void CFX_PSRenderer::SetColor(uint32_t color) {
  bool bCMYK = false;
  if (bCMYK != m_bCmykOutput || !m_bColorSet || m_LastColor != color) {
    std::ostringstream buf;
    if (bCMYK) {
      buf << FXSYS_GetCValue(color) / 255.0 << " "
          << FXSYS_GetMValue(color) / 255.0 << " "
          << FXSYS_GetYValue(color) / 255.0 << " "
          << FXSYS_GetKValue(color) / 255.0 << " k\n";
    } else {
      buf << FXARGB_R(color) / 255.0 << " " << FXARGB_G(color) / 255.0 << " "
          << FXARGB_B(color) / 255.0 << " rg\n";
    }
    if (bCMYK == m_bCmykOutput) {
      m_bColorSet = true;
      m_LastColor = color;
    }
    WriteToStream(&buf);
  }
}

void CFX_PSRenderer::FindPSFontGlyph(CFX_FaceCache* pFaceCache,
                                     CFX_Font* pFont,
                                     const FXTEXT_CHARPOS& charpos,
                                     int* ps_fontnum,
                                     int* ps_glyphindex) {
  int i = 0;
  for (const auto& pPSFont : m_PSFontList) {
    for (int j = 0; j < pPSFont->m_nGlyphs; j++) {
      if (pPSFont->m_Glyphs[j].m_pFont == pFont &&
          pPSFont->m_Glyphs[j].m_GlyphIndex == charpos.m_GlyphIndex &&
          ((!pPSFont->m_Glyphs[j].m_bGlyphAdjust && !charpos.m_bGlyphAdjust) ||
           (pPSFont->m_Glyphs[j].m_bGlyphAdjust && charpos.m_bGlyphAdjust &&
            (fabs(pPSFont->m_Glyphs[j].m_AdjustMatrix[0] -
                  charpos.m_AdjustMatrix[0]) < 0.01 &&
             fabs(pPSFont->m_Glyphs[j].m_AdjustMatrix[1] -
                  charpos.m_AdjustMatrix[1]) < 0.01 &&
             fabs(pPSFont->m_Glyphs[j].m_AdjustMatrix[2] -
                  charpos.m_AdjustMatrix[2]) < 0.01 &&
             fabs(pPSFont->m_Glyphs[j].m_AdjustMatrix[3] -
                  charpos.m_AdjustMatrix[3]) < 0.01)))) {
        *ps_fontnum = i;
        *ps_glyphindex = j;
        return;
      }
    }
    ++i;
  }

  if (m_PSFontList.empty() || m_PSFontList.back()->m_nGlyphs == 256) {
    m_PSFontList.push_back(pdfium::MakeUnique<CPSFont>());
    m_PSFontList.back()->m_nGlyphs = 0;
    std::ostringstream buf;
    buf << "8 dict begin/FontType 3 def/FontMatrix[1 0 0 1 0 0]def\n"
           "/FontBBox[0 0 0 0]def/Encoding 256 array def 0 1 255{Encoding "
           "exch/.notdef put}for\n"
           "/CharProcs 1 dict def CharProcs begin/.notdef {} def end\n"
           "/BuildGlyph{1 0 -10 -10 10 10 setcachedevice exch/CharProcs get "
           "exch 2 copy known not{pop/.notdef}if get exec}bind def\n"
           "/BuildChar{1 index/Encoding get exch get 1 index/BuildGlyph get "
           "exec}bind def\n"
           "currentdict end\n";
    buf << "/X" << static_cast<uint32_t>(m_PSFontList.size() - 1)
        << " exch definefont pop\n";
    WriteToStream(&buf);
    buf.str("");
  }

  *ps_fontnum = m_PSFontList.size() - 1;
  CPSFont* pPSFont = m_PSFontList[*ps_fontnum].get();
  int glyphindex = pPSFont->m_nGlyphs;
  *ps_glyphindex = glyphindex;
  pPSFont->m_Glyphs[glyphindex].m_GlyphIndex = charpos.m_GlyphIndex;
  pPSFont->m_Glyphs[glyphindex].m_pFont = pFont;
  pPSFont->m_Glyphs[glyphindex].m_bGlyphAdjust = charpos.m_bGlyphAdjust;
  if (charpos.m_bGlyphAdjust) {
    pPSFont->m_Glyphs[glyphindex].m_AdjustMatrix[0] = charpos.m_AdjustMatrix[0];
    pPSFont->m_Glyphs[glyphindex].m_AdjustMatrix[1] = charpos.m_AdjustMatrix[1];
    pPSFont->m_Glyphs[glyphindex].m_AdjustMatrix[2] = charpos.m_AdjustMatrix[2];
    pPSFont->m_Glyphs[glyphindex].m_AdjustMatrix[3] = charpos.m_AdjustMatrix[3];
  }
  pPSFont->m_nGlyphs++;

  CFX_Matrix matrix;
  if (charpos.m_bGlyphAdjust) {
    matrix =
        CFX_Matrix(charpos.m_AdjustMatrix[0], charpos.m_AdjustMatrix[1],
                   charpos.m_AdjustMatrix[2], charpos.m_AdjustMatrix[3], 0, 0);
  }
  const CFX_PathData* pPathData = pFaceCache->LoadGlyphPath(
      pFont, charpos.m_GlyphIndex, charpos.m_FontCharWidth);
  if (!pPathData)
    return;

  CFX_PathData TransformedPath(*pPathData);
  if (charpos.m_bGlyphAdjust)
    TransformedPath.Transform(matrix);

  std::ostringstream buf;
  buf << "/X" << *ps_fontnum << " Ff/CharProcs get begin/" << glyphindex
      << "{n ";
  for (size_t p = 0; p < TransformedPath.GetPoints().size(); p++) {
    CFX_PointF point = TransformedPath.GetPoint(p);
    switch (TransformedPath.GetType(p)) {
      case FXPT_TYPE::MoveTo: {
        buf << point.x << " " << point.y << " m\n";
        break;
      }
      case FXPT_TYPE::LineTo: {
        buf << point.x << " " << point.y << " l\n";
        break;
      }
      case FXPT_TYPE::BezierTo: {
        CFX_PointF point1 = TransformedPath.GetPoint(p + 1);
        CFX_PointF point2 = TransformedPath.GetPoint(p + 2);
        buf << point.x << " " << point.y << " " << point1.x << " " << point1.y
            << " " << point2.x << " " << point2.y << " c\n";
        p += 2;
        break;
      }
    }
  }
  buf << "f}bind def end\n";
  buf << "/X" << *ps_fontnum << " Ff/Encoding get " << glyphindex << "/"
      << glyphindex << " put\n";
  WriteToStream(&buf);
}

bool CFX_PSRenderer::DrawText(int nChars,
                              const FXTEXT_CHARPOS* pCharPos,
                              CFX_Font* pFont,
                              const CFX_Matrix* pObject2Device,
                              float font_size,
                              uint32_t color) {
  // Check object to device matrix first, since it can scale the font size.
  if ((pObject2Device->a == 0 && pObject2Device->b == 0) ||
      (pObject2Device->c == 0 && pObject2Device->d == 0)) {
    return true;
  }

  // Do not send near zero font sizes to printers. See crbug.com/767343.
  float scale =
      std::min(pObject2Device->GetXUnit(), pObject2Device->GetYUnit());
  static constexpr float kEpsilon = 0.01f;
  if (std::fabs(font_size * scale) < kEpsilon)
    return true;

  StartRendering();
  int alpha = FXARGB_A(color);
  if (alpha < 255)
    return false;

  SetColor(color);
  std::ostringstream buf;
  buf << "q[" << pObject2Device->a << " " << pObject2Device->b << " "
      << pObject2Device->c << " " << pObject2Device->d << " "
      << pObject2Device->e << " " << pObject2Device->f << "]cm\n";

  CFX_FontCache* pCache = CFX_GEModule::Get()->GetFontCache();
  CFX_FaceCache* pFaceCache = pCache->GetCachedFace(pFont);
  int last_fontnum = -1;
  for (int i = 0; i < nChars; i++) {
    int ps_fontnum, ps_glyphindex;
    FindPSFontGlyph(pFaceCache, pFont, pCharPos[i], &ps_fontnum,
                    &ps_glyphindex);
    if (last_fontnum != ps_fontnum) {
      buf << "/X" << ps_fontnum << " Ff " << font_size << " Fs Sf ";
      last_fontnum = ps_fontnum;
    }
    buf << pCharPos[i].m_Origin.x << " " << pCharPos[i].m_Origin.y << " m";
    ByteString hex = ByteString::Format("<%02X>", ps_glyphindex);
    buf << hex.AsStringView() << "Tj\n";
  }
  buf << "Q\n";
  WriteToStream(&buf);
  pCache->ReleaseCachedFace(pFont);
  return true;
}

void CFX_PSRenderer::WritePSBinary(const uint8_t* data, int len) {
  std::unique_ptr<uint8_t, FxFreeDeleter> dest_buf;
  uint32_t dest_size;
  CCodec_ModuleMgr* pEncoders = CPDF_ModuleMgr::Get()->GetCodecModule();
  if (pEncoders->GetBasicModule()->A85Encode({data, static_cast<size_t>(len)},
                                             &dest_buf, &dest_size)) {
    m_pStream->WriteBlock(dest_buf.get(), dest_size);
  } else {
    m_pStream->WriteBlock(data, len);
  }
}

void CFX_PSRenderer::WriteToStream(std::ostringstream* stringStream) {
  if (stringStream->tellp() > 0)
    m_pStream->WriteBlock(stringStream->str().c_str(), stringStream->tellp());
}
