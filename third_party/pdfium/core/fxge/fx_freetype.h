// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_FX_FREETYPE_H_
#define CORE_FXGE_FX_FREETYPE_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_LCD_FILTER_H
#include FT_MULTIPLE_MASTERS_H
#include FT_OUTLINE_H
#include FT_TRUETYPE_TABLES_H

using FXFT_FaceRec = struct FT_FaceRec_;
using FXFT_Face = FXFT_FaceRec*;
using FXFT_Library = void*;

using FXFT_MM_Var = FT_MM_Var*;
using FXFT_Open_Args = FT_Open_Args;
using FXFT_StreamRec = FT_StreamRec;
using FXFT_Stream = FT_StreamRec*;
using FXFT_BBox = FT_BBox;
using FXFT_Glyph = FT_Glyph;
using FXFT_CharMap = FT_CharMap;

using FXFT_Matrix = FT_Matrix;
using FXFT_Vector = FT_Vector;
using FXFT_Outline_Funcs = FT_Outline_Funcs;

#define FXFT_ENCODING_UNICODE FT_ENCODING_UNICODE
#define FXFT_ENCODING_ADOBE_STANDARD FT_ENCODING_ADOBE_STANDARD
#define FXFT_ENCODING_ADOBE_EXPERT FT_ENCODING_ADOBE_EXPERT
#define FXFT_ENCODING_ADOBE_LATIN_1 FT_ENCODING_ADOBE_LATIN_1
#define FXFT_ENCODING_APPLE_ROMAN FT_ENCODING_APPLE_ROMAN
#define FXFT_ENCODING_ADOBE_CUSTOM FT_ENCODING_ADOBE_CUSTOM
#define FXFT_ENCODING_MS_SYMBOL FT_ENCODING_MS_SYMBOL
#define FXFT_ENCODING_GB2312 FT_ENCODING_GB2312
#define FXFT_ENCODING_BIG5 FT_ENCODING_BIG5
#define FXFT_ENCODING_SJIS FT_ENCODING_SJIS
#define FXFT_ENCODING_JOHAB FT_ENCODING_JOHAB

#define FXFT_LOAD_NO_SCALE FT_LOAD_NO_SCALE
#define FXFT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH \
  FT_LOAD_IGNORE_GLOBAL_ADVANCE_WIDTH
#define FXFT_LOAD_NO_BITMAP FT_LOAD_NO_BITMAP

#define FXFT_RENDER_MODE_LCD FT_RENDER_MODE_LCD
#define FXFT_RENDER_MODE_MONO FT_RENDER_MODE_MONO
#define FXFT_RENDER_MODE_NORMAL FT_RENDER_MODE_NORMAL

#define FXFT_PIXEL_MODE_MONO FT_PIXEL_MODE_MONO

#define FXFT_STYLE_FLAG_ITALIC FT_STYLE_FLAG_ITALIC
#define FXFT_STYLE_FLAG_BOLD FT_STYLE_FLAG_BOLD

#define FXFT_FACE_FLAG_SFNT FT_FACE_FLAG_SFNT
#define FXFT_FACE_FLAG_TRICKY (1L << 13)

#define FXFT_GLYPH_BBOX_PIXELS FT_GLYPH_BBOX_PIXELS

#define FXFT_Open_Face(library, args, index, face)            \
  FT_Open_Face(static_cast<FT_Library>(library), args, index, \
               static_cast<FT_Face*>(face))
#define FXFT_Done_Face(face) FT_Done_Face(static_cast<FT_Face>(face))
#define FXFT_Done_FreeType(library) \
  FT_Done_FreeType(static_cast<FT_Library>(library))
#define FXFT_Init_FreeType(library) \
  FT_Init_FreeType(reinterpret_cast<FT_Library*>(library))
#define FXFT_Library_Version(library, amajor, aminor, apatch)               \
  FT_Library_Version(reinterpret_cast<FT_Library>(library), amajor, aminor, \
                     apatch)
#define FXFT_New_Memory_Face(library, base, size, index, face)            \
  FT_New_Memory_Face(static_cast<FT_Library>(library), base, size, index, \
                     static_cast<FT_Face*>(face))
#define FXFT_New_Face(library, filename, index, face)            \
  FT_New_Face(static_cast<FT_Library>(library), filename, index, \
              static_cast<FT_Face*>(face))
#define FXFT_Select_Charmap(face, encoding)     \
  FT_Select_Charmap(static_cast<FT_Face>(face), \
                    static_cast<FT_Encoding>(encoding))
#define FXFT_Set_Charmap(face, charmap) \
  FT_Set_Charmap(static_cast<FT_Face>(face), static_cast<FT_CharMap>(charmap))
#define FXFT_Load_Glyph(face, glyph_index, flags) \
  FT_Load_Glyph(static_cast<FT_Face>(face), glyph_index, flags)
#define FXFT_Get_Char_Index(face, code) \
  FT_Get_Char_Index(static_cast<FT_Face>(face), code)
#define FXFT_Get_Glyph_Name(face, index, buffer, size) \
  FT_Get_Glyph_Name(static_cast<FT_Face>(face), index, buffer, size)
#define FXFT_Get_Name_Index(face, name) \
  FT_Get_Name_Index(static_cast<FT_Face>(face), const_cast<char*>(name))
#define FXFT_Has_Glyph_Names(face) \
  ((static_cast<FT_Face>(face)->face_flags) & FT_FACE_FLAG_GLYPH_NAMES)
#define FXFT_Get_Postscript_Name(face) \
  FT_Get_Postscript_Name(static_cast<FT_Face>(face))
#define FXFT_Load_Sfnt_Table(face, tag, offset, buffer, length) \
  FT_Load_Sfnt_Table(static_cast<FT_Face>(face), tag, offset, buffer, length)
#define FXFT_Get_First_Char(face, glyph_index) \
  FT_Get_First_Char(static_cast<FT_Face>(face), glyph_index)
#define FXFT_Get_Next_Char(face, code, glyph_index) \
  FT_Get_Next_Char(static_cast<FT_Face>(face), code, glyph_index)
#define FXFT_Clear_Face_External_Stream(face) \
  (static_cast<FT_Face>(face)->face_flags &= ~FT_FACE_FLAG_EXTERNAL_STREAM)
#define FXFT_Get_Face_External_Stream(face) \
  ((static_cast<FT_Face>(face)->face_flags) & FT_FACE_FLAG_EXTERNAL_STREAM)
#define FXFT_Is_Face_TT_OT(face) \
  ((static_cast<FT_Face>(face)->face_flags) & FT_FACE_FLAG_SFNT)
#define FXFT_Is_Face_Tricky(face) \
  ((static_cast<FT_Face>(face)->face_flags) & FXFT_FACE_FLAG_TRICKY)
#define FXFT_Is_Face_fixedwidth(face) \
  ((static_cast<FT_Face>(face)->face_flags) & FT_FACE_FLAG_FIXED_WIDTH)
#define FXFT_Get_Face_Stream_Base(face) static_cast<FT_Face>(face)->stream->base
#define FXFT_Get_Face_Stream_Size(face) static_cast<FT_Face>(face)->stream->size
#define FXFT_Get_Face_Family_Name(face) static_cast<FT_Face>(face)->family_name
#define FXFT_Get_Face_Style_Name(face) static_cast<FT_Face>(face)->style_name
#define FXFT_Is_Face_Italic(face) \
  ((static_cast<FT_Face>(face)->style_flags) & FT_STYLE_FLAG_ITALIC)
#define FXFT_Is_Face_Bold(face) \
  ((static_cast<FT_Face>(face)->style_flags) & FT_STYLE_FLAG_BOLD)
#define FXFT_Get_Face_Charmaps(face) static_cast<FT_Face>(face)->charmaps
#define FXFT_Get_Glyph_HoriBearingX(face) \
  static_cast<FT_Face>(face)->glyph->metrics.horiBearingX
#define FXFT_Get_Glyph_HoriBearingY(face) \
  static_cast<FT_Face>(face)->glyph->metrics.horiBearingY
#define FXFT_Get_Glyph_Width(face) \
  static_cast<FT_Face>(face)->glyph->metrics.width
#define FXFT_Get_Glyph_Height(face) \
  static_cast<FT_Face>(face)->glyph->metrics.height
#define FXFT_Get_Face_CharmapCount(face) \
  static_cast<FT_Face>(face)->num_charmaps
#define FXFT_Get_Charmap_Encoding(charmap) \
  static_cast<FT_CharMap>(charmap)->encoding
#define FXFT_Get_Face_Charmap(face) static_cast<FT_Face>(face)->charmap
#define FXFT_Get_Charmap_PlatformID(charmap) \
  static_cast<FT_CharMap>(charmap)->platform_id
#define FXFT_Get_Charmap_EncodingID(charmap) \
  static_cast<FT_CharMap>(charmap)->encoding_id
#define FXFT_Get_Face_UnitsPerEM(face) static_cast<FT_Face>(face)->units_per_EM
#define FXFT_Get_Face_xMin(face) static_cast<FT_Face>(face)->bbox.xMin
#define FXFT_Get_Face_xMax(face) static_cast<FT_Face>(face)->bbox.xMax
#define FXFT_Get_Face_yMin(face) static_cast<FT_Face>(face)->bbox.yMin
#define FXFT_Get_Face_yMax(face) static_cast<FT_Face>(face)->bbox.yMax
#define FXFT_Get_Face_Height(face) static_cast<FT_Face>(face)->height
#define FXFT_Get_Face_Ascender(face) static_cast<FT_Face>(face)->ascender
#define FXFT_Get_Face_Descender(face) static_cast<FT_Face>(face)->descender
#define FXFT_Get_Glyph_HoriAdvance(face) \
  static_cast<FT_Face>(face)->glyph->metrics.horiAdvance
#define FXFT_Get_MM_Axis(var, index) static_cast<FT_MM_Var*>(var)->axis[index]
#define FXFT_Get_MM_Axis_Min(axis) (axis).minimum
#define FXFT_Get_MM_Axis_Max(axis) (axis).maximum
#define FXFT_Get_MM_Axis_Def(axis) (axis).def
#define FXFT_Free(face, p)                                                     \
  static_cast<FT_Face>(face)->memory->free(static_cast<FT_Face>(face)->memory, \
                                           p)
#define FXFT_Get_Glyph_Outline(face) \
  &(static_cast<FT_Face>(face)->glyph->outline)
#define FXFT_Render_Glyph(face, mode)                \
  FT_Render_Glyph(static_cast<FT_Face>(face)->glyph, \
                  static_cast<enum FT_Render_Mode_>(mode))
#define FXFT_Get_MM_Var(face, p) FT_Get_MM_Var(static_cast<FT_Face>(face), p)
#define FXFT_Set_MM_Design_Coordinates(face, n, p) \
  FT_Set_MM_Design_Coordinates(static_cast<FT_Face>(face), n, p)
#define FXFT_Set_Pixel_Sizes(face, w, h) \
  FT_Set_Pixel_Sizes(static_cast<FT_Face>(face), w, h)
#define FXFT_Set_Transform(face, m, d) \
  FT_Set_Transform(static_cast<FT_Face>(face), m, d)
#define FXFT_Outline_Embolden(outline, s) FT_Outline_Embolden(outline, s)
#define FXFT_Get_Glyph_Bitmap(face) static_cast<FT_Face>(face)->glyph->bitmap
#define FXFT_Get_Bitmap_Width(bitmap) (bitmap).width
#define FXFT_Get_Bitmap_Rows(bitmap) (bitmap).rows
#define FXFT_Get_Bitmap_PixelMode(bitmap) (bitmap).pixel_mode
#define FXFT_Get_Bitmap_Pitch(bitmap) (bitmap).pitch
#define FXFT_Get_Bitmap_Buffer(bitmap) (bitmap).buffer
#define FXFT_Get_Glyph_BitmapLeft(face) \
  static_cast<FT_Face>(face)->glyph->bitmap_left
#define FXFT_Get_Glyph_BitmapTop(face) \
  static_cast<FT_Face>(face)->glyph->bitmap_top
#define FXFT_Outline_Decompose(outline, funcs, params) \
  FT_Outline_Decompose(outline, funcs, params)
#define FXFT_Set_Char_Size(face, char_width, char_height, horz_resolution, \
                           vert_resolution)                                \
  FT_Set_Char_Size(face, char_width, char_height, horz_resolution,         \
                   vert_resolution)
#define FXFT_Get_Glyph(slot, aglyph) FT_Get_Glyph(slot, aglyph)
#define FXFT_Glyph_Get_CBox(glyph, bbox_mode, acbox) \
  FT_Glyph_Get_CBox(glyph, bbox_mode, acbox)
#define FXFT_Done_Glyph(glyph) FT_Done_Glyph(glyph)
#define FXFT_Library_SetLcdFilter(library, filter) \
  FT_Library_SetLcdFilter(static_cast<FT_Library>(library), filter)

int FXFT_unicode_from_adobe_name(const char* glyph_name);
void FXFT_adobe_name_from_unicode(char* name, wchar_t unicode);

#endif  // CORE_FXGE_FX_FREETYPE_H_
