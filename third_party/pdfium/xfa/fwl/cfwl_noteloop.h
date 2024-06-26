// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_NOTELOOP_H_
#define XFA_FWL_CFWL_NOTELOOP_H_

class CFWL_Widget;

class CFWL_NoteLoop {
 public:
  CFWL_NoteLoop();
  ~CFWL_NoteLoop() {}

  CFWL_Widget* GetForm() const { return m_pForm; }
  bool ContinueModal() const { return m_bContinueModal; }
  void EndModalLoop() { m_bContinueModal = false; }
  void SetMainForm(CFWL_Widget* pForm) { m_pForm = pForm; }

 private:
  CFWL_Widget* m_pForm;
  bool m_bContinueModal;
};

#endif  // XFA_FWL_CFWL_NOTELOOP_H_
