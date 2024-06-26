// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fwl/cfwl_notedriver.h"

#include <algorithm>
#include <utility>

#include "core/fxcrt/fx_extension.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "xfa/fwl/cfwl_app.h"
#include "xfa/fwl/cfwl_eventtarget.h"
#include "xfa/fwl/cfwl_messagekey.h"
#include "xfa/fwl/cfwl_messagekillfocus.h"
#include "xfa/fwl/cfwl_messagemouse.h"
#include "xfa/fwl/cfwl_messagemousewheel.h"
#include "xfa/fwl/cfwl_messagesetfocus.h"
#include "xfa/fwl/cfwl_noteloop.h"
#include "xfa/fwl/cfwl_widgetmgr.h"

CFWL_NoteDriver::CFWL_NoteDriver()
    : m_pNoteLoop(pdfium::MakeUnique<CFWL_NoteLoop>()) {
}

CFWL_NoteDriver::~CFWL_NoteDriver() = default;

void CFWL_NoteDriver::SendEvent(CFWL_Event* pNote) {
  for (const auto& pair : m_eventTargets) {
    if (pair.second->IsValid())
      pair.second->ProcessEvent(pNote);
  }
}

void CFWL_NoteDriver::RegisterEventTarget(CFWL_Widget* pListener,
                                          CFWL_Widget* pEventSource) {
  uint32_t key = pListener->GetEventKey();
  if (key == 0) {
    do {
      key = rand();
    } while (key == 0 || pdfium::ContainsKey(m_eventTargets, key));
    pListener->SetEventKey(key);
  }
  if (!m_eventTargets[key])
    m_eventTargets[key] = pdfium::MakeUnique<CFWL_EventTarget>(pListener);

  m_eventTargets[key]->SetEventSource(pEventSource);
}

void CFWL_NoteDriver::UnregisterEventTarget(CFWL_Widget* pListener) {
  uint32_t key = pListener->GetEventKey();
  if (key == 0)
    return;

  auto it = m_eventTargets.find(key);
  if (it != m_eventTargets.end())
    it->second->FlagInvalid();
}

bool CFWL_NoteDriver::SetFocus(CFWL_Widget* pFocus) {
  if (m_pFocus == pFocus)
    return true;

  CFWL_Widget* pPrev = m_pFocus.Get();
  m_pFocus = pFocus;
  if (pPrev) {
    if (IFWL_WidgetDelegate* pDelegate = pPrev->GetDelegate()) {
      CFWL_MessageKillFocus ms(pPrev, pPrev);
      pDelegate->OnProcessMessage(&ms);
    }
  }
  if (pFocus) {
    if (IFWL_WidgetDelegate* pDelegate = pFocus->GetDelegate()) {
      CFWL_MessageSetFocus ms(nullptr, pFocus);
      pDelegate->OnProcessMessage(&ms);
    }
  }
  return true;
}

void CFWL_NoteDriver::Run() {
#if _FX_OS_ == _FX_OS_LINUX_ || _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  for (;;) {
    CFWL_NoteLoop* pTopLoop = GetTopLoop();
    if (!pTopLoop || !pTopLoop->ContinueModal())
      break;
    UnqueueMessageAndProcess(pTopLoop);
  }
#endif  // _FX_OS_ == _FX_OS_LINUX_ || _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
}

void CFWL_NoteDriver::NotifyTargetHide(CFWL_Widget* pNoteTarget) {
  if (m_pFocus == pNoteTarget)
    m_pFocus = nullptr;
  if (m_pHover == pNoteTarget)
    m_pHover = nullptr;
  if (m_pGrab == pNoteTarget)
    m_pGrab = nullptr;
}

void CFWL_NoteDriver::NotifyTargetDestroy(CFWL_Widget* pNoteTarget) {
  if (m_pFocus == pNoteTarget)
    m_pFocus = nullptr;
  if (m_pHover == pNoteTarget)
    m_pHover = nullptr;
  if (m_pGrab == pNoteTarget)
    m_pGrab = nullptr;

  UnregisterEventTarget(pNoteTarget);
}

void CFWL_NoteDriver::QueueMessage(std::unique_ptr<CFWL_Message> pMessage) {
  m_NoteQueue.push_back(std::move(pMessage));
}

void CFWL_NoteDriver::UnqueueMessageAndProcess(CFWL_NoteLoop* pNoteLoop) {
  if (m_NoteQueue.empty())
    return;

  std::unique_ptr<CFWL_Message> pMessage = std::move(m_NoteQueue.front());
  m_NoteQueue.pop_front();
  if (!IsValidMessage(pMessage.get()))
    return;

  ProcessMessage(std::move(pMessage));
}

void CFWL_NoteDriver::ProcessMessage(std::unique_ptr<CFWL_Message> pMessage) {
  CFWL_Widget* pMessageForm = pMessage->GetDstTarget();
  if (!pMessageForm)
    return;

  if (!DispatchMessage(pMessage.get(), pMessageForm))
    return;

  if (pMessage->GetType() == CFWL_Message::Type::Mouse)
    MouseSecondary(pMessage.get());
}

bool CFWL_NoteDriver::DispatchMessage(CFWL_Message* pMessage,
                                      CFWL_Widget* pMessageForm) {
  switch (pMessage->GetType()) {
    case CFWL_Message::Type::SetFocus: {
      if (!DoSetFocus(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::KillFocus: {
      if (!DoKillFocus(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::Key: {
      if (!DoKey(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::Mouse: {
      if (!DoMouse(pMessage, pMessageForm))
        return false;
      break;
    }
    case CFWL_Message::Type::MouseWheel: {
      if (!DoWheel(pMessage, pMessageForm))
        return false;
      break;
    }
    default:
      break;
  }
  IFWL_WidgetDelegate* pDelegate = pMessage->GetDstTarget()->GetDelegate();
  if (pDelegate)
    pDelegate->OnProcessMessage(pMessage);

  return true;
}

bool CFWL_NoteDriver::DoSetFocus(CFWL_Message* pMessage,
                                 CFWL_Widget* pMessageForm) {
  m_pFocus = pMessage->GetDstTarget();
  return true;
}

bool CFWL_NoteDriver::DoKillFocus(CFWL_Message* pMessage,
                                  CFWL_Widget* pMessageForm) {
  if (m_pFocus == pMessage->GetDstTarget())
    m_pFocus = nullptr;
  return true;
}

bool CFWL_NoteDriver::DoKey(CFWL_Message* pMessage, CFWL_Widget* pMessageForm) {
  CFWL_MessageKey* pMsg = static_cast<CFWL_MessageKey*>(pMessage);
#if (_FX_OS_ != _FX_OS_MACOSX_)
  if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown &&
      pMsg->m_dwKeyCode == FWL_VKEY_Tab) {
    CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
    CFWL_Widget* pForm = GetMessageForm(pMsg->GetDstTarget());
    CFWL_Widget* pFocus = m_pFocus.Get();
    if (m_pFocus && pWidgetMgr->GetSystemFormWidget(m_pFocus.Get()) != pForm)
      pFocus = nullptr;

    bool bFind = false;
    CFWL_Widget* pNextTabStop = pWidgetMgr->NextTab(pForm, pFocus, bFind);
    if (!pNextTabStop) {
      bFind = false;
      pNextTabStop = pWidgetMgr->NextTab(pForm, nullptr, bFind);
    }
    if (pNextTabStop == pFocus)
      return true;
    if (pNextTabStop)
      SetFocus(pNextTabStop);
    return true;
  }
#endif

  if (!m_pFocus) {
    if (pMsg->m_dwCmd == FWL_KeyCommand::KeyDown &&
        pMsg->m_dwKeyCode == FWL_VKEY_Return) {
      CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
      CFWL_Widget* defButton = pWidgetMgr->GetDefaultButton(pMessageForm);
      if (defButton) {
        pMsg->SetDstTarget(defButton);
        return true;
      }
    }
    return false;
  }
  pMsg->SetDstTarget(m_pFocus.Get());
  return true;
}

bool CFWL_NoteDriver::DoMouse(CFWL_Message* pMessage,
                              CFWL_Widget* pMessageForm) {
  CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
  if (pMsg->m_dwCmd == FWL_MouseCommand::Leave ||
      pMsg->m_dwCmd == FWL_MouseCommand::Hover ||
      pMsg->m_dwCmd == FWL_MouseCommand::Enter) {
    return !!pMsg->GetDstTarget();
  }
  if (pMsg->GetDstTarget() != pMessageForm)
    pMsg->m_pos = pMsg->GetDstTarget()->TransformTo(pMessageForm, pMsg->m_pos);
  if (!DoMouseEx(pMsg, pMessageForm))
    pMsg->SetDstTarget(pMessageForm);
  return true;
}

bool CFWL_NoteDriver::DoWheel(CFWL_Message* pMessage,
                              CFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
  CFWL_MessageMouseWheel* pMsg = static_cast<CFWL_MessageMouseWheel*>(pMessage);
  CFWL_Widget* pDst = pWidgetMgr->GetWidgetAtPoint(pMessageForm, pMsg->m_pos);
  if (!pDst)
    return false;

  pMsg->m_pos = pMessageForm->TransformTo(pDst, pMsg->m_pos);
  pMsg->SetDstTarget(pDst);
  return true;
}

bool CFWL_NoteDriver::DoMouseEx(CFWL_Message* pMessage,
                                CFWL_Widget* pMessageForm) {
  CFWL_WidgetMgr* pWidgetMgr = pMessageForm->GetOwnerApp()->GetWidgetMgr();
  CFWL_Widget* pTarget = nullptr;
  if (m_pGrab)
    pTarget = m_pGrab.Get();

  CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
  if (!pTarget)
    pTarget = pWidgetMgr->GetWidgetAtPoint(pMessageForm, pMsg->m_pos);
  if (!pTarget)
    return false;
  if (pTarget && pMessageForm != pTarget)
    pMsg->m_pos = pMessageForm->TransformTo(pTarget, pMsg->m_pos);

  pMsg->SetDstTarget(pTarget);
  return true;
}

void CFWL_NoteDriver::MouseSecondary(CFWL_Message* pMessage) {
  CFWL_Widget* pTarget = pMessage->GetDstTarget();
  if (pTarget == m_pHover)
    return;

  CFWL_MessageMouse* pMsg = static_cast<CFWL_MessageMouse*>(pMessage);
  if (m_pHover) {
    CFWL_MessageMouse msLeave(nullptr, m_pHover.Get());
    msLeave.m_pos = pTarget->TransformTo(m_pHover.Get(), pMsg->m_pos);
    msLeave.m_dwFlags = 0;
    msLeave.m_dwCmd = FWL_MouseCommand::Leave;
    DispatchMessage(&msLeave, nullptr);
  }
  if (pTarget->GetClassID() == FWL_Type::Form) {
    m_pHover = nullptr;
    return;
  }
  m_pHover = pTarget;

  CFWL_MessageMouse msHover(nullptr, pTarget);
  msHover.m_pos = pMsg->m_pos;
  msHover.m_dwFlags = 0;
  msHover.m_dwCmd = FWL_MouseCommand::Hover;
  DispatchMessage(&msHover, nullptr);
}

bool CFWL_NoteDriver::IsValidMessage(CFWL_Message* pMessage) {
  CFWL_Widget* pForm = m_pNoteLoop->GetForm();
  return pForm && pForm == pMessage->GetDstTarget();
}

CFWL_Widget* CFWL_NoteDriver::GetMessageForm(CFWL_Widget* pDstTarget) {
  CFWL_Widget* pMessageForm = m_pNoteLoop->GetForm();
  if (!pMessageForm && pDstTarget) {
    CFWL_WidgetMgr* pWidgetMgr = pDstTarget->GetOwnerApp()->GetWidgetMgr();
    pMessageForm = pWidgetMgr->GetSystemFormWidget(pDstTarget);
  }
  return pMessageForm;
}

void CFWL_NoteDriver::ClearEventTargets() {
  auto it = m_eventTargets.begin();
  while (it != m_eventTargets.end()) {
    auto old = it++;
    if (!old->second->IsValid())
      m_eventTargets.erase(old);
  }
}
