// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FXFA_FXFA_H_
#define XFA_FXFA_FXFA_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/fx_dib.h"
#include "xfa/fxfa/fxfa_basic.h"

class CFXJSE_Value;
class CXFA_FFDoc;
class CXFA_FFPageView;
class CXFA_FFWidget;
class CXFA_Submit;
class IFWL_AdapterTimerMgr;
class IFX_SeekableReadStream;

// Note, values must match fpdf_formfill.h JSPLATFORM_ALERT_BUTTON_* flags.
enum class AlertButton {
  kDefault = 0,
  kOK = 0,
  kOKCancel = 1,
  kYesNo = 2,
  kYesNoCancel = 3,
};

// Note, values must match fpdf_formfill.h JSPLATFORM_ALERT_ICON_* flags.
enum class AlertIcon {
  kDefault = 0,
  kError = 0,
  kWarning = 1,
  kQuestion = 2,
  kStatus = 3,
  kAsterisk = 4,
};

// Note, values must match fpdf_formfill.h JSPLATFORM_ALERT_RETURN_* flags.
enum class AlertReturn {
  kOK = 1,
  kCancel = 2,
  kNo = 3,
  kYes = 4,
};

// Note, values must match fpdf_formfill.h FORMTYPE_* flags.
enum class FormType {
  kNone = 0,
  kAcroForm = 1,
  kXFAFull = 2,
  kXFAForeground = 3,
};

#define XFA_PRINTOPT_ShowDialog 0x00000001
#define XFA_PRINTOPT_CanCancel 0x00000002
#define XFA_PRINTOPT_ShrinkPage 0x00000004
#define XFA_PRINTOPT_AsImage 0x00000008
#define XFA_PRINTOPT_ReverseOrder 0x00000010
#define XFA_PRINTOPT_PrintAnnot 0x00000020

#define XFA_PAGEVIEWEVENT_PostAdded 1
#define XFA_PAGEVIEWEVENT_PostRemoved 3
#define XFA_PAGEVIEWEVENT_StopLayout 4

#define XFA_EVENTERROR_Success 1
#define XFA_EVENTERROR_Error -1
#define XFA_EVENTERROR_NotExist 0
#define XFA_EVENTERROR_Disabled 2

#define XFA_TRAVERSEWAY_Tranvalse 0x0001
#define XFA_TRAVERSEWAY_Form 0x0002

enum XFA_WidgetStatus {
  XFA_WidgetStatus_None = 0,

  XFA_WidgetStatus_Access = 1 << 0,
  XFA_WidgetStatus_ButtonDown = 1 << 1,
  XFA_WidgetStatus_Disabled = 1 << 2,
  XFA_WidgetStatus_Focused = 1 << 3,
  XFA_WidgetStatus_Highlight = 1 << 4,
  XFA_WidgetStatus_Printable = 1 << 5,
  XFA_WidgetStatus_RectCached = 1 << 6,
  XFA_WidgetStatus_TextEditValueChanged = 1 << 7,
  XFA_WidgetStatus_Viewable = 1 << 8,
  XFA_WidgetStatus_Visible = 1 << 9
};

enum XFA_WIDGETTYPE {
  XFA_WIDGETTYPE_Barcode,
  XFA_WIDGETTYPE_PushButton,
  XFA_WIDGETTYPE_CheckButton,
  XFA_WIDGETTYPE_RadioButton,
  XFA_WIDGETTYPE_DatetimeEdit,
  XFA_WIDGETTYPE_DecimalField,
  XFA_WIDGETTYPE_NumericField,
  XFA_WIDGETTYPE_Signature,
  XFA_WIDGETTYPE_TextEdit,
  XFA_WIDGETTYPE_DropdownList,
  XFA_WIDGETTYPE_ListBox,
  XFA_WIDGETTYPE_ImageField,
  XFA_WIDGETTYPE_PasswordEdit,
  XFA_WIDGETTYPE_Arc,
  XFA_WIDGETTYPE_Rectangle,
  XFA_WIDGETTYPE_Image,
  XFA_WIDGETTYPE_Line,
  XFA_WIDGETTYPE_Text,
  XFA_WIDGETTYPE_ExcludeGroup,
  XFA_WIDGETTYPE_Subform,
  XFA_WIDGETTYPE_Unknown,
};

// Probably should be called IXFA_AppDelegate.
class IXFA_AppProvider {
 public:
  virtual ~IXFA_AppProvider() = default;

  /**
   * Returns the language of the running host application. Such as zh_CN
   */
  virtual WideString GetLanguage() = 0;

  /**
   * Returns the platform of the machine running the script. Such as WIN
   */
  virtual WideString GetPlatform() = 0;

  /**
   * Get application name, such as Phantom.
   */
  virtual WideString GetAppName() = 0;

  /**
   * Get application message box title.
   */
  virtual WideString GetAppTitle() const = 0;

  /**
   * Causes the system to play a sound.
   * @param[in] dwType The system code for the appropriate sound.0 (Error)1
   * (Warning)2 (Question)3 (Status)4 (Default)
   */
  virtual void Beep(uint32_t dwType) = 0;

  /**
   * Displays a message box.
   * @param[in] wsMessage    - Message string to display in box.
   * @param[in] wsTitle      - Title string for box.
   * @param[in] dwIconType   - Icon type, refer to XFA_MBICON.
   * @param[in] dwButtonType - Button type, refer to XFA_MESSAGEBUTTON.
   * @return A valid integer representing the value of the button pressed by the
   * user, refer to XFA_ID.
   */
  virtual int32_t MsgBox(const WideString& wsMessage,
                         const WideString& wsTitle,
                         uint32_t dwIconType,
                         uint32_t dwButtonType) = 0;

  /**
   * Get a response from the user.
   * @param[in] wsQuestion      - Message string to display in box.
   * @param[in] wsTitle         - Title string for box.
   * @param[in] wsDefaultAnswer - Initial contents for answer.
   * @param[in] bMask           - Mask the user input with asterisks when true,
   * @return A string containing the user's response.
   */
  virtual WideString Response(const WideString& wsQuestion,
                              const WideString& wsTitle,
                              const WideString& wsDefaultAnswer,
                              bool bMask) = 0;

  /**
   * Download something from somewhere.
   * @param[in] wsURL - http, ftp, such as
   * "http://www.w3.org/TR/REC-xml-names/".
   */
  virtual RetainPtr<IFX_SeekableReadStream> DownloadURL(
      const WideString& wsURL) = 0;

  /**
   * POST data to the given url.
   * @param[in] wsURL         the URL being uploaded.
   * @param[in] wsData        the data being uploaded.
   * @param[in] wsContentType the content type of data including text/html,
   * text/xml, text/plain, multipart/form-data,
   *                          application/x-www-form-urlencoded,
   * application/octet-stream, any valid MIME type.
   * @param[in] wsEncode      the encode of data including UTF-8, UTF-16,
   * ISO8859-1, any recognized [IANA]character encoding
   * @param[in] wsHeader      any additional HTTP headers to be included in the
   * post.
   * @param[out] wsResponse   decoded response from server.
   * @return true Server permitted the post request, false otherwise.
   */
  virtual bool PostRequestURL(const WideString& wsURL,
                              const WideString& wsData,
                              const WideString& wsContentType,
                              const WideString& wsEncode,
                              const WideString& wsHeader,
                              WideString& wsResponse) = 0;

  /**
   * PUT data to the given url.
   * @param[in] wsURL         the URL being uploaded.
   * @param[in] wsData            the data being uploaded.
   * @param[in] wsEncode      the encode of data including UTF-8, UTF-16,
   * ISO8859-1, any recognized [IANA]character encoding
   * @return true Server permitted the post request, false otherwise.
   */
  virtual bool PutRequestURL(const WideString& wsURL,
                             const WideString& wsData,
                             const WideString& wsEncode) = 0;

  virtual std::unique_ptr<IFWL_AdapterTimerMgr> NewTimerMgr() = 0;
};

class IXFA_DocEnvironment {
 public:
  virtual ~IXFA_DocEnvironment() = default;

  virtual void SetChangeMark(CXFA_FFDoc* hDoc) = 0;
  virtual void InvalidateRect(CXFA_FFPageView* pPageView,
                              const CFX_RectF& rt) = 0;
  virtual void DisplayCaret(CXFA_FFWidget* hWidget,
                            bool bVisible,
                            const CFX_RectF* pRtAnchor) = 0;
  virtual bool GetPopupPos(CXFA_FFWidget* hWidget,
                           float fMinPopup,
                           float fMaxPopup,
                           const CFX_RectF& rtAnchor,
                           CFX_RectF* pPopupRect) = 0;
  virtual bool PopupMenu(CXFA_FFWidget* hWidget, CFX_PointF ptPopup) = 0;
  virtual void PageViewEvent(CXFA_FFPageView* pPageView, uint32_t dwFlags) = 0;
  virtual void WidgetPostAdd(CXFA_FFWidget* hWidget) = 0;
  virtual void WidgetPreRemove(CXFA_FFWidget* hWidget) = 0;

  virtual int32_t CountPages(CXFA_FFDoc* hDoc) = 0;
  virtual int32_t GetCurrentPage(CXFA_FFDoc* hDoc) = 0;
  virtual void SetCurrentPage(CXFA_FFDoc* hDoc, int32_t iCurPage) = 0;
  virtual bool IsCalculationsEnabled(CXFA_FFDoc* hDoc) = 0;
  virtual void SetCalculationsEnabled(CXFA_FFDoc* hDoc, bool bEnabled) = 0;
  virtual void GetTitle(CXFA_FFDoc* hDoc, WideString& wsTitle) = 0;
  virtual void SetTitle(CXFA_FFDoc* hDoc, const WideString& wsTitle) = 0;
  virtual void ExportData(CXFA_FFDoc* hDoc,
                          const WideString& wsFilePath,
                          bool bXDP) = 0;
  virtual void GotoURL(CXFA_FFDoc* hDoc, const WideString& bsURL) = 0;
  virtual bool IsValidationsEnabled(CXFA_FFDoc* hDoc) = 0;
  virtual void SetValidationsEnabled(CXFA_FFDoc* hDoc, bool bEnabled) = 0;
  virtual void SetFocusWidget(CXFA_FFDoc* hDoc, CXFA_FFWidget* hWidget) = 0;
  virtual void Print(CXFA_FFDoc* hDoc,
                     int32_t nStartPage,
                     int32_t nEndPage,
                     uint32_t dwOptions) = 0;
  virtual FX_ARGB GetHighlightColor(CXFA_FFDoc* hDoc) = 0;

#ifdef PDF_XFA_ELEMENT_SUBMIT_ENABLED
  virtual bool Submit(CXFA_FFDoc* hDoc, CXFA_Submit* submit) = 0;
#endif  // PDF_XFA_ELEMENT_SUBMIT_ENABLED

  virtual bool GetPropertyFromNonXFAGlobalObject(CXFA_FFDoc* hDoc,
                                                 ByteStringView szPropName,
                                                 CFXJSE_Value* pValue) = 0;
  virtual bool SetPropertyInNonXFAGlobalObject(CXFA_FFDoc* hDoc,
                                               ByteStringView szPropName,
                                               CFXJSE_Value* pValue) = 0;
  virtual RetainPtr<IFX_SeekableReadStream> OpenLinkedFile(
      CXFA_FFDoc* hDoc,
      const WideString& wsLink) = 0;
};

class IXFA_WidgetIterator {
 public:
  virtual ~IXFA_WidgetIterator() = default;

  virtual void Reset() = 0;
  virtual CXFA_FFWidget* MoveToFirst() = 0;
  virtual CXFA_FFWidget* MoveToLast() = 0;
  virtual CXFA_FFWidget* MoveToNext() = 0;
  virtual CXFA_FFWidget* MoveToPrevious() = 0;
  virtual CXFA_FFWidget* GetCurrentWidget() = 0;
  virtual bool SetCurrentWidget(CXFA_FFWidget* hWidget) = 0;
};

#endif  // XFA_FXFA_FXFA_H_
