/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <string>
#include <vector>
#include <errno.h>


#include "xp.h"

#include "format.h"
#include "logger.h"

extern Logger * logger;

////////////////////////////////////////////////////////////////////////////////
// From Google Chrome's string_util.cc

// It's possible for functions that use a va_list, such as StringPrintf, to
// invalidate the data in it upon use.  The fix is to make a copy of the
// structure before using it and use that copy instead.  va_copy is provided
// for this purpose.  MSVC does not provide va_copy, so define an
// implementation here.  It is not guaranteed that assignment is a copy, so the
// StringUtil.__GNUC__ unit test tests this capability.
#if defined(COMPILER_GCC)
#define GG_VA_COPY(a, b) (va_copy(a, b))
#elif defined(_MSC_VER)
#define GG_VA_COPY(a, b) (a = b)
#endif


// The arraysize(arr) macro returns the # of elements in an array arr.
// The expression is a compile-time constant, and therefore can be
// used in defining new arrays, for example.  If you use arraysize on
// a pointer by mistake, you will get a compile-time error.
//
// One caveat is that arraysize() doesn't accept any array of an
// anonymous type or a type defined inside a function.  In these rare
// cases, you have to use the unsafe ARRAYSIZE_UNSAFE() macro below.  This is
// due to a limitation in C++'s template system.  The limitation might
// eventually be removed, but it hasn't happened yet.

// This template function declaration is used in defining arraysize.
// Note that the function doesn't need an implementation, as we only
// use its type.
template <typename T, size_t N>
char (&ArraySizeHelper(T (&array)[N]))[N];

// That gcc wants both of these prototypes seems mysterious. VC, for
// its part, can't decide which to use (another mystery). Matching of
// template overloads: the final frontier.
#ifndef _MSC_VER
template <typename T, size_t N>
char (&ArraySizeHelper(const T (&array)[N]))[N];
#endif

#define arraysize(array) (sizeof(ArraySizeHelper(array)))

#if defined(_WIN32)
inline int vsnprintf(char* buffer, size_t size,
                     const char* format, va_list arguments) {
  int length = vsnprintf_s(buffer, size, size - 1, format, arguments);
  if (length < 0)
    return _vscprintf(format, arguments);
  return length;
}
#else
inline int vsnprintf(char* buffer, size_t size,
                     const char* format, va_list arguments) {
  return ::vsnprintf(buffer, size, format, arguments);
}
#endif


// Templatized backend for StringPrintF/StringAppendF. This does not finalize
// the va_list, the caller is expected to do that.
template <class StringType>
static void StringAppendVT(StringType* dst,
                           const typename StringType::value_type* format,
                           va_list ap) {
  // First try with a small fixed size buffer.
  // This buffer size should be kept in sync with StringUtilTest.GrowBoundary
  // and StringUtilTest.StringPrintfBounds.
  typename StringType::value_type stack_buf[1024];

  va_list ap_copy;
  GG_VA_COPY(ap_copy, ap);

#if !defined(_WIN32)
  errno = 0;
#endif
  int result = vsnprintf(stack_buf, arraysize(stack_buf), format, ap_copy);
  va_end(ap_copy);

  if (result >= 0 && result < static_cast<int>(arraysize(stack_buf))) {
    // It fit.
    dst->append(stack_buf, result);
    return;
  }

  // Repeatedly increase buffer size until it fits.
  int mem_length = arraysize(stack_buf);
  while (true) {
    if (result < 0) {
#if !defined(_WIN32)
      // On Windows, vsnprintf always returns the number of characters in a
      // fully-formatted string, so if we reach this point, something else is
      // wrong and no amount of buffer-doubling is going to fix it.
      if (errno != 0 && errno != EOVERFLOW)
#endif
      {
        // If an error other than overflow occurred, it's never going to work.
        return;
      }
      // Try doubling the buffer size.
      mem_length *= 2;
    } else {
      // We need exactly "result + 1" characters.
      mem_length = result + 1;
    }

    if (mem_length > 32 * 1024 * 1024) {
      // That should be plenty, don't try anything larger.  This protects
      // against huge allocations when using vsnprintf implementations that
      // return -1 for reasons other than overflow without setting errno.
      return;
    }

    std::vector<typename StringType::value_type> mem_buf(mem_length);

    // NOTE: You can only use a va_list once.  Since we're in a while loop, we
    // need to make a new copy each time so we don't use up the original.
    GG_VA_COPY(ap_copy, ap);
    result = vsnprintf(&mem_buf[0], mem_length, format, ap_copy);
    va_end(ap_copy);

    if ((result >= 0) && (result < mem_length)) {
      // It fit.
      dst->append(&mem_buf[0], result);
      return;
    }
  }
}

std::string StringPrintf(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  std::string result;
  StringAppendVT(&result, format, ap);
  va_end(ap);
  return result;
}

////////////////////////////////////////////////////////////////////////////////


char * FormatNPAPIError(int iError)
{
  static char szError[64];
  switch (iError)
  {
    case NPERR_NO_ERROR:
      sprintf(szError, "NPERR_NO_ERROR");
      break;
    case NPERR_GENERIC_ERROR:
      sprintf(szError, "NPERR_GENERIC_ERROR");
      break;
    case NPERR_INVALID_INSTANCE_ERROR:
      sprintf(szError, "NPERR_INVALID_INSTANCE_ERROR");
      break;
    case NPERR_INVALID_FUNCTABLE_ERROR:
      sprintf(szError, "NPERR_INVALID_FUNCTABLE_ERROR");
      break;
    case NPERR_MODULE_LOAD_FAILED_ERROR:
      sprintf(szError, "NPERR_MODULE_LOAD_FAILED_ERROR");
      break;
    case NPERR_OUT_OF_MEMORY_ERROR:
      sprintf(szError, "NPERR_OUT_OF_MEMORY_ERROR");
      break;
    case NPERR_INVALID_PLUGIN_ERROR:
      sprintf(szError, "NPERR_INVALID_PLUGIN_ERROR");
      break;
    case NPERR_INVALID_PLUGIN_DIR_ERROR:
      sprintf(szError, "NPERR_INVALID_PLUGIN_DIR_ERROR");
      break;
    case NPERR_INCOMPATIBLE_VERSION_ERROR:
      sprintf(szError, "NPERR_INCOMPATIBLE_VERSION_ERROR");
      break;
    case NPERR_INVALID_PARAM:
      sprintf(szError, "NPERR_INVALID_PARAM");
      break;
    case NPERR_INVALID_URL:
      sprintf(szError, "NPERR_INVALID_URL");
      break;
    case NPERR_FILE_NOT_FOUND:
      sprintf(szError, "NPERR_FILE_NOT_FOUND");
      break;
    case NPERR_NO_DATA:
      sprintf(szError, "NPERR_NO_DATA");
      break;
    case NPERR_STREAM_NOT_SEEKABLE:
      sprintf(szError, "NPERR_STREAM_NOT_SEEKABLE");
      break;
    default:
      sprintf(szError, "Unlisted error");
      break;
  }
  return &szError[0];
}

char * FormatNPAPIReason(int iReason)
{
  static char szReason[64];
  switch (iReason)
  {
    case NPRES_DONE:
      sprintf(szReason, "NPRES_DONE");
      break;
    case NPRES_NETWORK_ERR:
      sprintf(szReason, "NPRES_NETWORK_ERR");
      break;
    case NPRES_USER_BREAK:
      sprintf(szReason, "NPRES_USER_BREAK");
      break;
    default:
      sprintf(szReason, "Unlisted reason");
      break;
  }
  return &szReason[0];
}

char * FormatNPNVariable(NPNVariable var)
{
  static char szVar[80];
  switch (var)
  {
    case NPNVxDisplay:
      sprintf(szVar, "%i -- NPNVxDisplay", var);
      break;
    case NPNVxtAppContext:
      sprintf(szVar, "%i -- NPNVxtAppContext", var);
      break;
    case NPNVnetscapeWindow:
      sprintf(szVar, "%i -- NPNVnetscapeWindow", var);
      break;
    case NPNVjavascriptEnabledBool:
      sprintf(szVar, "%i -- NPNVjavascriptEnabledBool", var);
      break;
    case NPNVasdEnabledBool:
      sprintf(szVar, "%i -- NPNVasdEnabledBool", var);
      break;
    case NPNVisOfflineBool:
      sprintf(szVar, "%i -- NPNVisOfflineBool", var);
      break;
    default:
      sprintf(szVar, "%i -- Unlisted variable", var);
      break;
  }
  return &szVar[0];
}

char * FormatNPPVariable(NPPVariable var)
{
  static char szVar[80];
  switch (var)
  {
    case NPPVpluginNameString:
      sprintf(szVar, "%i -- NPPVpluginNameString", var);
      break;
    case NPPVpluginDescriptionString:
      sprintf(szVar, "%i -- NPPVpluginDescriptionString?", var);
      break;
    case NPPVpluginWindowBool:
      sprintf(szVar, "%i -- NPPVpluginWindowBool?", var);
      break;
    case NPPVpluginTransparentBool:
      sprintf(szVar, "%i -- NPPVpluginTransparentBool?", var);
      break;
    case NPPVjavaClass:
      sprintf(szVar, "%i -- NPPVjavaClass?", var);
      break;
    case NPPVpluginWindowSize:
      sprintf(szVar, "%i -- NPPVpluginWindowSize?", var);
      break;
    case NPPVpluginTimerInterval:
      sprintf(szVar, "%i -- NPPVpluginTimerInterval?", var);
      break;
    case NPPVpluginScriptableInstance:
      sprintf(szVar, "%i -- NPPVpluginScriptableInstance?", var);
      break;
    case NPPVpluginScriptableIID:
      sprintf(szVar, "%i -- NPPVpluginScriptableIID?", var);
      break;
    default:
      sprintf(szVar, "%i -- Unlisted variable?", var);
      break;
  }
  return &szVar[0];
}

BOOL FormatPCHARArgument(char * szBuf, int iLength, LogArgumentStruct * parg)
{
  if(iLength <= parg->iLength)
    return FALSE;

  if(parg->pData == NULL)
    sprintf(szBuf, "%#08lx", parg->dwArg);
  else
    sprintf(szBuf, "%#08lx(\"%s\")", parg->dwArg, (char *)parg->pData);
  return TRUE;
}

BOOL FormatBOOLArgument(char * szBuf, int iLength, LogArgumentStruct * parg)
{
  if(iLength <= 8)
    return FALSE;

  sprintf(szBuf, "%s", ((NPBool)parg->dwArg == TRUE) ? "TRUE" : "FALSE");
  return TRUE;
}

BOOL FormatPBOOLArgument(char * szBuf, int iLength, LogArgumentStruct * parg)
{
  if(iLength <= 8)
    return FALSE;

  sprintf(szBuf, "%#08lx(%s)", parg->dwArg, (*((NPBool *)parg->pData) == TRUE) ? "TRUE" : "FALSE");
  return TRUE;
}

static void makeAbbreviatedString(char * szBuf, int iSize, DWORD dwArg, int iLength, int iWrap)
{
  if(dwArg == 0L)
  {
    szBuf[0] = '\0';
    return;
  }

  if(iLength > iWrap)
  {
    int iRealWrap = (iSize > iWrap) ? iWrap : (iSize - 4);
    memcpy((LPVOID)&szBuf[0], (LPVOID)dwArg, iRealWrap);
    szBuf[iRealWrap]     = '.';
    szBuf[iRealWrap + 1] = '.';
    szBuf[iRealWrap + 2] = '.';
    szBuf[iRealWrap + 3] = '\0';
  }
  else
  {
    if(iLength >= iSize)
    {    
      memcpy((LPVOID)&szBuf[0], (LPVOID)dwArg, iSize - 4);
      szBuf[iSize]     = '.';
      szBuf[iSize + 1] = '.';
      szBuf[iSize + 2] = '.';
      szBuf[iSize + 3] = '\0';
    }
    else
    {
      memcpy((LPVOID)&szBuf[0], (LPVOID)dwArg, iLength);
      szBuf[iLength] = '\0';
    }
  }
}

LogItemStruct * makeLogItemStruct(NPAPI_Action action, 
                                  DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4, 
                                  DWORD dw5, DWORD dw6, DWORD dw7, BOOL bShort)
{
  int iWrap = 64;

  LogItemStruct * plis = new LogItemStruct;
  if(plis == NULL)
    return NULL;

  plis->action = action;
  plis->arg1.dwArg = dw1;
  plis->arg2.dwArg = dw2;
  plis->arg3.dwArg = dw3;
  plis->arg4.dwArg = dw4;
  plis->arg5.dwArg = dw5;
  plis->arg6.dwArg = dw6;
  plis->arg7.dwArg = dw7;

  char szTarget[1024] = {'\0'};
  char szBuf[1024] = {'\0'};

  if(bShort)
    return plis;

  switch (action)
  {
    case action_invalid:
      break;

    // NPN action
    case action_npn_version:
      plis->arg1.pData = new int[1];
      *(int*)(plis->arg1.pData) = *((int*)dw1);
      plis->arg1.iLength = sizeof(int);

      plis->arg2.pData = new int[1];
      *(int*)(plis->arg2.pData) = *((int*)dw2);
      plis->arg2.iLength = sizeof(int);

      plis->arg3.pData = new int[1];
      *(int*)(plis->arg3.pData) = *((int*)dw3);
      plis->arg3.iLength = sizeof(int);

      plis->arg4.pData = new int[1];
      *(int*)(plis->arg4.pData) = *((int*)dw4);
      plis->arg4.iLength = sizeof(int);

      break;
    case action_npn_get_url_notify:
      if(dw2 != 0L)
      {
        plis->arg2.iLength = strlen((char *)dw2) + 1;
        plis->arg2.pData = new char[plis->arg2.iLength];
        memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      }

      if(dw3 != 0L)
      {
        makeAbbreviatedString(szTarget, sizeof(szTarget), dw3, strlen((char *)dw3), iWrap);
        plis->arg3.iLength = strlen(szTarget) + 1;
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)&szTarget[0], plis->arg3.iLength);
      }
      break;
    case action_npn_get_url:
    {
      if(dw2 != 0L)
      {
        plis->arg2.iLength = strlen((char *)dw2) + 1;
        plis->arg2.pData = new char[plis->arg2.iLength];
        memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      }

      if(dw3 != 0L)
      {
        makeAbbreviatedString(szTarget, sizeof(szTarget), dw3, strlen((char *)dw3), iWrap);
        plis->arg3.iLength = strlen(szTarget) + 1;
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)&szTarget[0], plis->arg3.iLength);
      }
      break;
    }
    case action_npn_post_url_notify:
    {
      if(dw2 != 0L)
      {
        plis->arg2.iLength = strlen((char *)dw2) + 1;
        plis->arg2.pData = new char[plis->arg2.iLength];
        memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      }

      if(dw3 != 0L)
      {
        makeAbbreviatedString(szTarget, sizeof(szTarget), dw3, strlen((char *)dw3), iWrap);
        plis->arg3.iLength = strlen(szTarget) + 1;
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)&szTarget[0], plis->arg3.iLength);
      }

      makeAbbreviatedString(szBuf, sizeof(szBuf), dw5, strlen((char *)dw5), iWrap);
      plis->arg5.iLength = (int)dw4 + 1;
      plis->arg5.pData = new char[plis->arg5.iLength];
      memcpy(plis->arg5.pData, (LPVOID)&szBuf[0], plis->arg5.iLength);

      break;
    }
    case action_npn_post_url:
    {
      if(dw2 != 0L)
      {
        plis->arg2.iLength = strlen((char *)dw2) + 1;
        plis->arg2.pData = new char[plis->arg2.iLength];
        memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      }

      if(dw3 != 0L)
      {
        makeAbbreviatedString(szTarget, sizeof(szTarget), dw3, strlen((char *)dw3), iWrap);
        plis->arg3.iLength = strlen(szTarget) + 1;
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)&szTarget[0], plis->arg3.iLength);
      }

      makeAbbreviatedString(szBuf, sizeof(szBuf), dw5, strlen((char *)dw5), iWrap);
      plis->arg5.iLength = (int)dw4 + 1;
      plis->arg5.pData = new char[plis->arg5.iLength];
      memcpy(plis->arg5.pData, (LPVOID)&szBuf[0], plis->arg5.iLength);

      break;
    }
    case action_npn_new_stream:
    {
      if(dw2 != 0L)
      {
        plis->arg2.iLength = strlen((char *)dw2) + 1;
        plis->arg2.pData = new char[plis->arg2.iLength];
        memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      }

      makeAbbreviatedString(szTarget, sizeof(szTarget), dw3, strlen((char *)dw3), iWrap);
      plis->arg3.iLength = strlen(szTarget) + 1;
      plis->arg3.pData = new char[plis->arg3.iLength];
      memcpy(plis->arg3.pData, (LPVOID)&szTarget[0], plis->arg3.iLength);

      plis->arg4.pData = new char[sizeof(DWORD)];
      plis->arg4.iLength = sizeof(DWORD);
      memcpy(plis->arg4.pData, (LPVOID)dw4, plis->arg4.iLength);

      break;
    }
    case action_npn_destroy_stream:
      break;
    case action_npn_request_read:
      break;
    case action_npn_write:
    {
      makeAbbreviatedString(szBuf, sizeof(szBuf), dw4, strlen((char *)dw4), iWrap);
      plis->arg4.iLength = strlen(szBuf) + 1;
      plis->arg4.pData = new char[plis->arg4.iLength];
      memcpy(plis->arg4.pData, (LPVOID)&szBuf[0], plis->arg4.iLength);
      break;
    }
    case action_npn_status:
      if(dw2 != 0L)
      {
        plis->arg2.iLength = strlen((char *)dw2) + 1;
        plis->arg2.pData = new char[plis->arg2.iLength];
        memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      }
      break;
    case action_npn_user_agent:
      break;
    case action_npn_mem_alloc:
      break;
    case action_npn_mem_free:
      break;
    case action_npn_mem_flush:
      break;
    case action_npn_reload_plugins:
      break;
    case action_npn_get_java_env:
      break;
    case action_npn_get_java_peer:
      break;
    case action_npn_get_value:
      plis->arg3.iLength = sizeof(DWORD);
      plis->arg3.pData = new char[plis->arg3.iLength];
      memcpy(plis->arg3.pData, (LPVOID)dw3, plis->arg3.iLength);
      break;
    case action_npn_set_value:
      if(((NPPVariable)dw2 == NPPVpluginNameString) || ((NPPVariable)dw2 == NPPVpluginDescriptionString))
      {
        makeAbbreviatedString(szBuf, sizeof(szBuf), dw3, strlen((char *)dw3), iWrap);
        plis->arg3.iLength = strlen(szBuf) + 1;
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)&szBuf[0], plis->arg3.iLength);
      }
      else if(((NPPVariable)dw2 == NPPVpluginWindowBool) || ((NPPVariable)dw2 == NPPVpluginTransparentBool))
      {
        plis->arg3.iLength = sizeof(NPBool);
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)&dw3, plis->arg3.iLength);
      }
      else if((NPPVariable)dw2 == NPPVpluginWindowSize)
      {
        plis->arg3.iLength = sizeof(NPSize);
        plis->arg3.pData = new char[plis->arg3.iLength];
        memcpy(plis->arg3.pData, (LPVOID)dw3, plis->arg3.iLength);
      }
      break;
    case action_npn_invalidate_rect:
    {
      plis->arg2.iLength = sizeof(NPRect);
      plis->arg2.pData = new char[plis->arg2.iLength];
      memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      break;
    }
    case action_npn_invalidate_region:
      break;
    case action_npn_force_redraw:
      break;

    // NPP action
    case action_npp_new:
      plis->arg1.iLength = strlen((char *)dw1) + 1;
      plis->arg1.pData = new char[plis->arg1.iLength];
      memcpy(plis->arg1.pData, (LPVOID)dw1, plis->arg1.iLength);
      break;
    case action_npp_destroy:
      plis->arg2.iLength = sizeof(DWORD);
      plis->arg2.pData = new char[plis->arg2.iLength];
      memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      break;
    case action_npp_set_window:
      plis->arg2.iLength = sizeof(NPWindow);
      plis->arg2.pData = new char[plis->arg2.iLength];
      memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      break;
    case action_npp_new_stream:
      plis->arg2.iLength = strlen((char *)dw2) + 1;
      plis->arg2.pData = new char[plis->arg2.iLength];
      memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);

      plis->arg5.iLength = sizeof(uint16);
      plis->arg5.pData = new char[plis->arg5.iLength];
      memcpy(plis->arg5.pData, (LPVOID)dw5, plis->arg5.iLength);
      break;
    case action_npp_destroy_stream:
      break;
    case action_npp_stream_as_file:
      plis->arg3.iLength = strlen((char *)dw3) + 1;
      plis->arg3.pData = new char[plis->arg3.iLength];
      memcpy(plis->arg3.pData, (LPVOID)dw3, plis->arg3.iLength);
      break;
    case action_npp_write_ready:
      break;
    case action_npp_write:
    {
      if(dw5 != 0L)
      {
        makeAbbreviatedString(szBuf, sizeof(szBuf), dw5, strlen((char *)dw5), iWrap);
        plis->arg5.iLength = strlen(szBuf) + 1;
        plis->arg5.pData = new char[plis->arg5.iLength];
        memcpy(plis->arg5.pData, (LPVOID)&szBuf[0], plis->arg5.iLength);
      }
      break;
    }
    case action_npp_print:
      break;
    case action_npp_handle_event:
      break;
    case action_npp_url_notify:
      plis->arg2.iLength = strlen((char *)dw2) + 1;
      plis->arg2.pData = new char[plis->arg2.iLength];
      memcpy(plis->arg2.pData, (LPVOID)dw2, plis->arg2.iLength);
      break;
    case action_npp_get_java_class:
      break;
    case action_npp_get_value:
      break;
    case action_npp_set_value:
      break;

    default:
      break;
  }

  return plis;
}

void freeLogItemStruct(LogItemStruct * lis)
{
  if(lis)
    delete lis;
}

void formatLogItem(LogItemStruct * plis, std::string* output, BOOL bDOSStyle)
{
  static char szEOL[8];
  static char szEOI[256];
  static char szEndOfItem[] = "";

  if(bDOSStyle)
  {
    strcpy(szEOL, "\r\n");
    //strcpy(szEOI, szEndOfItem);
    //strcat(szEOI, "\r\n");
  }
  else
  {
    strcpy(szEOL, "\n");
    //strcpy(szEOI, szEndOfItem);
    //strcat(szEOI, "\n");
  }

  DWORD dw1 = plis->arg1.dwArg;
  DWORD dw2 = plis->arg2.dwArg;
  DWORD dw3 = plis->arg3.dwArg;
  DWORD dw4 = plis->arg4.dwArg;
  DWORD dw5 = plis->arg5.dwArg;
  DWORD dw6 = plis->arg6.dwArg;
  DWORD dw7 = plis->arg7.dwArg;

  char sz1[1024] = {'\0'};
  char sz2[1024] = {'\0'};
  char sz3[1024] = {'\0'};
  char sz4[1024] = {'\0'};
  char sz5[1024] = {'\0'};
  char sz6[1024] = {'\0'};

  switch (plis->action)
  {
    case action_invalid:
      break;

    // NPN action
    case action_npn_version:
      if((plis->arg1.pData != NULL)&&(plis->arg2.pData != NULL)&&(plis->arg3.pData != NULL)&&(plis->arg4.pData != NULL))
        *output = StringPrintf("NPN_Version(%#08lx, %#08lx, %#08lx, %#08lx)", dw1,dw2,dw3,dw4);
      else
        *output = StringPrintf("NPN_Version(%#08lx, %#08lx, %#08lx, %#08lx)", dw1,dw2,dw3,dw4);
      break;
    case action_npn_get_url_notify:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
      *output = StringPrintf("NPN_GetURLNotify(%#08lx, %s, %s, %#08lx)", dw1,sz2,sz3,dw4);
      break;
    }
    case action_npn_get_url:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
      *output = StringPrintf("NPN_GetURL(%#08lx, %s, %s)", dw1,sz2,sz3);
      break;
    }
    case action_npn_post_url_notify:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
      FormatPCHARArgument(sz5, sizeof(sz5), &plis->arg5);
      FormatBOOLArgument(sz6, sizeof(sz6), &plis->arg6);

      *output = StringPrintf("NPN_PostURLNotify(%#08lx, %s, %s, %li, %s, %s, %#08lx)", 
               dw1,sz2,sz3,(uint32)dw4,sz5,sz6,dw7);
      break;
    }
    case action_npn_post_url:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
      FormatPCHARArgument(sz5, sizeof(sz5), &plis->arg5);
      FormatBOOLArgument(sz6, sizeof(sz6), &plis->arg6);

      *output = StringPrintf("NPN_PostURL(%#08lx, %s, %s, %li, %s, %s)", 
               dw1,sz2,sz3,(uint32)dw4,sz5,sz6);
      break;
    }
    case action_npn_new_stream:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
      if(plis->arg4.pData != NULL)
        *output = StringPrintf("NPN_NewStream(%#08lx, %s, %s, %#08lx(%#08lx))", 
                 dw1, sz2,sz3,dw4,*(DWORD *)plis->arg4.pData);
      else
        *output = StringPrintf("NPN_NewStream(%#08lx, \"%s\", \"%s\", %#08lx)", dw1, sz2,sz3,dw4);
      break;
    }
    case action_npn_destroy_stream:
      *output = StringPrintf("NPN_DestroyStream(%#08lx, %#08lx, %s)", dw1,dw2,FormatNPAPIReason((int)dw3));
      break;
    case action_npn_request_read:
      *output = StringPrintf("NPN_RequestRead(%#08lx, %#08lx)", dw1, dw2);
      break;
    case action_npn_write:
    {
      FormatPCHARArgument(sz4, sizeof(sz4), &plis->arg4);
      *output = StringPrintf("NPN_Write(%#08lx, %#08lx, %li, %s)", dw1, dw2, (int32)dw3, sz4);
      break;
    }
    case action_npn_status:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      *output = StringPrintf("NPN_Status(%#08lx, %s)", dw1, sz2);
      break;
    }
    case action_npn_user_agent:
      *output = StringPrintf("NPN_UserAgent(%#08lx)", dw1);
      break;
    case action_npn_mem_alloc:
      *output = StringPrintf("NPN_MemAlloc(%li)", dw1);
      break;
    case action_npn_mem_free:
      *output = StringPrintf("NPN_MemFree(%#08lx)", dw1);
      break;
    case action_npn_mem_flush:
      *output = StringPrintf("NPN_MemFlush(%li)", dw1);
      break;
    case action_npn_reload_plugins:
    {
      FormatBOOLArgument(sz1, sizeof(sz1), &plis->arg1);
      *output = StringPrintf("NPN_ReloadPlugins(%s)", sz1);
      break;
    }
    case action_npn_get_java_env:
      *output = StringPrintf("NPN_GetJavaEnv()");
      break;
    case action_npn_get_java_peer:
      *output = StringPrintf("NPN_GetJavaPeer(%#08lx)", dw1);
      break;
    case action_npn_get_value:
    {
      switch(dw2)
      {
        case NPNVxDisplay:
        case NPNVxtAppContext:
        case NPNVnetscapeWindow:
          if(dw3 != 0L)
            *output = StringPrintf("NPN_GetValue(%#08lx, %s, %#08lx(%#08lx))",dw1,FormatNPNVariable((NPNVariable)dw2),dw3,*(DWORD *)dw3);
          else
            *output = StringPrintf("NPN_GetValue(%#08lx, %s, %#08lx)",dw1,FormatNPNVariable((NPNVariable)dw2),dw3);
          break;
        case NPNVjavascriptEnabledBool:
        case NPNVasdEnabledBool:
        case NPNVisOfflineBool:
          if(dw3 != 0L)
            *output = StringPrintf("NPN_GetValue(%#08lx, %s, %#08lx(%s))",
                     dw1,FormatNPNVariable((NPNVariable)dw2),dw3,
                     (((NPBool)*(DWORD *)dw3) == TRUE) ? "TRUE" : "FALSE");
          else
            *output = StringPrintf("NPN_GetValue(%#08lx, %s, %#08lx)",dw1,FormatNPNVariable((NPNVariable)dw2),dw3);
          break;
        default:
          break;
      }
      break;
    }
    case action_npn_set_value:

      if(((NPPVariable)dw2 == NPPVpluginNameString) || ((NPPVariable)dw2 == NPPVpluginDescriptionString))
      {
        FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
        *output = StringPrintf("NPN_SetValue(%#08lx, %s, %s)", dw1,FormatNPPVariable((NPPVariable)dw2),sz3);
      }
      else if(((NPPVariable)dw2 == NPPVpluginWindowBool) || ((NPPVariable)dw2 == NPPVpluginTransparentBool))
      {
        FormatPBOOLArgument(sz3, sizeof(sz3), &plis->arg3);
        *output = StringPrintf("NPN_SetValue(%#08lx, %s, %s)", 
                 dw1,FormatNPPVariable((NPPVariable)dw2),sz3);
      }
      else if((NPPVariable)dw2 == NPPVpluginWindowSize)
      {
        if(plis->arg3.pData != NULL)
        {
          int32 iWidth = ((NPSize *)plis->arg3.pData)->width;
          int32 iHeight = ((NPSize *)plis->arg3.pData)->height;
          *output = StringPrintf("NPN_SetValue(%#08lx, %s, %#08lx(%li,%li))", 
                   dw1,FormatNPPVariable((NPPVariable)dw2),dw3,iWidth,iHeight);
        }
        else
          *output = StringPrintf("NPN_SetValue(%#08lx, %s, %#08lx(?,?))", 
                   dw1,FormatNPPVariable((NPPVariable)dw2),dw3);
      }
      else
        *output = StringPrintf("NPN_SetValue(%#08lx, %s, %#08lx(What is it?))", dw1,FormatNPPVariable((NPPVariable)dw2),dw3);
      break;
    case action_npn_invalidate_rect:
    {
      if(plis->arg2.pData != NULL)
      {
        uint16 top    = ((NPRect *)plis->arg2.pData)->top;
        uint16 left   = ((NPRect *)plis->arg2.pData)->left;
        uint16 bottom = ((NPRect *)plis->arg2.pData)->bottom;
        uint16 right  = ((NPRect *)plis->arg2.pData)->right;
        *output = StringPrintf("NPN_InvalidateRect(%#08lx, %#08lx(%u,%u;%u,%u)", dw1,dw2,top,left,bottom,right);
      }
      else
        *output = StringPrintf("NPN_InvalidateRect(%#08lx, %#08lx(?,?,?,?)", dw1,dw2);
      break;
    }
    case action_npn_invalidate_region:
      *output = StringPrintf("NPN_InvalidateRegion(%#08lx, %#08lx)", dw1,dw2);
      break;
    case action_npn_force_redraw:
      *output = StringPrintf("NPN_ForceRedraw(%#08lx)", dw1);
      break;
    case action_npn_enumerate:
      *output = StringPrintf("NPN_Enumerate()");
      break;
    case action_npn_pop_popups_enabled_state:
      *output = StringPrintf("NPN_PopPopupsEnabledState()");
      break;
    case action_npn_push_popups_enabled_state:
      *output = StringPrintf("NPN_PushPopupsEnabledState()");
      break;
    case action_npn_set_exception:
      *output = StringPrintf("NPN_SetException(%s)", dw1);
      break;
    case action_npn_has_method:
      *output = StringPrintf("NPN_HasMethod(%d)", dw1);
      break;
    case action_npn_has_property:
      *output = StringPrintf("NPN_HasProperty(%d)", dw1);
      break;
    case action_npn_remove_property:
      *output = StringPrintf("NPN_RemoveProperty(%d)", dw1);
      break;
    case action_npn_set_property:
      *output = StringPrintf("NPN_SetProperty(%d)", dw1);
      break;
    case action_npn_get_property:
      *output = StringPrintf("NPN_GetProperty(%d)", dw1);
      break;
    case action_npn_evaluate:
      *output = StringPrintf("NPN_Evaluate(%s)", dw1);
      break;
    case action_npn_invoke_default:
      *output = StringPrintf("NPN_InvokeDefault(%#08lx)", dw1);
      break;
    case action_npn_invoke:
      *output = StringPrintf("NPN_Invoke(%#08lx)", dw1);
      break;
    case action_npn_release_object:
      *output = StringPrintf("NPN_ReleaseObject(%d)", dw1);
      break;
    case action_npn_retain_object:
      *output = StringPrintf("NPN_RetainObject(%d)", dw1);
      break;
    case action_npn_create_object:
      *output = StringPrintf("NPN_CreateObject(%#08lx)", dw1);
      break;
    case action_npn_int_from_identifier:
      *output = StringPrintf("NPN_IntFromIdentifier(%d)", dw1);
      break;
    case action_npn_utf8_from_identifier:
      *output = StringPrintf("NPN_UTF8FromIdentifier(%d)", dw1);
      break;
    case action_npn_identifier_is_string:
      *output = StringPrintf("NPN_IdentifierIsString(%d)", dw1);
      break;
    case action_npn_get_int_identifer:
      *output = StringPrintf("NPN_GetIntIdentifier(%d)", dw1);
      break;
    case action_npn_get_string_identifiers:
      *output = StringPrintf("NPN_GetStringIdentifier()");
      break;

    // NPP action
    case action_npp_new:
    {
      char szMode[16];
      switch (dw3)
      {
        case NP_EMBED:
          strcpy(szMode, "NP_EMBED");
          break;
        case NP_FULL:
          strcpy(szMode, "NP_FULL");
          break;
        default:
          strcpy(szMode, "[Invalid mode]");
          break;
      }
      *output = StringPrintf("NPP_New(\"%s\", %#08lx, %s, %i, %#08lx, %#08lx, %#08lx)", 
               (char *)dw1,dw2,szMode,(int)dw4,dw5,dw6,dw7);
      break;
    }
    case action_npp_destroy:
      *output = StringPrintf("NPP_Destroy(%#08lx, %#08lx(%#08lx))", dw1, dw2, *(DWORD *)plis->arg2.pData);
      break;
    case action_npp_set_window:
    {
      char szWindow[512];

      if(plis->arg2.pData != NULL)
      {
        char szType[80];
        switch (((NPWindow*)plis->arg2.pData)->type)
        {
          case NPWindowTypeWindow:
            sprintf(szType, "NPWindowTypeWindow");
            break;
          case NPWindowTypeDrawable:
            sprintf(szType, "NPWindowTypeDrawable");
            break;
          default:
            sprintf(szType, "[Unlisted type]");
            break;
        }
        sprintf(szWindow, "NPWindow: %#08lx, (%li,%li), (%li,%li), (%i,%i,%i,%i), %s", 
                 ((NPWindow*)plis->arg2.pData)->window, 
                 ((NPWindow*)plis->arg2.pData)->x, 
                 ((NPWindow*)plis->arg2.pData)->y, 
                 ((NPWindow*)plis->arg2.pData)->width, 
                 ((NPWindow*)plis->arg2.pData)->height, 
                 ((NPWindow*)plis->arg2.pData)->clipRect.top, 
                 ((NPWindow*)plis->arg2.pData)->clipRect.left, 
                 ((NPWindow*)plis->arg2.pData)->clipRect.bottom, 
                 ((NPWindow*)plis->arg2.pData)->clipRect.right, szType);
        *output = StringPrintf("NPP_SetWindow(%#08lx, %#08lx) %s", dw1,dw2,szWindow);
      }
      else
        *output = StringPrintf("NPP_SetWindow(%#08lx, %#08lx)", dw1,dw2);

      break;
    }
    case action_npp_new_stream:
    {
      switch (*(int16 *)plis->arg5.pData)
      {
        case NP_NORMAL:
          sprintf(sz5, "NP_NORMAL");
          break;
        case NP_ASFILEONLY:
          sprintf(sz5, "NP_ASFILEONLY");
          break;
        case NP_ASFILE:
          sprintf(sz5, "NP_ASFILE");
          break;
        default:
          sprintf(sz5, "[Unlisted type]");
          break;
      }
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      *output = StringPrintf("NPP_NewStream(%#08lx, %s, %#08lx (%s), %s, %s)", dw1, sz2, dw3, 
               ((NPStream*)dw3)->url, ((NPBool)dw4 == TRUE) ? "TRUE" : "FALSE", sz5);
      break;
    }
    case action_npp_destroy_stream:
      *output = StringPrintf("NPP_DestroyStream(%#08lx, %#08lx, %s)", dw1,dw2,FormatNPAPIReason((int)dw3));
      break;
    case action_npp_stream_as_file:
      FormatPCHARArgument(sz3, sizeof(sz3), &plis->arg3);
      *output = StringPrintf("NPP_StreamAsFile(%#08lx, %#08lx, %s)", dw1,dw2,sz3);
      break;
    case action_npp_write_ready:
      *output = StringPrintf("NPP_WriteReady(%#08lx, %#08lx)", dw1,dw2);
      break;
    case action_npp_write:
    {
      FormatPCHARArgument(sz5, sizeof(sz5), &plis->arg5);
      *output = StringPrintf("NPP_Write(%#08lx, %#08lx, %li, %li, %s))",dw1,dw2,dw3,dw4,sz5);
      break;
    }
    case action_npp_print:
      *output = StringPrintf("NPP_Print(%#08lx, %#08lx)", dw1, dw2);
      break;
    case action_npp_handle_event:
    {
      NPEvent *event = (NPEvent*)dw2;
      *output = StringPrintf("NPP_HandleEvent(%#08lx, %#08lx {event=%d, wParam=%#08lx lParam=%#08lx)", dw1,dw2,event->event, event->wParam, event->lParam);
      break;
    }
    case action_npp_url_notify:
    {
      FormatPCHARArgument(sz2, sizeof(sz2), &plis->arg2);
      *output = StringPrintf("NPP_URLNotify(%#08lx, %s, %s, %#08lx)", dw1,sz2,FormatNPAPIReason((int)dw3),dw4);
      break;
    }
    case action_npp_get_java_class:
      *output = StringPrintf("NPP_GetJavaClass()");
      break;
    case action_npp_get_value:
      *output = StringPrintf("NPP_GetValue(%#08lx, %s, %#08lx)", dw1,FormatNPPVariable((NPPVariable)dw2),dw3);
      break;
    case action_npp_set_value:
      *output = StringPrintf("NPP_SetValue(%#08lx, %s, %#08lx)", dw1,FormatNPNVariable((NPNVariable)dw2),dw3);
      break;

    default:
      *output = StringPrintf("Unknown action");
      break;
  }
  *output += szEOL;
  *output += szEOI;
}
