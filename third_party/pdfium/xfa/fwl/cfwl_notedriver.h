// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_NOTEDRIVER_H_
#define XFA_FWL_CFWL_NOTEDRIVER_H_

#include <deque>
#include <map>
#include <memory>
#include <vector>

#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_widget.h"
#include "xfa/fxgraphics/cxfa_graphics.h"

class CFWL_EventTarget;
class CFWL_NoteLoop;
class CFWL_TargetImp;
class CFWL_Widget;

class CFWL_NoteDriver {
 public:
  CFWL_NoteDriver();
  ~CFWL_NoteDriver();

  void SendEvent(CFWL_Event* pNote);

  void RegisterEventTarget(CFWL_Widget* pListener, CFWL_Widget* pEventSource);
  void UnregisterEventTarget(CFWL_Widget* pListener);
  void ClearEventTargets();

  CFWL_NoteLoop* GetTopLoop() const { return m_pNoteLoop.get(); }
  CFWL_Widget* GetFocus() const { return m_pFocus.Get(); }
  bool SetFocus(CFWL_Widget* pFocus);
  void SetGrab(CFWL_Widget* pGrab, bool bSet) {
    m_pGrab = bSet ? pGrab : nullptr;
  }

  void Run();
  void NotifyTargetHide(CFWL_Widget* pNoteTarget);
  void NotifyTargetDestroy(CFWL_Widget* pNoteTarget);
  void ProcessMessage(std::unique_ptr<CFWL_Message> pMessage);
  void QueueMessage(std::unique_ptr<CFWL_Message> pMessage);
  void UnqueueMessageAndProcess(CFWL_NoteLoop* pNoteLoop);

 private:
  bool DispatchMessage(CFWL_Message* pMessage, CFWL_Widget* pMessageForm);
  bool DoSetFocus(CFWL_Message* pMsg, CFWL_Widget* pMessageForm);
  bool DoKillFocus(CFWL_Message* pMsg, CFWL_Widget* pMessageForm);
  bool DoKey(CFWL_Message* pMsg, CFWL_Widget* pMessageForm);
  bool DoMouse(CFWL_Message* pMsg, CFWL_Widget* pMessageForm);
  bool DoWheel(CFWL_Message* pMsg, CFWL_Widget* pMessageForm);
  bool DoMouseEx(CFWL_Message* pMsg, CFWL_Widget* pMessageForm);
  void MouseSecondary(CFWL_Message* pMsg);
  bool IsValidMessage(CFWL_Message* pMessage);
  CFWL_Widget* GetMessageForm(CFWL_Widget* pDstTarget);

  std::deque<std::unique_ptr<CFWL_Message>> m_NoteQueue;
  std::unique_ptr<CFWL_NoteLoop> const m_pNoteLoop;
  std::map<uint32_t, std::unique_ptr<CFWL_EventTarget>> m_eventTargets;
  UnownedPtr<CFWL_Widget> m_pHover;
  UnownedPtr<CFWL_Widget> m_pFocus;
  UnownedPtr<CFWL_Widget> m_pGrab;
};

#endif  // XFA_FWL_CFWL_NOTEDRIVER_H_
