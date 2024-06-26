// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This "C" (not "C++") file ensures that the public headers compile
// and link for "C" (and not just "C++").

#include <stdio.h>

#include "fpdfsdk/fpdf_view_c_api_test.h"

#include "public/fpdf_annot.h"
#include "public/fpdf_attachment.h"
#include "public/fpdf_catalog.h"
#include "public/fpdf_dataavail.h"
#include "public/fpdf_doc.h"
#include "public/fpdf_edit.h"
#include "public/fpdf_ext.h"
#include "public/fpdf_flatten.h"
#include "public/fpdf_formfill.h"
#include "public/fpdf_fwlevent.h"
#include "public/fpdf_ppo.h"
#include "public/fpdf_progressive.h"
#include "public/fpdf_save.h"
#include "public/fpdf_searchex.h"
#include "public/fpdf_structtree.h"
#include "public/fpdf_sysfontinfo.h"
#include "public/fpdf_text.h"
#include "public/fpdf_transformpage.h"
#include "public/fpdfview.h"

// Scheme for avoiding LTO out of existence, warnings, etc.
typedef void (*fnptr)(void);  // Legal generic function type for casts.
fnptr g_c_api_test_fnptr = NULL;  // Extern, so can't know it doesn't change.
#define CHK(x) if ((fnptr)(x) == g_c_api_test_fnptr) return 0

// Function to call from gtest harness to ensure linker resolution.
int CheckPDFiumCApi() {
    // fpdf_annot.h
    CHK(FPDFAnnot_AppendAttachmentPoints);
    CHK(FPDFAnnot_AppendObject);
    CHK(FPDFAnnot_CountAttachmentPoints);
    CHK(FPDFAnnot_GetAP);
    CHK(FPDFAnnot_GetAttachmentPoints);
    CHK(FPDFAnnot_GetColor);
    CHK(FPDFAnnot_GetFlags);
    CHK(FPDFAnnot_GetFormFieldAtPoint);
    CHK(FPDFAnnot_GetFormFieldFlags);
    CHK(FPDFAnnot_GetLinkedAnnot);
    CHK(FPDFAnnot_GetObject);
    CHK(FPDFAnnot_GetObjectCount);
    CHK(FPDFAnnot_GetRect);
    CHK(FPDFAnnot_GetStringValue);
    CHK(FPDFAnnot_GetSubtype);
    CHK(FPDFAnnot_GetValueType);
    CHK(FPDFAnnot_HasAttachmentPoints);
    CHK(FPDFAnnot_HasKey);
    CHK(FPDFAnnot_IsObjectSupportedSubtype);
    CHK(FPDFAnnot_IsSupportedSubtype);
    CHK(FPDFAnnot_RemoveObject);
    CHK(FPDFAnnot_SetAP);
    CHK(FPDFAnnot_SetAttachmentPoints);
    CHK(FPDFAnnot_SetColor);
    CHK(FPDFAnnot_SetFlags);
    CHK(FPDFAnnot_SetRect);
    CHK(FPDFAnnot_SetStringValue);
    CHK(FPDFAnnot_UpdateObject);
    CHK(FPDFPage_CloseAnnot);
    CHK(FPDFPage_CreateAnnot);
    CHK(FPDFPage_GetAnnot);
    CHK(FPDFPage_GetAnnotCount);
    CHK(FPDFPage_GetAnnotIndex);
    CHK(FPDFPage_RemoveAnnot);

    // fpdf_attachment.h
    CHK(FPDFAttachment_GetFile);
    CHK(FPDFAttachment_GetName);
    CHK(FPDFAttachment_GetStringValue);
    CHK(FPDFAttachment_GetValueType);
    CHK(FPDFAttachment_HasKey);
    CHK(FPDFAttachment_SetFile);
    CHK(FPDFAttachment_SetStringValue);
    CHK(FPDFDoc_AddAttachment);
    CHK(FPDFDoc_DeleteAttachment);
    CHK(FPDFDoc_GetAttachment);
    CHK(FPDFDoc_GetAttachmentCount);

    // fpdf_catalog.h
    CHK(FPDFCatalog_IsTagged);

    // fpdf_dataavail.h
    CHK(FPDFAvail_Create);
    CHK(FPDFAvail_Destroy);
    CHK(FPDFAvail_GetDocument);
    CHK(FPDFAvail_GetFirstPageNum);
    CHK(FPDFAvail_IsDocAvail);
    CHK(FPDFAvail_IsFormAvail);
    CHK(FPDFAvail_IsLinearized);
    CHK(FPDFAvail_IsPageAvail);

    // fpdf_doc.h
    CHK(FPDFAction_GetDest);
    CHK(FPDFAction_GetFilePath);
    CHK(FPDFAction_GetType);
    CHK(FPDFAction_GetURIPath);
    CHK(FPDFBookmark_Find);
    CHK(FPDFBookmark_GetAction);
    CHK(FPDFBookmark_GetDest);
    CHK(FPDFBookmark_GetFirstChild);
    CHK(FPDFBookmark_GetNextSibling);
    CHK(FPDFBookmark_GetTitle);
    CHK(FPDFDest_GetDestPageIndex);
    CHK(FPDFDest_GetLocationInPage);
    CHK(FPDFDest_GetView);
    CHK(FPDFLink_CountQuadPoints);
    CHK(FPDFLink_Enumerate);
    CHK(FPDFLink_GetAction);
    CHK(FPDFLink_GetAnnotRect);
    CHK(FPDFLink_GetDest);
    CHK(FPDFLink_GetLinkAtPoint);
    CHK(FPDFLink_GetLinkZOrderAtPoint);
    CHK(FPDFLink_GetQuadPoints);
    CHK(FPDF_GetMetaText);
    CHK(FPDF_GetPageLabel);

    // fpdf_edit.h
    CHK(FPDFFont_Close);
    CHK(FPDFFormObj_CountObjects);
    CHK(FPDFFormObj_GetMatrix);
    CHK(FPDFFormObj_GetObject);
    CHK(FPDFImageObj_GetBitmap);
    CHK(FPDFImageObj_GetImageDataDecoded);
    CHK(FPDFImageObj_GetImageDataRaw);
    CHK(FPDFImageObj_GetImageFilter);
    CHK(FPDFImageObj_GetImageFilterCount);
    CHK(FPDFImageObj_GetImageMetadata);
    CHK(FPDFImageObj_GetMatrix);
    CHK(FPDFImageObj_LoadJpegFile);
    CHK(FPDFImageObj_LoadJpegFileInline);
    CHK(FPDFImageObj_SetBitmap);
    CHK(FPDFImageObj_SetMatrix);
    CHK(FPDFPageObjMark_CountParams);
    CHK(FPDFPageObjMark_GetName);
    CHK(FPDFPageObjMark_GetParamBlobValue);
    CHK(FPDFPageObjMark_GetParamIntValue);
    CHK(FPDFPageObjMark_GetParamKey);
    CHK(FPDFPageObjMark_GetParamStringValue);
    CHK(FPDFPageObjMark_GetParamValueType);
    CHK(FPDFPageObjMark_RemoveParam);
    CHK(FPDFPageObjMark_SetBlobParam);
    CHK(FPDFPageObjMark_SetIntParam);
    CHK(FPDFPageObjMark_SetStringParam);
    CHK(FPDFPageObj_AddMark);
    CHK(FPDFPageObj_CountMarks);
    CHK(FPDFPageObj_CreateNewPath);
    CHK(FPDFPageObj_CreateNewRect);
    CHK(FPDFPageObj_CreateTextObj);
    CHK(FPDFPageObj_Destroy);
    CHK(FPDFPageObj_GetBounds);
    CHK(FPDFPageObj_GetFillColor);
    CHK(FPDFPageObj_GetLineCap);
    CHK(FPDFPageObj_GetLineJoin);
    CHK(FPDFPageObj_GetMark);
    CHK(FPDFPageObj_GetStrokeColor);
    CHK(FPDFPageObj_GetStrokeWidth);
    CHK(FPDFPageObj_GetType);
    CHK(FPDFPageObj_HasTransparency);
    CHK(FPDFPageObj_NewImageObj);
    CHK(FPDFPageObj_NewTextObj);
    CHK(FPDFPageObj_RemoveMark);
    CHK(FPDFPageObj_SetBlendMode);
    CHK(FPDFPageObj_SetFillColor);
    CHK(FPDFPageObj_SetLineCap);
    CHK(FPDFPageObj_SetLineJoin);
    CHK(FPDFPageObj_SetStrokeColor);
    CHK(FPDFPageObj_SetStrokeWidth);
    CHK(FPDFPageObj_Transform);
    CHK(FPDFPage_CountObjects);
    CHK(FPDFPage_Delete);
    CHK(FPDFPage_GenerateContent);
    CHK(FPDFPage_GetObject);
    CHK(FPDFPage_GetRotation);
    CHK(FPDFPage_HasTransparency);
    CHK(FPDFPage_InsertObject);
    CHK(FPDFPage_New);
    CHK(FPDFPage_RemoveObject);
    CHK(FPDFPage_SetRotation);
    CHK(FPDFPage_TransformAnnots);
    CHK(FPDFPathSegment_GetClose);
    CHK(FPDFPathSegment_GetPoint);
    CHK(FPDFPathSegment_GetType);
    CHK(FPDFPath_BezierTo);
    CHK(FPDFPath_Close);
    CHK(FPDFPath_CountSegments);
    CHK(FPDFPath_GetDrawMode);
    CHK(FPDFPath_GetFillColor);
    CHK(FPDFPath_GetMatrix);
    CHK(FPDFPath_GetPathSegment);
    CHK(FPDFPath_GetStrokeColor);
    CHK(FPDFPath_LineTo);
    CHK(FPDFPath_MoveTo);
    CHK(FPDFPath_SetDrawMode);
    CHK(FPDFPath_SetFillColor);
    CHK(FPDFPath_SetLineCap);
    CHK(FPDFPath_SetLineJoin);
    CHK(FPDFPath_SetMatrix);
    CHK(FPDFPath_SetStrokeColor);
    CHK(FPDFPath_SetStrokeWidth);
    CHK(FPDFTextObj_GetFontName);
    CHK(FPDFTextObj_GetFontSize);
    CHK(FPDFTextObj_GetText);
    CHK(FPDFText_GetMatrix);
    CHK(FPDFText_GetTextRenderMode);
    CHK(FPDFText_LoadFont);
    CHK(FPDFText_LoadStandardFont);
    CHK(FPDFText_SetFillColor);
    CHK(FPDFText_SetText);
    CHK(FPDF_CreateNewDocument);

    // fpdf_ext.h
    CHK(FPDFDoc_GetPageMode);
    CHK(FSDK_SetLocaltimeFunction);
    CHK(FSDK_SetTimeFunction);
    CHK(FSDK_SetUnSpObjProcessHandler);

    // fpdf_flatten.h
    CHK(FPDFPage_Flatten);

    // fpdf_fwlevent.h - no exports.

    // fpdf_formfill.h
    CHK(FORM_CanRedo);
    CHK(FORM_CanUndo);
    CHK(FORM_DoDocumentAAction);
    CHK(FORM_DoDocumentJSAction);
    CHK(FORM_DoDocumentOpenAction);
    CHK(FORM_DoPageAAction);
    CHK(FORM_ForceToKillFocus);
    CHK(FORM_GetFocusedText);
    CHK(FORM_GetSelectedText);
    CHK(FORM_OnAfterLoadPage);
    CHK(FORM_OnBeforeClosePage);
    CHK(FORM_OnChar);
    CHK(FORM_OnFocus);
    CHK(FORM_OnKeyDown);
    CHK(FORM_OnKeyUp);
    CHK(FORM_OnLButtonDoubleClick);
    CHK(FORM_OnLButtonDown);
    CHK(FORM_OnLButtonUp);
    CHK(FORM_OnMouseMove);
#ifdef PDF_ENABLE_XFA
    CHK(FORM_OnRButtonDown);
    CHK(FORM_OnRButtonUp);
#endif
    CHK(FORM_Redo);
    CHK(FORM_ReplaceSelection);
    CHK(FORM_Undo);
    CHK(FPDFDOC_ExitFormFillEnvironment);
    CHK(FPDFDOC_InitFormFillEnvironment);
    CHK(FPDFPage_FormFieldZOrderAtPoint);
    CHK(FPDFPage_HasFormFieldAtPoint);
    CHK(FPDF_FFLDraw);
#ifdef _SKIA_SUPPORT_
    CHK(FPDF_FFLRecord);
#endif
    CHK(FPDF_GetFormType);
#ifdef PDF_ENABLE_XFA
    CHK(FPDF_LoadXFA);
#endif
    CHK(FPDF_RemoveFormFieldHighlight);
    CHK(FPDF_SetFormFieldHighlightAlpha);
    CHK(FPDF_SetFormFieldHighlightColor);

    // fpdf_ppo.h
    CHK(FPDF_CopyViewerPreferences);
    CHK(FPDF_ImportNPagesToOne);
    CHK(FPDF_ImportPages);

    // fpdf_progressive.h
    CHK(FPDF_RenderPageBitmap_Start);
    CHK(FPDF_RenderPage_Close);
    CHK(FPDF_RenderPage_Continue);

    // fpdf_save.h
    CHK(FPDF_SaveAsCopy);
    CHK(FPDF_SaveWithVersion);

    // fpdf_searchex.h
    CHK(FPDFText_GetCharIndexFromTextIndex);
    CHK(FPDFText_GetTextIndexFromCharIndex);

    // fpdf_structtree.h
    CHK(FPDF_StructElement_CountChildren);
    CHK(FPDF_StructElement_GetAltText);
    CHK(FPDF_StructElement_GetChildAtIndex);
    CHK(FPDF_StructElement_GetMarkedContentID);
    CHK(FPDF_StructElement_GetTitle);
    CHK(FPDF_StructElement_GetType);
    CHK(FPDF_StructTree_Close);
    CHK(FPDF_StructTree_CountChildren);
    CHK(FPDF_StructTree_GetChildAtIndex);
    CHK(FPDF_StructTree_GetForPage);

    // fpdf_sysfontinfo.h
    CHK(FPDF_AddInstalledFont);
    CHK(FPDF_FreeDefaultSystemFontInfo);
    CHK(FPDF_GetDefaultSystemFontInfo);
    CHK(FPDF_GetDefaultTTFMap);
    CHK(FPDF_SetSystemFontInfo);

    // fpdf_text.h
    CHK(FPDFLink_CloseWebLinks);
    CHK(FPDFLink_CountRects);
    CHK(FPDFLink_CountWebLinks);
    CHK(FPDFLink_GetRect);
    CHK(FPDFLink_GetURL);
    CHK(FPDFLink_LoadWebLinks);
    CHK(FPDFText_ClosePage);
    CHK(FPDFText_CountChars);
    CHK(FPDFText_CountRects);
    CHK(FPDFText_FindClose);
    CHK(FPDFText_FindNext);
    CHK(FPDFText_FindPrev);
    CHK(FPDFText_FindStart);
    CHK(FPDFText_GetBoundedText);
    CHK(FPDFText_GetCharBox);
    CHK(FPDFText_GetCharIndexAtPos);
    CHK(FPDFText_GetCharOrigin);
    CHK(FPDFText_GetFontInfo);
    CHK(FPDFText_GetFontSize);
    CHK(FPDFText_GetRect);
    CHK(FPDFText_GetSchCount);
    CHK(FPDFText_GetSchResultIndex);
    CHK(FPDFText_GetText);
    CHK(FPDFText_GetUnicode);
    CHK(FPDFText_LoadPage);

    // fpdf_transformpage.h
    CHK(FPDFPageObj_TransformClipPath);
    CHK(FPDFPage_GetArtBox);
    CHK(FPDFPage_GetBleedBox);
    CHK(FPDFPage_GetCropBox);
    CHK(FPDFPage_GetMediaBox);
    CHK(FPDFPage_GetTrimBox);
    CHK(FPDFPage_InsertClipPath);
    CHK(FPDFPage_SetArtBox);
    CHK(FPDFPage_SetBleedBox);
    CHK(FPDFPage_SetCropBox);
    CHK(FPDFPage_SetMediaBox);
    CHK(FPDFPage_SetTrimBox);
    CHK(FPDFPage_TransFormWithClip);
    CHK(FPDF_CreateClipPath);
    CHK(FPDF_DestroyClipPath);

    // fpdfview.h
    CHK(FPDFBitmap_Create);
    CHK(FPDFBitmap_CreateEx);
    CHK(FPDFBitmap_Destroy);
    CHK(FPDFBitmap_FillRect);
    CHK(FPDFBitmap_GetBuffer);
    CHK(FPDFBitmap_GetFormat);
    CHK(FPDFBitmap_GetHeight);
    CHK(FPDFBitmap_GetStride);
    CHK(FPDFBitmap_GetWidth);
#ifdef PDF_ENABLE_XFA
    CHK(FPDF_BStr_Clear);
    CHK(FPDF_BStr_Init);
    CHK(FPDF_BStr_Set);
#endif
    CHK(FPDF_CloseDocument);
    CHK(FPDF_ClosePage);
    CHK(FPDF_CountNamedDests);
    CHK(FPDF_DestroyLibrary);
    CHK(FPDF_DeviceToPage);
    CHK(FPDF_DocumentHasValidCrossReferenceTable);
    CHK(FPDF_GetDocPermissions);
    CHK(FPDF_GetFileVersion);
    CHK(FPDF_GetLastError);
    CHK(FPDF_GetNamedDest);
    CHK(FPDF_GetNamedDestByName);
    CHK(FPDF_GetPageBoundingBox);
    CHK(FPDF_GetPageCount);
    CHK(FPDF_GetPageHeight);
    CHK(FPDF_GetPageSizeByIndex);
    CHK(FPDF_GetPageWidth);
#ifdef PDF_ENABLE_V8
    CHK(FPDF_GetRecommendedV8Flags);
#endif
    CHK(FPDF_GetSecurityHandlerRevision);
    CHK(FPDF_InitLibrary);
    CHK(FPDF_InitLibraryWithConfig);
    CHK(FPDF_LoadCustomDocument);
    CHK(FPDF_LoadDocument);
    CHK(FPDF_LoadMemDocument);
    CHK(FPDF_LoadPage);
    CHK(FPDF_PageToDevice);
#ifdef _WIN32
    CHK(FPDF_RenderPage);
#endif
    CHK(FPDF_RenderPageBitmap);
    CHK(FPDF_RenderPageBitmapWithMatrix);
#ifdef _SKIA_SUPPORT_
    CHK(FPDF_RenderPageSkp);
#endif
#if defined(_WIN32)
    CHK(FPDF_SetPrintMode);
#if defined(PDFIUM_PRINT_TEXT_WITH_GDI)
    CHK(FPDF_SetPrintTextWithGDI);
#endif
#endif
    CHK(FPDF_SetSandBoxPolicy);
#if defined(_WIN32) && defined(PDFIUM_PRINT_TEXT_WITH_GDI)
    CHK(FPDF_SetTypefaceAccessibleFunc);
#endif
    CHK(FPDF_VIEWERREF_GetDuplex);
    CHK(FPDF_VIEWERREF_GetName);
    CHK(FPDF_VIEWERREF_GetNumCopies);
    CHK(FPDF_VIEWERREF_GetPrintPageRange);
    CHK(FPDF_VIEWERREF_GetPrintPageRangeCount);
    CHK(FPDF_VIEWERREF_GetPrintPageRangeElement);
    CHK(FPDF_VIEWERREF_GetPrintScaling);

    return 1;
}

#undef CHK
