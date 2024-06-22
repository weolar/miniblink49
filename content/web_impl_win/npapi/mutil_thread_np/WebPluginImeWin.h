// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef content_web_impl_win_npapi_WebPlugiImeWin_h
#define content_web_impl_win_npapi_WebPlugiImeWin_h

#include "third_party/npapi/bindings/npapi.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include <string>
#include <xstring>
#include <vector>
#include <windows.h>

namespace blink {
class IntRect;
}

namespace content {

class PluginInstance;

// A class that emulates an IME for windowless plugins. A windowless plugin
// does not have a window. Therefore, we cannot attach an IME to a windowless
// plugin. To allow such windowless plugins to use IMEs without any changes to
// them, this class receives the IME data from a browser and patches IMM32
// functions to return the IME data when a windowless plugin calls IMM32
// functions. I would not Flash retrieves pointers to IMM32 functions with
// getProcAddress(), this class also needs a hook to getProcAddress() to
// dispatch IMM32 function calls from a plugin to this class as listed in the
// following snippet.
//
//   FARPROC WINAPI GetProcAddressPatch(HMODULE module, LPCSTR name) {
//     FARPROC* proc = WebPluginIMEWin::getProcAddress(name);
//     if (proc)
//       return proc;
//     return ::GetProcAddress(module, name);
//   }
//   ...
//   app::win::IATPatchFunction get_proc_address;
//   get_proc_address.Patch(
//       GetPluginPath().value().c_str(), "kernel32.dll", "GetProcAddress",
//       GetProcAddressPatch);
//
// After we successfuly dispatch IMM32 calls from a plugin to this class, we
// need to update its IME data so the class can return it to the plugin through
// its IMM32 calls. To update the IME data, we call compositionUpdated() or
// compositionCompleted() BEFORE sending an IMM32 Window message to the plugin
// with a sendEvents() call as listed in the following snippet. (Plugins call
// IMM32 functions when it receives IMM32 window messages. We need to update the
// IME data of this class before sending IMM32 messages so the plugins can get
// the latest data.)
//
//   WebPluginIMEWin ime;
//   ...
//   std::wstring text = "composing";
//   std::vector<int> clauses;
//   clauses.push_back(0);
//   clauses.push_back(text.length());
//   std::vector<int> target;
//   ime.compositionUpdated(text, clauses, target, text.length());
//   ime.sendEvents(instance());
//
//   std::wstring result = "result";
//   ime.compositionCompleted(result);
//   ime.sendEvents(instance());
//
// This class also provides getStatus() so we can retrieve the IME status
// changed by a plugin with IMM32 functions. This function is mainly used for
// retrieving the position of a caret.
//
class WebPluginIMEWin {
public:
    // A simple class that allows a plugin to access a WebPluginIMEWin instance
    // only in a scope.
    class ScopedLock {
    public:
        explicit ScopedLock(WebPluginIMEWin* instance) : m_instance(instance) {
            if (m_instance)
                m_instance->lock();
        }
        ~ScopedLock() {
            if (m_instance)
                m_instance->unlock();
        }

    private:
        WebPluginIMEWin* m_instance;
    };

    WebPluginIMEWin();
    ~WebPluginIMEWin();

    static bool hookGetProcAddress(HMODULE module);
    static FARPROC WINAPI GetProcAddressPatch(HMODULE module, LPCSTR name);

    // Sends raw IME events sent from a browser to this IME emulator and updates
    // the list of Windows events to be sent to a plugin. A raw IME event is
    // mapped to two or more Windows events and it is not so trivial to send these
    // Windows events to a plugin. This function inserts Windows events in the
    // order expected by a plugin.
    void compositionUpdated(const String& text,
        std::vector<int> clauses,
        std::vector<int> target,
        int cursorPosition);
    void compositionCompleted(const String& text);

    // Send all the events added in Update() to a plugin.
    void sendEvents(int id, NPP instance, NPP_HandleEventProcPtr event);

    // Retrieves the status of this IME emulator.
    bool getStatus(int* inputType, blink::IntRect* caretRect);

    // Returns the pointers to IMM32-emulation functions implemented by this
    // class. This function is used for over-writing the ones returned from
    // getProcAddress() calls of Win32 API.
    static FARPROC getProcAddress(const char* name);

private:
    // Allow (or disallow) the patch functions to use this WebPluginIMEWin
    // instance through our patch functions. Our patch functions need a static
    // member variable |instance_| to access a WebPluginIMEWIn instance. We lock
    // this static variable to prevent two or more plugins from accessing a
    // WebPluginIMEWin instance.
    void lock();
    void unlock();

    // Retrieve the instance of this class.
    static WebPluginIMEWin* getInstance(HIMC context);

    // IMM32 patch functions implemented by this class.
    static BOOL WINAPI immAssociateContextEx(HWND window, HIMC context, DWORD flags);
    static LONG WINAPI immGetCompositionStringW(HIMC context, DWORD index, LPVOID dstData, DWORD dstSize);
    static HIMC WINAPI immGetContext(HWND window);
    static BOOL WINAPI immReleaseContext(HWND window, HIMC context);
    static BOOL WINAPI immSetCandidateWindow(HIMC context, CANDIDATEFORM* candidate);
    static BOOL WINAPI immSetOpenStatus(HIMC context, BOOL open);

    // a list of NPEvents to be sent to a plugin.
    std::vector<NPEvent> m_events;

    // The return value for GCS_COMPSTR.
    String m_compositionText;

    // The return value for GCS_RESULTSTR.
    String m_resultText;

    // The return value for GCS_COMPATTR.
    std::string m_compositionAttributes;

    // The return value for GCS_COMPCLAUSE.
    std::vector<uint32> m_compositionClauses;

    // The return value for GCS_RESULTCLAUSE.
    uint32 m_resultClauses[2];

    // The return value for GCS_CURSORPOS.
    int m_cursorPosition;

    // The return value for GCS_DELTASTART.
    int m_deltaStart;

    // Whether we are composing text. This variable is used for sending a
    // WM_IME_STARTCOMPOSITION message when we start composing IME text.
    bool m_composingText;

    // Whether a plugin supports IME messages. When a plugin cannot handle
    // IME messages, we need to send the IME text with WM_CHAR messages as Windows
    // does.
    bool m_supportImeMessages;

    // The IME status received from a plugin.
    bool m_statusUpdated;
    int m_inputType;
    blink::IntRect m_caretRect;

    // The pointer to the WebPluginIMEWin instance used by patch functions.
    static WebPluginIMEWin* m_instance;
};

}  // namespace content

#endif  // content_web_impl_win_npapi_WebPlugiImeWin_h
