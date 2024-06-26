// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_CFWL_WIDGET_H_
#define XFA_FWL_CFWL_WIDGET_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"
#include "xfa/fde/cfde_data.h"
#include "xfa/fwl/cfwl_event.h"
#include "xfa/fwl/cfwl_themepart.h"
#include "xfa/fwl/cfwl_widgetmgr.h"
#include "xfa/fwl/cfwl_widgetproperties.h"
#include "xfa/fwl/fwl_widgethit.h"
#include "xfa/fwl/ifwl_widgetdelegate.h"

enum class FWL_Type {
  Unknown = 0,

  Barcode,
  Caret,
  CheckBox,
  ComboBox,
  DateTimePicker,
  Edit,
  Form,
  FormProxy,
  ListBox,
  MonthCalendar,
  PictureBox,
  PushButton,
  ScrollBar,
  SpinButton,
  ToolTip
};

class CFWL_App;
class CFWL_AppImp;
class CFWL_MessageKey;
class CFWL_Widget;
class CFWL_WidgetMgr;
class CXFA_FFWidget;
class IFWL_ThemeProvider;

class CFWL_Widget : public IFWL_WidgetDelegate {
 public:
  ~CFWL_Widget() override;

  virtual FWL_Type GetClassID() const = 0;
  virtual bool IsForm() const;
  virtual CFX_RectF GetAutosizedWidgetRect();
  virtual CFX_RectF GetWidgetRect();
  virtual CFX_RectF GetClientRect();
  virtual void ModifyStylesEx(uint32_t dwStylesExAdded,
                              uint32_t dwStylesExRemoved);
  virtual void SetStates(uint32_t dwStates);
  virtual void RemoveStates(uint32_t dwStates);
  virtual void Update() = 0;
  virtual FWL_WidgetHit HitTest(const CFX_PointF& point);
  virtual void DrawWidget(CXFA_Graphics* pGraphics,
                          const CFX_Matrix& matrix) = 0;
  virtual void SetThemeProvider(IFWL_ThemeProvider* pThemeProvider);

  // IFWL_WidgetDelegate.
  void OnProcessMessage(CFWL_Message* pMessage) override;
  void OnProcessEvent(CFWL_Event* pEvent) override;
  void OnDrawWidget(CXFA_Graphics* pGraphics,
                    const CFX_Matrix& matrix) override;

  void InflateWidgetRect(CFX_RectF& rect);
  void SetWidgetRect(const CFX_RectF& rect);

  void SetParent(CFWL_Widget* pParent);

  bool IsVisible() const;
  bool IsOverLapper() const;
  bool IsPopup() const;
  bool IsChild() const;

  CFWL_Widget* GetOwner() { return m_pWidgetMgr->GetOwnerWidget(this); }
  CFWL_Widget* GetOuter() const { return m_pOuter; }

  uint32_t GetStyles() const;
  void ModifyStyles(uint32_t dwStylesAdded, uint32_t dwStylesRemoved);
  uint32_t GetStylesEx() const;
  uint32_t GetStates() const;

  void LockUpdate() { m_iLock++; }
  void UnlockUpdate() {
    if (IsLocked())
      m_iLock--;
  }

  CFX_PointF TransformTo(CFWL_Widget* pWidget, const CFX_PointF& point);
  CFX_Matrix GetMatrix() const;
  IFWL_ThemeProvider* GetThemeProvider() const {
    return m_pProperties->m_pThemeProvider.Get();
  }

  void SetDelegate(IFWL_WidgetDelegate* delegate) { m_pDelegate = delegate; }
  IFWL_WidgetDelegate* GetDelegate() {
    return m_pDelegate ? m_pDelegate.Get() : this;
  }
  const IFWL_WidgetDelegate* GetDelegate() const {
    return m_pDelegate ? m_pDelegate.Get() : this;
  }

  const CFWL_App* GetOwnerApp() const { return m_pOwnerApp.Get(); }
  uint32_t GetEventKey() const { return m_nEventKey; }
  void SetEventKey(uint32_t key) { m_nEventKey = key; }

  CXFA_FFWidget* GetLayoutItem() const { return m_pLayoutItem; }
  void SetLayoutItem(CXFA_FFWidget* pItem) { m_pLayoutItem = pItem; }

  void RepaintRect(const CFX_RectF& pRect);

 protected:
  CFWL_Widget(const CFWL_App* app,
              std::unique_ptr<CFWL_WidgetProperties> properties,
              CFWL_Widget* pOuter);

  bool IsEnabled() const;
  bool IsLocked() const { return m_iLock > 0; }
  bool HasBorder() const;
  CFX_RectF GetEdgeRect() const;
  float GetCXBorderSize() const;
  float GetCYBorderSize() const;
  CFX_RectF GetRelativeRect() const;
  IFWL_ThemeProvider* GetAvailableTheme() const;
  CFX_SizeF CalcTextSize(const WideString& wsText,
                         IFWL_ThemeProvider* pTheme,
                         bool bMultiLine);
  void CalcTextRect(const WideString& wsText,
                    IFWL_ThemeProvider* pTheme,
                    const FDE_TextStyle& dwTTOStyles,
                    FDE_TextAlignment iTTOAlign,
                    CFX_RectF* pRect);
  void SetGrab(bool bSet);
  void RegisterEventTarget(CFWL_Widget* pEventSource);
  void UnregisterEventTarget();
  void DispatchEvent(CFWL_Event* pEvent);
  void DrawBorder(CXFA_Graphics* pGraphics,
                  CFWL_Part iPartBorder,
                  IFWL_ThemeProvider* pTheme,
                  const CFX_Matrix& pMatrix);

  UnownedPtr<const CFWL_App> const m_pOwnerApp;
  UnownedPtr<CFWL_WidgetMgr> const m_pWidgetMgr;
  std::unique_ptr<CFWL_WidgetProperties> m_pProperties;
  CFWL_Widget* m_pOuter;
  int32_t m_iLock;

 private:
  CFWL_Widget* GetParent() const { return m_pWidgetMgr->GetParentWidget(this); }
  CFX_SizeF GetOffsetFromParent(CFWL_Widget* pParent);
  void DrawBackground(CXFA_Graphics* pGraphics,
                      CFWL_Part iPartBk,
                      IFWL_ThemeProvider* pTheme,
                      const CFX_Matrix* pMatrix);
  void NotifyDriver();
  bool IsParent(CFWL_Widget* pParent);

  CXFA_FFWidget* m_pLayoutItem;
  uint32_t m_nEventKey;
  UnownedPtr<IFWL_WidgetDelegate> m_pDelegate;
};

#endif  // XFA_FWL_CFWL_WIDGET_H_
