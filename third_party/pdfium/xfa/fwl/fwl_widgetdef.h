// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef XFA_FWL_FWL_WIDGETDEF_H_
#define XFA_FWL_FWL_WIDGETDEF_H_

#define FWL_WGTSTYLE_OverLapper 0
#define FWL_WGTSTYLE_Popup (1L << 0)
#define FWL_WGTSTYLE_Child (2L << 0)
#define FWL_WGTSTYLE_WindowTypeMask (3L << 0)
#define FWL_WGTSTYLE_Border (1L << 2)
#define FWL_WGTSTYLE_VScroll (1L << 11)
#define FWL_WGTSTYLE_Group (1L << 22)
#define FWL_WGTSTYLE_NoBackground (1L << 28)

#define FWL_WGTSTATE_Disabled (1L << 2)
#define FWL_WGTSTATE_Focused (1L << 4)
#define FWL_WGTSTATE_Invisible (1L << 5)
#define FWL_WGTSTATE_MAX (6)

#define FWL_VKEY_Back 0x08
#define FWL_VKEY_Tab 0x09
#define FWL_VKEY_NewLine 0x0A
#define FWL_VKEY_Clear 0x0C
#define FWL_VKEY_Return 0x0D
#define FWL_VKEY_Shift 0x10
#define FWL_VKEY_Control 0x11
#define FWL_VKEY_Menu 0x12
#define FWL_VKEY_Pause 0x13
#define FWL_VKEY_Capital 0x14
#define FWL_VKEY_Kana 0x15
#define FWL_VKEY_Hangul 0x15
#define FWL_VKEY_Junja 0x17
#define FWL_VKEY_Final 0x18
#define FWL_VKEY_Hanja 0x19
#define FWL_VKEY_Kanji 0x19
#define FWL_VKEY_Escape 0x1B
#define FWL_VKEY_Convert 0x1C
#define FWL_VKEY_NonConvert 0x1D
#define FWL_VKEY_Accept 0x1E
#define FWL_VKEY_ModeChange 0x1F
#define FWL_VKEY_Space 0x20
#define FWL_VKEY_Prior 0x21
#define FWL_VKEY_Next 0x22
#define FWL_VKEY_End 0x23
#define FWL_VKEY_Home 0x24
#define FWL_VKEY_Left 0x25
#define FWL_VKEY_Up 0x26
#define FWL_VKEY_Right 0x27
#define FWL_VKEY_Down 0x28
#define FWL_VKEY_Select 0x29
#define FWL_VKEY_Print 0x2A
#define FWL_VKEY_Execute 0x2B
#define FWL_VKEY_Snapshot 0x2C
#define FWL_VKEY_Insert 0x2D
#define FWL_VKEY_Delete 0x2E
#define FWL_VKEY_Help 0x2F
#define FWL_VKEY_0 0x30
#define FWL_VKEY_1 0x31
#define FWL_VKEY_2 0x32
#define FWL_VKEY_3 0x33
#define FWL_VKEY_4 0x34
#define FWL_VKEY_5 0x35
#define FWL_VKEY_6 0x36
#define FWL_VKEY_7 0x37
#define FWL_VKEY_8 0x38
#define FWL_VKEY_9 0x39
#define FWL_VKEY_A 0x41
#define FWL_VKEY_B 0x42
#define FWL_VKEY_C 0x43
#define FWL_VKEY_D 0x44
#define FWL_VKEY_E 0x45
#define FWL_VKEY_F 0x46
#define FWL_VKEY_G 0x47
#define FWL_VKEY_H 0x48
#define FWL_VKEY_I 0x49
#define FWL_VKEY_J 0x4A
#define FWL_VKEY_K 0x4B
#define FWL_VKEY_L 0x4C
#define FWL_VKEY_M 0x4D
#define FWL_VKEY_N 0x4E
#define FWL_VKEY_O 0x4F
#define FWL_VKEY_P 0x50
#define FWL_VKEY_Q 0x51
#define FWL_VKEY_R 0x52
#define FWL_VKEY_S 0x53
#define FWL_VKEY_T 0x54
#define FWL_VKEY_U 0x55
#define FWL_VKEY_V 0x56
#define FWL_VKEY_W 0x57
#define FWL_VKEY_X 0x58
#define FWL_VKEY_Y 0x59
#define FWL_VKEY_Z 0x5A
#define FWL_VKEY_LWin 0x5B
#define FWL_VKEY_Command 0x5B
#define FWL_VKEY_RWin 0x5C
#define FWL_VKEY_Apps 0x5D
#define FWL_VKEY_Sleep 0x5F
#define FWL_VKEY_NumPad0 0x60
#define FWL_VKEY_NumPad1 0x61
#define FWL_VKEY_NumPad2 0x62
#define FWL_VKEY_NumPad3 0x63
#define FWL_VKEY_NumPad4 0x64
#define FWL_VKEY_NumPad5 0x65
#define FWL_VKEY_NumPad6 0x66
#define FWL_VKEY_NumPad7 0x67
#define FWL_VKEY_NumPad8 0x68
#define FWL_VKEY_NumPad9 0x69
#define FWL_VKEY_Multiply 0x6A
#define FWL_VKEY_Add 0x6B
#define FWL_VKEY_Separator 0x6C
#define FWL_VKEY_Subtract 0x6D
#define FWL_VKEY_Decimal 0x6E
#define FWL_VKEY_Divide 0x6F
#define FWL_VKEY_F1 0x70
#define FWL_VKEY_F2 0x71
#define FWL_VKEY_F3 0x72
#define FWL_VKEY_F4 0x73
#define FWL_VKEY_F5 0x74
#define FWL_VKEY_F6 0x75
#define FWL_VKEY_F7 0x76
#define FWL_VKEY_F8 0x77
#define FWL_VKEY_F9 0x78
#define FWL_VKEY_F10 0x79
#define FWL_VKEY_F11 0x7A
#define FWL_VKEY_F12 0x7B
#define FWL_VKEY_F13 0x7C
#define FWL_VKEY_F14 0x7D
#define FWL_VKEY_F15 0x7E
#define FWL_VKEY_F16 0x7F
#define FWL_VKEY_F17 0x80
#define FWL_VKEY_F18 0x81
#define FWL_VKEY_F19 0x82
#define FWL_VKEY_F20 0x83
#define FWL_VKEY_F21 0x84
#define FWL_VKEY_F22 0x85
#define FWL_VKEY_F23 0x86
#define FWL_VKEY_F24 0x87
#define FWL_VKEY_NunLock 0x90
#define FWL_VKEY_Scroll 0x91
#define FWL_VKEY_LShift 0xA0
#define FWL_VKEY_RShift 0xA1
#define FWL_VKEY_LControl 0xA2
#define FWL_VKEY_RControl 0xA3
#define FWL_VKEY_LMenu 0xA4
#define FWL_VKEY_RMenu 0xA5
#define FWL_VKEY_BROWSER_Back 0xA6
#define FWL_VKEY_BROWSER_Forward 0xA7
#define FWL_VKEY_BROWSER_Refresh 0xA8
#define FWL_VKEY_BROWSER_Stop 0xA9
#define FWL_VKEY_BROWSER_Search 0xAA
#define FWL_VKEY_BROWSER_Favorites 0xAB
#define FWL_VKEY_BROWSER_Home 0xAC
#define FWL_VKEY_VOLUME_Mute 0xAD
#define FWL_VKEY_VOLUME_Down 0xAE
#define FWL_VKEY_VOLUME_Up 0xAF
#define FWL_VKEY_MEDIA_NEXT_Track 0xB0
#define FWL_VKEY_MEDIA_PREV_Track 0xB1
#define FWL_VKEY_MEDIA_Stop 0xB2
#define FWL_VKEY_MEDIA_PLAY_Pause 0xB3
#define FWL_VKEY_MEDIA_LAUNCH_Mail 0xB4
#define FWL_VKEY_MEDIA_LAUNCH_MEDIA_Select 0xB5
#define FWL_VKEY_MEDIA_LAUNCH_APP1 0xB6
#define FWL_VKEY_MEDIA_LAUNCH_APP2 0xB7
#define FWL_VKEY_OEM_1 0xBA
#define FWL_VKEY_OEM_Plus 0xBB
#define FWL_VKEY_OEM_Comma 0xBC
#define FWL_VKEY_OEM_Minus 0xBD
#define FWL_VKEY_OEM_Period 0xBE
#define FWL_VKEY_OEM_2 0xBF
#define FWL_VKEY_OEM_3 0xC0
#define FWL_VKEY_OEM_4 0xDB
#define FWL_VKEY_OEM_5 0xDC
#define FWL_VKEY_OEM_6 0xDD
#define FWL_VKEY_OEM_7 0xDE
#define FWL_VKEY_OEM_8 0xDF
#define FWL_VKEY_OEM_102 0xE2
#define FWL_VKEY_ProcessKey 0xE5
#define FWL_VKEY_Packet 0xE7
#define FWL_VKEY_Attn 0xF6
#define FWL_VKEY_Crsel 0xF7
#define FWL_VKEY_Exsel 0xF8
#define FWL_VKEY_Ereof 0xF9
#define FWL_VKEY_Play 0xFA
#define FWL_VKEY_Zoom 0xFB
#define FWL_VKEY_NoName 0xFC
#define FWL_VKEY_PA1 0xFD
#define FWL_VKEY_OEM_Clear 0xFE
#define FWL_VKEY_Unknown 0

#endif  // XFA_FWL_FWL_WIDGETDEF_H_
